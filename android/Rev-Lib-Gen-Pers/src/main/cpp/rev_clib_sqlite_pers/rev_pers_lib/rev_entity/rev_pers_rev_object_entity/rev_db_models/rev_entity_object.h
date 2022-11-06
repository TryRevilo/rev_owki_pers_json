//
// Created by rev on 7/9/18.
//

#ifndef REVCORE_REV_ENTITY_OBJECT_H
#define REVCORE_REV_ENTITY_OBJECT_H

typedef struct {

    char *_name;
    char *_description;

    long _revEntityGUID;

    long _revOwnerEntityGUID;
    long _revContainerEntityGUID;

    char *_timeCreated;
    char *_timeUpdated;

} RevObjectEntity;

#endif //REVCORE_REV_ENTITY_OBJECT_H
