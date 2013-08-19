
extern "C"
{
	void * g_malloc( size_t );
	void g_free( const void * );
}

void * operator new( size_t size ) throw()
{
	return g_malloc( size );
}

void operator delete( void * p ) throw()
{
	g_free( p );
}

#include <windows.h>
#include <string>
#include <map>

using namespace std;

static map<void *, string>		Objects;
static int						NewError = 0;
static int						FreeError = 0;

static CRITICAL_SECTION			Counter_ml;

#define LOCK() EnterCriticalSection( &Counter_ml );
#define UNLOCK() LeaveCriticalSection( &Counter_ml );

extern "C"
{
	typedef void ( *CountMonoObject )( const char *, size_t size, void * userdata );

	void InitMonoObjects( void );
	void NewMonoObject( void * point, const char * type, const char * name );
	void FreeMonoObject( void * point );
	void StatisticMonoObject( CountMonoObject cb, void * userdata );
}

void InitMonoObjects( void )
{
	InitializeCriticalSection( &Counter_ml );
}

void NewMonoObject( void * point, const char * type, const char * name )
{
	LOCK();

	auto	fullname = strlen( type ) > 0 ? ( ( string )type + "." + name ) : name;
	auto	it = Objects.find( point );

	if( it == Objects.end() )
		Objects.insert( pair<void *, string>( point, fullname ) );
	else
	{
		++NewError;
		if( it->second != fullname )
			it->second = fullname;
	}

	UNLOCK();
}

void FreeMonoObject( void * point )
{
	LOCK();

	auto	it = Objects.find( point );

	if( it != Objects.end() )
		Objects.erase( it );
	else
		++FreeError;

	UNLOCK();
}

void StatisticMonoObject( CountMonoObject cb, void * userdata )
{
	LOCK();

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

	UNLOCK();
}