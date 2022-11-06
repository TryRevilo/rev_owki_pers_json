//
// Created by rev on 8/10/18.
//

#include "../../../../rev_pers_libs_ext/libs_cust_rev/rev_list/rev_linked_list.h"

#ifndef REVCORE_REV_PERS_READ_REV_ENTITY_METASTRINGS_H
#define REVCORE_REV_PERS_READ_REV_ENTITY_METASTRINGS_H

int revEntityMetastringExists(char *revMetastringValue);

long getRevEntityMetaStringValueId(char *revMetastringValue);

char *getRevEntityMetaStringById(long id);

list *revPersGetALLRevEntityRevEntityMetastringIdByValue(char *metastringValue);

#endif //REVCORE_REV_PERS_READ_REV_ENTITY_METASTRINGS_H
