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
    DeltaMatrixInfo info(4, 9, 7);
    
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
            
            info.possibleMoveMethod(i, j);
        }
    }
    
    DeltaMatrix dm(false);
    dm.init(&info);
    SpRowMat& d = dm.getD();
    std::cout << d << std::endl << std::endl;
    
    
    MoveMethodCandidateSetIndexByValue& result =  dm.getSortedMoveMethodCandidates();
    for( MoveMethodCandidateSetIndexByValue::iterator it = result.begin();
        it != result.end(); it++ )
    {
        std::cout << "(e, c, v):" << it->entityIdx << ", " << it->toClassIdx << ", " << it->value << "\n";
    }
    
    
    dm.move(1, 0, 3);
    dm.eval();
    std::cout << d << std::endl << std::endl;
    
    result =  dm.getSortedMoveMethodCandidates();
    for( MoveMethodCandidateSetIndexByValue::iterator it = result.begin();
        it != result.end(); it++ )
    {
        std::cout << "(e, c, v):" << it->entityIdx << ", " << it->toClassIdx << ", " << it->value << "\n";
    }
    
    dm.move(1, 3, 0);
    dm.eval();
    
    std::cout << d << std::endl << std::endl;
}

#define CC 700
#define EC 12000
#define MC 9000


void unittest2()
{
    DeltaMatrixInfo info(CC, EC, MC);
    int membershipInfo[MC];
    
    srandomdev();
    uint64_t start, end, interm;

    
    for( int i = 0; i < EC; i++ )
    {
        for( int j = i + 1; j < EC; j++ )
        {
            if( (random() % 100) < (100 * 10. / EC))
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
    
    for( int i = 0; i < MC; i++ )
    {
        for( int j = 0; j < CC; j++ )
        {
            info.possibleMoveMethod(i, j);
        }
    }
    
    printf("Complete loading...(class:%d, entity:%d)\n\n", CC, EC);
    
    
    DeltaMatrix dm(false);
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
    
    MoveMethodCandidateSet testCandidateSet;
    
    for( int i = 0; i < 50; i++ )
    {
        int methodIdx = random() % MC;
        int fromClassIdx = membershipInfo[methodIdx];
        int toClassIdx = random() % CC;
        
        while( toClassIdx == fromClassIdx )
        {
            toClassIdx = random() % CC;
        }
        
        start = getTimestamp();
        dm.move(methodIdx, fromClassIdx, toClassIdx);
        interm = getTimestamp();
        dm.eval();
        end = getTimestamp();
        
        printf("%d th calculation: %llu (move: %llu, eval: %llu)\n", i, end-start, interm-start, end-interm);
        
        start = getTimestamp();
        
        MoveMethodCandidateSetIndexByValue& valueIndex = dm.getSortedMoveMethodCandidates();
        
        end = getTimestamp();
        std::cout << "get sorted set by update: " << (end - start) << std::endl;
        
        start = getTimestamp();
        dm.createMoveMethodCandidateSet(testCandidateSet);
        end = getTimestamp();
        std::cout << "get sorted set by creating: " << (end - start) << std::endl;


        if( valueIndex == testCandidateSet.get<0>() )
        {
            std::cout << "[SUCCESS] updateed sorted set is equal to created one\n";
        }
        else
        {
            
            fprintf(stderr, "[FAIL] updateed sorted set is not equal to created one(updated:%lu, created:%lu\n", valueIndex.size(), testCandidateSet.get<0>().size());
        }
        
    }
    
    

}



int main(int argc, const char * argv[]) {
//    unittest1();
    unittest2();
    return 0;
}
