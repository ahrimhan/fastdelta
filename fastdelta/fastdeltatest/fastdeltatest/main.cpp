//
//  main.cpp
//  fastdeltatest
//
//  Created by 송인권 [igsong] on 6/22/16.
//  Copyright © 2016 kr.ac.kaist. All rights reserved.
//

#include <iostream>
#include "DeltaMatrix.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "DeltaValue.hpp"

static int mm[9][4] =
   {{0, 1, 0, 0},
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {0, 0, 0, 1}};

static int linkm[9][9] =
   {{0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 1, 0, 1, 0, 1, 1, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 2},
    {0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 2, 0, 0, 0, 0}};

void unittest1()
{
    DeltaMatrixInfo info(4, 9, 7, 7);
    
    for( int i = 0; i < 9; i++ )
    {
        for( int j = i + 1; j < 9; j++ )
        {
            if( linkm[i][j] )
            {
                for( int k = 0; k < linkm[i][j]; k++ )
                {
                    info.addLink(i, j);
                }
            }
        }
        
        for( int j = 0; j < 4; j++ )
        {
            if( mm[i][j] )
            {
                info.addMembership(i, j);
            }
            
            if( i < 7 )
            {
                info.possibleMoveMethod(i, j);
            }
        }
    }
    
    DeltaMatrix dm;
    dm.init(&info);
    SpRowMat& d = dm.getLinkMatrix(0).getD();
    std::cout << d << std::endl << std::endl;
    
    
    MoveMethodCandidateParetoFrontIterator* result =  dm.getSortedMoveMethodCandidates();
    while( result->hasNext() )
    {
        std::cout << *(result->next()) << "\n";
    }
    delete result;
    
    
    dm.move(1, 0, 3);
    dm.eval();
    std::cout << d << std::endl << std::endl;
    
    result =  dm.getSortedMoveMethodCandidates();
    while( result->hasNext() )
    {
        std::cout << *(result->next()) << "\n";
    }
    
    dm.move(1, 3, 0);
    dm.eval();
    
    std::cout << d << std::endl << std::endl;
}

#define CC 10
#define EC 20
#define MC 16
#define PMC 12


void unittest2()
{
    DeltaMatrixInfo info(CC, EC, MC, PMC);
    int membershipInfo[MC];
    
    srandomdev();
    uint64_t start, end, interm;

    
    for( int i = 0; i < EC; i++ )
    {
        for( int j = i + 1; j < EC; j++ )
        {
//            if( (random() % 100) < (100 * 20. / EC))
            if( (random() % 100) < 50 )
            {
                info.addLink(i, j);
            }
        }
   
        {
            int classIdx = random() % CC;
            info.addMembership(i, classIdx);
            if( i < MC )
            {
                membershipInfo[i] = classIdx;
            }
        }
    }
    
    for( int i = 0; i < PMC; i++ )
    {
        for( int j = 0; j < CC; j++ )
        {
            info.possibleMoveMethod(i, j);
        }
    }
    
    printf("Complete loading...(class:%d, entity:%d)\n\n", CC, EC);
    
    
    DeltaMatrix dm;
    start = getTimestamp();
    dm.init(&info);
    end = getTimestamp();
    std::cout << "Initial calculation: " << (end - start) << std::endl;
    
    /*
    DeltaValue dv;
    dv.init(&info);
    
    start = getTimestamp();
    float testD = dv.getDValue(1000, 0, 1);
    end = getTimestamp();
    printf("Test Delta calculation: %llu\n", end-start);
    printf("testD = %f, D = %f\n", testD, dm.getD().coeff(1000, 1));
    */
    
    MoveMethodCandidateParetoFrontSet testCandidateSet;
    
    for( int i = 0; i < 5; i++ )
    {
        int methodIdx = random() % PMC;
        int fromClassIdx = membershipInfo[methodIdx];
        int toClassIdx = random() % CC;
        
        while( toClassIdx == fromClassIdx )
        {
            toClassIdx = random() % CC;
        }
        
        start = getTimestamp();
        dm.move(methodIdx, fromClassIdx, toClassIdx);
        membershipInfo[methodIdx] = toClassIdx;
        interm = getTimestamp();
        dm.eval();
        end = getTimestamp();
        
        printf("%d th calculation: %llu (move: %llu, eval: %llu)\n", i, end-start, interm-start, end-interm);
        
        start = getTimestamp();
        
        MoveMethodCandidateParetoFrontIterator* iter1 = dm.getSortedMoveMethodCandidates();
        
        end = getTimestamp();
        std::cout << "get sorted set by update: " << (end - start) << std::endl;
        
        start = getTimestamp();
        dm.createMoveMethodCandidateSet(testCandidateSet);
        end = getTimestamp();
        std::cout << "get sorted set by creating: " << (end - start) << std::endl;

        
        MoveMethodCandidateParetoFrontIterator* iter2 = testCandidateSet.getIterator();
        
        
        while( iter1->hasNext() )
        {
            iter1->next();
//            std::cout << *(iter1->next()) << std::endl;
        }
        
        std::cout << "======================================\n";
        
        while( iter2->hasNext() )
        {
            iter2->next();
//            std::cout << *(iter2->next()) << std::endl;
        }
        
        
        if( testCandidateSet == dm.getParetoSet() )
        {
            std::cout << "[SUCCESS] updateed sorted set is equal to created one\n";
        }
        else
        {
            
            fprintf(stderr, "[FAIL] updateed sorted set is not equal to created one\n");
        }
        
    }
    
    

}



int main(int argc, const char * argv[]) {
//    unittest1();
    unittest2();
    return 0;
}
