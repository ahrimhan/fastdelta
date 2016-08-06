//
//  handle.h
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/20/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#ifndef _HANDLE_H_INCLUDED_
#define _HANDLE_H_INCLUDED_

#include <jni.h>

jfieldID getHandleField(JNIEnv *env, jobject obj);


template <typename T>
T *getHandle(JNIEnv *env, jobject obj)
{
    jlong handle = env->GetLongField(obj, getHandleField(env, obj));
    return reinterpret_cast<T *>(handle);
}

template <typename T>
void setHandle(JNIEnv *env, jobject obj, T *t)
{
    jlong handle = reinterpret_cast<jlong>(t);
    env->SetLongField(obj, getHandleField(env, obj), handle);
}

void unsetHandle(JNIEnv *env, jobject obj);
void setIntField(JNIEnv* env, jobject obj, char* fieldName, jint value);
void setFloatField(JNIEnv* env, jobject obj, char* fieldName, jfloat value);
void setFloatArrayField(JNIEnv* env, jobject obj, char* fieldName, const float* valueList, int valueCount);

#endif
