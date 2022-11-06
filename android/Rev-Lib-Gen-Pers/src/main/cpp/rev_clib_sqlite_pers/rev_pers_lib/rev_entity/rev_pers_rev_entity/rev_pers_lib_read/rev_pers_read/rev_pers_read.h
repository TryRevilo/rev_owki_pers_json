//
// Created by rev on 11/6/18.
//

#ifndef REVCAMPANN_REV_PERS_READ_H
#define REVCAMPANN_REV_PERS_READ_H

#include "../../rev_db_models/rev_entity.h"

list *revPersGetALLRevEntityGUIDsByOwnerGUID(long ownerGUID);

list *revPersGetALLRevEntityGUIDsByOwnerGUID_Type(char *revEntityType, long ownerGUID);

list *revPersGetALLRevEntityGUIDs_By_ContainerGUID(long revEntityContainerGUID);

list *revPersGetALLRevEntityGUIDs_By_ContainerEntityGUID(long revContainerEntityGUID, char *revEntityType);

list *revPersGetALLRevEntityGUIDs_SQL_IN(char *sql_IN);

int revEntitySubtypeExists_BY_OWNER_GUID(int revEntityOwnerGUID, char *revEntitySubtype);

int revEntityExistsByLocalEntityGUID(long remoteRevEntityGUID);

int revEntityExistsByRemoteEntityGUID(long remoteRevEntityGUID);

long revEntitySubtypeExists_BY_CONTAINER_GUID(int revEntityContainerGUID, char *revEntitySubtype);

long revGetPublicationDate(long localRevEntityGUID);

long getRemoteRevEntityGUID(long localRevEntityGUID);

long getLocalRevEntityGUID_By_RemoteRevEntityGUID(long remoteRevEntityGUID);

RevEntity revPersGetRevEntityByGUID(long revEntityGUID);

RevEntity revPersGetRevEntity_By_RemoteRevEntityGUID(long remoteRevEntityGUID);

list *revPersGetALLEntitySubtypeGUIDsByOwnerGUID(char *revEntitySubtype, long ownerGUID);

list *revPersGetALLRevEntityGUIDs_By_ResStatus(int resolveStatus);

list *revPersGetALLRemoteRevEntityGUIDs_By_ResStatus(int resolveStatus);

list *revPersGetALLRevEntitySubtypeGUIDs_By_ResolveStatus(char *revEntitySubtype, int resolveStatus);

long getRevEntityGUID_By_RevEntityOwnerGUID_Subtype(int revEntityOwnerGUID, char *revEntitySubtype);

long getRevEntityGUIDByRevEntityContainerEntityGUID_Subtype(int revEntityContainerGUID, char *revEntitySubtype);

list *revPersGetALLRevEntityGUIDs_By_RevEntityType(char *revEntityType);

list *revPersGetALLRevEntityTYPE(char *revEntityType);

list *revPersGet_ALL_RevEntity_By_RevEntityContainerGUID_SubTYPE(long revEntityContainerGUID, char *revEntitySubType);

list *revPersGetALLRevEntity_By_SubType(char *revEntitySubType);

int getNumberOfUnreadRevEntites();

list *revPersGetALLRevEntityUnSyched();

list *revPersGetALLRevEntityUnSychedByType(char *revEntityType);

list *revPersGetALLRevEntitySubTYPEs(char *revEntitySubtype);

char *getRevEntityTypeByRevEntityGUID(int revEntityGUID);

char *getRevEntitySubtypeByRevEntityGUID(int revEntityGUID);

#endif //REVCAMPANN_REV_PERS_READ_H
