//
// Created by rev on 7/9/18.
//

#ifndef REVCORE_REV_ENTITY_METADATA_H
#define REVCORE_REV_ENTITY_METADATA_H

typedef struct RevEntityMetadata {

    int _resolveStatus;
    long _metadataId;
    long _remoteRevMetadataId;
    long _metadataOwnerGUID;
    long _metadataValueId;
    char *_metadataName;
    char *_metadataValue;

    char *_timeCreated;
    char *_timeUpdated;

    long long _revTimeCreated;
    long _revTimePublished;
    long _revTimePublishedUpdated;
} RevEntityMetadata;

#endif //REVCORE_REV_ENTITY_METADATA_H
