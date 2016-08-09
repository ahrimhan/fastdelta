//
//  MoveMethodCandidate.cpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 8. 5..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#include "MoveMethodCandidate.hpp"


bool operator< (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
    return lhs.id < rhs.id;
}

bool operator> (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
    return lhs.id > rhs.id;
}

bool operator== (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
    
    return lhs.id == rhs.id;
}

bool operator!= (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const MoveMethodCandidate& mmc)
{
    os << "(e, c, v):" << mmc.entityIdx << "," << mmc.toClassIdx << ",";
    for( int i = 0; i < LINK_MATRIX_COUNT; i++)
    {
        os << ',' << mmc.valueList[i];
    }
    return os;
}
