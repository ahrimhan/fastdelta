//
//  DeltaMatrix.cpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/20/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#include "DeltaMatrix.hpp"
#include <iostream>
#include <stdio.h>
#include <libiomp/omp.h>

bool operator< (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
    return lhs.value < rhs.value;
}

bool operator> (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
    return lhs.value > rhs.value;
}

bool operator== (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
//    fprintf(stderr, "equality check! id(%llX, %llX), entityIdx(%x, %x), toClassIdx(%x, %x), value(%f, %f)\n",
//            lhs.id, rhs.id, lhs.entityIdx, rhs.entityIdx, lhs.toClassIdx, rhs.toClassIdx,
//            lhs.value, rhs.value );
    return lhs.id == rhs.id && lhs.value == rhs.value;
}

bool operator!= (const MoveMethodCandidate& lhs, const MoveMethodCandidate& rhs)
{
    return !(lhs == rhs);
}

void DeltaMatrix::internal_init(DeltaMatrix& dm, DeltaMatrixInfo* info)
{
    printf("thread count: %d\n", Eigen::nbThreads( ));
    dm.classCount = info->getClassCount();
    dm.entityCount = info->getEntityCount();
    dm.methodCount = info->getMethodCount();
    dm.fieldCount = dm.entityCount - dm.methodCount;
    
    int entityCount = dm.entityCount;
    int classCount = dm.classCount;
    int methodCount = dm.methodCount;
    int fieldCount = dm.fieldCount;
        

    SpRowMat mr(entityCount, classCount);
    SpRowMat l(entityCount, entityCount);
    SpRowMat identity(methodCount, methodCount);
    
    identity.setIdentity();
    
    mr.setFromTriplets(info->membershipInfoList.begin(), info->membershipInfoList.end());
    SpColMat mc = mr;
    SpColMat mtc = mr.transpose();
    SpColMat mic = DColMat::Constant(entityCount, classCount, 1) - mc;
    uint64_t start2 = getTimestamp();
    SpRowMat mmt = (mr * mtc);
    printf("mr * mtc = %u (%d by %d)\n", getTimestamp() - start2, mr.rows(), mr.cols());

    
 
    l.setFromTriplets(info->linkInfoList.begin(), info->linkInfoList.end());
    
    dm.l = l;
#ifndef DONT_USE_COHESION
    SpRowMat methodToField = l.topRightCorner(methodCount, fieldCount);
    
    uint64_t start = getTimestamp();
    SpRowMat t = methodToField * methodToField.transpose();
    printf("mtf * mtf_T = %u (%d by %d) nonZeros=%d\n", getTimestamp() - start, methodToField.rows(), methodToField.cols(), methodToField.nonZeros());
    
    t = t - t.cwiseProduct(identity);
    
    t.conservativeResize(entityCount, entityCount);
    
    dm.l += t * COHESION_WEIGHT;
#endif
    
//    for( int i = 0; i < methodCount; i++ )
//    {
//        dm.entities.push_back(DeltaMatrixEntity(i, true));
//    }
//    
//    for( int i = methodCount; i < entityCount; i++ )
//    {
//        dm.entities.push_back(DeltaMatrixEntity(i, false));
//    }
//    
//    for (int i=0; i < dm.l.outerSize(); i++)
//    {
//        for (SpRowMat::InnerIterator it(dm.l,i); it; ++it)
//        {
//            dm.entities.at(it.row()).addRelatedEntity(&(dm.entities.at(it.col())));
//        }
//    }
    
    SpRowMat lint = dm.l.cwiseProduct(mmt);
    SpRowMat lext = dm.l.cwiseProduct(DRowMat::Constant(entityCount, entityCount, 1) - mmt);
    
    dm.mr = mr;
    dm.mc = mc;
    dm.mtc = mtc;
    dm.mic = mic;
    dm.mmt = mmt;
    dm.lint = lint;
    dm.lext = lext;
    dm.af.resize(entityCount, classCount);
    dm.possibleMoveMethodMatrix.resize(entityCount, classCount);
    
    
    dm.possibleMoveMethodMatrix.setFromTriplets(info->possibleMoveMethodList.begin(), info->possibleMoveMethodList.end());
    
    dm.eval();
    
    dm.prevD = dm.D;
    dm.prevAdjustedD = dm.adjustedD;
    
    dm.createMoveMethodCandidateSet(dm.moveMethodCandidateSet);
    
//    SpRowMat pint = lint * mic;
//    SpRowMat pext = lext * mc;
//    SpRowMat pintq = pint + DRowMat::Constant(entityCount, classCount, 1);
//
//#ifndef DONT_USE_QUOTIENT
//    dm.D =  (pint - pext).cwiseQuotient(pintq);
//#else
//    dm.D = pint - pext;
//#endif
    
    

}



void DeltaMatrix::init(DeltaMatrixInfo* info)
{
    DeltaMatrix::internal_init(*this, info);
}

void DeltaMatrix::move(int entityIdx, int fromClassIdx, int toClassIdx)
{
    changeMembership(entityIdx, fromClassIdx, 0);
    changeMembership(entityIdx, toClassIdx, 1);
    
    SpRowMat::RowXpr mr_row = mr.row(entityIdx);
    
    SpRowMat mmt_row = mr_row * mtc;
    mmt.row(entityIdx) = mmt_row;

    mmt.reserve(VectorXi::Constant(entityCount, 2));
    
    for( int i = 0; i < mtc.cols(); i++ )
    {
        mmt.coeffRef(i, entityIdx) = mmt.coeff(entityIdx, i);
    }
    
    lint.row(entityIdx) = l.row(entityIdx).cwiseProduct(mmt_row);
    lext.row(entityIdx) = l.row(entityIdx).cwiseProduct(DRowMat::Constant(1, entityCount, 1) - mmt_row);
    
    lint.reserve(VectorXi::Constant(entityCount, 2));
    lext.reserve(VectorXi::Constant(entityCount, 2));
    for( int i = 0; i < mmt.cols(); i++ )
    {
        lint.coeffRef(i, entityIdx) = lint.coeff(entityIdx, i);
        lext.coeffRef(i, entityIdx) = lext.coeff(entityIdx, i);
    }
    
    lint.makeCompressed();
    lext.makeCompressed();
}

void DeltaMatrix::eval()
{
    uint64_t start = getTimestamp();
    
    prevD = D;

    
    DRowMat pint = lint * mic;
    SpRowMat pext = (lext * mc).pruned();
    
    
    SpRowMat spView = pint.sparseView();
    SpRowMat spViewQ = spView + DColMat::Constant(entityCount, classCount, 1);
    
#ifndef DONT_USE_QUOTIENT
    D = (spView - pext).cwiseProduct(possibleMoveMethodMatrix).cwiseQuotient(spViewQ).pruned();
#else
    D = (spView - pext).cwiseProduct(possibleMoveMethodMatrix).pruned();
#endif
    
    if( useAdjust )
    {
        prevAdjustedD = adjustedD;
        adjustedD = (D + af).pruned();
        af *= ADJUST_DECAY_RATE;
    }
    
    fprintf(stderr, "Elapsed time for native-eval:%u\n", getTimestamp() - start);
}


void DeltaMatrix::createMoveMethodCandidateSet(MoveMethodCandidateSet& candidateSet)
{
    SpRowMat& DD = useAdjust ? adjustedD : D;
    omp_set_nested(0);
    
    candidateSet.clear();
    MoveMethodCandidateSetIndexByValue& index = candidateSet.get<0>();
    index.clear();
    
    for (int i=0; i < DD.outerSize(); i++)
    {
        for (SpRowMat::InnerIterator it(DD,i); it; ++it)
        {
            if( it.row() < methodCount )
            {
                if( it.value() < 0 )
                {
                    index.insert(MoveMethodCandidate(it.row(), it.col(), it.value()));
                }
            }
            else
            {
                break;
            }
        }
    }
}


MoveMethodCandidateSetIndexByValue& DeltaMatrix::getSortedMoveMethodCandidates()
{
//    printf("prevD: %d, %d\n", prevD.rows(), prevD.cols());
//    printf("prevAdjustedD: %d, %d\n", prevAdjustedD.rows(), prevAdjustedD.cols());
    
    SpRowMat& DD = useAdjust ? adjustedD : D;
    SpRowMat& prevDD = useAdjust ? prevAdjustedD : prevD;
    
    uint64_t start, interm, end;
    
    start = getTimestamp();
    
    interm = getTimestamp();
    
    SpRowMat diffMat = (DD - prevDD).pruned();
    
    MoveMethodCandidateSetIndexByID& index = moveMethodCandidateSet.get<1>();


    end = getTimestamp();
    fprintf(stderr, "Build: %s %s\n", __DATE__, __TIME__);
    fprintf(stderr, "total: %llu, masking: %llu, minus: %llu\n", end-start, interm-start, end-interm);


//    printf("DDD nonZeroSize:%d\n", DD.nonZeros());
//    printf("prevDDD nonZeroSize:%d\n", prevDD.nonZeros());
//    printf("diffMat nonZeroSize:%d\n", diffMat.nonZeros());
    
    for (int i=0; i < diffMat.outerSize(); i++)
    {
        for (SpRowMat::InnerIterator it(diffMat,i); it; ++it)
        {
            if( it.row() < methodCount )
            {
                float value = DD.coeff(it.row(), it.col());
                MoveMethodCandidateSetIndexByID::iterator it2 = index.find(MoveMethodCandidate::createID(it.row(), it.col()));
                if( it2 != index.end() )
                {
                    if( value < 0 )
                    {
                        index.replace(it2, MoveMethodCandidate(it.row(), it.col(), value));
                    }
                    else
                    {
                        index.erase(it2);
                    }
                }
                else if( value < 0 )
                {
                    index.insert(MoveMethodCandidate(it.row(), it.col(), value));
                }
            }
            else
            {
                break;
            }
        }
    }
    
    
    
    return moveMethodCandidateSet.get<0>();
}