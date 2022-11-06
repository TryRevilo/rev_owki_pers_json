#include "rev_table_create.h"

extern "C" {
#include "rev_pers_lib_create/db_create/rev_entity_feedreaderdbhelper.h"
#include "../rev_pers_rev_group_entity/db_create/rev_group_entity_feedreaderdbhelper.h"
#include "../../rev_entity_data/rev_pers_rev_entity_metadata/rev_pers_lib_create/db_create/rev_entity_metadata_feedreaderdbhelper.h"
#include "../rev_pers_rev_object_entity/rev_pers_lib_create/db_create/rev_object_entity_feedreaderdbhelper.h"
#include "../../rev_entity_data/rev_pers_relationships/rev_pers_lib_create/db_create/rev_entity_relationships_feedreaderdbhelper.h"
#include "../../rev_entity_data/rev_pers_rev_entity_metastrings/rev_pers_lib_create/db_create/rev_entity_metastring_feedreaderdbhelper.h"
#include "../rev_pers_rev_user_entity/rev_pers_lib_create/db_create/rev_user_entity_feedreaderdbhelper.h"
#include "../../rev_entity_data/rev_pers_rev_entity_annotations/rev_pers_lib_create/db_create/rev_entity_annotation_feedreaderdbhelper.h"
#include "../../rev_timeline/rev_pers_lib_create/db_create/rev_timeline_feedreaderdbhelper.h"
}

int revTablesInit() {
    int tableStats = 0;

    revTableCreate_REV_ENTITY();

    revTableCreate_REV_OBJECT_ENTITY();
    revTableCreate_REV_GROUP_ENTITY();
    revTableCreate_REV_USER_ENTITY();
    revTableCreate_REV_ENTITY_METADATA();
    revTableCreate_REV_ENTITY_METASTRINGS();
    revTableCreate_REV_ENTITY_RELATIONSHIPS();
    revTableCreate_REV_ENTITY_ANNOTATIONS();

    return tableStats;
}
