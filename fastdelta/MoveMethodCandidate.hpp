//
//  MoveMethodCandidate.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 8. 5..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef MoveMethodCandidate_hpp
#define MoveMethodCandidate_hpp

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <iostream>
#include <set>
#include <unordered_set>
#include "TypeDef.h"

#pragma GCC visibility push(default)

class MoveMethodCandidateParetoFront;
typedef std::shared_ptr<MoveMethodCandidateParetoFront> MoveMethodCandidateParetoFrontPtr;


class MoveMethodCandidate;
typedef std::shared_ptr<MoveMethodCandidate> MoveMethodCandidatePtr;

struct MoveMethodCandidatePtrHash {
    size_t operator() (const MoveMethodCandidate* mmc) const {
        return (size_t)mmc;
    }
};

struct EqualFn
{
    bool operator() (MoveMethodCandidate* t1, MoveMethodCandidate* t2) const
    {
        return t1 == t2;
    }
};

typedef std::unordered_set<MoveMethodCandidate*, MoveMethodCandidatePtrHash, EqualFn> MoveMethodCandidateSet;




class MoveMethodCandidate
{
private:
    
public:

    uint64_t id;
    int entityIdx;
    int toClassIdx;
    float valueList[LINK_MATRIX_COUNT];
    MoveMethodCandidateParetoFrontPtr belongingParetoFront;
    
    MoveMethodCandidateSet betterCandidate;
    MoveMethodCandidateSet worseCandidate;
    
    
    MoveMethodCandidate(int _entityIdx, int _toClassIdx, float* values) :
    entityIdx(_entityIdx), toClassIdx(_toClassIdx)
    {
        id = createID(_entityIdx, _toClassIdx);
        for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
        {
            valueList[i] = values[i];
        }
    }
    
    MoveMethodCandidate(const MoveMethodCandidate& mmc) :
    id(mmc.id), entityIdx(mmc.entityIdx), toClassIdx(mmc.toClassIdx), belongingParetoFront(mmc.belongingParetoFront)
    {
        for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
        {
            valueList[i] = mmc.valueList[i];
        }
    }
    
    static uint64_t createID(int _entityIdx, int _toClassIdx)
    {
        uint64_t ret = _entityIdx;
        ret = (ret << 32) | _toClassIdx;
        return ret;
    }
    
    void updateValue(float* values)
    {
        for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
        {
            valueList[i] = values[i];
        }
    }
    
    
    friend bool operator<(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
    friend bool operator>(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
    friend bool operator==(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
    friend bool operator!=(const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
    
    friend std::ostream& operator<<(std::ostream& os, const MoveMethodCandidate& dt);
};




bool operator< (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
bool operator> (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
bool operator== (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
bool operator!= (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs);
std::ostream& operator<<(std::ostream& os, const MoveMethodCandidate& mmc);

typedef boost::multi_index_container<
MoveMethodCandidatePtr,
boost::multi_index::indexed_by<
boost::multi_index::ordered_unique<
boost::multi_index::member<MoveMethodCandidate,uint64_t,&MoveMethodCandidate::id>,
std::less<uint64_t>
>
>
> MoveMethodCandidateContainer;
typedef MoveMethodCandidateContainer::nth_index<0>::type MoveMethodCandidateContainerIndex;


#pragma GCC visibility pop


#endif /* MoveMethodCandidate_hpp */
