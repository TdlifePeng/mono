/*
 * mono-gc.h: GC related public interface
 *
 */
#ifndef __METADATA_MONO_GC_H__
#define __METADATA_MONO_GC_H__

#include <mono/metadata/object.h>

G_BEGIN_DECLS

void   mono_gc_collect         (int generation);
int    mono_gc_max_generation  (void);
int    mono_gc_get_generation  (MonoObject *object);
int    mono_gc_collection_count (int generation);
gint64 mono_gc_get_used_size   (void);
gint64 mono_gc_get_heap_size   (void);
int    mono_gc_invoke_finalizers (void);

MonoArray* mono_unity_liveness_calculation_from_statics_managed_InternalCall(MonoReflectionType * filter_type);
void mono_unity_count_objects_InternalCall( MonoString * filepath );
void mono_unity_type_references_InternalCall( MonoString * type, MonoString * filepath );
void mono_unity_type_reverse_references_InternalCall( MonoString * type, MonoString * filepath, gint maxdepth );
MonoObject * mono_object_new_outgc_InternalCall( MonoReflectionType * type );
MonoString * mono_string_clone_outgc_InternalCall( MonoString * str, BOOL intern );
MonoArray * mono_array_new_outgc_InternalCall( MonoReflectionType * type, mono_array_size_t n );
void mono_object_free_outgc_InternalCall( MonoObject * obj );
void mono_force_free_outgc_InternalCall( MonoObject * obj );
size_t mono_out_of_heap_size( void );

G_END_DECLS

#endif /* __METADATA_MONO_GC_H__ */

