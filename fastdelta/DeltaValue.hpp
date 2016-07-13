//
//  DeltaValue.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 2..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef DeltaValue_hpp
#define DeltaValue_hpp

#include <stdio.h>
#include <vector>
#include <set>
#include "TypeDef.h"
#include "DeltaMatrixInfo.hpp"
#include "DeltaMatrixEntity.hpp"

//  D = (L^(MR*MRT))*(MI=(1-MC)) - (L^(1-(MR*MR.T)))*MC

#pragma GCC visibility push(default)


class DeltaValue
{
private:
    SpRowMat mr;
    SpColMat mc;
    SpRowMat l;
    SpColMat lc;
    

    
    int entityCount;
    int methodCount;
    int fieldCount;
    int classCount;
    

    static void internal_init(DeltaMatrix& dm, DeltaMatrixInfo* info);
    
    inline void changeMembership(int entityIdx, int classIdx, int value)
    {
        mr.coeffRef(entityIdx, classIdx) = value;
        mc.coeffRef(entityIdx, classIdx) = value;
    }
    
    void evalPint(SpRowMat& pint_1, DRowMat& pint);
    
    
public:
    DeltaValue()
    {
    }
    
    DeltaValue(const DeltaValue& d) : mr(d.mr), mc(d.mc), l(d.l), entityCount(d.entityCount),
    methodCount(d.methodCount), fieldCount(d.fieldCount), classCount(d.classCount)
    {
        
    }
    
    ~DeltaValue()
    {
    }
    
    void init(DeltaMatrixInfo* info);
    
    void move(int entityIdx, int fromClassIdx, int toClassIdx);
    
    int createClass();
    
    float getDValue(int entityIdx, int fromClassIdx, int toClassIdx);
    
    float getExtractClassDValue(int sourceClassIdx, int* entityIndices, int len);

};

#pragma GCC visibility pop

#endif /* DeltaValue_hpp */
