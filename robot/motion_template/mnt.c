#include "mnt.h"
#include "xml_loaded.h"
#include "object.h"
#include "var.h"

#include "posix_ifos.h"
#include "posix_string.h"

/*
 * 缺页规则:
 * 1. 必须指定缺页源和缺页目标对象
 * 2. 必须指定缺页源和缺页目标的操作字节偏移
 * 3. 同时不指定缺页源和缺页目标的位偏移，则按照字节对齐的方式执行缺页
 * 4. 一旦指定了缺页源或缺页目标的位偏移，则 len_ 无效，缺页操作按位执行
 *  4.1 如果 blen_ 无效， 则该缺页操作项无效
 *  4.2 如果 fbit tbit 均为零， 而 blen>0, 该缺页项也被认定为按位缺页(只要指定了 blen, 则操作将忽略 len)
 * 5. 可选项 bfill_， 在按位缺页操作时，如果源和目标的缺页位数不一样， 则可以要求强制按较大的拷贝位数补位
 */
typedef struct {
    int from_; /* 数据源对象ID */
    int src_byte_offset_; /* 拷贝源字节偏移 */
    int src_bit_offset_; /* 拷贝源（字节偏移后）字节内的位偏移 */
    int to_; /* 数据目标对象ID */
    int dst_byte_offset_; /* 拷贝目标字节偏移 */
    int dst_bit_offset_; /* 拷贝目标（字节偏移后）字节内的位偏移 */
    int bytes_; /* 拷贝字节长度 */
    int bits_; /* 拷贝位长度 */
    int bfill_; /*使用位缺页的方案时，如果拷贝源的位数与拷贝目标的位数不一致， 则可以选择自动高位补位，补位数据为 bfill_ 的第0位*/
} mnti__mount_pagefault_t;

static
int mnti__fill_binding(mnti__mount_pagefault_t *mnt, xmlNode *node) {

    xmlAttr *attr;
    const char *sz;

    if (!mnt || !node) {
        return -1;
    }

    attr = node->properties;
    if (!attr) {
        return -1;
    }

    while (attr) {
        if (0 == xmlStrcmp(attr->name, BAD_CAST"from") || 0 == xmlStrcmp(attr->name, BAD_CAST"src_object_id")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->from_ = strtol(sz, NULL, 10);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"foff") || 0 == xmlStrcmp(attr->name, BAD_CAST"src_byte_offset")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->src_byte_offset_ = strtol(sz, NULL, 10);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"fbit") || 0 == xmlStrcmp(attr->name, BAD_CAST"src_bit_offset")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->src_bit_offset_ = strtol(sz, NULL, 10);
                if (mnt->src_bit_offset_ >= 8) { /* 位偏移相对于字节内而言 */
                    return -1;
                }
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"to") || 0 == xmlStrcmp(attr->name, BAD_CAST"dst_object_id")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->to_ = strtol(sz, NULL, 10);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"toff") || 0 == xmlStrcmp(attr->name, BAD_CAST"dst_byte_offset")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->dst_byte_offset_ = strtol(sz, NULL, 10);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"tbit") || 0 == xmlStrcmp(attr->name, BAD_CAST"dst_bit_offset")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->dst_bit_offset_ = strtol(sz, NULL, 10);
                if (mnt->src_bit_offset_ >= 8) { /* 位偏移相对于字节内而言 */
                    return -1;
                }
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"len") || 0 == xmlStrcmp(attr->name, BAD_CAST"bytes")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->bytes_ = strtol(sz, NULL, 10);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"blen") || 0 == xmlStrcmp(attr->name, BAD_CAST"bits")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->bits_ = strtol(sz, NULL, 10);
            }
        } else if (0 == xmlStrcmp(attr->name, BAD_CAST"bfill")) {
            sz = get_first_text_properties(attr);
            if (sz) {
                mnt->bfill_ = strtol(sz, NULL, 10);
                mnt->bfill_ &= 1; /* 只有最低位有效 */
            }
        }
        attr = attr->next;
    }

    /* 配置标定为按位缺页， 但是没有指定拷贝位个数, 则此项 mnt 无效 */
    if ((mnt->src_bit_offset_ > 0) || (mnt->dst_bit_offset_ > 0)) {
        if (mnt->bits_ <= 0) {
            return -1;
        }
    }
        
    return 0;
}

static
void mnti__build_mapping(const mnti__mount_pagefault_t *mnt) {

    objhld_t hld_from, hld_to;
    var__functional_object_t *ob_to;
    var__pagefault_mapping_t *mapping;

    if (!mnt) return;

    hld_from = var__getobj_handle_byid(mnt->from_);
    hld_to = var__getobj_handle_byid(mnt->to_);

    /* 构建一个映射， 并插入到映射目标对象 */
    ob_to = objrefr(hld_to);
    if (ob_to) {
        mapping = (var__pagefault_mapping_t *) malloc(sizeof (var__pagefault_mapping_t));
        if (mapping) {
            mapping->from_ = hld_from;

            mapping->src_byte_offset_ = mnt->src_byte_offset_;
            mapping->src_bit_offset_ = mnt->src_bit_offset_;

            mapping->dst_byte_offset_ = mnt->dst_byte_offset_;
            mapping->dst_bit_offset_ = mnt->dst_bit_offset_;

            mapping->bfill_ = mnt->bfill_;
            
            /* 确定缺页的 mode(按位缺页或字节对齐) */
            if ((mnt->src_bit_offset_ > 0) || (mnt->dst_bit_offset_ > 0) || (mnt->bits_ > 0)) {
                mapping->len_ = mnt->bits_;
                mapping->mode_ = kPageFaultMode_Bits;
            } else {
                mapping->len_ = mnt->bytes_;
                mapping->mode_ = kPageFaultMode_ByteAlign;
            }

            list_add_tail(&mapping->entry_, &ob_to->pagefault_);
        }
        objdefr(hld_to);
    }
}

/* 加载变量挂载关系 */
int mnt__load_setting() {

    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
    xmlNodePtr element = NULL;
    char xml_path[MAXPATH];
    const char *pedir;

    pedir = posix__getpedir();
    if (!pedir) {
        return -1;
    }
#if _WIN32
    posix__sprintf(xml_path, cchof(xml_path), "%s\\etc\\mnt.xml", pedir);
#else
    posix__sprintf(xml_path, cchof(xml_path), "/etc/agv/mnt.xml", pedir);
#endif
    if (NULL == (doc = xmlReadFile(xml_path, "GB2312", 256))) {
        return -1;
    }

    if (NULL == (root = xmlDocGetRootElement(doc))) {
        return -1;
    }

    element = root->xmlChildrenNode;
    while (element) {
        if (0 == xmlStrcmp(BAD_CAST"binding", element->name)) {
            mnti__mount_pagefault_t mnt;
            memset(&mnt, 0, sizeof (mnt));
            if (mnti__fill_binding(&mnt, element) >= 0) {
                mnti__build_mapping(&mnt);
            }
        }

        element = element->next;
    }

    xmlFreeDoc(doc);
    return 0;
}