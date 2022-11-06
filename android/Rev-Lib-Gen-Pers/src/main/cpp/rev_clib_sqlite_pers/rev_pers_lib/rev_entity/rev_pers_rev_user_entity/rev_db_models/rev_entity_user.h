//
// Created by rev on 7/9/18.
//

#ifndef REVCORE_REV_ENTITY_USER_H
#define REVCORE_REV_ENTITY_USER_H

typedef struct {

    char *_full_names;
    char *_email;

    long _revEntityGUID;

    long _revOwnerEntityGUID;
    long _revContainerEntityGUID;

} RevUserEntity;

#endif //REVCORE_REV_ENTITY_USER_H
