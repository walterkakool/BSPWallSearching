#ifndef components_hpp
#define components_hpp
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
//#include <ncurses.h>
#include <fstream>

using namespace std;

namespace BSP{

	class Segment {

		private:
			std::complex<int> top_left;
			std::complex<int> bottom_right;

			Segment* positive_side;
			Segment* negative_side;
			Segment* parent;

		public:
			static BSP::Segment* to_leaf(BSP::Segment* current_node, std::complex<int> location);
			static BSP::Segment* find_north_sibling(BSP::Segment* current_seg, int last_north);
			static BSP::Segment* find_south_sibling(BSP::Segment* current_seg, int last_south);
			static BSP::Segment* find_east_sibling(BSP::Segment* current_seg, int last_east);
			static BSP::Segment* find_west_sibling(BSP::Segment* current_seg, int last_west);
			static void update_north(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling);
			static void update_south(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling);
			static void update_east(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling);
			static void update_west(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling);
			static void update_location_walls(BSP::Segment* current_seg, std::complex<int> pt, int ros, int cls);

			static void upddate_parents(Segment* root);

			Segment();

			~Segment();

			void set_top_left(std::complex<int> top);
			void set_bottom_right(std::complex<int> bottom);

			void set_parent(Segment* parent);
			void set_positive_side(Segment* pos);
			void set_negative_side(Segment* neg);
		
			Segment* get_parent();
			Segment* get_positive_side();
			Segment* get_negative_side();

			std::complex<int> get_top_left();
			std::complex<int> get_bottom_right();

			void clean_children(BSP::Segment* parent);

			std::pair<bool, BSP::Segment*> build_tree(const std::complex<int> top_left, const std::complex<int> bottom_right);
	};

}

namespace Common {

	class Maze {

		public:
			static boost::multi_array<int, 2> maze;
			static int rows;
			static int cols;
			static int load_maze(std::string input);

			Maze();	
	};

	class Location {

		public:
			static int north_wall;
			static int south_wall;
			static int east_wall;
			static int west_wall;

			static std::complex<int> point;

			static std::complex<int> north_wall_point;
			static std::complex<int> south_wall_point;
			static std::complex<int> east_wall_point;
			static std::complex<int> west_wall_point;

			static void reset();

			Location();	
	};	

	class Naive {

		public:
			Naive();

			void update_walls();
	};

}


#endif
