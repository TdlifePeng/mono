
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


void LockGC();
void UnlockGC();
void LockMonoObjects();
void UnlockMonoObjects();


void InitMonoObjects( void );

typedef const char * ( *TypeFullNameGetter )( const void * vt );
typedef void ( *TypeFullNameFreer )( const char * str );
void NewMonoObject( const void * point, const void * vt, TypeFullNameGetter getter, TypeFullNameFreer freer );

void FreeMonoObject( const void * point );

void MonoObjectUnsafeFree( const void * point );


typedef void ( *CountMonoObject )( const char * point, size_t count, size_t size, void * userdata );
void StatisticMonoObject( CountMonoObject cb, void * userdata );

typedef BOOL ( *ReferMonoObject )( const void * point, const char * pname, const void * refer, const char * rname, void * userdata );
void StatisticMonoObjectRefer( const char * type, ReferMonoObject cb, void * userdata );
void StatisticMonoObjectReverseRefer( const char * type, ReferMonoObject cb, void * userdata, int maxdepth );


// 使用者考虑什么时候锁什么

typedef BOOL ( *EachMonoObject )( const void * point, const void * vt, void * userdata );
void ForEachMonoObject( EachMonoObject cb, void * userdata );
void ForEachMonoType( const char * type, EachMonoObject cb, void * userdata );

typedef BOOL ( *EachMonoObjectRefer )( const void * point, const void * refer, void * userdata );
BOOL ForEachMonoObjectRefer( const void * point, EachMonoObjectRefer cb, void * userdata );


#ifdef __cplusplus
};
#endif