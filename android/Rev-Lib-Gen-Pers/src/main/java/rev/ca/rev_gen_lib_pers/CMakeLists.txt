cmake_minimum_required(VERSION 3.6.0)

project(rev-gen-lib-pers)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

############################################################################


# configure import libs
set(REV_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(REV_INCLUDE_DIR ${REV_LIBS_DIR}/include)

add_library(lib_gmath STATIC IMPORTED)
set_target_properties(lib_gmath PROPERTIES IMPORTED_LOCATION
        ${REV_INCLUDE_DIR}/gmath/lib/${ANDROID_ABI}/libgmath.a)

set(REV_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

add_library(lib_gperf SHARED IMPORTED)
set_target_properties(lib_gperf PROPERTIES IMPORTED_LOCATION
        ${REV_INCLUDE_DIR}/gperf/lib/${ANDROID_ABI}/libgperf.so)


# build application's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

add_library(hello-libs SHARED
        src/main/jni/hello-libs.cpp)

target_include_directories(hello-libs PRIVATE
        PUBLIC ${REV_INCLUDE_DIR}/gmath/include
        PUBLIC ${REV_INCLUDE_DIR}/gperf/include
        ## /home/rev/Downloads/rev_data/rev_progs/rev_android_studio/rev_and_studio_sdk/ndk/25.1.8937393/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include
        )

target_link_libraries(hello-libs
        lib_gmath
        lib_gperf

        ${log-lib})

## START REV SQLITE3

add_library( sqlite3_base STATIC IMPORTED )
set_target_properties( sqlite3_base PROPERTIES IMPORTED_LOCATION ${REV_LIBS_DIR}/include/sqlite3/${ANDROID_ABI}/libsqlite3.a)

#############


## START REV EXT LIBS

set(REV_LIBS_EXT_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_libs_ext)

message(">>>> ${REV_INCLUDE_DIR}/nlohmann/json.hpp >>>>")

set(REV_LIBS_EXT_SRCS
        ${REV_LIBS_EXT_PROJECT_SOURCE_DIR}/libs_cust_rev/rev_gen_functions.c
        ${REV_LIBS_EXT_PROJECT_SOURCE_DIR}/libs_cust_rev/rev_gen_functions.h
        ${REV_LIBS_EXT_PROJECT_SOURCE_DIR}/libs_cust_rev/rev_list/rev_linked_list.c
        ${REV_LIBS_EXT_PROJECT_SOURCE_DIR}/libs_cust_rev/rev_list/rev_linked_list.h
        ${REV_LIBS_EXT_PROJECT_SOURCE_DIR}/libs_ext/cJSON/cJSON.c
        ${REV_LIBS_EXT_PROJECT_SOURCE_DIR}/libs_ext/cJSON/cJSON.h
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/include/sqlite3.h
        )

add_library(rev_pers_lib_includes SHARED
        ${REV_LIBS_EXT_SRCS}
        )

message("CMAKE_SOURCE_DIR >>>>>>> ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR >>>>>>> ${CMAKE_SOURCE_DIR}")

set(RES_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/res/libs)

set(REV_SQLITE3_DIR ${REV_LIBS_DIR}/rev_lib_sqlite3)

target_include_directories(rev_pers_lib_includes
        PUBLIC ${REV_SQLITE3_DIR}/include
        PUBLIC ${REV_INCLUDE_DIR}/nlohmann
        )

target_link_libraries( # Specifies the target library.
        rev_pers_lib_includes
        sqlite3_base
        android

        ${log-lib})

## START REV DB INIT

set(REV_DB_INIT_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_db_init)

set(REV_DB_INIT_SRCS
        ${REV_DB_INIT_PROJECT_SOURCE_DIR}/rev_db_init.c
        ${REV_DB_INIT_PROJECT_SOURCE_DIR}/rev_db_init.h
        )

add_library(rev_db_init SHARED
        ${REV_DB_INIT_SRCS})

target_link_libraries( # Specifies the target library.
        rev_db_init
        rev_pers_lib_includes

        ${log-lib})

## START REV PERS LIB CONNECTORS

set(REV_PERS_LIB_CONNECTORS_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_pers_lib_connectors)

set(REV_LIB_CONNECTORS_SRCS
        ${REV_PERS_LIB_CONNECTORS_PROJECT_SOURCE_DIR}/rev_perslib_create_init.c
        ${REV_PERS_LIB_CONNECTORS_PROJECT_SOURCE_DIR}/rev_perslib_create_init.h
        )

add_library(rev_lib_connectors SHARED
        ${REV_LIB_CONNECTORS_SRCS})

target_link_libraries( # Specifies the target library.
        rev_lib_connectors
        rev_pers_lib_includes
        rev_pers_rev_entity

        ${log-lib})

## START REV ENTITY

set(REV_ENTITY_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_entity)

set(REV_ENTITY_SRCS
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/init_rev_pers_rev_entity.c
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/init_rev_pers_rev_entity.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_table_create.cpp
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_table_create.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_delete/rev_pers_delete.c
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_delete/rev_pers_delete.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY.c
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_feedreaderdbhelper.c
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_feedreaderdbhelper.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_entity.c
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_entity.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_read/rev_pers_read/rev_pers_read.c
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_read/rev_pers_read/rev_pers_read.h
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update.c
        ${REV_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update.h
        )

add_library(rev_pers_rev_entity SHARED
        ${REV_ENTITY_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_entity
        rev_pers_lib_includes
        rev_db_init

        rev_pers_rev_object_entity
        rev_pers_rev_group_entity
        rev_pers_rev_user_entity
        rev_pers_entity_metastrings
        rev_pers_rev_entity_metadata
        rev_pers_rev_entity_relationships
        rev_pers_rev_entity_annotations
        rev_pers_rev_timeline

        ${log-lib})

## START REV OBJECT ENTITY

set(REV_OBJECT_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_object_entity)

set(REV_OBJECT_ENTITY_SRCS
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity_object.h
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_OBJECT_ENTITY.c
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_OBJECT_ENTITY.h
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_object_entity_feedreaderdbhelper.c
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_object_entity_feedreaderdbhelper.h
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_object_entity.c
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_object_entity.h
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_read/rev_pers_read/rev_pers_read_rev_object_entity.c
        ${REV_OBJECT_PROJECT_SOURCE_DIR}/rev_pers_lib_read/rev_pers_read/rev_pers_read_rev_object_entity.h
        )

add_library(rev_pers_rev_object_entity SHARED
        ${REV_OBJECT_ENTITY_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_object_entity
        rev_pers_lib_includes
        rev_db_init

        ${log-lib})

## START REV GROUP ENTITY

set(REV_GROUP_ENTITY_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_group_entity)

set(REV_GROUP_ENTITY_SRCS
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_rev_group_entity.c
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_rev_group_entity.h
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/db_create/FEED_ENTRY_REV_GROUP_ENTITY.c
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/db_create/FEED_ENTRY_REV_GROUP_ENTITY.h
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/db_create/rev_group_entity_feedreaderdbhelper.c
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/db_create/rev_group_entity_feedreaderdbhelper.h
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity_group.h
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_group_entity.c
        ${REV_GROUP_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_group_entity.h
        )

add_library(rev_pers_rev_group_entity SHARED
        ${REV_GROUP_ENTITY_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_group_entity
        rev_pers_lib_includes
        rev_db_init

        ${log-lib})

## START REV USER ENTITY

set(REV_USER_ENTITY_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity/rev_pers_rev_user_entity)

set(REV_USER_ENTITY_PROJECT_SOURCE_DIR
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity_user.h
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_USER_ENTITY.c
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_USER_ENTITY.h
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_user_entity_feedreaderdbhelper.c
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_user_entity_feedreaderdbhelper.h
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_user_entity.c
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_user_entity.h
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_read/rev_pers_read/rev_pers_read_rev_user_entity.c
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR}/rev_pers_lib_read/rev_pers_read/rev_pers_read_rev_user_entity.h
        )

add_library(rev_pers_rev_user_entity SHARED
        ${REV_USER_ENTITY_PROJECT_SOURCE_DIR})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_user_entity
        rev_pers_lib_includes
        rev_db_init

        ${log-lib})

####################################################
## DATA

## START REV ENTITY METASTRINGS

set(REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_metastrings)

set(REV_ENTITY_METASTRINGS_SRCS
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity_metastrings.h
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_METASTRING.c
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_METASTRING.h
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_metastring_feedreaderdbhelper.c
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_metastring_feedreaderdbhelper.h
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_entity_metastrings.c
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_entity_metastrings.h
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_metastrings.c
        ${REV_ENTITY_METASTRINGS_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_metastrings.h
        )

add_library(rev_pers_entity_metastrings SHARED
        ${REV_ENTITY_METASTRINGS_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_entity_metastrings
        rev_pers_lib_includes
        rev_db_init

        ${log-lib})

## START REV ENTITY METADATA

set(REV_ENTITY_METADATA_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_metadata)

set(REV_ENTITY_METADATA_SRCS
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity_metadata.h
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_METADATA.c
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_METADATA.h
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_metadata_feedreaderdbhelper.c
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_metadata_feedreaderdbhelper.h
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_entity_metadata.c
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_entity_metadata.h
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_metadata.c
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_metadata.h
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update_rev_entity_metadata.c
        ${REV_ENTITY_METADATA_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update_rev_entity_metadata.h
        )

add_library(rev_pers_rev_entity_metadata SHARED
        ${REV_ENTITY_METADATA_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_entity_metadata
        rev_pers_lib_includes
        rev_db_init
        rev_pers_entity_metastrings

        ${log-lib})

## START REV ENTITY RELATIONSHIPS

set(REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_relationships)

set(REV_RELATIONSHIPS_ENTITY_SRCS
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity_relationships.h
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_RELATIONSHIPS.c
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_RELATIONSHIPS.h
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_relationships_feedreaderdbhelper.c
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_relationships_feedreaderdbhelper.h
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_relationships.c
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_relationships.h
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_relationships.c
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_relationships.h
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update_rev_entity_rel.c
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update_rev_entity_rel.h
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_delete/rev_pers_rel_delete.h
        ${REV_ENTITY_RELATIONSHIPS_PROJECT_SOURCE_DIR}/rev_pers_delete/rev_pers_rel_delete.c
        )

add_library(rev_pers_rev_entity_relationships SHARED
        ${REV_RELATIONSHIPS_ENTITY_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_entity_relationships
        rev_pers_lib_includes
        rev_db_init
        rev_pers_entity_metastrings

        ${log-lib})

############################################################################

## START REV ENTITY ANNOTATIONS

set(REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_entity_data/rev_pers_rev_entity_annotations)

set(REV_ENTITY_ANNOTATION_SRCS
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_db_models/rev_entity_annotation.h
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_ANNOTATIONS.c
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/FEED_ENTRY_REV_ENTITY_ANNOTATIONS.h
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_annotation_feedreaderdbhelper.c
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_entity_annotation_feedreaderdbhelper.h
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_annotation.c
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_annotation.h
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_annotations.c
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_entity_annotations.h
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update_rev_entity_ann.c
        ${REV_ENTITY_ANNOTATION_PROJECT_SOURCE_DIR}/rev_pers_update/rev_pers_update_rev_entity_ann.h
        )

add_library(rev_pers_rev_entity_annotations SHARED
        ${REV_ENTITY_ANNOTATION_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_entity_annotations
        rev_pers_lib_includes
        rev_db_init
        rev_pers_entity_metastrings

        ${log-lib})


############################################################################

## START REV TIMELINE

set(REV_TIMELINE_PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/rev_clib_sqlite_pers/rev_pers_lib/rev_timeline)

set(REV_TIMELINE_SRCS
        ${REV_TIMELINE_PROJECT_SOURCE_DIR}/rev_db_models/rev_timeline.h
        ${REV_TIMELINE_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_timeline_feedreaderdbhelper.c
        ${REV_TIMELINE_PROJECT_SOURCE_DIR}/rev_pers_lib_create/db_create/rev_timeline_feedreaderdbhelper.h
        ${REV_TIMELINE_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_timeline.c
        ${REV_TIMELINE_PROJECT_SOURCE_DIR}/rev_pers_lib_create/rev_pers_create/rev_pers_rev_timeline.h
        ${REV_TIMELINE_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_timeline.h
        ${REV_TIMELINE_PROJECT_SOURCE_DIR}/rev_pers_read/rev_pers_read_rev_timeline.c
        )

add_library(rev_pers_rev_timeline SHARED
        ${REV_TIMELINE_SRCS})

target_link_libraries( # Specifies the target library.
        rev_pers_rev_timeline
        rev_pers_lib_includes
        rev_db_init

        ${log-lib})

############################################################################
## CREATE ##
add_library(rev-pers-lib-db-create SHARED
        src/main/jni/rev-pers-lib-create.cpp
        src/main/jni/rev_metadata_jni_loader.h
        src/main/jni/rev_metadata_jni_loader.cpp
        src/main/jni/rev_annotation_jni_loader.h
        src/main/jni/rev_annotation_jni_loader.cpp
        src/main/jni/rev_pers_entity_type_init/rev_pers_object_init.h
        src/main/jni/rev_pers_entity_type_init/rev_pers_object_init.cpp
        )

target_link_libraries( # Specifies the target library.
        rev-pers-lib-db-create
        rev_lib_connectors
        rev_pers_rev_entity

        rev_pers_lib_includes

        hello-libs

        ${log-lib})


############################################################################
## READ ##

add_library(rev-pers-lib-db-read SHARED
        src/main/jni/rev_pers_jni_structs.h
        src/main/jni/rev-pers-lib-read.h
        src/main/jni/rev-pers-lib-read.cpp)

target_link_libraries( # Specifies the target library.
        rev-pers-lib-db-read
        rev-pers-lib-db-create

        ${log-lib})


############################################################################
## UPDATE ##

add_library(rev-pers-lib-db-update SHARED
        src/main/jni/rev_pers_jni_structs.h
        src/main/jni/rev-pers-lib-update.cpp)

target_link_libraries( # Specifies the target library.
        rev-pers-lib-db-update
        rev-pers-lib-db-read
        rev-pers-lib-db-create

        ${log-lib})


############################################################################
## DELETE ##

add_library(rev-pers-lib-db-delete SHARED
        src/main/jni/rev_pers_jni_structs.h
        src/main/jni/rev-pers-lib-delete.cpp)

target_link_libraries( # Specifies the target library.
        rev-pers-lib-db-delete
        rev-pers-lib-db-update
        rev-pers-lib-db-read
        rev-pers-lib-db-create

        ${log-lib})






