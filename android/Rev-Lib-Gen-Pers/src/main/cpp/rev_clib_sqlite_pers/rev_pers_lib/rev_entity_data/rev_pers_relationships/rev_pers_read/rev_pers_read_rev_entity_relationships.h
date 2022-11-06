//
// Created by rev on 8/2/18.
//

#ifndef REVCORE_REV_PERS_READ_REV_ENTITY_RELATIONSHIPS_H
#define REVCORE_REV_PERS_READ_REV_ENTITY_RELATIONSHIPS_H

#include "../../../rev_entity/rev_pers_rev_entity/rev_db_models/rev_entity.h"
#include "../rev_db_models/rev_entity_relationships.h"

long revGetLastRelSubjectGUID_By_CreatedDate_RelType(char *revRelType);

int revGetAnyRelExists_By_TypeValueId_LocalGUIDs(char *revRelType, long revSubjectGUID, long revTargetGuid);

int revGetAnyRelExists_By_TypeValueId_RemoteGUIDs(char *revRelType, long revRemoteSubjectGUID, long revRemoteTargetGuid);

int revGetAnyRelExists_By_ResStatus_TypeValueId_RemoteGUIDs(int revResStatus, char *revRelType, long revRemoteSubjectGUID, long revRemoteTargetGuid);

long getRevRelationshipTypeValueId(long revEntitySubjectGUID, long revEntityTargetGuid);

long getRevEntityRelationshipId_By_RelType_Subject_Target(char *revEntityRelationship, long remoteRevEntitySubjectGuid, long remoteRevEntityTargetGuid);

long getRevRelationshipTypeValueId_BY_RemoteTargetGUID(long revEntitySubjectGUID, long remoteRevEntityTargetGuid);

int revEntityRelationshipExists(char *revEntityRelationship, long revEntitySubjectGUID, long revEntityTargetGuid);

int revEntityRelationshipExists_BY_RemoteTargetGUID(char *revEntityRelationship, long revEntitySubjectGUID, long remoteRevEntityTargetGuid);

RevEntityRelationship revPersGetRevEntityRelById(long revEntityRelationshipId);

list *revPersGetRevEntityRels_By_ResStatus(int revResStatus);

list *revPersGetALLRevEntityRelValIds_By_RevResStatus(int revResolveStatus);

list *revPersGetALLRevEntityRelationshipsAcceptedUnSyched(long revEntityTargetGUID, int revRelResolveStatus);

long getRevRelationshipSubjectGUID_By_RelId(long relationshipId);

long getRevRelationshipTargetGUID_By_RelationshipId(long relationshipId);

long getRevRelationshipTargetGUIDByRelationshipValueId(long relationshipValueId);

list *revPersGetALLRevEntityRelationshipsByRelTypeValueId(long relTypeValueId);

list *revPersGetUnresolvedRemoteSubjectGUIDsRelIds();

list *revPersGetUnresolvedRemoteGUIDsRelId();

list *revPersGetRevEntityRelsSubjectsByRelTypeValueId(long relTypeValueId, long revEntitySubjectGUID);

list *revPersGetRevEntityRelsTargetsByRelTypeValueId(long relTypeValueId, long revEntityTargetGUID);

list *revPersGetRevEntityRelsSubjects_By_RelTypeValueId_TargetGUID_ResolveStatus(int relTypeValueId, long revEntityTargetGUID, int revEntityResolveStatus);

list *revPersGetRemoteRelsGUIDs_By_RelTypeValueId_RevEntityGUID_ResolveStatus(int relTypeValueId, long revEntityGUID, int revEntityResolveStatus);

list *revPersGetAllRevEntityRels_By_RelType_ValueId_ResolveStatus(int relTypeValueId, long revEntityGUID, int revResolveStatus);

list *revPersGetALLRevEntityRelationshipsSubjectGUID();

list *revPersGetALLRevRels_RemoteRelId_By_ResolveStatus(int revRelResolveStatus);

list *revPersGetALLRevRels_RemoteRelId_By_ResolveStatus_RemoteTargetGUID(int revRelResolveStatus, long remoteTargetGUID);

list *revPersGetALLRevEntityRelationshipsSubjectGUID_BY_TARGET_GUID(long targetGUID);

list *revPersGetALLRevEntityRelationshipsTargets(char *revEntityrelationship, long subjectGUID);

list *revPersGetALLRevEntityRelGUIDs_By_RelType_RemoteRevEntityGUID(char *revEntityrelationship, long remoteRevEntityGUID);

list *revPersGetALLRelSubjectGUIDs_By_TargetGUID(long revTargetGUID);

list *revPersGetALLRevEntityRelationshipsSubjects(char *revEntityrelationship, long subjectGUID);

list *revPersGetUnresolvedRemoteTargetGUIDSRelIds() ;

char *getRevRelationshipTimeCreated(long relationshipId);

#endif //REVCORE_REV_PERS_READ_REV_ENTITY_RELATIONSHIPS_H
