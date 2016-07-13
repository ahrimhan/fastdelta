//
//  CreateClassOperation.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 2..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef CreateClassOperation_hpp
#define CreateClassOperation_hpp

#include <stdio.h>
#include "RefactoringOperation.hpp"
#include <functional>


class CreateClassOperation : public RefactoringOperation
{
    std::function<void(int)> callback;
    
public:
    
    CreateClassOperation(std::function<void(int)> cb) : callback(cb)
    {
    }
    
    void done(int createdClassIdx)
    {
        if( callback != NULL )
        {
            callback(createdClassIdx);
        }
    }
    
    virtual RefactoringType getType() {
        return RT_CREATE_CLASS;
    }
};
#endif /* CreateClassOperation_hpp */
