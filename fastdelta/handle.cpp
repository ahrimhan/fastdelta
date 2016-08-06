//
//  handle.cpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 8. 3..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#include "handle.h"

jfieldID getHandleField(JNIEnv *env, jobject obj)
{
    jclass c = env->GetObjectClass(obj);
    // J is the type signature for long:
    return env->GetFieldID(c, "nativeHandle", "J");
}



void unsetHandle(JNIEnv *env, jobject obj)
{
    env->SetLongField(obj, getHandleField(env, obj), 0);
}

void setIntField(JNIEnv* env, jobject obj, char* fieldName, jint value)
{
    jclass c = env->GetObjectClass(obj);
    
    env->SetIntField(obj, env->GetFieldID(c, fieldName, "I"), value);
}

void setFloatField(JNIEnv* env, jobject obj, char* fieldName, jfloat value)
{
    jclass c = env->GetObjectClass(obj);
    
    env->SetFloatField(obj, env->GetFieldID(c, fieldName, "F"), value);
}

void setFloatArrayField(JNIEnv* env, jobject obj, char* fieldName, const float* valueList, int valueCount)
{
    jclass c = env->GetObjectClass(obj);
    jfloatArray result;
    
    result = env->NewFloatArray(valueCount);
    env->SetFloatArrayRegion(result, 0, valueCount, valueList);
    env->SetObjectField(obj, env->GetFieldID(c, fieldName, "[F"), result);
}