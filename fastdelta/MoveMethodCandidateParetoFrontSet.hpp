//
//  MoveMethodCandidateParetoFrontContainer.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 8. 5..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef MoveMethodCandidateParetoFrontSet_hpp
#define MoveMethodCandidateParetoFrontSet_hpp

#include "MoveMethodCandidate.hpp"

#include <boost/multi_index/sequenced_index.hpp>
#include <boost/shared_ptr.hpp>

#pragma GCC visibility push(default)

typedef boost::multi_index_container<
MoveMethodCandidatePtr,
boost::multi_index::indexed_by<
boost::multi_index::ordered_unique<
boost::multi_index::member<MoveMethodCandidate,uint64_t,&MoveMethodCandidate::id>,
std::less<uint64_t>
>
>
> MoveMethodCandidateContainer;
typedef MoveMethodCandidateContainer::nth_index<0>::type MoveMethodCandidateContainerIndex;


class MoveMethodCandidateParetoFront
{
    MoveMethodCandidateContainer container;
public:
    MoveMethodCandidateParetoFront()
    {
        
    }
    
    void put(MoveMethodCandidatePtr candidate);
    MoveMethodCandidatePtr get(int _entityIdx, int _toClassIdx);
    void remove(int _entityIdx, int _toClassIdx);
    void remove(MoveMethodCandidateContainerIndex::iterator it);
    
    MoveMethodCandidateContainerIndex::iterator begin() const;
    MoveMethodCandidateContainerIndex::iterator end() const;
    
    size_t size()
    {
        return container.size();
    }
};



typedef boost::multi_index_container<
    MoveMethodCandidateParetoFrontPtr,
    boost::multi_index::indexed_by<boost::multi_index::sequenced<>>
> MoveMethodCandidateParetoContainer;
typedef MoveMethodCandidateParetoContainer::nth_index<0>::type MoveMethodCandidateParetoContainerIndex;


class MoveMethodCandidateParetoFrontIterator
{
private:
    MoveMethodCandidateParetoContainer& paretoContainer;
    MoveMethodCandidateParetoContainerIndex::iterator currentParetoFront;
    MoveMethodCandidateContainerIndex::iterator currentCandidate;

public:
    MoveMethodCandidateParetoFrontIterator(MoveMethodCandidateParetoContainer& _paretoContainer)
    : paretoContainer(_paretoContainer)
    {
        currentParetoFront = paretoContainer.get<0>().begin();
        currentCandidate = (*currentParetoFront)->begin();
    }
    
    MoveMethodCandidateParetoFrontIterator(const MoveMethodCandidateParetoFrontIterator& iterator)
    : paretoContainer(iterator.paretoContainer), currentParetoFront(iterator.currentParetoFront),
    currentCandidate(iterator.currentCandidate)
    {
        
    }
    bool hasNext();
    MoveMethodCandidatePtr next();
};

class MoveMethodCandidateParetoFrontSet
{
private:
    MoveMethodCandidateParetoContainer paretoFrontContainer;
    MoveMethodCandidateContainer allCandidateContainer;
    
    
public:
    MoveMethodCandidateParetoFrontSet()
    {
        
    }
    
    void set(int _entityIdx, int _toClassIdx, float* values);
    MoveMethodCandidatePtr get(int _entityIdx, int _toClassIdx);
    MoveMethodCandidateParetoFrontIterator* getIterator();
    
    friend class MoveMethodCandidateParetoFrontIterator;
    friend bool operator== (const MoveMethodCandidateParetoFrontSet& lhs, const MoveMethodCandidateParetoFrontSet& rhs);

};


bool operator== (const MoveMethodCandidateParetoFrontSet& lhs, const MoveMethodCandidateParetoFrontSet& rhs);


#pragma GCC visibility pop

#endif /* MoveMethodCandidateParetoFrontContainer_hpp */
