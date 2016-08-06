//
//  LinkMatrix.cpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 16..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#include "LinkMatrix.hpp"
#include "DeltaMatrix.hpp"

void LinkMatrix::initialize(const SpRowMat& l, bool useQuotient)
{
    L = l;
    this->useQuotient = useQuotient;
    L.conservativeResize(dm->methodPossibleToMoveCount, dm->entityCount);
    
    fprintf(stderr, "L:(%d, %d)\n", L.rows(), L.cols());
    fprintf(stderr, "mmt:(%d, %d)\n", dm->mmt.rows(), dm->mmt.cols());
    
    Lint = L.cwiseProduct(dm->mmt);
    Lext = L.cwiseProduct(DRowMat::Constant(dm->methodPossibleToMoveCount, dm->entityCount, 1) - dm->mmt);
    
    eval();
    prevD = D;    
}

void LinkMatrix::update(int entityIdx, int fromClassIdx, int toClassIdx, SpRowMat& mmt_row)
{
    Lint.row(entityIdx) = L.row(entityIdx).cwiseProduct(mmt_row);
    Lext.row(entityIdx) = L.row(entityIdx).cwiseProduct(
                            DRowMat::Constant(1, dm->entityCount, 1) - mmt_row);
    
    Lint.reserve(VectorXi::Constant(dm->entityCount, 2));
    Lext.reserve(VectorXi::Constant(dm->entityCount, 2));
    for( int i = 0; i < Lext.rows(); i++ )
    {
        Lint.coeffRef(i, entityIdx) = Lint.coeff(entityIdx, i);
        Lext.coeffRef(i, entityIdx) = Lext.coeff(entityIdx, i);
    }
    
    Lint.makeCompressed();
    Lext.makeCompressed();
    
}

void LinkMatrix::eval()
{
    uint64_t stage0 = getTimestamp();
    
    prevD = D;
    
    uint64_t stage1 = getTimestamp();
    
    SpRowMat pint(dm->methodPossibleToMoveCount, dm->classCount);
    SpRowMat pintQ(dm->methodPossibleToMoveCount, dm->classCount);
    
    pint.reserve(VectorXi::Constant(dm->methodPossibleToMoveCount, dm->classCount));
    pintQ.reserve(VectorXi::Constant(dm->methodPossibleToMoveCount, dm->classCount));
    
//#pragma omp parallel for
    for (int i=0; i < dm->mr.outerSize(); i++)
    {
        for (SpRowMat::InnerIterator it(dm->mr,i); it; ++it)
        {
            float sum = Lint.row(it.row()).sum();
            pintQ.row(it.row()) = DRowMat::Constant(1, dm->classCount, sum + 1).sparseView();
            pint.row(it.row()) = DRowMat::Constant(1, dm->classCount, sum).sparseView();
            pint.coeffRef(it.row(), it.col()) = 0;
        }
    }
    
    uint64_t stage2 = getTimestamp();
    
    SpRowMat pext = Lext * dm->mc;
    uint64_t stage3 = getTimestamp();
    
    if (useQuotient) {
        D = (pint - pext).cwiseProduct(dm->possibleMoveMethodMatrix).cwiseQuotient(pintQ).pruned();
    }
    else
    {
        D = (pint - pext).cwiseProduct(dm->possibleMoveMethodMatrix).pruned();
    }
    
    uint64_t stage4 = getTimestamp();
    
    fprintf(stderr, "Elapsed time for eval(total:%lld, stage1:%lld, stage2:%lld, stage3:%lld stage4:%lld)\n",
            stage4 - stage0, stage1 - stage0, stage2 - stage1, stage3 - stage2, stage4 - stage3);
}

void LinkMatrix::addDiffD(SpRowMat& diffD)
{
    diffD += (D - prevD).cwiseAbs();
}