//
//  LinkMatrix.hpp
//  fastdelta
//
//  Created by 송인권 [igsong] on 2016. 7. 16..
//  Copyright © 2016년 kr.ac.kaist. All rights reserved.
//

#ifndef LinkMatrix_hpp
#define LinkMatrix_hpp

#include <stdio.h>
#include "TypeDef.h"


class LinkMatrix
{
public:
    SpRowMat L;
    SpRowMat Lint;
    SpRowMat Lext;
    SpRowMat prevD;
    SpRowMat D;
    DeltaMatrix* dm;
    bool useQuotient;
    
    LinkMatrix()
    {
    }
    
    LinkMatrix(DeltaMatrix* deltaMatrix) : dm(deltaMatrix)
    {
    }
    
    LinkMatrix(const LinkMatrix& mat) : dm(mat.dm)
    {
    }
    
    void initialize(const SpRowMat& l, bool useQuotient);
    
    void update(int entityIdx, int fromClassIdx, int toClassIdx, SpRowMat& mmt_row);
    
    void eval();
    
    void addDiffD(SpRowMat& diffD);
    
    SpRowMat& getD()
    {
        return D;
    }
};

#endif /* LinkMatrix_hpp */
