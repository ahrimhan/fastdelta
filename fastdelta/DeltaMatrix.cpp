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
#include "LinkMatrix.hpp"

void DeltaMatrix::initMembership(DeltaMatrixInfo* info)
{
    mr.resize(entityCount, classCount);
    mr.setFromTriplets(info->membershipInfoList.begin(), info->membershipInfoList.end());
    mc = mr;
    mtc = mr.transpose();
    mr.conservativeResize(methodPossibleToMoveCount, classCount);
    mmt = (mr * mtc);
}

void DeltaMatrix::createCouplingBasedLink(DeltaMatrixInfo* info, float methodCallWeight, float fieldAccessWeight, LinkMatrix& link, bool useQuotient)
{
    SpRowMat lm(entityCount, entityCount);
    SpRowMat lf(entityCount, entityCount);
    
    lm.setFromTriplets(info->methodCallList.begin(), info->methodCallList.end());
    lf.setFromTriplets(info->fieldAccessList.begin(), info->fieldAccessList.end());
    
    link.initialize(lm * methodCallWeight + lf * fieldAccessWeight, useQuotient);
    
}

void DeltaMatrix::createSharedFieldAccessLink(DeltaMatrixInfo* info, LinkMatrix& link)
{
    SpRowMat l(entityCount, entityCount);
    SpRowMat identity(methodCount, methodCount);
    identity.setIdentity();
    
    l.setFromTriplets(info->methodCallList.begin(), info->methodCallList.end());
    l.setFromTriplets(info->fieldAccessList.begin(), info->fieldAccessList.end());
    
    SpRowMat methodToField = l.topRightCorner(methodCount, entityCount - methodCount);
    SpRowMat ssm = methodToField * methodToField.transpose();
    
    ssm -= ssm.cwiseProduct(identity);
    
    link.initialize(ssm, false);
}

void DeltaMatrix::createSharedMethodCallLink(DeltaMatrixInfo* info, LinkMatrix& link)
{
    SpRowMat l(entityCount, entityCount);
    SpRowMat identity(methodCount, methodCount);
    identity.setIdentity();
    
    l.setFromTriplets(info->methodCallList.begin(), info->methodCallList.end());
    l.setFromTriplets(info->fieldAccessList.begin(), info->fieldAccessList.end());
    
    l.conservativeResize(methodCount, methodCount);
    SpRowMat ssm = l * l.transpose();
    ssm -= ssm.cwiseProduct(identity);
    
    link.initialize(ssm, false);
}

void DeltaMatrix::init(DeltaMatrixInfo* info)
{
    classCount = info->getClassCount();
    entityCount = info->getEntityCount();
    methodCount = info->getMethodCount();
    methodPossibleToMoveCount = info->getMethodPossibleToMoveCount();
    
    initMembership(info);

    possibleMoveMethodMatrix.resize(methodPossibleToMoveCount, classCount);
    
    possibleMoveMethodMatrix.setFromTriplets(info->possibleMoveMethodList.begin(),
                                             info->possibleMoveMethodList.end());
    
    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
    {
        linkMatrixList[i].dm = this;
    }
    
    createCouplingBasedLink(info, 1.0, 1.0, linkMatrixList[0], false);
    createCouplingBasedLink(info, 0.7, 1.0, linkMatrixList[1], true);
//    createCouplingBasedLink(info, 1.0, 0.7, linkMatrixList[2], true);
//    createSharedFieldAccessLink(info, linkMatrixList[3]);
//    createSharedMethodCallLink(info, linkMatrixList[3]);
    
    createMoveMethodCandidateSet(moveMethodCandidateParetoFrontSet);
}


void DeltaMatrix::move(int entityIdx, int fromClassIdx, int toClassIdx)
{
    changeMembership(entityIdx, fromClassIdx, 0);
    changeMembership(entityIdx, toClassIdx, 1);
    
    
    SpRowMat::RowXpr mr_row = mr.row(entityIdx);
    
    SpRowMat mmt_row = mr_row * mtc;
    mmt.row(entityIdx) = mmt_row;

    mmt.reserve(VectorXi::Constant(entityCount, 2));
    
    for( int i = 0; i < mmt.rows(); i++ )
    {
        mmt.coeffRef(i, entityIdx) = mmt.coeff(entityIdx, i);
    }
    
    
    mr = mr.pruned();
    
    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
    {
        linkMatrixList[i].update(entityIdx, fromClassIdx, toClassIdx, mmt_row);
    }
  }

void DeltaMatrix::eval()
{
    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
    {
        linkMatrixList[i].eval();
    }
}


void DeltaMatrix::createMoveMethodCandidateSet(MoveMethodPartialOrderSet& mmcSet)
{
    for (int i=0; i < methodPossibleToMoveCount; i++)
    {
        for (int j=0; j < classCount; j++)
        {
            float values[LINK_MATRIX_COUNT];
            
            for( int k = 0; k < LINK_MATRIX_COUNT; k++ )
            {
                values[k] = linkMatrixList[k].D.coeff(i, j);
            }
            
            mmcSet.set(i, j, values);
            
            
            
//            MoveMethodPartialOrderIterator* it = mmcSet.getIterator();
//            
//            while( it->hasNext() )
//            {
//                std::cout << *(it->next()) << std::endl;
//            }
//            std::cout << "---------------------\n";
//            delete it;
        }
    }
    
    
}


MoveMethodPartialOrderIterator* DeltaMatrix::getSortedMoveMethodCandidates()
{
    SpRowMat diffMat(methodPossibleToMoveCount, classCount);
    
    for( int i = 0; i < LINK_MATRIX_COUNT; i++ )
    {
        linkMatrixList[i].addDiffD(diffMat);
    }
    
    
    for (int i=0; i < diffMat.outerSize(); i++)
    {
        for (SpRowMat::InnerIterator it(diffMat,i); it; ++it)
        {
            float values[LINK_MATRIX_COUNT];
            
            for( int k = 0; k < LINK_MATRIX_COUNT; k++ )
            {
                values[k] = linkMatrixList[k].D.coeff(it.row(), it.col());
            }
            
            moveMethodCandidateParetoFrontSet.set(it.row(), it.col(), values);
        }
    }
    
    
    
    return moveMethodCandidateParetoFrontSet.getIterator();
}