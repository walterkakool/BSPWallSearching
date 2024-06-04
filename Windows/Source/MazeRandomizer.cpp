#include <cmath>
#include <complex>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
using namespace std;

#define MAX_ITERATION 64

int main()
{
      ofstream fMaze ("maze.txt");
      srand( (unsigned int)time(NULL) );
      std::vector< std::complex<unsigned int> > givenPoints;  

      for(int i = 0; i < MAX_ITERATION; ++i) givenPoints.push_back( std::complex<unsigned int>( rand()%1020, rand()%1020 ) );
      fMaze<<"P1 1024 1024 ";

     std::complex<unsigned int> currentPoint(1,1); 
      
      for(int i = 0; i < 1024*1024; ++i){

            
            if( i/1024 == 1023 || i/1024 == 0 ){
            
                if( !(i%1024) )  //index 0
                    fMaze<<"\n1";
                else
                fMaze<<"1";

            }  else{
            
                if( !(i%1024) )  //index 0
                    fMaze<<"\n1";
                else{

                currentPoint = std::complex<unsigned int> ( rand()%1024, rand()%1024 );

                    for(int k = 0; k < MAX_ITERATION; ++k) {

                        if( currentPoint.real() == givenPoints[k].real() || currentPoint.imag() == givenPoints[k].imag() ) {
                            
                            if( i == 1025 )
                                fMaze<<"0";
                            else
                                fMaze<<"1";

                            k = MAX_ITERATION;

                        }else if ( k == MAX_ITERATION -1  ) fMaze<<"0"; 

                    }
                            
                }
            }
      }

      fMaze.close();
}
