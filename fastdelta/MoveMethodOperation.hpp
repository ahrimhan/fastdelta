//
//  MoveMethodOperation.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 2..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef MoveMethodOperation_hpp
#define MoveMethodOperation_hpp

#include <stdio.h>
#include "RefactoringOperation.hpp"

class MoveMethodOperation : public RefactoringOperation
{
public:
    int entityIdx;
    int fromClassIdx;
    int toClassIdx;

    
    MoveMethodOperation(int _entityIdx, int _fromClassIdx, int _toClassIdx)
    : entityIdx(_entityIdx), fromClassIdx(_fromClassIdx), toClassIdx(_toClassIdx)
    {
    }
    
    
    virtual RefactoringType getType() {
        return RT_MOVE_METHOD;
    }
};

#endif /* MoveMethodOperation_hpp */
