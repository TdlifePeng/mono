
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


void LockGC();
void UnlockGC();
void LockMonoObjects();
void UnlockMonoObjects();


void InitMonoObjects( void );
void NewMonoObject( const void * point, const char * spacename, const char * name );
void FreeMonoObject( const void * point );


typedef void ( *CountMonoObject )( const char * point, size_t count, size_t size, void * userdata );
void StatisticMonoObject( CountMonoObject cb, void * userdata );

typedef BOOL ( *ReferMonoObject )( const void * point, const char * pname, const void * refer, const char * rname, void * userdata );
void StatisticMonoObjectRefer( const char * type, ReferMonoObject cb, void * userdata );
void StatisticMonoObjectReverseRefer( const char * type, ReferMonoObject cb, void * userdata, int maxdepth );

typedef BOOL ( *EachMonoObject )( const void * point, const char * type, void * );
void ForEachMonoObjects( EachMonoObject cb, void * userdata );

typedef BOOL ( *EachMonoObjectRefer )( const void * point, const void * refer, void * );
BOOL ForEachMonoObjectRefer( const void * point, EachMonoObjectRefer cb, void * userdata );


#ifdef __cplusplus
};
#endif