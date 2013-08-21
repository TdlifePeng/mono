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
void mono_unity_snapshot_objects_InternalCall( MonoString * filepath );
void mono_unity_objects_references_InternalCall( MonoString * type, MonoString * filepath );
void mono_unity_objects_reverse_references_InternalCall( MonoString * type, MonoString * filepath, gint maxdepth );

G_END_DECLS

#endif /* __METADATA_MONO_GC_H__ */

