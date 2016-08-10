//
//  MoveMethodPartialOrderSet.cpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 8. 8..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#include "MoveMethodPartialOrderSet.hpp"




bool MoveMethodPartialOrderIterator::hasNext()
{
    return queue.size() > 0;
}


MoveMethodCandidate* MoveMethodPartialOrderIterator::next()
{
    if( queue.size() == 0 ) return NULL;
    
    MoveMethodCandidate* ret = queue.front();
    queue.erase(queue.begin());
    
    for( MoveMethodCandidateSet::iterator it = ret->worseCandidate.begin();
        it != ret->worseCandidate.end(); it++ )
    {
        if( visitedNode.find(*it) == visitedNode.end() )
        {
            queue.push_back(*it);
            visitedNode.insert(*it);
        }
    }
    
    return ret;
}




//static int compareVector(float* lhs, float* rhs)
//{
//    int l = 0;
//    int w = 0;
//    
//    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
//    {
//        if( lhs[i] < rhs[i] )
//        {
//            w++;
//        }
//        else if( lhs[i] > rhs[i] )
//        {
//            l++;
//        }
//    }
//    
//    if( w == 0 && l > 0 )
//    {
//        return 1;
//    }
//    else if( l == 0 && w > 0 )
//    {
//        return -1;
//    }
//    else
//    {
//        return 0;
//    }
//}

typedef union {
    float f;
    struct {
        unsigned int mantisa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } parts;
} double_cast;


static int fastCompareVector(float* lhs, float* rhs)
{
//    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
//    {
//        if( lhs[i] < rhs[i] )
//        {
//            l|=0x1;
//        }
//        else if( lhs[i] > rhs[i] )
//        {
//            l|=0x2;
//        }
//    }
    int l = 0;
    double_cast f1;
    double_cast f2;
    
    f1.f = (lhs[0] - rhs[0]);
    f2.f = (rhs[0] - lhs[0]);
    
    l |= f1.parts.sign | f2.parts.sign << 1;

    f1.f = (lhs[1] - rhs[1]);
    f2.f = (rhs[1] - lhs[1]);

    l |= f1.parts.sign | f2.parts.sign << 1;

    return l;
}

static int compareCandidate(MoveMethodCandidate* a, MoveMethodCandidate* b)
{
    return fastCompareVector(a->valueList, b->valueList);
}



static void linkCandidates(MoveMethodCandidate* betterOne, MoveMethodCandidate* worseOne)
{
    betterOne->worseCandidate.insert(worseOne);
    worseOne->betterCandidate.insert(betterOne);
}

static void unlinkCandidates(MoveMethodCandidate* betterOne, MoveMethodCandidate* worseOne)
{
    betterOne->worseCandidate.erase(worseOne);
    worseOne->betterCandidate.erase(betterOne);
}

void MoveMethodPartialOrderSet::set(int _entityIdx, int _toClassIdx, float* values)
{
    
    MoveMethodCandidate* mmcPtr;
    
    bool hasNeg = false;
    
    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
    {
        if( values[i] < 0 )
        {
            hasNeg = true;
        }
    }
    
    if( !hasNeg )
    {
        return;
    }

    MoveMethodCandidateContainerIndex::iterator findIt = allCandidate.get<0>().find(MoveMethodCandidate::createID(_entityIdx, _toClassIdx));

    
    if( findIt != allCandidate.get<0>().end() )
    {
        mmcPtr = (*findIt).get();
        for( MoveMethodCandidateSet::iterator it2 = mmcPtr->betterCandidate.begin();
            it2 != mmcPtr->betterCandidate.end(); it2++ )
        {
            (*it2)->worseCandidate.erase(mmcPtr);
            
            for( MoveMethodCandidateSet::iterator it3 = mmcPtr->worseCandidate.begin();
                it3 != mmcPtr->worseCandidate.end(); it3++ )
            {
                (*it3)->betterCandidate.erase(mmcPtr);
                
                (*it3)->betterCandidate.insert((*it2));
                (*it2)->worseCandidate.insert((*it3));
            }
        }
        
        mmcPtr->betterCandidate.clear();
        mmcPtr->worseCandidate.clear();
        
        mmcPtr->updateValue(values);
    }
    else
    {
        mmcPtr = new MoveMethodCandidate(_entityIdx, _toClassIdx, values);
        allCandidate.get<0>().insert(MoveMethodCandidatePtr(mmcPtr));
    }
    
    
    for( MoveMethodCandidateContainerIndex::iterator containerIt = allCandidate.get<0>().begin();
        containerIt != allCandidate.get<0>().end(); ++containerIt )
    {
        MoveMethodCandidate* existingCandidatePtr = (*containerIt).get();
        int res = compareCandidate(mmcPtr, existingCandidatePtr);
        if( res == 1 )
        {
            // mmcPtr is better than existingCandidatePtr
            
            bool needToLink = true;
            
            for( MoveMethodCandidateSet::iterator setIt = mmcPtr->worseCandidate.begin();
                setIt != mmcPtr->worseCandidate.end(); )
            {
                MoveMethodCandidateSet::iterator currentSetIt = setIt++;
                MoveMethodCandidate* candidateInWorseSetPtr = (*currentSetIt);
                int res2 = compareCandidate(candidateInWorseSetPtr, existingCandidatePtr);
                if( res2 == 1 )
                {
                    needToLink = false;
                    break;
                }
                else if( res2 == 2 )
                {
                    unlinkCandidates(mmcPtr, candidateInWorseSetPtr);
                }
            }
            
            if( needToLink )
            {
                linkCandidates(mmcPtr, existingCandidatePtr);
            }
            
            MoveMethodCandidateSet::iterator firstSetIt =firstSet.find(existingCandidatePtr);
            if( firstSetIt != firstSet.end() )
            {
                firstSet.erase(firstSetIt);
            }
        }
        else if( res == 2 )
        {
            // mmcPtr is worse than exisingCandidatePtr
            
            bool needToLink = true;
            
            for( MoveMethodCandidateSet::iterator setIt = mmcPtr->betterCandidate.begin();
                setIt != mmcPtr->betterCandidate.end(); )
            {
                MoveMethodCandidateSet::iterator currentSetIt = setIt++;
                MoveMethodCandidate* candidateInBetterSetPtr = (*currentSetIt);
                int res2 = compareCandidate(candidateInBetterSetPtr, existingCandidatePtr);
                if( res2 == 1 )
                {
                    unlinkCandidates(candidateInBetterSetPtr, mmcPtr);
                }
                else if( res2 == 2 )
                {
                    needToLink = false;
                    break;
                }
            }
            
            if( needToLink )
            {
                linkCandidates(existingCandidatePtr, mmcPtr);
            }
        }
    }
    
    for( MoveMethodCandidateSet::iterator setIt1 = mmcPtr->betterCandidate.begin();
       setIt1 != mmcPtr->betterCandidate.end(); setIt1++ )
    {
        for( MoveMethodCandidateSet::iterator setIt2 = mmcPtr->worseCandidate.begin();
            setIt2 != mmcPtr->worseCandidate.end(); setIt2++ )
        {
            MoveMethodCandidate* candidateInBetterSetPtr = (*setIt1);
            MoveMethodCandidate* candidateInWorseSetPtr = (*setIt2);
            
            
            MoveMethodCandidateSet::iterator findSetIt = candidateInBetterSetPtr->worseCandidate.find(candidateInWorseSetPtr);
            
            unlinkCandidates(candidateInBetterSetPtr, candidateInWorseSetPtr);
        }
    }
    
    // transitive reduction between better and worse set
    
    if( mmcPtr->betterCandidate.size() == 0 )
    {
        firstSet.insert(mmcPtr);
    }
    
    if( allCandidate.size() % 1000 == 0 )
    {
        fprintf(stderr, "size: %d\n", allCandidate.size());
    }
}

MoveMethodCandidatePtr MoveMethodPartialOrderSet::get(int _entityIdx, int _toClassIdx)
{
    MoveMethodCandidateContainerIndex::iterator findIt = allCandidate.get<0>().find(MoveMethodCandidate::createID(_entityIdx, _toClassIdx));
    
    
    if( findIt != allCandidate.get<0>().end() )
    {
        return (*findIt);
    }
    
    return NULL;
}

MoveMethodPartialOrderIterator* MoveMethodPartialOrderSet::getIterator()
{
    return new MoveMethodPartialOrderIterator(firstSet);
}

MoveMethodPartialOrderIterator* MoveMethodPartialOrderSet::getIterator() const
{
    return new MoveMethodPartialOrderIterator(firstSet);
}

bool operator== (const MoveMethodPartialOrderSet& lhs, const MoveMethodPartialOrderSet& rhs)
{
    if( lhs.allCandidate.size() != rhs.allCandidate.size() )
    {
        return false;
    }
    
    MoveMethodPartialOrderIterator* lhsIterator = lhs.getIterator();
    MoveMethodPartialOrderIterator* rhsIterator = rhs.getIterator();
    
    bool ret = true;
    
    while(lhsIterator->hasNext())
    {
        MoveMethodCandidate* lhsElem = lhsIterator->next();
        MoveMethodCandidate* rhsElem = rhsIterator->next();
        
        if( *lhsElem != *rhsElem )
        {
            ret = false;
            break;
        }
    }
    
    delete(lhsIterator);
    delete(rhsIterator);
    
    return ret;
}
