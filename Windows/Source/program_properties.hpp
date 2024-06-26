#ifndef program_properties_hpp
#define program_properties_hpp

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
#include <d2d1.h>

#pragma comment(lib, "d2d1")

using namespace std;
//using namespace ProgramProperties;

namespace ProgramProperties{

	struct StateProperties {

		public:

			bool    started;
			bool	isLocation;
			bool	isF4;
			bool	isF5;
			bool	cleaned;
			bool	path2long;
			bool	closeIt;

			string	f2Mssg;  //F2 content
			string  f2path;
			
			//Maze's total width and height
			unsigned int width;
			unsigned int height;

			unsigned int verticalPg;   //Vertical page index for screen-scrolling
			unsigned int horizontalPg; //Horizontal page index for screen-scrolling

			int		locX;  //x-axis current location
			int		locY;  //y-axis current location

			int		mvX;  //x-axis movement
			int		mvY;  //y-axis movement

			unsigned int itrCounts;
			unsigned int mazeStart;
	};

	static void initPropertiesStates ( StateProperties& pStateProperties );
	static void initPropertiesF2 ( StateProperties& pStateProperties, int size );
	static void rstIterator ( StateProperties& pStateProperties);
	static void rstLoc ( StateProperties& pStateProperties);
	static void rstMv  ( StateProperties& pStateProperties);
	static void rstDimensions ( StateProperties& pStateProperties);
	static bool pageMatched ( StateProperties& pStateProperties, int rcBottom, int rcRight);

}



#endif
