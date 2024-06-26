#include "pch.h"
#include "framework.h"
#include "bsp_maze.h"

#include <iostream>
#include <queue>
#include <stdio.h>
#include <iomanip>
#include <typeinfo>
#include <assert.h>   
#include <list>
#include <string>
#include <cmath>
#include <iterator>
#include <cstddef>
#include <array>
#include <math.h>
#include <type_traits>
#include <cstdint>
#include <boost/intrusive/rbtree.hpp>
#include <boost/multi_array.hpp>
#include <exception>
#include <typeinfo>
#include <functional>
#include <stdexcept>
#include <memory>
#include <algorithm>  
#include <utility>
#include <set>
#include <numbers>
#include <complex>
#include <numbers>
#include <iterator> 
#include <condition_variable> 
#include <thread>         
#include <mutex>  
#include <cstdlib>
#include <fstream>

#include "program_properties.hpp";

using namespace ProgramProperties;

namespace ProgramProperties {

    //initialize states 
    static void initPropertiesStates ( StateProperties& pStateProperties ) { 
        
        pStateProperties.cleaned     = false;
        pStateProperties.isF4       = false;
        pStateProperties.isF5        = false;
        pStateProperties.path2long   = false;
        pStateProperties.started     = false;
        pStateProperties.isLocation  = false;
        pStateProperties.closeIt     = false;
    }

    //intialize string content
    static void initPropertiesF2 ( StateProperties& pStateProperties, int size ) {  

        pStateProperties.f2Mssg = '\0';  
        pStateProperties.f2path = '\0'; 

        for ( int i = 0; i< size - 1; ++i  ) { 
        
            pStateProperties.f2Mssg += '\0';
            pStateProperties.f2path += '\0';
        }    

    };

    static void rstIterator ( StateProperties& pStateProperties ) { 

        pStateProperties.itrCounts = 0;
    }

    static void rstMazeStart ( StateProperties& pStateProperties ) { 

        pStateProperties.mazeStart = 0;
    }

    static void rstLoc ( StateProperties& pStateProperties ) { 

        pStateProperties.locX = 1;
        pStateProperties.locY = 70;
    }

    static void rstMv ( StateProperties& pStateProperties ) { 

        pStateProperties.mvX = 0;
        pStateProperties.mvY = 0;
    }

    static void rstDimensions ( StateProperties& pStateProperties ) { 
        
        pStateProperties.width             = 0;
        pStateProperties.height            = 0;
        pStateProperties.verticalPg        = 0;
        pStateProperties.horizontalPg      = 0; 

    }

    static bool pageMatched ( StateProperties& pStateProperties, int rcBottom, int rcRight ) { 
        

        return  
            (pStateProperties.locX + pStateProperties.mvX/10)/(rcRight/10)  
                                    ==  
                        pStateProperties.horizontalPg

                                    &&

            (pStateProperties.locY + pStateProperties.mvY/10)/(rcBottom/10) 
                                    ==
                          pStateProperties.verticalPg;

    }
}
