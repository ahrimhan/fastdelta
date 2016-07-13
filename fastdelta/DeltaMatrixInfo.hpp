//
//  DeltaMatrixInfo.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/22/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#ifndef DeltaMatrixInfo_hpp
#define DeltaMatrixInfo_hpp

#include "TypeDef.h"

#pragma GCC visibility push(default)

class DeltaMatrixInfo
{
private:
    int classCount;
    int entityCount;
    int methodCount;
    
    std::vector<DTT> membershipInfoList;
    std::vector<DTT> linkInfoList;
    std::vector<DTT> possibleMoveMethodList;
    

    
public:
    DeltaMatrixInfo(int classCount, int entityCount, int methodCount)
    {
        this->classCount = classCount;
        this->entityCount = entityCount;
        this->methodCount = methodCount;
        
        possibleMoveMethodList.reserve(this->classCount * 15);
        membershipInfoList.reserve(this->classCount * 15);
        linkInfoList.reserve(this->entityCount * 15);
        
    }
    
    void addLink(int fromEntityIdx, int toEntityIdx)
    {
        if( toEntityIdx < methodCount )
        {
            linkInfoList.push_back(DTT(fromEntityIdx, toEntityIdx, COUPLING_WEIGHT));
            linkInfoList.push_back(DTT(toEntityIdx, fromEntityIdx, COUPLING_WEIGHT));
        }
        else
        {
            linkInfoList.push_back(DTT(fromEntityIdx, toEntityIdx, COUPLING_WEIGHT + COHESION_WEIGHT));
            linkInfoList.push_back(DTT(toEntityIdx, fromEntityIdx, COUPLING_WEIGHT + COHESION_WEIGHT));
        }
    }
    
    void addMembership(int entityIdx, int classIdx)
    {
        membershipInfoList.push_back(DTT(entityIdx, classIdx, 1));
    }
    
    void possibleMoveMethod(int entityIdx, int classIdx)
    {
        possibleMoveMethodList.push_back(DTT(entityIdx, classIdx, 1));
    }
    
    int getClassCount()
    {
        return classCount;
    }
    
    int getEntityCount()
    {
        return entityCount;
    }
    
    int getMethodCount()
    {
        return methodCount;
    }
    
    friend class DeltaMatrix;
    friend class DeltaValue;
};

#pragma GCC visibility pop


#endif /* DeltaMatrixInfo_hpp */