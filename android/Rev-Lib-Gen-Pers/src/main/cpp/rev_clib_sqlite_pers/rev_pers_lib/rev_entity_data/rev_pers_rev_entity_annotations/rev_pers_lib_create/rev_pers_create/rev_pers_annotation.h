//
// Created by rev on 8/21/18.
//

#ifndef REVCORE_REV_PERS_ANNOTATION_H
#define REVCORE_REV_PERS_ANNOTATION_H

long revPersAnnotation(char *revEntityAnnotationName, char *revEntityAnnotationValue, long revEntityGUID, long revEntityContainerGUID);

long revPersAnnotationStruct(RevEntityAnnotation *revEntityAnnotation);

#endif //REVCORE_REV_PERS_ANNOTATION_H
