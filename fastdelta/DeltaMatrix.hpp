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
//#include <set>
#include "TypeDef.h"
#include "DeltaMatrixInfo.hpp"
#include "DeltaMatrixEntity.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>


//  D = (L^(MR*MRT))*(MI=(1-MC)) - (L^(1-(MR*MR.T)))*MC

#pragma GCC visibility push(default)

class MoveMethodCandidate
{
public:
    uint64_t id;
    int entityIdx;
    int toClassIdx;
    float value;
    
    MoveMethodCandidate(int _entityIdx, int _toClassIdx, float _value) :
    entityIdx(_entityIdx), toClassIdx(_toClassIdx), value(_value)
    {
        id = createID(_entityIdx, _toClassIdx);
    }
    
    MoveMethodCandidate(const MoveMethodCandidate& mmc) :
    id(mmc.id), entityIdx(mmc.entityIdx), toClassIdx(mmc.toClassIdx), value(mmc.value)
    {
    }
    
    static uint64_t createID(int _entityIdx, int _toClassIdx)
    {
        uint64_t ret = _entityIdx;
        ret = (ret << 32) | _toClassIdx;
        return ret;
    }
    
    
    friend bool operator<(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
    friend bool operator>(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
    friend bool operator==(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
    friend bool operator!=(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
};



typedef boost::multi_index_container<
    MoveMethodCandidate,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
            boost::multi_index::composite_key<MoveMethodCandidate,
                boost::multi_index::member<MoveMethodCandidate,float,&MoveMethodCandidate::value>,
                boost::multi_index::member<MoveMethodCandidate,uint64_t,&MoveMethodCandidate::id>
            >
        >,
        boost::multi_index::ordered_unique<
            boost::multi_index::member<MoveMethodCandidate,uint64_t,&MoveMethodCandidate::id>
        >
    >
> MoveMethodCandidateSet;

typedef MoveMethodCandidateSet::nth_index<0>::type MoveMethodCandidateSetIndexByValue;

typedef MoveMethodCandidateSet::nth_index<1>::type MoveMethodCandidateSetIndexByID;


class DeltaMatrix
{
private:
    SpRowMat mr;
    SpColMat mc;
    SpColMat mtc;
    DColMat mic;
    SpRowMat mmt;

    SpRowMat l;
    SpRowMat lint;
    SpRowMat lext;

    SpRowMat prevD;
    SpRowMat D;
    SpRowMat possibleMoveMethodMatrix;
    SpRowMat af;
    SpRowMat adjustedD;
    SpRowMat prevAdjustedD;
    
    int entityCount;
    int methodCount;
    int fieldCount;
    int classCount;
    
    bool useAdjust;
    
    std::vector<DeltaMatrixEntity> entities;
    
    
    MoveMethodCandidateSet moveMethodCandidateSet;
    
    
    static void internal_init(DeltaMatrix& dm, DeltaMatrixInfo* info);
    
    inline void changeMembership(int entityIdx, int classIdx, int value)
    {
        mr.coeffRef(entityIdx, classIdx) = value;
        mtc.coeffRef(classIdx, entityIdx) = value;

        
        mc.coeffRef(entityIdx, classIdx) = value;
        mic.coeffRef(entityIdx, classIdx) = value == 1 ? 0 : 1;
    }
    
    void evalPint(SpRowMat& pint_1, DRowMat& pint);
    
    
public:
    DeltaMatrix(bool _useAdjust) : useAdjust(_useAdjust)
    {
    }
    
    DeltaMatrix(const DeltaMatrix& dm) : useAdjust(dm.useAdjust), mr(dm.mr), mc(dm.mc),
    mtc(dm.mtc), mic(dm.mic), l(dm.l), mmt(dm.mmt), lint(dm.lint), lext(dm.lext), D(dm.D)
    {
    }
    
    ~DeltaMatrix()
    {
    }
        
    void init(DeltaMatrixInfo* info);
    
    void createMoveMethodCandidateSet(MoveMethodCandidateSet& candidateSet);
    
    void move(int entityIdx, int fromClassIdx, int toClassIdx);
    
    void eval();
    
    MoveMethodCandidateSetIndexByValue& getSortedMoveMethodCandidates();
    
    DeltaMatrix* copy() { return new DeltaMatrix(*this); }
    
    SpRowMat& getD() { return D; }
    
    SpRowMat& getAdjustedD() { return adjustedD; }
};

#pragma GCC visibility pop


#endif /* DeltaMatrix_hpp */
