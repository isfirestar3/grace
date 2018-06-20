#if !defined XML_LOADED_H
#define XML_LOADED_H

#include "libxml/parser.h"
#include "libxml/tree.h"
#include <stdint.h>
#include "vartypes.h"

extern
const char *get_first_text_value(xmlNodePtr node);
extern
double covert_first_text_value_d(xmlNodePtr node);
extern
int covert_first_text_value_i(xmlNodePtr node);
extern
long covert_first_text_value_l(xmlNodePtr node);
extern
void covert_first_text_value_s(xmlNodePtr node, char *target, int cch);
extern
uint64_t covert_first_text_value_ull(xmlNodePtr node);
extern
const char *get_first_text_properties(xmlAttr *attr);
extern
int xml__read_data(const char *subdir, const char *file, const char *rootname, int( *handler)(xmlNodePtr element));
extern
int xml__check_candevice_head(xmlNode *node, var__can_device_t *candev);

#endif