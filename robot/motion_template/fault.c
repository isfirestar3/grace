#include "var.h"
#include "mnt.h"
#include "clist.h"

static const unsigned char BYTE_BIT_MAP[] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};

static
int mnti__page_fault(var__functional_object_t *obj, void *usrdat) {
    struct list_head *pos, *cursor;
    var__pagefault_mapping_t *mapping;
    var__functional_object_t *from;

    if (!obj) return -1;

    if (!list_empty(&obj->pagefault_)) {
        list_for_each_safe(pos, cursor, &obj->pagefault_) {
            mapping = containing_record(pos, var__pagefault_mapping_t, entry_);

            from = objrefr(mapping->from_);
            if (!from) {
                list_del(pos);
                INIT_LIST_HEAD(pos);
                free(mapping);
            } else {
                /* 按字节对齐缺页， len描述拷贝字节数， 按位缺页，len描述拷贝位数 */
                if (mapping->mode_ == kPageFaultMode_ByteAlign) {
                    memcpy((char *) obj->body_ + mapping->dst_byte_offset_, (const char *) from->body_ + mapping->src_byte_offset_, mapping->len_);
                } else {
                    if (mapping->mode_ == kPageFaultMode_Bits) {
                        char *p_dst = (char *) obj->body_ + mapping->dst_byte_offset_;
                        const char *p_src = (const char *) from->body_ + mapping->src_byte_offset_;
                        int i;

                        /* len 范围内， 非字节对齐， 按位缺页 */
                        for (i = 0; i < mapping->len_; i++) {
                            (p_src[(i + mapping->src_bit_offset_) / 8] & BYTE_BIT_MAP[(i + mapping->src_bit_offset_) % 8]) ?
                                    (p_dst[(i + mapping->dst_bit_offset_) / 8] |= BYTE_BIT_MAP[(i + mapping->dst_bit_offset_) % 8]) :
                                    (p_dst[(i + mapping->dst_bit_offset_) / 8] &= ~BYTE_BIT_MAP[(i + mapping->dst_bit_offset_) % 8]);
                        }
                    }
                }
                objdefr(mapping->from_);
            }
        }
    }
    return 0;
}

void mnt__executive_page_fault() {
    var__traverse_global_objects(mnti__page_fault, NULL);
}
