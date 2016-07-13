//
//  MoveFieldOperation.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 2..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef MoveFieldOperation_hpp
#define MoveFieldOperation_hpp

#include <stdio.h>
#include "RefactoringOperation.hpp"

class MoveFieldOperation : public RefactoringOperation
{
public:
    int entityIdx;
    int fromClassIdx;
    int toClassIdx;
    
    
    MoveFieldOperation(int _entityIdx, int _fromClassIdx, int _toClassIdx)
    : entityIdx(_entityIdx), fromClassIdx(_fromClassIdx), toClassIdx(_toClassIdx)
    {
    }
    
    
    virtual RefactoringType getType() {
        return RT_MOVE_FIELD;
    }
};
#endif /* MoveFieldOperation_hpp */
