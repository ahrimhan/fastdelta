//
//  handle.h
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/20/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#ifndef _HANDLE_H_INCLUDED_
#define _HANDLE_H_INCLUDED_

jfieldID getHandleField(JNIEnv *env, jobject obj)
{
    jclass c = env->GetObjectClass(obj);
    // J is the type signature for long:
    return env->GetFieldID(c, "nativeHandle", "J");
}



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

void unsetHandle(JNIEnv *env, jobject obj)
{
    env->SetLongField(obj, getHandleField(env, obj), 0);
}

inline void setIntField(JNIEnv* env, jobject obj, char* fieldName, jint value)
{
    jclass c = env->GetObjectClass(obj);
    
    env->SetIntField(obj, env->GetFieldID(c, fieldName, "I"), value);
}

inline void setFloatField(JNIEnv* env, jobject obj, char* fieldName, jfloat value)
{
    jclass c = env->GetObjectClass(obj);
    
    env->SetFloatField(obj, env->GetFieldID(c, fieldName, "F"), value);
}

#endif
