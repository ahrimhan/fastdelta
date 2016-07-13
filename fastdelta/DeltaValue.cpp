//
//  DeltaValue.cpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 2..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#include "DeltaValue.hpp"

void DeltaValue::init(DeltaMatrixInfo *info)
{
    classCount = info->getClassCount();
    entityCount = info->getEntityCount();
    methodCount = info->getMethodCount();
    fieldCount = entityCount - methodCount;
    
    
    SpRowMat identity(methodCount, methodCount);
    
    
    identity.setIdentity();
    
    mr.resize(entityCount, classCount);
    l.resize(entityCount, entityCount);
    
    mr.setFromTriplets(info->membershipInfoList.begin(), info->membershipInfoList.end());
    mc = mr;
    
    
    l.setFromTriplets(info->linkInfoList.begin(), info->linkInfoList.end());
    
#ifndef DONT_USE_COHESION
    SpRowMat methodToField = l.topRightCorner(methodCount, fieldCount);
    
    uint64_t start = getTimestamp();
    SpRowMat t = methodToField * methodToField.transpose();
    printf("mtf * mtf_T = %u (%d by %d) nonZeros=%d\n", getTimestamp() - start, methodToField.rows(), methodToField.cols(), methodToField.nonZeros());
    
    t = t - t.cwiseProduct(identity);
    
    t.conservativeResize(entityCount, entityCount);
    
    l += t * COHESION_WEIGHT;
#endif
    
    lc = l;
}

void DeltaValue::move(int entityIdx, int fromClassIdx, int toClassIdx)
{
    changeMembership(entityIdx, fromClassIdx, 0);
    changeMembership(entityIdx, toClassIdx, 1);
}

int DeltaValue::createClass()
{
    classCount++;
    mr.conservativeResize(entityCount, classCount);
    mc.conservativeResize(entityCount, classCount);
    return classCount - 1;
}


float DeltaValue::getExtractClassDValue(int sourceClassIdx, int* entityIndices, int len)
{
    DeltaValue dv = *this;
    
    int newClassIdx = dv.createClass();
    
    float dValue = 0;
    
    for( int i = 0; i < len; i++ )
    {
        dValue += dv.getDValue(entityIndices[i], sourceClassIdx, newClassIdx);
        dv.move(entityIndices[i], sourceClassIdx, newClassIdx);
    }
    
    return dValue;
}


float DeltaValue::getDValue(int entityIdx, int fromClassIdx, int toClassIdx)
{
    SpRowMat mat1 = l.row(entityIdx).cwiseProduct(mr.transpose().row(fromClassIdx));
    SpRowMat mat2= l.row(entityIdx).cwiseProduct(mr.transpose().row(toClassIdx));
    
    SpRowMat mat = mat1 + mat2;
    
    int reducedEntityCount = mat.nonZeros() + 1;
    int reducedClassCount = 2;
    
    SpRowMat pMat(reducedEntityCount, entityCount);
    SpColMat cMat(classCount, 2);
    
    int k = 1;
    
    pMat.coeffRef(0, entityIdx) = 1;
    
    for (int i=0; i < mat.outerSize(); ++i)
    {
        for (SpRowMat::InnerIterator it(mat,i); it; ++it)
        {
            pMat.coeffRef(k, it.col()) = 1;
            k++;
        }
    }
    
    cMat.coeffRef(toClassIdx, 0) = 1;
    cMat.coeffRef(fromClassIdx, 1) = 1;
    
    SpRowMat MRP = pMat * mc * cMat;
    SpColMat MCP = MRP;
    
//    SpColMat MTCP = MRP.transpose();
//    SpColMat MICP = DColMat::Constant(reducedEntityCount, reducedClassCount, 1) - MCP;
    SpRowMat MMTP = (MRP * MRP.transpose());
    
    SpRowMat LP = pMat * l * pMat.transpose();
    
    
    SpRowMat lint = LP.cwiseProduct(MMTP);
    SpRowMat lext = LP.cwiseProduct(DRowMat::Constant(reducedEntityCount, reducedEntityCount, 1) - MMTP);
    
    SpRowMat pint = lint * (DColMat::Constant(reducedEntityCount, reducedClassCount, 1) - MCP);
    SpRowMat pext = lext * MCP;
    SpRowMat pintq = pint + DRowMat::Constant(reducedEntityCount, reducedClassCount, 1);
    
#ifndef DONT_USE_QUOTIENT
    SpRowMat D =  (pint - pext).cwiseQuotient(pintq);
#else
    SpRowMat D = pint - pext;
#endif
    
    return D.coeff(0, 0);
}
