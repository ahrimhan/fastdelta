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
#include "TypeDef.h"

#pragma GCC visibility push(default)

class MoveMethodCandidateParetoFront;
typedef std::shared_ptr<MoveMethodCandidateParetoFront> MoveMethodCandidateParetoFrontPtr;


class MoveMethodCandidate;
typedef std::shared_ptr<MoveMethodCandidate> MoveMethodCandidatePtr;


class MoveMethodCandidate
{
private:
    
public:

    uint64_t id;
    int entityIdx;
    int toClassIdx;
    float valueList[LINK_MATRIX_COUNT];
    MoveMethodCandidateParetoFrontPtr belongingParetoFront;
    std::vector<MoveMethodCandidatePtr> dominatingCandidate;
    
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


#pragma GCC visibility pop


#endif /* MoveMethodCandidate_hpp */
