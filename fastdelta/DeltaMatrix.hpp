//
//  DeltaMatrix.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/20/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#ifndef DeltaMatrix_hpp
#define DeltaMatrix_hpp

#include <stdio.h>
#include <vector>
#include <map>
#include "TypeDef.h"
#include "DeltaMatrixInfo.hpp"
#include "DeltaMatrixEntity.hpp"
#include "LinkMatrix.hpp"
#include "MoveMethodCandidate.hpp"
#include "MoveMethodCandidateParetoFrontSet.hpp"


//  D = (L^(MR*MRT))*(MI=(1-MC)) - (L^(1-(MR*MR.T)))*MC

#pragma GCC visibility push(default)


class LinkMatrix;

class DeltaMatrix
{
private:
    SpRowMat mr;
    SpColMat mc;
    SpColMat mtc;
    SpRowMat mmt;

    LinkMatrix linkMatrixList[LINK_MATRIX_COUNT];
    
    SpRowMat possibleMoveMethodMatrix;
    MoveMethodCandidateParetoFrontSet moveMethodCandidateParetoFrontSet;

    int entityCount;
    int methodCount;
    int classCount;
    int methodPossibleToMoveCount;
        
    std::vector<DeltaMatrixEntity> entities;
    
    
    void changeMembership(int entityIdx, int classIdx, int value)
    {
        if( entityIdx < methodPossibleToMoveCount )
        {
            mr.coeffRef(entityIdx, classIdx) = value;
        }
        mtc.coeffRef(classIdx, entityIdx) = value;
        mc.coeffRef(entityIdx, classIdx) = value;
    }
    
//    void evalPint(SpRowMat& pint_1, DRowMat& pint);
    
    
    void initMembership(DeltaMatrixInfo* info);
    void createCouplingBasedLink(DeltaMatrixInfo* info,
                                 float methodCallWeight, float fieldAccessWeight, LinkMatrix& link, bool useQuotient);
    void createSharedFieldAccessLink(DeltaMatrixInfo* info, LinkMatrix& link);
    void createSharedMethodCallLink(DeltaMatrixInfo* info, LinkMatrix& link);

    void initLink(DeltaMatrixInfo* info);
    
public:
    DeltaMatrix()
    {
    }
    
    ~DeltaMatrix()
    {
    }
        
    void init(DeltaMatrixInfo* info);
    
    void createMoveMethodCandidateSet(MoveMethodCandidateParetoFrontSet& mmcSet);
    
    void move(int entityIdx, int fromClassIdx, int toClassIdx);
    
    void eval();
    
    MoveMethodCandidateParetoFrontIterator* getSortedMoveMethodCandidates();
        
    friend class LinkMatrix;
    
    LinkMatrix& getLinkMatrix(int idx)
    {
        return linkMatrixList[idx];
    }
    
    MoveMethodCandidateParetoFrontSet& getParetoSet()
    {
        return moveMethodCandidateParetoFrontSet;
    }
};




#pragma GCC visibility pop


#endif /* DeltaMatrix_hpp */
