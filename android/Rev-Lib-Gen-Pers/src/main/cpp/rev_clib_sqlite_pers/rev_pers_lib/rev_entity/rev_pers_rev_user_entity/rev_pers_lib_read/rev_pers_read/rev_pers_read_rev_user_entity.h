//
// Created by rev on 8/16/18.
//

#ifndef REVCORE_REV_PERS_READ_REV_USER_ENTITY_H
#define REVCORE_REV_PERS_READ_REV_USER_ENTITY_H

#include "../../rev_db_models/rev_entity_user.h"

int totalLocalRevUserEntites();

RevUserEntity revPersGet_RevRevUserEntityByEmail_Phone(char *revEmailPhone);

#endif //REVCORE_REV_PERS_READ_REV_USER_ENTITY_H
