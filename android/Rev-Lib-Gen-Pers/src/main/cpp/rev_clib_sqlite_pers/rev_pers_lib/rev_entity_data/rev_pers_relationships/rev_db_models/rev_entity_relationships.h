//
// Created by rev on 7/9/18.
//

#ifndef REVCORE_REV_ENTITY_RELATIONSHIPS_H
#define REVCORE_REV_ENTITY_RELATIONSHIPS_H

typedef struct RevEntityRelationship {

    int _revResolveStatus;
    int _revEntityRelationshipTypeValueId;
    char *_revEntityRelationshipType;
    long _revEntityRelationshipId;
    long _remoteRevEntityRelationshipId;

    long _revEntitySubjectGUID;
    long _remoteRevEntitySubjectGUID;
    long _revEntityTargetGUID;
    long _remoteRevEntityTargetGUID;

    char *_timeCreated;
    char *_timeUpdated;

    long _revTimeCreated;
    long _revTimePublished;
    long _revTimePublishedUpdated;
} RevEntityRelationship;

#endif //REVCORE_REV_ENTITY_RELATIONSHIPS_H
