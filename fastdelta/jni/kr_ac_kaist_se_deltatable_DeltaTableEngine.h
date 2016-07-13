/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class kr_ac_kaist_se_deltatable_DeltaTableEngine */

#ifndef _Included_kr_ac_kaist_se_deltatable_DeltaTableEngine
#define _Included_kr_ac_kaist_se_deltatable_DeltaTableEngine
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaTableEngine
 * Method:    initialize
 * Signature: (Lkr/ac/kaist/se/deltatable/DeltaTableInfo;)V
 */
JNIEXPORT void JNICALL Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_initialize
  (JNIEnv *, jobject, jobject);

/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaTableEngine
 * Method:    move
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_move
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaTableEngine
 * Method:    eval
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_eval
  (JNIEnv *, jobject);

/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaTableEngine
 * Method:    _getTopK
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_kr_ac_kaist_se_deltatable_DeltaTableEngine__1getTopK
  (JNIEnv *, jobject, jint);

/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaTableEngine
 * Method:    dispose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_dispose
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif