
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

	map<string, int>	Count;

	for( auto it = Objects.cbegin(); it != Objects.cend(); ++it )
	{
		auto		cit = Count.find( it->second );

		if( cit != Count.end() )
			++cit->second;
		else
			Count[ it->second ] = 1;
	}

	for( auto it = Count.cbegin(); it != Count.cend(); ++it )
		cb( it->first.c_str(), it->second, userdata );

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

static void StatisticMonoObjectReverseRefer( list<const void *> & points, set<const void *> & processed, ReferMonoObject cb, void * userdata, int maxdepth )
{
	auto									depth = 1;
	map<const void *, set<const void *>>	map;

	while( !points.empty() )
	{
		auto		point = points.front();

		points.pop_front();
		if( point == NULL )
		{
			if( maxdepth > 0 && depth == maxdepth )
				break;

			++depth;
			continue;
		}

		auto		islast = points.size() > 0 && points.front() == NULL;
		auto		first = true;

		for( auto it = Objects.cbegin(); it != Objects.cend(); ++it )
			if( it->first != point )
			{
				auto		refit = map.find( it->first );

				if( refit == map.end() )
				{
					refit = map.insert( pair<const void *, set<const void *>>( it->first, set<const void *>() ) ).first;

					auto		hdp = HBLKPTR( it->first );
					auto		hhdr = HDR( hdp );
					auto		first = true;

					for( auto i = 0; i < hhdr->hb_sz; ++i )
					{
						auto		ref = ( const void * )( ( ( const word * )it->first )[ i ] );

						if( Objects.find( ref ) != Objects.end() )
							refit->second.insert( ref );
					}
				}

				if( refit->second.find( point ) != refit->second.end() )
				{
					if( processed.find( it->first ) == processed.end() )
					{
						points.push_back( it->first );
						processed.insert( it->first );
					}

					if( !first )
						cb( NULL, NULL, it->first, Objects[ it->first ].c_str(), userdata );
					else
					{
						first = false;
						cb( point, Objects[ point ].c_str(), it->first, Objects[ it->first ].c_str(), userdata );
					}
					break;
				}
			}

		if( islast )
			points.push_back( NULL );
	}
}

void StatisticMonoObjectReverseRefer( const char * type, ReferMonoObject cb, void * userdata, int maxdepth )
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
	points.push_back( NULL );

	StatisticMonoObjectReverseRefer( points, set, cb, userdata, maxdepth );

	UnlockMonoObjects();
	UnlockGC();
}