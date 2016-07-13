//
//  RemoveClassOperation.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 2..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef RemoveClassOperation_hpp
#define RemoveClassOperation_hpp

#include <stdio.h>
#include "RefactoringOperation.hpp"


class RemoveClassOperation : public RefactoringOperation
{
public:
    int removingClassIdx;

    RemoveClassOperation(int classIdx) : removingClassIdx(classIdx)
    {
    }
    
    virtual RefactoringType getType() {
        return RT_REMOVE_CLASS;
    }
};

#endif /* RemoveClassOperation_hpp */
