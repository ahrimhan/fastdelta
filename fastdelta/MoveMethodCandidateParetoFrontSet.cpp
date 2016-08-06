//
//  MoveMethodCandidateParetoFrontContainer.cpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 8. 5..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#include "MoveMethodCandidateParetoFrontSet.hpp"


#pragma mark - MoveMethodCandidateParetoFront

void MoveMethodCandidateParetoFront::put(MoveMethodCandidatePtr candidate)
{
    container.get<0>().insert(candidate);
}

MoveMethodCandidatePtr MoveMethodCandidateParetoFront::get(int _entityIdx, int _toClassIdx)
{
    MoveMethodCandidateContainerIndex::iterator it = container.get<0>().find(MoveMethodCandidate::createID(_entityIdx, _toClassIdx));
    
    if( it != container.get<0>().end() )
    {
        return *it;
    }
    else
    {
        return NULL;
    }
}

void MoveMethodCandidateParetoFront::remove(int _entityIdx, int _toClassIdx)
{
    container.get<0>().erase(MoveMethodCandidate::createID(_entityIdx, _toClassIdx));
}

void MoveMethodCandidateParetoFront::remove(MoveMethodCandidateContainerIndex::iterator it)
{
    container.get<0>().erase(it);
}

MoveMethodCandidateContainerIndex::iterator MoveMethodCandidateParetoFront::begin() const
{
    return container.get<0>().begin();
}

MoveMethodCandidateContainerIndex::iterator MoveMethodCandidateParetoFront::end() const
{
    return container.get<0>().end();
}


#pragma mark - MoveMethodCandidateParetoFrontIterator

bool MoveMethodCandidateParetoFrontIterator::hasNext()
{
    
    if( currentParetoFront != paretoContainer.get<0>().end() )
    {
        return true;
    }
    
    return false;
}

MoveMethodCandidatePtr MoveMethodCandidateParetoFrontIterator::next()
{
    if( currentCandidate != (*currentParetoFront)->end() &&
       currentParetoFront != paretoContainer.get<0>().end() )
    {
        MoveMethodCandidatePtr ret = (*currentCandidate);
        std::cout << *ret << std::endl;
        currentCandidate++;
        
        while( currentCandidate == (*currentParetoFront)->end() )
        {
            currentParetoFront++;
            
            std::cout << "-----------------------------\n";
            
            if( currentParetoFront != paretoContainer.get<0>().end() )
            {
                currentCandidate = (*currentParetoFront)->begin();
            }
            else
            {
                break;
            }
        }
        
        return ret;
    }

    return NULL;
}

#pragma mark - MoveMethodCandidateParetoFrontSet

int compareVector(float* lhs, float* rhs)
{
    int l = 0;
    int w = 0;
    
    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
    {
        if( lhs[i] < rhs[i] )
        {
            w++;
        }
        else if( lhs[i] > rhs[i] )
        {
            l++;
        }
    }
    
    if( w == 0 && l > 0 )
    {
        return 1;
    }
    else if( l == 0 && w > 0 )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int compareVector2Bucket(float* lhs, MoveMethodCandidateParetoFrontPtr rhs)
{
    int w = 0;
    int l = 0;
    int e = 0;
    
    for( MoveMethodCandidateContainerIndex::iterator it = rhs->begin();
        it != rhs->end();
        it++ )
    {
        int res = compareVector(lhs, (*it)->valueList);
        if( res < 0 )
        {
            w++;
        }
        else if( res > 0 )
        {
            l++;
        }
        else
        {
            e++;
        }
    }
    
    if( l == 0 && w > 0 ) return -1;
    else if( w == 0 && l > 0 ) return 1;
//    else if( l + w == 0 ) return 0;
    
    return 0;
    
    throw "Bucket is contaminated!!";
}

void MoveMethodCandidateParetoFrontSet::set(int _entityIdx, int _toClassIdx, float* values)
{
    uint64_t id = MoveMethodCandidate::createID(_entityIdx, _toClassIdx);
    
    bool hasNeg = false;
    
    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
    {
        if( values[i] < 0 )
        {
            hasNeg = true;
        }
    }
    
    MoveMethodCandidateContainerIndex& allCandidateIndex = allCandidateContainer.get<0>();
    
    MoveMethodCandidateContainerIndex::iterator it = allCandidateIndex.find(id);
    
    MoveMethodCandidatePtr mmcPtr;
    
    if( it != allCandidateIndex.end() )
    {
        mmcPtr = (*it);
        
        MoveMethodCandidateParetoFrontPtr paretoFront = mmcPtr->belongingParetoFront;
        mmcPtr->belongingParetoFront = NULL;
        
        paretoFront->remove(_entityIdx, _toClassIdx);
        
        
        if( !hasNeg )
        {
            allCandidateIndex.erase(it);
        }
    }
    else if( hasNeg )
    {
        mmcPtr = MoveMethodCandidatePtr(new MoveMethodCandidate(_entityIdx, _toClassIdx, values));
        allCandidateIndex.insert(mmcPtr);
    }
    
    if( hasNeg )
    {
        MoveMethodCandidateParetoContainerIndex& paretoContainerIndex = paretoFrontContainer.get<0>();
        
        
        MoveMethodCandidateParetoContainerIndex::iterator paretoContainerIterator = paretoContainerIndex.begin();
        for( ;
            paretoContainerIterator != paretoContainerIndex.end(); paretoContainerIterator++ )
        {
            MoveMethodCandidateParetoFrontPtr paretoFrontPtr = (*paretoContainerIterator);
            
            
            int res = compareVector2Bucket(values, paretoFrontPtr);
            if( res == 0 )
            {
                mmcPtr->belongingParetoFront = paretoFrontPtr;
                paretoFrontPtr->put(mmcPtr);
                break;
            }
            else if( res < 0 )
            {
                MoveMethodCandidateParetoFrontPtr newParetoFrontPtr(new MoveMethodCandidateParetoFront());
                newParetoFrontPtr->put(mmcPtr);
                mmcPtr->belongingParetoFront = newParetoFrontPtr;
                paretoContainerIndex.insert(paretoContainerIterator, newParetoFrontPtr);
                
                break;
            }
        }
        
        if( paretoContainerIterator == paretoContainerIndex.end() )
        {
            MoveMethodCandidateParetoFrontPtr newParetoFrontPtr(new MoveMethodCandidateParetoFront());
            newParetoFrontPtr->put(mmcPtr);
            mmcPtr->belongingParetoFront = newParetoFrontPtr;
            paretoContainerIndex.insert(paretoContainerIterator, newParetoFrontPtr);
        }
    }
 
}

MoveMethodCandidatePtr MoveMethodCandidateParetoFrontSet::get(int _entityIdx, int _toClassIdx)
{
    uint64_t id = MoveMethodCandidate::createID(_entityIdx, _toClassIdx);
    MoveMethodCandidateContainerIndex& allCandidateContainerIndex = allCandidateContainer.get<0>();
    MoveMethodCandidateContainerIndex::iterator it = allCandidateContainerIndex.find(id);
    
    if( it != allCandidateContainerIndex.end() )
    {
        return (*it);
    }
    
    return NULL;
}

MoveMethodCandidateParetoFrontIterator* MoveMethodCandidateParetoFrontSet::getIterator()
{
    return new MoveMethodCandidateParetoFrontIterator(paretoFrontContainer);
}


bool operator== (const MoveMethodCandidateParetoFrontSet& lhs, const MoveMethodCandidateParetoFrontSet& rhs)
{
    return lhs.paretoFrontContainer == rhs.paretoFrontContainer;
}