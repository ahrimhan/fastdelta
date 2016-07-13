//
//  RefactoringOperation.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 2..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef RefactoringOperation_hpp
#define RefactoringOperation_hpp

#include <stdio.h>

typedef enum {
    RT_MOVE_METHOD,
    RT_MOVE_FIELD,
    RT_CREATE_CLASS,
    RT_REMOVE_CLASS
} RefactoringType;


class RefactoringOperation
{
public:
    virtual RefactoringType getType() = 0;
};

#endif /* RefactoringOperation_hpp */
