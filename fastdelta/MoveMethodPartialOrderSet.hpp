//
//  MoveMethodPartialOrderSet.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 8. 8..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef MoveMethodPartialOrderSet_hpp
#define MoveMethodPartialOrderSet_hpp

#include "MoveMethodCandidate.hpp"
#include <set>

#pragma GCC visibility push(default)


class MoveMethodPartialOrderIterator
{
private:
    std::vector<MoveMethodCandidate*> queue;
    MoveMethodCandidateSet visitedNode;
public:
    MoveMethodPartialOrderIterator(MoveMethodCandidateSet& firstSet)
    {
        queue.insert(queue.end(), firstSet.begin(), firstSet.end());
        visitedNode.insert(firstSet.begin(), firstSet.end());
    }
    
    MoveMethodPartialOrderIterator(const MoveMethodCandidateSet& firstSet)
    {
        queue.insert(queue.end(), firstSet.begin(), firstSet.end());
        visitedNode.insert(firstSet.begin(), firstSet.end());
    }
    
    bool hasNext();
    MoveMethodCandidate* next();
};



class MoveMethodPartialOrderSet
{
private:
    MoveMethodCandidateSet firstSet;
    MoveMethodCandidateContainer allCandidate;
    
public:
    void set(int _entityIdx, int _toClassIdx, float* values);
    MoveMethodCandidatePtr get(int _entityIdx, int _toClassIdx);
    MoveMethodPartialOrderIterator* getIterator() const;
    MoveMethodPartialOrderIterator* getIterator();

    friend bool operator== (const MoveMethodPartialOrderSet& lhs, const MoveMethodPartialOrderSet& rhs);
};

bool operator== (const MoveMethodPartialOrderSet& lhs, const MoveMethodPartialOrderSet& rhs);


#pragma GCC visibility pop


#endif /* MoveMethodPartialOrderSet_hpp */
