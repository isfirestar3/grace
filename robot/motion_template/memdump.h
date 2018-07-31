#if !defined MEMDUMP_H
#define MEMDUMP_H

extern
int allocate_memory_dump(void **dump_object);
extern
void destroy_memory_dump(void *dump_object) ;
extern
int add_memory_record(void *dump_object, const void *memory_data, int sizecb);
extern
void move_memory_record_tonext(void *dump_object);
extern
int dump_memory_record(const void *dump_object);

#endif