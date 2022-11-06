//
// Created by rev on 7/9/18.
//

#ifndef REVCORE_REV_ENTITY_METASTRINGS_H
#define REVCORE_REV_ENTITY_METASTRINGS_H

typedef struct {

    long _metastringId;
    char *_metastringValue;

    char *_timeCreated;
    char *_timeUpdated;

    long long _revTimeCreated;
    long _revTimePublished;
    long _revTimePublishedUpdated;
} RevEntityMetastring;

#endif //REVCORE_REV_ENTITY_METASTRINGS_H
