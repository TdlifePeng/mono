
#pragma once

typedef void ( *CountMonoObject )( const char *, size_t size, void * userdata );

void InitMonoObjects( void );
void NewMonoObject( void * point, const char * type, const char * name );
void FreeMonoObject( void * point );
void StatisticMonoObject( CountMonoObject cb, void * userdata );
