#ifndef program_properties_hpp
#define program_properties_hpp
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

using namespace std;
//using namespace ProgramProperties;

namespace ProgramProperties{

	struct StateProperties {

		public:

			bool    started;
			bool	TABed;
			bool	F2ed;
			bool	cleaned;
			string	F2Mssg;
	};

}



#endif
