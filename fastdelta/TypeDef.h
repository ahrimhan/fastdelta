//
//  TypeDef.h
//  fastdelta
//
//  Created by 송인권 [igsong] on 6/22/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#ifndef TypeDef_h
#define TypeDef_h

#include <Eigen/Eigen>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseCore>



#define EXPORT __attribute__((visibility("default")))

using namespace Eigen;

typedef Triplet<float> DTT;

//typedef SparseMatrix<int,ColMajor> ISpColMat;
//typedef SparseMatrix<int,RowMajor> ISpRowMat;
//
//typedef Matrix<int, Dynamic, Dynamic, ColMajor> IDColMat;
//typedef Matrix<int, Dynamic, Dynamic, RowMajor> IDRowMat;


typedef SparseMatrix<float,ColMajor> SpColMat;
typedef SparseMatrix<float,RowMajor> SpRowMat;

typedef Matrix<float, Dynamic, Dynamic, ColMajor> DColMat;
typedef Matrix<float, Dynamic, Dynamic, RowMajor> DRowMat;

class DeltaMatrix;
class DeltaMatrixInfo;


#include <sys/time.h>

inline uint64_t getTimestamp()
{
    struct timeval tv;
    uint64_t t;
    gettimeofday(&tv, NULL);
    t = tv.tv_sec * 1000000ull + tv.tv_usec;
    return t;
}

//#define DONT_USE_QUOTIENT
//#define ONLY_USE_COUPLING_WEIGHT

#define COHESION_WEIGHT 0.7
#define LINK_MATRIX_COUNT 2

#endif /* TypeDef_h */
