
extern "C"
{
	void * g_malloc( size_t );
	void g_free( const void * );
}

static void * operator new( size_t size ) throw()
{
	return g_malloc( size );
}

static void operator delete( void * p ) throw()
{
	g_free( p );
}

#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <list>
#include <set>

extern "C"
{
	#include "private/gc_priv.h"
};

#include "MonoObjects.h"

using namespace std;

static map<const void *, string>	Objects;									// 跨lib引用问题多多
static int							NewError = 0;
static int							FreeError = 0;

static CRITICAL_SECTION				Counter_ml;


void LockMonoObjects()
{
	EnterCriticalSection( &Counter_ml );
}

void UnlockMonoObjects()
{
	LeaveCriticalSection( &Counter_ml );
}

void InitMonoObjects( void )
{
	InitializeCriticalSection( &Counter_ml );
}

void NewMonoObject( const void * point, const char * spacename, const char * name )
{
	LockMonoObjects();

	auto	fullname = strlen( spacename ) > 0 ? ( ( string )spacename + "." + name ) : name;
	auto	it = Objects.find( point );

	if( it == Objects.end() )
		Objects.insert( pair<const void *, string>( point, fullname ) );
	else
	{
		++NewError;
		if( it->second != fullname )
			it->second = fullname;
	}

	UnlockMonoObjects();
}

void FreeMonoObject( const void * point )
{
	LockMonoObjects();

	auto	it = Objects.find( point );

	if( it != Objects.end() )
		Objects.erase( it );
	else
		++FreeError;

	UnlockMonoObjects();
}

void StatisticMonoObject( CountMonoObject cb, void * userdata )
{
	LockMonoObjects();

	map<string, pair<int, const void *>>	Count;

	for( auto it = Objects.cbegin(); it != Objects.cend(); ++it )
	{
		auto		cit = Count.find( it->second );

		if( cit != Count.end() )
			++cit->second.first;
		else
			Count[ it->second ] = pair<int, const void *>( 1, it->first );
	}

	for( auto it = Count.cbegin(); it != Count.cend(); ++it )
	{
		auto		hdp = HBLKPTR( it->second.second );
		auto		hhdr = HDR( hdp );

		cb( it->first.c_str(), it->second.first, hhdr->hb_sz * sizeof( word ), userdata );
	}

	UnlockMonoObjects();
}

void ForEachMonoObject( EachMonoObject cb, void * userdata )
{
	for( auto it = Objects.cbegin(); it != Objects.cend(); ++it )
		if( cb( it->first, it->second.c_str(), userdata ) )
			break;
}

BOOL ForEachMonoObjectRefer( const void * point, EachMonoObjectRefer cb, void * userdata )
{
	if( Objects.find( point ) == Objects.end() )
		return FALSE;

	auto		hdp = HBLKPTR( point );
	auto		hhdr = HDR( hdp );

	for( auto i = 0; i < hhdr->hb_sz; ++i )
	{
		auto		refer = ( const void * )( ( ( const word * )point )[ i ] );

		if( Objects.find( refer ) != Objects.end() )
			if( cb( point, refer, userdata ) )
				break;
	}

	return TRUE;
}

static void StatisticMonoObjectRefer( list<const void *> & points, set<const void *> & set, ReferMonoObject cb, void * userdata )
{
	while( !points.empty() )
	{
		auto		point = points.front();

		points.pop_front();

		auto		hdp = HBLKPTR( point );
		auto		hhdr = HDR( hdp );
		auto		first = true;

		for( auto i = 0; i < hhdr->hb_sz; ++i )
		{
			auto		refer = ( const void * )( ( ( const word * )point )[ i ] );

			if( Objects.find( refer ) != Objects.end() )
			{
				if( set.find( refer ) == set.end() )
				{
					points.push_back( refer );
					set.insert( refer );
				}

				if( !first )
					cb( NULL, NULL, refer, Objects[ refer ].c_str(), userdata );
				else
				{
					first = false;
					cb( point, Objects[ point ].c_str(), refer, Objects[ refer ].c_str(), userdata );
				}
			}
		}

		if( first )
			cb( point, Objects[ point ].c_str(), NULL, NULL, userdata );
	}
}

void StatisticMonoObjectRefer( const char * type, ReferMonoObject cb, void * userdata )
{
	LockGC();
	LockMonoObjects();

	list<const void *>	points;
	set<const void *>	set;

	for_each( Objects.cbegin(), Objects.cend(),
		[ type, &points, &set ]( const pair<const void *, string> & item )
		{
			if( item.second == type )
			{
				points.push_back( item.first );
				set.insert( item.first );
			}
		} );

	StatisticMonoObjectRefer( points, set, cb, userdata );

	UnlockMonoObjects();
	UnlockGC();
}

static void StatisticMonoObjectReverseRefer( list<const void *> & points, ReferMonoObject cb, void * userdata, int maxdepth )
{
	map<const void *, vector<const void *>>	reverses;

	for( auto it = Objects.cbegin(); it != Objects.cend(); ++it )
	{
		auto		hdp = HBLKPTR( it->first );
		auto		hhdr = HDR( hdp );
		auto		first = true;

		for( auto i = 0; i < hhdr->hb_sz; ++i )
		{
			auto		ref = ( const void * )( ( ( const word * )it->first )[ i ] );

			if( Objects.find( ref ) != Objects.end() )
			{
				auto		rit = reverses.find( ref );

				if( rit == reverses.end() )
					rit = reverses.insert( pair<const void *, vector<const void *>>( ref, vector<const void *>() ) ).first;

				rit->second.push_back( it->first );
			}
		}
	}

	set<const void *>		processed;

	for( auto it = points.cbegin(); it != points.cend(); ++it )
		processed.insert( *it );
	points.push_back( NULL );

	auto		depth = 1;

	while( !points.empty() )
	{
		auto		point = points.front();

		points.pop_front();
		if( point == NULL )
		{
			if( depth == maxdepth )
				break;

			++depth;
			continue;
		}

		auto		islast = points.size() > 0 && points.front() == NULL;
		auto		first = true;
		auto		rrit = reverses.find( point );

		if( rrit != reverses.cend() )
		{
			for( auto rit = rrit->second.cbegin(); rit != rrit->second.cend(); ++rit )
			{
				if( processed.find( *rit ) == processed.end() )
				{
					points.push_back( *rit );
					processed.insert( *rit );
				}

				if( !first )
					cb( NULL, NULL, *rit, Objects[ *rit ].c_str(), userdata );
				else
				{
					first = false;
					cb( point, Objects[ point ].c_str(), *rit, Objects[ *rit ].c_str(), userdata );
				}
			}
		}
		else
			cb( point, Objects[ point ].c_str(), NULL, NULL, userdata );

		if( islast )
			points.push_back( NULL );
	}
}

void StatisticMonoObjectReverseRefer( const char * type, ReferMonoObject cb, void * userdata, int maxdepth )
{
	LockGC();
	LockMonoObjects();

	list<const void *>	points;

	for_each( Objects.cbegin(), Objects.cend(),
		[ type, &points ]( const pair<const void *, string> & item )
	{
		if( item.second == type )
			points.push_back( item.first );
	} );

	StatisticMonoObjectReverseRefer( points, cb, userdata, maxdepth );

	UnlockMonoObjects();
	UnlockGC();
}