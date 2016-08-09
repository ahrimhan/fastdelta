//
//  jni_support.c
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/20/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#include <stdio.h>
#include "kr_ac_kaist_se_deltatable_DeltaTableInfo.h"
#include "kr_ac_kaist_se_deltatable_DeltaTableEngine.h"
#include "kr_ac_kaist_se_deltatable_DeltaTableEntryIterator.h"
#include "kr_ac_kaist_se_deltatable_DeltaValueEngine.h"
#include "handle.h"
#include "DeltaMatrix.hpp"
#include "DeltaValue.hpp"

class EntryIterator {
public:
    MoveMethodPartialOrderIterator* curIter;
    int cutoff;
    int curCount;
    
    EntryIterator(MoveMethodPartialOrderIterator* v, int k) : curIter(v), cutoff(k)
    {
        curCount = 0;
    }
};

void Java_kr_ac_kaist_se_deltatable_DeltaTableInfo_initialize(JNIEnv *env, jobject obj,
                                                              jint classCount, jint entityCount, jint methodCount, jint methodPossibleToMoveCount)
{
    DeltaMatrixInfo *info = new DeltaMatrixInfo(classCount, entityCount, methodCount, methodPossibleToMoveCount);
    if( info )
    {
        setHandle(env, obj, info);
    }
}

void Java_kr_ac_kaist_se_deltatable_DeltaTableInfo_addLink(JNIEnv *env, jobject obj, jint fromEntityIdx, jint toEntityIdx)
{
    DeltaMatrixInfo* info = getHandle<DeltaMatrixInfo>(env, obj);
    
    if( info )
    {
        info->addLink(fromEntityIdx, toEntityIdx);
    }
}

void Java_kr_ac_kaist_se_deltatable_DeltaTableInfo_addMembership(JNIEnv *env, jobject obj, jint entityIdx, jint classIdx)
{
    DeltaMatrixInfo* info = getHandle<DeltaMatrixInfo>(env, obj);
    
    if( info )
    {
        info->addMembership(entityIdx, classIdx);
    }
}

void Java_kr_ac_kaist_se_deltatable_DeltaTableInfo_possibleMoveMethod(JNIEnv *env, jobject obj, jint entityIdx, jint classIdx)
{
    DeltaMatrixInfo* info = getHandle<DeltaMatrixInfo>(env, obj);
    
    if( info )
    {
        info->possibleMoveMethod(entityIdx, classIdx);
    }
}


jint Java_kr_ac_kaist_se_deltatable_DeltaTableInfo_getClassCount(JNIEnv *env, jobject obj)
{
    jint ret = 0;
    DeltaMatrixInfo* info = getHandle<DeltaMatrixInfo>(env, obj);

    if( info )
    {
        ret = info->getClassCount();
    }
    return ret;
}

jint Java_kr_ac_kaist_se_deltatable_DeltaTableInfo_getEntityCount(JNIEnv *env, jobject obj)
{
    jint ret = 0;
    DeltaMatrixInfo* info = getHandle<DeltaMatrixInfo>(env, obj);
    
    if( info )
    {
        ret = info->getEntityCount();
    }
    return ret;
}

void Java_kr_ac_kaist_se_deltatable_DeltaTableInfo_dispose(JNIEnv *env, jobject obj)
{
    DeltaMatrixInfo* info = getHandle<DeltaMatrixInfo>(env, obj);
    if( info )
    {
        delete info;
    }
}



void Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_initialize(JNIEnv *env, jobject obj, jobject info)
{
    DeltaMatrix* dm = new DeltaMatrix();
    DeltaMatrixInfo* i = getHandle<DeltaMatrixInfo>(env, info);

    if( dm && i )
    {
        dm->init(i);
        setHandle(env, obj, dm);
    }
}

void Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_move(JNIEnv *env, jobject obj, jint entityIdx, jint fromClassIdx , jint toClassIdx)
{
    DeltaMatrix* dm = getHandle<DeltaMatrix>(env, obj);
    
    if( dm )
    {
        dm->move(entityIdx, fromClassIdx, toClassIdx);
    }
}

void Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_eval(JNIEnv *env, jobject obj)
{
    DeltaMatrix* dm = getHandle<DeltaMatrix>(env, obj);
    
    if( dm )
    {
        dm->eval();
    }
}


jlong Java_kr_ac_kaist_se_deltatable_DeltaTableEngine__1getTopK(JNIEnv *env, jobject obj, jint k)
{
    DeltaMatrix* dm = getHandle<DeltaMatrix>(env, obj);
    
    if( dm )
    {
    
        EntryIterator* ei = new EntryIterator(dm->getSortedMoveMethodCandidates(), k);
        
        jlong handle = reinterpret_cast<jlong>(ei);
        
        return handle;
    }
    else
    {
        return 0;
    }
}


void Java_kr_ac_kaist_se_deltatable_DeltaTableEngine_dispose(JNIEnv *env, jobject obj)
{
    DeltaMatrix* dm = getHandle<DeltaMatrix>(env, obj);
    
    if( dm )
    {
        delete dm;
    }
}




jint Java_kr_ac_kaist_se_deltatable_DeltaTableEntryIterator__1hasNext(JNIEnv *env, jobject obj)
{
    EntryIterator* ei = getHandle<EntryIterator>(env, obj);
    
    if( ei && ei->curIter->hasNext() &&
       (ei->cutoff <= 0 || ei->curCount < ei->cutoff) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

jint Java_kr_ac_kaist_se_deltatable_DeltaTableEntryIterator__1next(JNIEnv *env, jobject obj, jobject target)
{
    EntryIterator* ei = getHandle<EntryIterator>(env, obj);
   
    if( ei && ei->curIter->hasNext() &&
       (ei->cutoff <= 0 || ei->curCount < ei->cutoff))
    {
        MoveMethodCandidatePtr candidate = ei->curIter->next();
        setIntField(env, target, "toClassIdx", candidate->toClassIdx);
        setIntField(env, target, "entityIdx", candidate->entityIdx);
        setFloatArrayField(env, target, "deltaValueList", candidate->valueList, LINK_MATRIX_COUNT);
        ei->curCount++;
        return 1;
    }
    else
    {
        return 0;
    }
}

void JNICALL Java_kr_ac_kaist_se_deltatable_DeltaTableEntryIterator_dispose(JNIEnv *env, jobject obj)
{
    EntryIterator* ei = getHandle<EntryIterator>(env, obj);
    
    if( ei )
    {
        delete ei->curIter;
        delete ei;
    }

}


void Java_kr_ac_kaist_se_deltatable_DeltaValueEngine_initialize(JNIEnv *env, jobject obj, jobject info)
{
    DeltaValue* dm = new DeltaValue();
    DeltaMatrixInfo* i = getHandle<DeltaMatrixInfo>(env, info);
    
    if( dm && i )
    {
        dm->init(i);
        setHandle(env, obj, dm);
    }
}

void Java_kr_ac_kaist_se_deltatable_DeltaValueEngine_move(JNIEnv *env, jobject obj, jint entityIdx, jint fromClassIdx, jint toClassIdx)
{
    DeltaValue* dm = getHandle<DeltaValue>(env, obj);
    
    if( dm )
    {
        dm->move(entityIdx, fromClassIdx, toClassIdx);
    }
}

jint Java_kr_ac_kaist_se_deltatable_DeltaValueEngine_createClass(JNIEnv *env, jobject obj)
{
    jint ret = 0;
    DeltaValue* dm = getHandle<DeltaValue>(env, obj);
    
    if( dm )
    {
        ret = dm->createClass();
    }
    
    return ret;
}

/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaValueEngine
 * Method:    getDValue
 * Signature: (III)F
 */
jfloat Java_kr_ac_kaist_se_deltatable_DeltaValueEngine_getDValue(JNIEnv *env, jobject obj, jint entityIdx, jint fromClassIdx, jint toClassIdx)
{
    jfloat ret = 0;
    DeltaValue* dm = getHandle<DeltaValue>(env, obj);
    
    if( dm )
    {
        ret = dm->getDValue(entityIdx, fromClassIdx, toClassIdx);
    }
    
    return ret;
}

/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaValueEngine
 * Method:    getExtractClassDValue
 * Signature: (I[I)F
 */
jfloat Java_kr_ac_kaist_se_deltatable_DeltaValueEngine_getExtractClassDValue(JNIEnv *env, jobject obj, jint sourceClassIdx, jintArray entityIndices)
{
    jfloat ret = 0;
    DeltaValue* dm = getHandle<DeltaValue>(env, obj);
    const jsize length = env->GetArrayLength(entityIndices);
    jint *ei = env->GetIntArrayElements(entityIndices, NULL);
    
    if( dm )
    {
        ret = dm->getExtractClassDValue(sourceClassIdx, ei, length);
    }
    
    env->ReleaseIntArrayElements(entityIndices, ei, NULL);
    
    return ret;
}

/*
 * Class:     kr_ac_kaist_se_deltatable_DeltaValueEngine
 * Method:    dispose
 * Signature: ()V
 */
void Java_kr_ac_kaist_se_deltatable_DeltaValueEngine_dispose(JNIEnv *env, jobject obj)
{
    DeltaValue* dm = getHandle<DeltaValue>(env, obj);
    
    if( dm )
    {
        delete dm;
    }
}