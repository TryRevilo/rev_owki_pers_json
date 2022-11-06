//
// Created by rev on 7/2/18.
//

#include <jni.h>

#include <malloc.h>
#include<string>
#include<vector>
#include <string.h>
#include <cstring>

#include <android/log.h>

#include "rev_pers_jni_structs.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_metadata/rev_db_models/rev_entity_metadata.h"
#include "rev_metadata_jni_loader.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_annotations/rev_db_models/rev_entity_annotation.h"
#include "rev_annotation_jni_loader.h"
#include <locale.h>

extern "C" {
#include "../cpp/rev_clib_sqlite_pers/rev_pers_libs_ext/libs_cust_rev/rev_list/rev_linked_list.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_group_entity/rev_db_models/rev_entity_group.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_group_entity/rev_pers_read/rev_pers_read_rev_group_entity.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_entity/rev_pers_lib_read/rev_pers_read/rev_pers_read.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_object_entity/rev_pers_lib_read/rev_pers_read/rev_pers_read_rev_object_entity.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_object_entity/rev_db_models/rev_entity_object.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_metadata/rev_pers_read/rev_pers_read_rev_entity_metadata.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_user_entity/rev_pers_lib_read/rev_pers_read/rev_pers_read_rev_user_entity.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_libs_ext/libs_cust_rev/rev_gen_functions.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_user_entity/rev_pers_lib_create/rev_pers_create/rev_pers_rev_user_entity.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_annotations/rev_pers_read/rev_pers_read_rev_entity_annotations.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_metastrings/rev_pers_read/rev_pers_read_rev_entity_metastrings.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_timeline/rev_pers_read/rev_pers_read_rev_timeline.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_relationships/rev_pers_read/rev_pers_read_rev_entity_relationships.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_user_entity/rev_db_models/rev_entity_user.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_relationships/rev_db_models/rev_entity_relationships.h"
#include "../cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_relationships/rev_pers_update/rev_pers_update_rev_entity_rel.h"
}

REV_ENTITY_JNI_POSREC *revEntityJniPosRec = NULL;

REV_ENTITY_RELATIONSHIP_JNI_POSREC *revEntityRelationshipJniPosrec = NULL;

std::vector<RevEntity *> searchRecordResultRevEntity;
std::vector<RevEntityMetadata> searchRecordResultRevEntityMetadata;
std::vector<RevEntityAnnotation> searchRecordResultRevEntityAnnotation;
std::vector<RevEntityRelationship> searchRecordResultRevEntityRelationship;
std::vector<long> searchRecordResultLong;

bool IsUTF8(const void* pBuffer, long size) {
	bool IsUTF8 = true;
	unsigned char* start = (unsigned char*)pBuffer;
	unsigned char* end = (unsigned char*)pBuffer + size;
	while (start < end)
	{
		if (*start < 0x80) // (10000000): value less then 0x80 ASCII char
		{
			start++;
		}
		else if (*start < (0xC0)) // (11000000): between 0x80 and 0xC0 UTF-8 char
		{
			IsUTF8 = false;
			break;
		}
		else if (*start < (0xE0)) // (11100000): 2 bytes UTF-8 char
		{
			if (start >= end - 1)
				break;
			if ((start[1] & (0xC0)) != 0x80)
			{
				IsUTF8 = false;
				break;
			}
			start += 2;
		}
		else if (*start < (0xF0)) // (11110000): 3 bytes UTF-8 char
		{
			if (start >= end - 2)
				break;
			if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
			{
				IsUTF8 = false;
				break;
			}
			start += 3;
		}
		else
		{
			IsUTF8 = false;
			break;
		}
	}
	return IsUTF8;
}

bool revPersGetRevEntityDataRevEntity(void *data) {
	searchRecordResultRevEntity.push_back((RevEntity *) data);
	return true;
}

bool revPersGetRevEntityMetadata(void *data) {
	searchRecordResultRevEntityMetadata.push_back(*(RevEntityMetadata *) data);
	return true;
}

bool revPersGetRevEntityAnnotation(void *data) {
	searchRecordResultRevEntityAnnotation.push_back(*(RevEntityAnnotation *) data);
	return true;
}

bool revPersGetRevEntityRelationship(void *data) {
	searchRecordResultRevEntityRelationship.push_back(*(RevEntityRelationship *) data);
	return true;
}

bool revPersGetRevEntityDataLong(void *data) {
	searchRecordResultLong.push_back(*(long *) data);
	return true;
}

/**
*   Fills JNI details.
*/
void LoadRevEntityJniPosRec(JNIEnv *env) {

	if (revEntityJniPosRec != NULL)
		return;

	revEntityJniPosRec = new REV_ENTITY_JNI_POSREC;

	revEntityJniPosRec->cls = env->FindClass("rev/ca/rev_gen_lib_pers/RevDBModels/RevEntity");

	if (revEntityJniPosRec->cls != NULL) {
		printf("sucessfully created class");
	}

	revEntityJniPosRec->constructortor_ID = env->GetMethodID(revEntityJniPosRec->cls, "<init>", "()V");
	if (revEntityJniPosRec->constructortor_ID != NULL) {
		printf("sucessfully created ctorID");
	}

	revEntityJniPosRec->_revEntityType_ID = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityType", "Ljava/lang/String;");
	revEntityJniPosRec->_revEntitySubType_ID = env->GetFieldID(revEntityJniPosRec->cls, "_revEntitySubType", "Ljava/lang/String;");

	revEntityJniPosRec->_revEntityResolveStatus = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityResolveStatus", "I");
	revEntityJniPosRec->_revEntityChildableStatus = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityChildableStatus", "I");

	revEntityJniPosRec->_revEntityGUID_ID = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityGUID", "Ljava/lang/Long;");
	revEntityJniPosRec->_remoteRevEntityGUID_ID = env->GetFieldID(revEntityJniPosRec->cls, "_remoteRevEntityGUID", "Ljava/lang/Long;");
	revEntityJniPosRec->_revEntityOwnerGUID_ID = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityOwnerGUID", "Ljava/lang/Long;");
	revEntityJniPosRec->_revEntityContainerGUID_ID = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityContainerGUID", "Ljava/lang/Long;");
	revEntityJniPosRec->_timeCreated_ID = env->GetFieldID(revEntityJniPosRec->cls, "_timeCreated", "Ljava/lang/String;");
	revEntityJniPosRec->_timeUpdated_ID = env->GetFieldID(revEntityJniPosRec->cls, "_timeUpdated", "Ljava/lang/String;");
	revEntityJniPosRec->_revEntityAccessPermission_ID = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityAccessPermission", "I");

	revEntityJniPosRec->_revEntityMetadataList_ID = env->GetFieldID(revEntityJniPosRec->cls, "_revEntityMetadataList", "Ljava/util/List;");

	revEntityJniPosRec->_revPublisherEntity = env->GetFieldID(revEntityJniPosRec->cls, "_revPublisherEntity", "Lrev/ca/rev_gen_lib_pers/RevDBModels/RevEntity;");

	revEntityJniPosRec->_revTimeCreated = env->GetFieldID(revEntityJniPosRec->cls, "_revTimeCreated", "J");
	revEntityJniPosRec->_revTimePublished = env->GetFieldID(revEntityJniPosRec->cls, "_revTimePublished", "J");
	revEntityJniPosRec->_revTimePublishedUpdated = env->GetFieldID(revEntityJniPosRec->cls, "_revTimePublishedUpdated", "J");
}

void LoadRevEntityRelationshipJniPosRec(JNIEnv *env) {
	if (revEntityRelationshipJniPosrec != NULL)
		return;

	revEntityRelationshipJniPosrec = new REV_ENTITY_RELATIONSHIP_JNI_POSREC;

	revEntityRelationshipJniPosrec->cls = env->FindClass("rev/ca/rev_gen_lib_pers/RevDBModels/RevEntityRelationship");

	if (revEntityRelationshipJniPosrec->cls != NULL) {
		printf("sucessfully created class");
	} else {
		__android_log_print(ANDROID_LOG_ERROR, "MyApp", "ERR : sucessfully created class");
	}

	revEntityRelationshipJniPosrec->constructortor_ID = env->GetMethodID(revEntityRelationshipJniPosrec->cls, "<init>", "()V");
	if (revEntityRelationshipJniPosrec->constructortor_ID != NULL) {
		printf("sucessfully created ctorID");
	} else {
		__android_log_print(ANDROID_LOG_ERROR, "MyApp", "ERR : sucessfully created ctorID");
	}

	revEntityRelationshipJniPosrec->_revResolveStatus = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_revResolveStatus", "I");
	revEntityRelationshipJniPosrec->_revEntityRelationshipType_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_revEntityRelationshipType", "Ljava/lang/String;");

	revEntityRelationshipJniPosrec->_revEntityRelationshipTypeValueId_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_revEntityRelationshipTypeValueId", "Ljava/lang/Long;");
	revEntityRelationshipJniPosrec->_revEntityRelationshipId_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_revEntityRelationshipId", "Ljava/lang/Long;");
	revEntityRelationshipJniPosrec->_remoteRevEntityRelationshipId = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_remoteRevEntityRelationshipId", "Ljava/lang/Long;");

	revEntityRelationshipJniPosrec->_revEntitySubjectGUID_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_revEntitySubjectGUID", "Ljava/lang/Long;");
	revEntityRelationshipJniPosrec->_remoteRevevEntitySubjectGUID_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_remoteRevEntitySubjectGUID", "Ljava/lang/Long;");

	revEntityRelationshipJniPosrec->_revEntityTargetGUID_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_revEntityTargetGUID", "Ljava/lang/Long;");
	revEntityRelationshipJniPosrec->_remoteRevEntityTargetGUID_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_remoteRevEntityTargetGUID", "Ljava/lang/Long;");

	revEntityRelationshipJniPosrec->_timeCreated_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_timeCreated", "Ljava/lang/String;");
	revEntityRelationshipJniPosrec->_timeUpdated_ID = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_timeUpdated", "Ljava/lang/String;");
    revEntityRelationshipJniPosrec->_revTimeCreated = env->GetFieldID(revEntityRelationshipJniPosrec->cls, "_revTimeCreated", "J");

}

void FillDataRecValuesToRevEntityRelationshipJni(JNIEnv *env, jobject jPosRec, RevEntityRelationship revEntityRelationship) {
	jint _revResolveStatus = revEntityRelationship._revResolveStatus;
	env->SetIntField(jPosRec, revEntityRelationshipJniPosrec->_revResolveStatus, _revResolveStatus);

	if ((revEntityRelationship._revEntityRelationshipType != NULL) && (revEntityRelationship._revEntityRelationshipType[0] != '\0')) {
		char *revEntityRelationshipType = revEntityRelationship._revEntityRelationshipType;
		jstring revEntityRelationshipType_Obj = env->NewStringUTF(revEntityRelationshipType);
		env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_revEntityRelationshipType_ID, revEntityRelationshipType_Obj);
		env->DeleteLocalRef(revEntityRelationshipType_Obj);
	}

	jclass clsLong = (env)->FindClass("java/lang/Long");
	jmethodID const_ethodId = env->GetMethodID(clsLong, "<init>", "(J)V");

	jobject _revEntityRelationshipId_Obj = env->NewObject(clsLong, const_ethodId, revEntityRelationship._revEntityRelationshipId);
	env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_revEntityRelationshipId_ID, _revEntityRelationshipId_Obj);
	env->DeleteLocalRef(_revEntityRelationshipId_Obj);

	jobject _remoteRevEntityRelationshipId_Obj = env->NewObject(clsLong, const_ethodId, revEntityRelationship._remoteRevEntityRelationshipId);
	env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_remoteRevEntityRelationshipId, _remoteRevEntityRelationshipId_Obj);
	env->DeleteLocalRef(_remoteRevEntityRelationshipId_Obj);

	jobject _revEntityRelationshipTypeValueId_Obj = env->NewObject(clsLong, const_ethodId, revEntityRelationship._revEntityRelationshipTypeValueId);
	env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_revEntityRelationshipTypeValueId_ID, _revEntityRelationshipTypeValueId_Obj);
	env->DeleteLocalRef(_revEntityRelationshipTypeValueId_Obj);

	jobject _revEntitySubjectGUID_Obj = env->NewObject(clsLong, const_ethodId, revEntityRelationship._revEntitySubjectGUID);
	env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_revEntitySubjectGUID_ID, _revEntitySubjectGUID_Obj);
	env->DeleteLocalRef(_revEntitySubjectGUID_Obj);

	/** _remoteRevEntitySubjectGUID **/
	jobject _remoteRevevEntitySubjectGUID_Obj = env->NewObject(clsLong, const_ethodId, revEntityRelationship._remoteRevEntitySubjectGUID);
	env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_remoteRevevEntitySubjectGUID_ID, _remoteRevevEntitySubjectGUID_Obj);
	env->DeleteLocalRef(_remoteRevevEntitySubjectGUID_Obj);

	jobject _revEntityTargetGUID_Obj = env->NewObject(clsLong, const_ethodId, revEntityRelationship._revEntityTargetGUID);
	env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_revEntityTargetGUID_ID, _revEntityTargetGUID_Obj);
	env->DeleteLocalRef(_revEntityTargetGUID_Obj);

    /** _remoteRevEntityTargetGUID **/
    jobject _remoteRevEntityTargetGUID_Obj = env->NewObject(clsLong, const_ethodId, revEntityRelationship._remoteRevEntityTargetGUID);
    env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_remoteRevEntityTargetGUID_ID, _remoteRevEntityTargetGUID_Obj);
    env->DeleteLocalRef(_remoteRevEntityTargetGUID_Obj);

    /** _revTimeCreated **/
    env->SetLongField(jPosRec, revEntityRelationshipJniPosrec->_revTimeCreated, revEntityRelationship._revTimeCreated);

    char *revTimeCreated = strdup(revEntityRelationship._timeCreated);

	if ((revTimeCreated != NULL) && (revTimeCreated[0] != '\0')) {
		if (IsUTF8(revTimeCreated,strlen(revTimeCreated)))
		{
			jstring timeCreated_Obj = env->NewStringUTF(revTimeCreated);
			env->SetObjectField(jPosRec, revEntityRelationshipJniPosrec->_timeCreated_ID, timeCreated_Obj);
			env->DeleteLocalRef(timeCreated_Obj);
		}
	}
}

void FillDataRecValuesToJni(JNIEnv *env, jobject jPosRec, RevEntity *cPosRec) {
	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	if (((cPosRec->_revEntityType)[0] == '\0') || !cPosRec->_revEntityType) {
		__android_log_print(ANDROID_LOG_ERROR, "MyApp", "cPosRec->_revEntityType NULL FILL");
		return;
	}

	const char *value = strdup(cPosRec->_revEntityType);

	if (!IsUTF8(value,strlen(value)))
	{
		__android_log_print(ANDROID_LOG_ERROR, "MyApp", "value >>> %s : getRevEntityByRevEntityGUID >>> - NO UTF : %ld", cPosRec->_revEntityGUID);
		return;
	}

	jint revEntityChildableStatus = cPosRec->_revEntityChildableStatus;
	env->SetIntField(jPosRec, revEntityJniPosRec->_revEntityChildableStatus, revEntityChildableStatus);

	jstring revEntityType_Obj = env->NewStringUTF(cPosRec->_revEntityType);
	env->SetObjectField(jPosRec, revEntityJniPosRec->_revEntityType_ID, revEntityType_Obj);
	env->DeleteLocalRef(revEntityType_Obj);

	jstring revEntitySubType_Obj = env->NewStringUTF(cPosRec->_revEntitySubType);
	env->SetObjectField(jPosRec, revEntityJniPosRec->_revEntitySubType_ID, revEntitySubType_Obj);
	env->DeleteLocalRef(revEntitySubType_Obj);

	jclass clsLong = (env)->FindClass("java/lang/Long");
	jmethodID const_longMethodId = env->GetMethodID(clsLong, "<init>", "(J)V");

	jobject revEntityGUID_Obj = env->NewObject(clsLong, const_longMethodId, (jlong) (cPosRec->_revEntityGUID));
	env->SetObjectField(jPosRec, revEntityJniPosRec->_revEntityGUID_ID, revEntityGUID_Obj);
	env->DeleteLocalRef(revEntityGUID_Obj);

	jobject remoteRevEntityGUID_Obj = env->NewObject(clsLong, const_longMethodId, (jlong) cPosRec->_remoteRevEntityGUID);
	env->SetObjectField(jPosRec, revEntityJniPosRec->_remoteRevEntityGUID_ID, remoteRevEntityGUID_Obj);
	env->DeleteLocalRef(remoteRevEntityGUID_Obj);

	jobject revOwnerEntityGUID_Obj = env->NewObject(clsLong, const_longMethodId, (jlong) cPosRec->_revOwnerEntityGUID);
	env->SetObjectField(jPosRec, revEntityJniPosRec->_revEntityOwnerGUID_ID, revOwnerEntityGUID_Obj);
	env->DeleteLocalRef(revOwnerEntityGUID_Obj);

	jobject revContainerEntityGUID_Obj = env->NewObject(clsLong, const_longMethodId, (jlong) cPosRec->_revContainerEntityGUID);
	env->SetObjectField(jPosRec, revEntityJniPosRec->_revEntityContainerGUID_ID, revContainerEntityGUID_Obj);
	env->DeleteLocalRef(revContainerEntityGUID_Obj);

	jstring timeCreated_Obj = env->NewStringUTF(cPosRec->_timeCreated);
	env->SetObjectField(jPosRec, revEntityJniPosRec->_timeCreated_ID, timeCreated_Obj);
	env->DeleteLocalRef(timeCreated_Obj);

	jint revEntityAccessPermission = cPosRec->_revEntityAccessPermission;
	env->SetIntField(jPosRec, revEntityJniPosRec->_revEntityAccessPermission_ID, revEntityAccessPermission);

	if (cPosRec->_revTimeCreated != NULL) {
		jlong _revTimeCreated = cPosRec->_revTimeCreated;
		env->SetLongField(jPosRec, revEntityJniPosRec->_revTimeCreated, _revTimeCreated);
	}

	jlong _revTimePublished = cPosRec->_revTimePublished;
	env->SetLongField(jPosRec, revEntityJniPosRec->_revTimePublished, _revTimePublished);

	jint _revTimePublishedUpdated = cPosRec->_revEntityChildableStatus;
	env->SetLongField(jPosRec, revEntityJniPosRec->_revTimePublishedUpdated, _revTimePublishedUpdated);

	list revEntityMetadataList_c = *(revPersGetALLRevEntityRevEntityMetadataByOwnerGUID(cPosRec->_revEntityGUID));

	if (list_size(&revEntityMetadataList_c) > 0) {
		revEntityMetadataJniPosRec = NULL;
		LoadRevEntityMetadataJniPosRec(env);

		list_for_each(&revEntityMetadataList_c, revPersGetRevEntityMetadata);

		// First, get all the methods we need:
		jclass arrayListClass = env->FindClass("java/util/ArrayList");
		jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
		jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

		// The list we're going to return:
		jobject revMetadataJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

		for (size_t i = 0; i < searchRecordResultRevEntityMetadata.size(); i++) {
			jobject jPosRec = env->NewObject(revEntityMetadataJniPosRec->cls, revEntityMetadataJniPosRec->constructortor_ID);
			FillDataRecValuesToRevMetadataJni(env, jPosRec, searchRecordResultRevEntityMetadata[i]);
			env->CallBooleanMethod(revMetadataJObjectArrayList, addMethod, jPosRec);
		}

		searchRecordResultRevEntityMetadata.clear();

		env->SetObjectField(jPosRec, revEntityJniPosRec->_revEntityMetadataList_ID, revMetadataJObjectArrayList);
	}

	/** LOAD REV ENTITY PUBLISHER

	long revEntityPublisherGUID = cPosRec->_revOwnerEntityGUID;

	if (revEntityPublisherGUID > 0) {
        RevEntity revEntityPublisher = revPersGetRevEntityByGUID(revEntityPublisherGUID);

        if (strcmp(revEntityPublisher._revEntityType, "rev_user_entity") == 0) {
            jobject jPosRecRevPublisherEntity = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
            FillDataRecValuesToJni(env, jPosRecRevPublisherEntity, &revEntityPublisher);

            env->SetObjectField(jPosRec, revEntityJniPosRec->_revPublisherEntity, jPosRecRevPublisherEntity);
            env->DeleteLocalRef(jPosRecRevPublisherEntity);
        }
	}
	  **/
}

/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/


/** REV  ENTITY **/


extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revEntityExistsByRemoteEntityGUID(JNIEnv *env, jobject instance, jlong remoteRevEntityGUID) {

	return revEntityExistsByRemoteEntityGUID((long) remoteRevEntityGUID);

}


extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revEntitySubtypeExists_1BY_1OWNER_1GUID(JNIEnv *env, jobject instance, jlong revEntityOwnerGUID, jstring revEntitySubtype_) {
	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);

	int exists = revEntitySubtypeExists_BY_OWNER_GUID((long) revEntityOwnerGUID, strdup(revEntitySubtype));

	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);

	return exists;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revEntitySubtypeExists_1BY_1CONTAINER_1GUID(JNIEnv *env, jobject instance, jlong revEntityContainerGUID, jstring revEntitySubtype_) {
	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);
	long exists = revEntitySubtypeExists_BY_CONTAINER_GUID((long) revEntityContainerGUID, strdup(revEntitySubtype));
	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);
	return exists;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetPublicationDate(JNIEnv *env, jobject instance, jlong localRevEntityGUID) {
	return revGetPublicationDate((long) localRevEntityGUID);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRemoteRevEntityGUID(JNIEnv *env, jobject instance, jlong localRevEntityGUID) {
	return getRemoteRevEntityGUID((long) localRevEntityGUID);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getLocalRevEntityGUID_1By_1RemoteRevEntityGUID(JNIEnv *env, jobject instance, jlong remoteRevEntityGUID) {
	return getLocalRevEntityGUID_By_RemoteRevEntityGUID((long) remoteRevEntityGUID);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityByGUID(JNIEnv *env, jobject instance, jlong revEntityGUID) {
	long c_revEntityGUID = (long) revEntityGUID;

	if (c_revEntityGUID < 1)
		return NULL;

	RevEntity revEntity = revPersGetRevEntityByGUID(c_revEntityGUID);

	if (revEntity._isNull == 0) {
		revEntityJniPosRec = NULL;
		LoadRevEntityJniPosRec(env);

		jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
		FillDataRecValuesToJni(env, jPosRec, &revEntity);

		return jPosRec;
	}

	return NULL;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntity_1By_1RemoteRevEntityGUID(JNIEnv *env, jobject instance, jlong remoteRevEntityGUID) {

	long c_remoteRevEntityGUID = (long) remoteRevEntityGUID;

	if (c_remoteRevEntityGUID < 1 || revEntityExistsByRemoteEntityGUID(c_remoteRevEntityGUID) == -1) return NULL;

	RevEntity revEntity = revPersGetRevEntity_By_RemoteRevEntityGUID(c_remoteRevEntityGUID);

	if (!revEntity._isNull) {
		revEntityJniPosRec = NULL;
		LoadRevEntityJniPosRec(env);

		jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
		FillDataRecValuesToJni(env, jPosRec, &revEntity);

		return jPosRec;
	}

	return NULL;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityName(JNIEnv *env, jobject instance, jlong revEntityGUID) {

	if (revEntityExistsByLocalEntityGUID((long) revEntityGUID) == -1) return env->NewStringUTF("Name is unset");

	char *revEntityType = getRevEntityTypeByRevEntityGUID((long) revEntityGUID);

	const char* value = revEntityType;
	if (!value) {
		return env->NewStringUTF("Name is unset");
	}
	if (!IsUTF8(value,strlen(value)))
	{
		return env->NewStringUTF("Name is unset");
	}

	if ((revEntityType != NULL) && (revEntityType[0] != '\0')) {
		if (strcmp(strdup(revEntityType), "rev_group_entity") == 0)
			return env->NewStringUTF(revPersGet_RevEntityGroupByGUID(revEntityGUID)._name);

		if (strcmp(strdup(revEntityType), "rev_object") == 0)
			return env->NewStringUTF(revPersGet_RevObjectEntityByGUID(revEntityGUID)._name);

		if (strcmp(strdup(revEntityType), "rev_user_entity") == 0)
			return env->NewStringUTF(revPersGet_RevRevUserEntityByRevEntityGUID(revEntityGUID)._full_names);
	}

	return env->NewStringUTF("Name is unset");

}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityTypeGUIDs(JNIEnv *env, jobject instance, jstring revEntityType_) {
	const char *revEntityType = env->GetStringUTFChars(revEntityType_, 0);

	list revEntityTypeList = *(revPersGetALLRevEntityGUIDs_By_RevEntityType(strdup(revEntityType)));

	list_for_each(&revEntityTypeList, revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityType_, revEntityType);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityTypes(JNIEnv *env, jobject instance, jstring revEntityType_) {
	const char *revEntityType = env->GetStringUTFChars(revEntityType_, 0);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list revEntityGUIDsList = *(revPersGetALLRevEntitySubTYPEs(strdup(revEntityType)));

	if (list_size(&revEntityGUIDsList) < 1)
		return revRetJObjectArrayList;

	list_for_each(&revEntityGUIDsList, revPersGetRevEntityDataLong);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityType_, revEntityType);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityGUIDs_1By_1ContainerGUID(JNIEnv *env, jobject instance, jlong revEntityContainerGUID) {
	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	list revEntityTypeList = *(revPersGetALLRevEntityGUIDs_By_ContainerGUID((long) revEntityContainerGUID));

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	if (list_size(&revEntityTypeList) < 1)
		return revRetJObjectArrayList;

	list_for_each(&revEntityTypeList, revPersGetRevEntityDataLong);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityGUIDs_1By_1RevEntityType(JNIEnv *env, jobject instance, jstring revEntityType_) {
	const char *revEntityType = env->GetStringUTFChars(revEntityType_, 0);

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	list revEntityTypeList = *(revPersGetALLRevEntityGUIDs_By_RevEntityType(strdup(revEntityType)));

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	if (list_size(&revEntityTypeList) < 1)
		return revRetJObjectArrayList;

	list_for_each(&revEntityTypeList, revPersGetRevEntityDataLong);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityType_, revEntityType);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityTYPE(JNIEnv *env, jobject instance, jstring revEntityType_) {

	const char *revEntityType = env->GetStringUTFChars(revEntityType_, 0);

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	list revEntityTypeList = *(revPersGetALLRevEntityTYPE(strdup(revEntityType)));

	jobjectArray jPosRecArray = env->NewObjectArray(searchRecordResultRevEntity.size(), revEntityJniPosRec->cls, NULL);

	if (list_size(&revEntityTypeList) < 1)
		return jPosRecArray;

	list_for_each(&revEntityTypeList, revPersGetRevEntityDataRevEntity);

	jPosRecArray = env->NewObjectArray(searchRecordResultRevEntity.size(), revEntityJniPosRec->cls, NULL);

	for (size_t i = 0; i < searchRecordResultRevEntity.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
		FillDataRecValuesToJni(env, jPosRec, searchRecordResultRevEntity[i]);
		env->SetObjectArrayElement(jPosRecArray, i, jPosRec);

		env->DeleteLocalRef(jPosRec);
	}

	env->ReleaseStringUTFChars(revEntityType_, revEntityType);

	searchRecordResultRevEntity.clear();

	return jPosRecArray;

}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntity_1By_1SubType(JNIEnv *env, jobject instance, jstring revEntitySubType_) {
	const char *revEntitySubType = env->GetStringUTFChars(revEntitySubType_, 0);

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	list revEntityTypeList = *(revPersGetALLRevEntity_By_SubType(strdup(revEntitySubType)));

	jobjectArray jPosRecArray = env->NewObjectArray(searchRecordResultRevEntity.size(), revEntityJniPosRec->cls, NULL);

	if (list_size(&revEntityTypeList) < 1)
		return jPosRecArray;

	list_for_each(&revEntityTypeList, revPersGetRevEntityDataRevEntity);

	jPosRecArray = env->NewObjectArray(searchRecordResultRevEntity.size(), revEntityJniPosRec->cls, NULL);

	for (size_t i = 0; i < searchRecordResultRevEntity.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
		FillDataRecValuesToJni(env, jPosRec, searchRecordResultRevEntity[i]);
		env->SetObjectArrayElement(jPosRecArray, i, jPosRec);

		env->DeleteLocalRef(jPosRec);
	}

	searchRecordResultRevEntity.clear();

	env->ReleaseStringUTFChars(revEntitySubType_, revEntitySubType);

	return jPosRecArray;
}

extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getNumberOfUnreadRevEntites(JNIEnv *env, jobject instance) {
	return getNumberOfUnreadRevEntites();
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGet_1ALL_1RevEntity_1By_1RevEntityContainerGUID_1SubTYPE(JNIEnv *env, jobject instance, jlong revEntityContainerGUID, jstring revEntitySubType_) {
	const char *revEntitySubType = env->GetStringUTFChars(revEntitySubType_, 0);

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);
	list_for_each(revPersGet_ALL_RevEntity_By_RevEntityContainerGUID_SubTYPE((long) revEntityContainerGUID, strdup(revEntitySubType)), revPersGetRevEntityDataRevEntity);

	jobjectArray jPosRecArray = env->NewObjectArray(searchRecordResultRevEntity.size(), revEntityJniPosRec->cls, NULL);

	for (size_t i = 0; i < searchRecordResultRevEntity.size(); i++) {
		if (searchRecordResultRevEntity[i] == NULL || searchRecordResultRevEntity[i] -> _revEntityType[0] == '\0') continue;

		jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
		FillDataRecValuesToJni(env, jPosRec, searchRecordResultRevEntity[i]);
		env->SetObjectArrayElement(jPosRecArray, i, jPosRec);
	}

	searchRecordResultRevEntity.clear();

	env->ReleaseStringUTFChars(revEntitySubType_, revEntitySubType);

	return jPosRecArray;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityUnSyched(JNIEnv *env, jobject instance) {
	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	list revList = *(revPersGetALLRevEntityUnSyched());
	list_for_each(&revList, revPersGetRevEntityDataRevEntity);

	jobjectArray jPosRecArray = env->NewObjectArray(searchRecordResultRevEntity.size(), revEntityJniPosRec->cls, NULL);

	for (size_t i = 0; i < searchRecordResultRevEntity.size(); i++) {
		if (searchRecordResultRevEntity[i] == NULL || searchRecordResultRevEntity[i] -> _revEntityType[0] == '\0') continue;

		jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
		FillDataRecValuesToJni(env, jPosRec, searchRecordResultRevEntity[i]);
		env->SetObjectArrayElement(jPosRecArray, i, jPosRec);
	}

	searchRecordResultRevEntity.clear();

	return jPosRecArray;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityUnSychedByType(JNIEnv *env, jobject instance, jstring revEntityType_) {
	const char *revEntityType = env->GetStringUTFChars(revEntityType_, 0);

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	list revEntitiesList = *(revPersGetALLRevEntityUnSychedByType(strdup(revEntityType)));
	list_for_each(&revEntitiesList, revPersGetRevEntityDataRevEntity);

	jobjectArray jPosRecArray = env->NewObjectArray(searchRecordResultRevEntity.size(), revEntityJniPosRec->cls, NULL);

	for (size_t i = 0; i < searchRecordResultRevEntity.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);
		FillDataRecValuesToJni(env, jPosRec, searchRecordResultRevEntity[i]);
		env->SetObjectArrayElement(jPosRecArray, i, jPosRec);

		env->DeleteLocalRef(jPosRec);
	}

	searchRecordResultRevEntity.clear();

	env->ReleaseStringUTFChars(revEntityType_, revEntityType);

	return jPosRecArray;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityGUIDsByOwnerGUID(JNIEnv *env, jobject instance, jlong revEntityOwnerGUID) {

	list_for_each(revPersGetALLRevEntityGUIDsByOwnerGUID((long) revEntityOwnerGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;

}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityGUIDs_1By_1ContainerEntityGUID_1RevEntityType(JNIEnv *env, jobject instance, jlong revContainerEntityGUID,
																																 jstring revEntityType_) {
	const char *revEntityType = env->GetStringUTFChars(revEntityType_, 0);

	list_for_each(revPersGetALLRevEntityGUIDs_By_ContainerEntityGUID((long) revContainerEntityGUID, strdup(revEntityType)), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityType_, revEntityType);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityGUIDs_1By_1OwnerGUID_1RevEntityType(JNIEnv *env, jobject instance, jlong revEntityOwnerGUID,
																													   jstring revEntityType_) {
	const char *revEntityType = env->GetStringUTFChars(revEntityType_, 0);

	list_for_each(revPersGetALLRevEntityGUIDsByOwnerGUID_Type(strdup(revEntityType), (long) revEntityOwnerGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityType_, revEntityType);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLREntitySubtypeByOwnerGUID(JNIEnv *env, jobject instance, jstring revEntitySubtype_, jlong revEntityOwnerGUID) {
	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);

	list_for_each(revPersGetALLEntitySubtypeGUIDsByOwnerGUID(strdup(revEntitySubtype), (long) revEntityOwnerGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityGUIDs_1By_1ResStatus(JNIEnv *env, jobject instance, jint resolveStatus) {
	list revList = *(revPersGetALLRevEntityGUIDs_By_ResStatus((int) resolveStatus));
	list_for_each(&revList, revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRemoteRevEntityGUIDs_1By_1ResStatus(JNIEnv *env, jobject instance, jint resolveStatus) {
	list revList = *(revPersGetALLRemoteRevEntityGUIDs_By_ResStatus((int) resolveStatus));
	list_for_each(&revList, revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntitySubtypeGUIDs_1By_1ResolveStatus(JNIEnv *env, jobject instance, jstring revEntitySubtype_, jint resolveStatus) {
	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);

	list_for_each(revPersGetALLRevEntitySubtypeGUIDs_By_ResolveStatus(strdup(revEntitySubtype), (int) resolveStatus), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);

	return revRetJObjectArrayList;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityGUID_1By_1RevEntityOwnerGUID_1Subtype(JNIEnv *env, jobject instance, jlong revEntityOwnerGUID, jstring revEntitySubtype_) {
	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);

	long revEntityGUID = getRevEntityGUID_By_RevEntityOwnerGUID_Subtype((long) revEntityOwnerGUID, strdup(revEntitySubtype));

	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);

	return revEntityGUID;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityByRevEntityOwnerGUID_1Subtype(JNIEnv *env, jobject instance, jlong revEntityOwnerGUID, jstring revEntitySubtype_) {
	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);

	long revEntityGUID = getRevEntityGUID_By_RevEntityOwnerGUID_Subtype((long) revEntityOwnerGUID, strdup(revEntitySubtype));

	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);
	jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);

	if (revEntityGUID > 0) {
		RevEntity revEntity = revPersGetRevEntityByGUID(revEntityGUID);

		if (revEntity._revEntityType[0] != '\0') {
			FillDataRecValuesToJni(env, jPosRec, &revEntity);
			return jPosRec;
		}
	}

	return NULL;
}

#include <chrono>
#include <thread>


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntity_1By_1RevEntityContainerGUID_1Subtype(JNIEnv *env, jobject instance, jlong revEntityContainerGUID, jstring revEntitySubtype_) {

	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);

	long revEntityGUID = getRevEntityGUIDByRevEntityContainerEntityGUID_Subtype((long) revEntityContainerGUID, strdup(revEntitySubtype));

	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);
	jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);

	if (revEntityGUID > 0) {
		RevEntity revEntity = revPersGetRevEntityByGUID(revEntityGUID);

		if (revEntity._revEntityType[0] != '\0') {
			FillDataRecValuesToJni(env, jPosRec, &revEntity);
			return jPosRec;
		}
	}

	return NULL;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityGUIDByRevEntityContainerGUID_1Subtype(JNIEnv *env, jobject instance, jlong revEntityContainerGUID, jstring revEntitySubtype_) {
	const char *revEntitySubtype = env->GetStringUTFChars(revEntitySubtype_, 0);

	long revEntityGUID = getRevEntityGUIDByRevEntityContainerEntityGUID_Subtype((long) revEntityContainerGUID, strdup(revEntitySubtype));

	env->ReleaseStringUTFChars(revEntitySubtype_, revEntitySubtype);

	return revEntityGUID;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityTypeByRevEntityGUID(JNIEnv *env, jobject instance, jlong revEntityGUID) {
	return env->NewStringUTF(getRevEntityTypeByRevEntityGUID(revEntityGUID));
}

extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntitySubtypeByRevEntityGUID(JNIEnv *env, jobject instance, jlong revEntityGUID) {
	return env->NewStringUTF(getRevEntitySubtypeByRevEntityGUID((long) revEntityGUID));
}


/** END REV  ENTITY **/


/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/



/**  REV  USER ENTITY **/

extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_totalLocalRevUserEntites(JNIEnv *env, jobject instance) {
	return totalLocalRevUserEntites();

}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevUserEntityByEmail_1Phone(JNIEnv *env, jobject instance, jstring revEmailPhone_) {

	revEntityJniPosRec = NULL;
	LoadRevEntityJniPosRec(env);

	const char *revEmailPhone = env->GetStringUTFChars(revEmailPhone_, 0);

	RevUserEntity revUserEntity = revPersGet_RevRevUserEntityByEmail_Phone(strdup(revEmailPhone));

	env->ReleaseStringUTFChars(revEmailPhone_, revEmailPhone);

	jobject jPosRec = env->NewObject(revEntityJniPosRec->cls, revEntityJniPosRec->constructortor_ID);

	if ((revUserEntity._email[0] != '\0') && strcmp(revUserEntity._email, "NULL") != 0) {
		RevEntity revEntity = revPersGetRevEntityByGUID(revUserEntity._revEntityGUID);

		if (revEntity._revEntityType[0] != '\0') {
			FillDataRecValuesToJni(env, jPosRec, &revEntity);
			return jPosRec;
		}
	}

	return NULL;
}


/** END REV USER ENTITY **/


/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/


/** REV ENTITY RELATIONSHIPS **/


extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetLastRelSubjectGUID_1By_1CreatedDate_1RelType(JNIEnv *env, jobject instance, jstring revRelType_) {
	const char *revRelType = env->GetStringUTFChars(revRelType_, 0);

	long revLastRelDate = revGetLastRelSubjectGUID_By_CreatedDate_RelType(strdup(revRelType));

	env->ReleaseStringUTFChars(revRelType_, revRelType);

	return revLastRelDate;
}


extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revEntityRelationshipExists(JNIEnv *env, jobject instance, jstring revEntityRelationship_, jlong revEntitySubjectGUID, jlong revEntityTargetGuid) {
	const char *revEntityRelationship = env->GetStringUTFChars(revEntityRelationship_, 0);

	int i = revEntityRelationshipExists(strdup(revEntityRelationship), (long) revEntitySubjectGUID, (long) revEntityTargetGuid);

	env->ReleaseStringUTFChars(revEntityRelationship_, revEntityRelationship);

	return i;
}


extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revEntityRelationshipExists_1BY_1RemoteTargetGUID(JNIEnv *env, jobject instance, jstring revEntityRelationship_, jlong revEntitySubjectGUID,
																											   jlong remoteRevEntityTargetGuid) {
	const char *revEntityRelationship = env->GetStringUTFChars(revEntityRelationship_, 0);

	int i = revEntityRelationshipExists_BY_RemoteTargetGUID(strdup(revEntityRelationship), (long) revEntitySubjectGUID, (long) remoteRevEntityTargetGuid);

	env->ReleaseStringUTFChars(revEntityRelationship_, revEntityRelationship);

	return i;

}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityRels_1By_1ResStatus(JNIEnv *env, jobject instance, jint revResStatus) {

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list revItemsList_C = *(revPersGetRevEntityRels_By_ResStatus((int) revResStatus));
	int revItemsListSize = list_size(&revItemsList_C);

	if (revItemsListSize == 0) return revRetJObjectArrayList;

	list_for_each(&revItemsList_C, revPersGetRevEntityRelationship);

	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		if (revEntityExistsByLocalEntityGUID(searchRecordResultRevEntityRelationship[i]._revEntitySubjectGUID) == -1 || revEntityExistsByLocalEntityGUID(searchRecordResultRevEntityRelationship[i]._revEntityTargetGUID) == -1) {
			revPersUpdateRelResStatus_By_RelId(searchRecordResultRevEntityRelationship[i]._revEntityRelationshipId, -3);
			continue;
		}

		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRelationshipsAcceptedUnSyched(JNIEnv *env, jobject instance, jlong revEntityTargetGUID, jint revRelResolveStatus) {

	list *revItemsList_C = revPersGetALLRevEntityRelationshipsAcceptedUnSyched((long) revEntityTargetGUID, (int) revRelResolveStatus);
	int revItemsListSize = list_size(revItemsList_C);

	if (revItemsListSize == 0) return NULL;

	list_for_each(revItemsList_C, revPersGetRevEntityRelationship);

	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRevEntityRelationshipsByRelTypeValueId(JNIEnv *env, jobject instance, jlong relTypeValueId) {

	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	list_for_each(revPersGetALLRevEntityRelationshipsByRelTypeValueId((long) relTypeValueId), revPersGetRevEntityRelationship);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityRelsSubjectsByRelTypeValueId(JNIEnv *env, jobject instance, jlong relTypeValueId, jlong revEntitySubjectGUID) {

	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject retJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list *revEntitySubjectsList = revPersGetRevEntityRelsSubjectsByRelTypeValueId((long) relTypeValueId, (long) revEntitySubjectGUID);

	if (list_size(revEntitySubjectsList) < 1)
		return retJObjectArrayList;

	list_for_each(revEntitySubjectsList, revPersGetRevEntityRelationship);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(retJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return retJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityRelsSubjects_1By_1RelTypeValueId_1TargetGUID_1ResolveStatus(JNIEnv *env, jobject instance, jint relTypeValueId, jlong revEntityTargetGUID, jint revEntityResolveStatus) {
	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list relTargetsList = *(revPersGetRevEntityRelsSubjects_By_RelTypeValueId_TargetGUID_ResolveStatus((int) relTypeValueId, (long) revEntityTargetGUID, (int) revEntityResolveStatus));

	if (list_size(&relTargetsList) < 1)
		return revRetJObjectArrayList;

	list_for_each(&relTargetsList, revPersGetRevEntityRelationship);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		if (searchRecordResultRevEntityRelationship[i]._revEntityRelationshipId < 1) continue;

		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return revRetJObjectArrayList;
}

#include <map>

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRemoteRelsGUIDs_1By_1RelTypeValueId_1RevEntityGUID_1ResolveStatus(JNIEnv *env, jobject instance, jint relTypeValueId, jlong revEntityGUID, jint revEntityResolveStatus) {
	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetContainerJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list revRelsList = *(revPersGetRemoteRelsGUIDs_By_RelTypeValueId_RevEntityGUID_ResolveStatus((int) relTypeValueId, (long) revEntityGUID, (int) revEntityResolveStatus));
	list_for_each(&revRelsList, revPersGetRevEntityRelationship);

	jclass cls = env->FindClass("java/lang/Long");
	jmethodID longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		jobject revRemoteSubjectGUID_OBJ = env->NewObject(cls, longConstructor, searchRecordResultRevEntityRelationship[i]._remoteRevEntitySubjectGUID);
		jobject revRemoteTargetGUID_OBJ = env->NewObject(cls, longConstructor, searchRecordResultRevEntityRelationship[i]._remoteRevEntityTargetGUID);

		// The list we're going to return:
		jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, revRemoteSubjectGUID_OBJ);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, revRemoteTargetGUID_OBJ);

		env->CallBooleanMethod(revRetContainerJObjectArrayList, addMethod, revRetJObjectArrayList);
	}

	return revRetContainerJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetAllRevEntityRels_1By_1RelType_1ValueId_1ResolveStatus(JNIEnv *env, jobject instance, jint relTypeValueId, jlong revEntityGUID,
																															 jint revResolveStatus) {

	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject retJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list *revEntityRelsList = revPersGetAllRevEntityRels_By_RelType_ValueId_ResolveStatus((int) relTypeValueId, (long) revEntityGUID, (int) revResolveStatus);

	if (list_size(revEntityRelsList) < 1)
		return retJObjectArrayList;

	list_for_each(revEntityRelsList, revPersGetRevEntityRelationship);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(retJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return retJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityRelsTargetsByRelTypeValueId(JNIEnv *env, jobject instance, jlong relTypeValueId, jlong revEntityTargetGUID) {

	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	list_for_each(revPersGetRevEntityRelsTargetsByRelTypeValueId((long) relTypeValueId, (long) revEntityTargetGUID), revPersGetRevEntityRelationship);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevRelationshipSubjectGUID_1By_1RelId(JNIEnv *env, jobject instance, jlong relationshipId) {

	return getRevRelationshipSubjectGUID_By_RelId((long) relationshipId);

}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevRelationshipTargetGUID_1By_1RelationshipId(JNIEnv *env, jobject instance, jlong relationshipId) {

	return getRevRelationshipTargetGUID_By_RelationshipId((long) relationshipId);

}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityRelationshipTargetGUIDByRelationshipValueId(JNIEnv *env, jobject instance, jlong relationshipValueId) {

	return getRevRelationshipTargetGUIDByRelationshipValueId((long) relationshipValueId);

}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityRelationshipId_1By_1RelType_1Subject_1Target(JNIEnv *env, jobject instance, jstring revEntityRelationship_, jlong remoteRevEntitySubjectGuid, jlong remoteRevEntityTargetGuid) {

	const char *revEntityRelationship = env->GetStringUTFChars(revEntityRelationship_, 0);
	long revRelId = getRevEntityRelationshipId_By_RelType_Subject_Target(strdup(revEntityRelationship), (long) remoteRevEntitySubjectGuid, (long) remoteRevEntityTargetGuid);
	env->ReleaseStringUTFChars(revEntityRelationship_, revEntityRelationship);

	return revRelId;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRelationshipsTargets(JNIEnv *env, jobject instance, jstring revEntityRelationship_, jlong revEntitySubjectGUID) {

	const char *revEntityRelationship = env->GetStringUTFChars(revEntityRelationship_, 0);

	list_for_each(revPersGetALLRevEntityRelationshipsTargets(strdup(revEntityRelationship), (long) revEntitySubjectGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityRelationship_, revEntityRelationship);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRelGUIDs_1By_1RelType_1RemoteRevEntityGUID(JNIEnv *env, jobject instance, jstring revEntityrelationship_, jlong revEntityGUID) {

	const char *revEntityrelationship = env->GetStringUTFChars(revEntityrelationship_, 0);

	list_for_each(revPersGetALLRevEntityRelGUIDs_By_RelType_RemoteRevEntityGUID(
			strdup(revEntityrelationship), (long) revEntityGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		if (searchRecordResultLong[i] == (long) revEntityGUID) continue;

		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityrelationship_, revEntityrelationship);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevRels_1RemoteRelId_1By_1ResolveStatus(JNIEnv *env, jobject instance, jint revRelResolveStatus) {

	list revRemoteRelsIdsList = *(revPersGetALLRevRels_RemoteRelId_By_ResolveStatus((int) revRelResolveStatus));
	list_for_each(&revRemoteRelsIdsList, revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevRels_1RemoteRelId_1By_1ResolveStatus_1RemoteTargetGUID(
		JNIEnv *env, jobject instance, jint revRelResolveStatus, jlong remoteTargetGUID) {

	list_for_each(revPersGetALLRevRels_RemoteRelId_By_ResolveStatus_RemoteTargetGUID((int) revRelResolveStatus, (long) remoteTargetGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;

}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRelValIds_1By_1RevResStatus(JNIEnv *env, jobject instance, jint revResStatus) {

	list_for_each(revPersGetALLRevEntityRelValIds_By_RevResStatus((int) revResStatus), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetUnresolvedRemoteSubjectGUIDSRelIds(JNIEnv *env, jobject instance) {

	list_for_each(revPersGetUnresolvedRemoteSubjectGUIDsRelIds(), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetUnresolvedRemoteTargetGUIDSRelIds(JNIEnv *env, jobject instance) {

	list_for_each(revPersGetUnresolvedRemoteTargetGUIDSRelIds(), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRelSubjectGUIDs_1By_1TargetGUID(JNIEnv *env, jobject instance, jlong revEntityTargetGUID) {

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list revEntityGUIDsList = *(revPersGetALLRelSubjectGUIDs_By_TargetGUID((long) revEntityTargetGUID));

	if (list_size(&revEntityGUIDsList) < 1)
		return revRetJObjectArrayList;

	list_for_each(&revEntityGUIDsList, revPersGetRevEntityDataLong);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRelationshipsSubjects(JNIEnv *env, jobject instance, jstring revEntityRelationship_, jlong revEntityTargetGUID) {
	const char *revEntityRelationship = env->GetStringUTFChars(revEntityRelationship_, 0);

	list_for_each(revPersGetALLRevEntityRelationshipsSubjects(strdup(revEntityRelationship), (long) revEntityTargetGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
		env->DeleteLocalRef(obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revEntityRelationship_, revEntityRelationship);

	return revRetJObjectArrayList;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRelationshipsSubjectGUID_1BY_1TARGET_1GUID(
		JNIEnv *env, jobject instance, jlong targetGUID) {

	list_for_each(revPersGetALLRevEntityRelationshipsSubjectGUID_BY_TARGET_GUID((long) targetGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetUnresolvedRemoteGUIDsRelId(JNIEnv *env, jobject instance) {

	revEntityRelationshipJniPosrec = NULL;
	LoadRevEntityRelationshipJniPosRec(env);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject retJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list *revEntityRelsList = revPersGetUnresolvedRemoteGUIDsRelId();

	if (list_size(revEntityRelsList) < 1)
		return retJObjectArrayList;

	list_for_each(revEntityRelsList, revPersGetRevEntityRelationship);

	for (int i = 0; i < searchRecordResultRevEntityRelationship.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityRelationshipJniPosrec->cls, revEntityRelationshipJniPosrec->constructortor_ID);
		FillDataRecValuesToRevEntityRelationshipJni(env, jPosRec, searchRecordResultRevEntityRelationship[i]);
		env->CallBooleanMethod(retJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityRelationship.clear();

	return retJObjectArrayList;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRelationshipsSubjectGUID(JNIEnv *env, jobject instance) {

	list_for_each(revPersGetALLRevEntityRelationshipsSubjectGUID(), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevRelationshipTimeCreated(JNIEnv *env, jobject instance, jlong relationshipId) {

	char *timeCreated = getRevRelationshipTimeCreated((long) relationshipId);

	return env->NewStringUTF(timeCreated);
}


/** END REV ENTITY RELATIONSHIPS **/


/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/


/** REV ENTITY METADATA **/


extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getMetadataValueId_1By_1MetadataId(JNIEnv *env, jobject instance, jlong metadataId) {

	return getMetadataValueId_By_MetadataId((long) metadataId);

}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetRevEntityMetadataId_1By_1RevMetadataName_1RevEntityGUID(JNIEnv *env, jobject instance, jstring revMetadataName_, jlong revEntityGUID) {
	const char *revMetadataName = env->GetStringUTFChars(revMetadataName_, 0);

	long revMetadataId = revGetRevEntityMetadataId_By_RevMetadataName_RevEntityGUID(strdup(revMetadataName), (long) revEntityGUID);

	env->ReleaseStringUTFChars(revMetadataName_, revMetadataName);

	return revMetadataId;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetRevEntityMetadataValue_1By_1RevMetadataName_1RevEntityGUID(JNIEnv *env, jobject instance, jstring revMetadataName_, jlong revEntityGUID) {
	const char *revMetadataName = env->GetStringUTFChars(revMetadataName_, 0);

	char *revMetadataValue = revGetRevEntityMetadataValue_By_RevMetadataName_RevEntityGUID(strdup(revMetadataName), (long) revEntityGUID);
	env->ReleaseStringUTFChars(revMetadataName_, revMetadataName);

	return env->NewStringUTF(strdup(revMetadataValue));

}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityMetadataByRevEntityGUID(JNIEnv *env, jobject instance, jlong revEntityGUID) {

	revEntityMetadataJniPosRec = NULL;
	LoadRevEntityMetadataJniPosRec(env);

	list revMetadataList = *(revPersGetALLRevEntityRevEntityMetadataByOwnerGUID((long) revEntityGUID));

	list_for_each(&revMetadataList, revPersGetRevEntityMetadata);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultRevEntityMetadata.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityMetadataJniPosRec->cls, revEntityMetadataJniPosRec->constructortor_ID);
		FillDataRecValuesToRevMetadataJni(env, jPosRec, searchRecordResultRevEntityMetadata[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityMetadata.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityMetadataByResolveStatus(JNIEnv *env, jobject instance, jint resolveStatus) {

	revEntityMetadataJniPosRec = LoadRevEntityMetadataJniPosRec(env);

	list_for_each(revPersGetALLRevEntityMetadataByResolveStatus((int) resolveStatus), revPersGetRevEntityMetadata);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultRevEntityMetadata.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityMetadataJniPosRec->cls, revEntityMetadataJniPosRec->constructortor_ID);
		FillDataRecValuesToRevMetadataJni(env, jPosRec, searchRecordResultRevEntityMetadata[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityMetadata.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityMetadataId_1By_1metadataName_1ResolveStatus(JNIEnv *env, jobject instance, jstring metadataName_,
																															   jint resolveStatus) {
	const char *metadataName = env->GetStringUTFChars(metadataName_, 0);

	list_for_each(revPersGetALLRevEntityMetadataId_By_metadataName_ResolveStatus(strdup(metadataName), (int) resolveStatus), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// For Long
	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(metadataName_, metadataName);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityMetadataIds_1By_1ResStatus(JNIEnv *env, jobject instance, jint resolveStatus) {
	list_for_each(revPersGetALLRevEntityMetadataIds_By_ResStatus((int) resolveStatus), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// For Long
	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityMetadataIds_1By_1ResStatus_1RevEntityGUID(JNIEnv *env, jobject instance, jint resolveStatus, jlong revEntityGUID) {
	list_for_each(revPersGetALLRevEntityMetadataIds_By_ResStatus_RevEntityGUID((int) resolveStatus, (long) revEntityGUID), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// For Long
	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityMetadataUnsynched(JNIEnv *env, jobject instance) {

	revEntityMetadataJniPosRec = NULL;
	LoadRevEntityMetadataJniPosRec(env);

	list revList_c = *(revPersGetALLRevEntityMetadataUnsynched());

	list_for_each(&revList_c, revPersGetRevEntityMetadata);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultRevEntityMetadata.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityMetadataJniPosRec->cls, revEntityMetadataJniPosRec->constructortor_ID);
		FillDataRecValuesToRevMetadataJni(env, jPosRec, searchRecordResultRevEntityMetadata[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityMetadata.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityMetadata_1By_1MetadataId(JNIEnv *env, jobject instance, jlong revMetadataId) {
	revEntityMetadataJniPosRec = NULL;
	LoadRevEntityMetadataJniPosRec(env);

	RevEntityMetadata revEntityMetadata = *(revPersGetRevEntityMetadata_By_MetadataId((long long) revMetadataId));

	if (revEntityMetadata._metadataId > 0) {
		jobject jPosRec = env->NewObject(revEntityMetadataJniPosRec->cls, revEntityMetadataJniPosRec->constructortor_ID);
		FillDataRecValuesToRevMetadataJni(env, jPosRec, revEntityMetadata);

		return jPosRec;
	}

	return NULL;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityMetadataUnsynched_1By_1RevEntityGUID(JNIEnv *env, jobject instance, jlong revEntityGUID) {
	revEntityMetadataJniPosRec = NULL;
	LoadRevEntityMetadataJniPosRec(env);

	list revUnsynchedMetadata = *(revPersGetALLRevEntityMetadataUnsynched_By_RevEntityGUID((long) revEntityGUID));

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	if (list_size(&revUnsynchedMetadata) > 0) {
		list_for_each(&revUnsynchedMetadata, revPersGetRevEntityMetadata);

		for (size_t i = 0; i < searchRecordResultRevEntityMetadata.size(); i++) {
			jobject jPosRec = env->NewObject(revEntityMetadataJniPosRec->cls, revEntityMetadataJniPosRec->constructortor_ID);
			FillDataRecValuesToRevMetadataJni(env, jPosRec, searchRecordResultRevEntityMetadata[i]);
			env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
		}

		searchRecordResultRevEntityMetadata.clear();
	}

	return revRetJObjectArrayList;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRevEntityMetadataBy_1MetadataName_1OwnerGUID(JNIEnv *env, jobject instance, jlong revEntityGUID,
																																jstring metadataName_) {
	const char *metadataName = env->GetStringUTFChars(metadataName_, 0);

	revEntityMetadataJniPosRec = NULL;
	LoadRevEntityMetadataJniPosRec(env);

	list_for_each(revPersGetALLRevEntityRevEntityMetadataBy_MetadataName_OwnerGUID(strdup(metadataName), (long) revEntityGUID), revPersGetRevEntityMetadata);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultRevEntityMetadata.size(); i++) {
		jobject jPosRec = env->NewObject(revEntityMetadataJniPosRec->cls, revEntityMetadataJniPosRec->constructortor_ID);
		FillDataRecValuesToRevMetadataJni(env, jPosRec, searchRecordResultRevEntityMetadata[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, jPosRec);
	}

	searchRecordResultRevEntityMetadata.clear();

	env->ReleaseStringUTFChars(metadataName_, metadataName);

	return revRetJObjectArrayList;
}


/** END REV ENTITY METADATA **/


/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/


/** REV ENTITY METASTRINGS **/

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityMetaStringValueId(JNIEnv *env, jobject instance, jstring metastringValue_) {
	const char *metastringValue = env->GetStringUTFChars(metastringValue_, 0);

	long revMetastringValueId = getRevEntityMetaStringValueId(strdup(metastringValue));

	env->ReleaseStringUTFChars(metastringValue_, metastringValue);

	return revMetastringValueId;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityRevEntityMetastringIdByValue(JNIEnv *env, jobject instance, jstring metastringValue_) {

	const char *metastringValue = env->GetStringUTFChars(metastringValue_, 0);

	list_for_each(revPersGetALLRevEntityRevEntityMetastringIdByValue(strdup(metastringValue)), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
		//
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(metastringValue_, metastringValue);

	return revRetJObjectArrayList;
}


/** END REV ENTITY METASTRINGS **/


/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/


/** REV ENTITY ANOTATIONS **/

extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revEntityAnnotationExists(JNIEnv *env, jobject instance, jstring revAnnotationName_, jlong revEntityGUID, jlong ownerEntityGUID) {

	const char *revAnnotationName = env->GetStringUTFChars(revAnnotationName_, 0);

	int annotationValueId = revEntityAnnotationExists_ByOwnerEntityGUID(strdup(revAnnotationName), (long) revEntityGUID, (long) ownerEntityGUID);

	env->ReleaseStringUTFChars(revAnnotationName_, revAnnotationName);

	return annotationValueId;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityAnnoationValueIdBy_1revAnnotationName_1RevEntityGUID_1RevEntityOwnerGUID(JNIEnv *env, jobject instance,
																																				  jstring revAnnotationName_, jlong revEntityGUID,
																																				  jlong revEntityOwnerGUID) {
	const char *revAnnotationName = env->GetStringUTFChars(revAnnotationName_, 0);

	long annotationValueId = getRevAnnotationValueIdByOwnerEntityGUID(strdup(revAnnotationName), (long) revEntityGUID, (long) revEntityOwnerGUID);

	env->ReleaseStringUTFChars(revAnnotationName_, revAnnotationName);

	return annotationValueId;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityAnnoationValueBy_1revAnnotationName_1RevEntityGUID_1RevEntityOwnerGUID(JNIEnv *env, jobject instance,
																																				jstring revAnnotationName_,
																																				jlong revEntityGUID, jlong revEntityOwnerGUID) {
	const char *revAnnotationName = env->GetStringUTFChars(revAnnotationName_, 0);

	char *revEntityAnnotationValue = getRevEntityAnnotationValue(strdup(revAnnotationName), (long) revEntityGUID, (long) revEntityOwnerGUID);

	env->ReleaseStringUTFChars(revAnnotationName_, revAnnotationName);

	return env->NewStringUTF(revEntityAnnotationValue);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getAllRevEntityAnnoationValueByRevEntityContainerGUID_1Subtype(JNIEnv *env, jobject instance, jstring revAnnotationName_,
																															jlong revEntityGUID) {
	const char *revAnnotationName = env->GetStringUTFChars(revAnnotationName_, 0);

	list *valueIds = getAllRevEntityAnnotationId(strdup(revAnnotationName), (long) revEntityGUID);

	list_for_each(valueIds, revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		char *value = getRevEntityMetaStringById((jlong) searchRecordResultLong[i]);

		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, env->NewStringUTF(value));
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revAnnotationName_, revAnnotationName);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getAllRevEntityAnnoationIds_1By_1RevEntityGUID(JNIEnv *env, jobject instance, jlong revEntityGUID) {

	list *valueIds = getAllRevEntityAnnoationIds_By_RevEntityGUID((long) revEntityGUID);

	list_for_each(valueIds, revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getAllRevEntityAnnoationIds_1By_1RevEntityContainer_1GUID(JNIEnv *env, jobject instance, jstring revAnnotationName_, jlong revEntityContainerGUID) {

	const char *revAnnotationName = env->GetStringUTFChars(revAnnotationName_, 0);

	list *valueIds = getAllRevEntityAnnoationIds_By_RevEntityContainer_GUID(strdup(revAnnotationName), (long) revEntityContainerGUID);

	list_for_each(valueIds, revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(revAnnotationName_, revAnnotationName);

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getAllRevEntityAnnoationIds_1By_1ResStatus(JNIEnv *env, jobject instance, jint revAnnResStatus) {

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	list *revAnnIdsList = getAllRevEntityAnnoationIds_By_ResStatus((int) revAnnResStatus);

	if (list_size(revAnnIdsList) == 0) return revRetJObjectArrayList;

	list_for_each(revAnnIdsList, revPersGetRevEntityDataLong);

	jclass cls = env->FindClass("java/lang/Long");
	jmethodID longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		jobject obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetRevEntityAnn_1By_1LocalAnnId(JNIEnv *env, jobject instance, jlong revAnnotationId) {
	RevEntityAnnotation revEntityAnnotation = *(revPersGetRevEntityAnn_By_LocalAnnId((long) revAnnotationId));

	REV_ENTITY_ANNOTATION_JNI_POSREC *rev_entity_annotation_jni_posrec = LoadRevEntityAnnotationJniPosRec(env);
	jobject jPosRec = env->NewObject(rev_entity_annotation_jni_posrec->cls, rev_entity_annotation_jni_posrec->constructortor_ID);
	FillDataRecValuesToRevAnnotationJni(env, jPosRec, revEntityAnnotation);

	return jPosRec;
}


/** END REV ENTITY ANOTATIONS **/


/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/
/** ######################################################################### **/


/** REV ENTITY TIMELINE **/

extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revTimelineEntityExists_1BY_1RevEntityGUID(JNIEnv *env, jobject instance, jlong revEntityGUID) {

	return revTimelineEntityExists_BY_RevEntityGUID((long) revEntityGUID);

}


extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevTimelineEntityUnResolved(JNIEnv *env, jobject instance) {

	list_for_each(revPersGetALLRevTimelineEntityUnResolved(), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		// Create a object of type Long.
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
		//
	}

	searchRecordResultLong.clear();

	return revRetJObjectArrayList;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevAnnotationOwnerGUID_1ByAnnotationId(JNIEnv *env, jobject instance, jlong annotationId) {

	return getRevAnnotationOwnerGUID_ByAnnotationId((long) annotationId);

}

extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetRevEntityMetadataOwnerGUID(JNIEnv *env, jobject instance, jlong metadataId) {

	return revGetRevEntityMetadataOwnerGUID(metadataId);

}

extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_getRevEntityMetaStringValue_1By_1metastringId(JNIEnv *env, jobject instance, jlong metastringId) {

	return env->NewStringUTF(getRevEntityMetaStringById((long) metastringId));
}

extern "C"
JNIEXPORT jobject JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revPersGetALLRevEntityGUIDs_1SQL_1IN(JNIEnv *env, jobject instance, jstring sql_IN_) {
	const char *sql_IN = env->GetStringUTFChars(sql_IN_, 0);

	list_for_each(revPersGetALLRevEntityGUIDs_SQL_IN(strdup(sql_IN)), revPersGetRevEntityDataLong);

	// First, get all the methods we need:
	jclass arrayListClass = env->FindClass("java/util/ArrayList");
	jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
	jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

	jmethodID longConstructor;
	jclass cls;
	jobject obj;

	cls = env->FindClass("java/lang/Long");
	longConstructor = env->GetMethodID(cls, "<init>", "(J)V");

	// The list we're going to return:
	jobject revRetJObjectArrayList = env->NewObject(arrayListClass, arrayListConstructor);

	for (size_t i = 0; i < searchRecordResultLong.size(); i++) {
		obj = env->NewObject(cls, longConstructor, (jlong) searchRecordResultLong[i]);
		env->CallBooleanMethod(revRetJObjectArrayList, addMethod, obj);
	}

	searchRecordResultLong.clear();

	env->ReleaseStringUTFChars(sql_IN_, sql_IN);

	return revRetJObjectArrayList;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetRevEntityMetadataOwnerGUID_1By_1MetadataName_1MetadataValue(JNIEnv *env, jobject instance, jstring revMetadataName_, jstring revMetadataValue_) {
	const char *revMetadataName = env->GetStringUTFChars(revMetadataName_, 0);
	const char *revMetadataValue = env->GetStringUTFChars(revMetadataValue_, 0);

	long revEntityGUID = revGetRevEntityMetadataOwnerGUID_By_MetadataName_MetadataValue(strdup(revMetadataName), strdup(revMetadataValue));

	env->ReleaseStringUTFChars(revMetadataName_, revMetadataName);
	env->ReleaseStringUTFChars(revMetadataValue_, revMetadataValue);

	return revEntityGUID;
}

extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetAnyRelExists_1By_1TypeValueId_1LocalGUIDs(JNIEnv *env, jobject instance, jstring revRelType_, jlong revSubjectGUID, jlong revTargetGuid) {
	const char *revRelType = env->GetStringUTFChars(revRelType_, 0);

	int revRelExists = revGetAnyRelExists_By_TypeValueId_LocalGUIDs(strdup(revRelType),(long) revSubjectGUID, (long) revTargetGuid);

	env->ReleaseStringUTFChars(revRelType_, revRelType);

	return revRelExists;
}

extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetAnyRelExists_1By_1TypeValueId_1RemoteGUIDs(JNIEnv *env, jobject instance, jstring revRelType_, jlong revRemoteSubjectGUID, jlong revRemoteTargetGuid) {
	const char *revRelType = env->GetStringUTFChars(revRelType_, 0);

	int revRelExists = revGetAnyRelExists_By_TypeValueId_RemoteGUIDs(strdup(revRelType),(long) revRemoteSubjectGUID, (long) revRemoteTargetGuid);

	env->ReleaseStringUTFChars(revRelType_, revRelType);

	return revRelExists;
}

extern "C"
JNIEXPORT jint JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetAnyRelExists_1By_1ResStatus_1TypeValueId_1RemoteGUIDs(JNIEnv *env, jobject instance, jint revResStatus, jstring revRelType_, jlong revRemoteSubjectGUID, jlong revRemoteTargetGuid) {
	const char *revRelType = env->GetStringUTFChars(revRelType_, 0);

	int revRelExists = revGetAnyRelExists_By_ResStatus_TypeValueId_RemoteGUIDs((int) revResStatus, strdup(revRelType),(long) revRemoteSubjectGUID, (long) revRemoteTargetGuid);

	env->ReleaseStringUTFChars(revRelType_, revRelType);

	return revRelExists;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_rev_ca_rev_1gen_1lib_1pers_c_1libs_1core_RevPersLibRead_revGetConvertedTimeFromMills(JNIEnv *env, jobject instance, jlong revMillisecsTime) {
	if (((long) revMillisecsTime) < 1) return env->NewStringUTF("- - - Unset Date ->");

	return env->NewStringUTF(strdup(revLocalTimer((long) revMillisecsTime)));
}


