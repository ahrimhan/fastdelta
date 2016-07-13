//
//  DeltaMatrixEntity.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/28/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#ifndef DeltaMatrixEntity_hpp
#define DeltaMatrixEntity_hpp

#include <stdio.h>
#include <vector>

class DeltaMatrixEntity;


class DeltaMatrixEntity
{
private:
    std::vector<DeltaMatrixEntity*> relatedEntities;
    std::vector<DeltaMatrixEntity*> relatedMethods;
    std::vector<DeltaMatrixEntity*> relatedFields;
    int idx;
    bool method;
    
public:
    DeltaMatrixEntity(int index, bool isMethod) : idx(index), method(isMethod)
    {
    }
    
    
    void addRelatedEntity(DeltaMatrixEntity* entity)
    {
        if( entity->method )
        {
            relatedMethods.push_back(entity);
        }
        else
        {
            relatedFields.push_back(entity);
        }
        
        relatedEntities.push_back(entity);
    }
    
    std::vector<DeltaMatrixEntity*>& getRelatedEntities()
    {
        return relatedEntities;
    }
    
    std::vector<DeltaMatrixEntity*>& getRelatedMethods()
    {
        return relatedMethods;
    }
    
    std::vector<DeltaMatrixEntity*>& getRelatedFields()
    {
        return relatedFields;
    }
};

#endif /* DeltaMatrixEntity_hpp */
