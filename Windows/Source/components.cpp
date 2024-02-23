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
#include "components.hpp"

using namespace std;
using namespace BSP;
using namespace Common;

namespace BSP {


            BSP::Segment* BSP::Segment::to_leaf(BSP::Segment* current_node, std::complex<int> location) {

                BSP::Segment* pos = current_node->get_positive_side();
                BSP::Segment* neg = current_node->get_negative_side();
                BSP::Segment* next_node = nullptr;

                if (pos == nullptr && neg == nullptr) return current_node;

                if (pos->get_top_left().real() <= location.real() && pos->get_top_left().imag() >= location.imag()
                    && pos->get_bottom_right().real() >= location.real() && pos->get_bottom_right().imag() <= location.imag()) {

                    next_node = pos;
                    return to_leaf(next_node, location);
                }
                else {

                    next_node = neg;
                    return to_leaf(next_node, location);
                }
            }

            BSP::Segment* BSP::Segment::find_north_sibling(BSP::Segment* current_seg, int last_north) {

                if (current_seg == nullptr) return nullptr;

                if (current_seg->get_top_left().imag() > last_north) {

                    return current_seg->get_positive_side();
                }
                else {

                    return find_north_sibling(current_seg->get_parent(), last_north);
                }
            }

            BSP::Segment* BSP::Segment::find_south_sibling(BSP::Segment* current_seg, int last_south) {

                if (current_seg == nullptr) return nullptr;

                if (current_seg->get_bottom_right().imag() < last_south) {

                    return current_seg->get_negative_side();
                }
                else {

                    return find_south_sibling(current_seg->get_parent(), last_south);
                }
            }

            BSP::Segment* BSP::Segment::find_east_sibling(BSP::Segment* current_seg, int last_east) {

                if (current_seg == nullptr) return nullptr;

                if (current_seg->get_bottom_right().real() > last_east) {


                    return current_seg->get_positive_side();
                }
                else {

                    return find_east_sibling(current_seg->get_parent(), last_east);
                }
            }

            BSP::Segment* BSP::Segment::find_west_sibling(BSP::Segment* current_seg, int last_west) {

                if (current_seg == nullptr) return nullptr;

                if (current_seg->get_top_left().real() < last_west) {

                    return current_seg->get_negative_side();
                }
                else {

                    return find_west_sibling(current_seg->get_parent(), last_west);
                }
            }

            void BSP::Segment::update_north(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling) {

                if (current_seg == nullptr) return;

                int current_north = current_seg->get_top_left().imag();
                int current_south = current_seg->get_bottom_right().imag();

                if (current_seg->get_positive_side() == nullptr && current_seg->get_negative_side() == nullptr)
                    goto run_it;
                else
                    goto skip;

            run_it:
                if (in_sibling) {

                    if (Common::Maze::maze[current_south][point.real()] == 1 && Location::north_wall != 1) {

                        Location::north_wall = 1;
                        Location::north_wall_point = std::complex<int>(point.real(), current_south);
                    }
                }

                if (Common::Maze::maze[current_north][point.real()] == 1 && Location::north_wall != 1) {

                    Location::north_wall = 1;
                    Location::north_wall_point = std::complex<int>(point.real(), current_north);
                }

                if (current_north + 1 == rows - 1 && Location::north_wall != 1) {

                    if (Common::Maze::maze[rows - 1][point.real()] == 1) {

                        Location::north_wall = 1;
                        Location::north_wall_point = std::complex<int>(point.real(), rows - 1);
                    }
                }

            skip:
                if (Location::north_wall != 1) {

                    update_north(current_seg->get_negative_side(), point, rows, cols, in_sibling);
                    update_north(current_seg->get_positive_side(), point, rows, cols, in_sibling);
                }
            }

            void BSP::Segment::update_south(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling) {

                if (current_seg == nullptr) return;

                int current_north = current_seg->get_top_left().imag();
                int current_south = current_seg->get_bottom_right().imag();

                if (current_seg->get_positive_side() == nullptr && current_seg->get_negative_side() == nullptr)
                    goto run_it;
                else
                    goto skip;

            run_it:
                if (in_sibling) {

                    if (Common::Maze::maze[current_north][point.real()] == 1 && Location::south_wall != 1) {

                        Location::south_wall = 1;
                        Location::south_wall_point = std::complex<int>(point.real(), current_north);
                    }
                }

                if (Common::Maze::maze[current_south][point.real()] == 1 && Location::south_wall != 1) {

                    Location::south_wall = 1;
                    Location::south_wall_point = std::complex<int>(point.real(), current_south);
                }

                if (current_south - 1 == 0 && Location::south_wall != 1) {

                    if (Common::Maze::maze[0][point.real()] == 1) {

                        Location::south_wall = 1;
                        Location::south_wall_point = std::complex<int>(point.real(), 0);
                    }
                }

            skip:
                if (Location::south_wall != 1) {

                    update_south(current_seg->get_positive_side(), point, rows, cols, in_sibling);
                    update_south(current_seg->get_negative_side(), point, rows, cols, in_sibling);
                }
            }

            void BSP::Segment::update_east(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling) {

                if (current_seg == nullptr) return;

                int current_east = current_seg->get_bottom_right().real();
                int current_west = current_seg->get_top_left().real();

                if (current_seg->get_positive_side() == nullptr && current_seg->get_negative_side() == nullptr)
                    goto run_it;
                else
                    goto skip;

            run_it:
                if (in_sibling) {

                    if (Common::Maze::maze[point.imag()][current_west] == 1 && Location::east_wall != 1) {

                        Location::east_wall = 1;
                        Location::east_wall_point = std::complex<int>(current_west, point.imag());
                    }
                }

                if (Common::Maze::maze[point.imag()][current_east] == 1 && Location::east_wall != 1) {

                    Location::east_wall = 1;
                    Location::east_wall_point = std::complex<int>(current_east, point.imag());
                }

                if (current_seg->get_bottom_right().real() + 1 == cols - 1 && Location::east_wall != 1) {

                    if (Common::Maze::maze[point.imag()][cols - 1] == 1) {

                        Location::east_wall = 1;
                        Location::east_wall_point = std::complex<int>(cols - 1, point.imag());
                    }
                }

            skip:
                if (Location::east_wall != 1) {

                    update_east(current_seg->get_negative_side(), point, rows, cols, in_sibling);
                    update_east(current_seg->get_positive_side(), point, rows, cols, in_sibling);
                }
            }

            void BSP::Segment::update_west(BSP::Segment* current_seg, std::complex<int> point, int rows, int cols, bool in_sibling) {

                if (current_seg == nullptr) return;

                int current_east = current_seg->get_bottom_right().real();
                int current_west = current_seg->get_top_left().real();

                if (current_seg->get_positive_side() == nullptr && current_seg->get_negative_side() == nullptr)
                    goto run_it;
                else
                    goto skip;

            run_it:
                if (in_sibling) {

                    if (Common::Maze::maze[point.imag()][current_east] == 1 && Location::west_wall != 1) {

                        Location::west_wall = 1;
                        Location::west_wall_point = std::complex<int>(current_east, point.imag());
                    }
                }

                if (Common::Maze::maze[point.imag()][current_west] == 1 && Location::west_wall != 1) {

                    Location::west_wall = 1;
                    Location::west_wall_point = std::complex<int>(current_west, point.imag());
                }

                if (current_west - 1 == 0 && Location::east_wall != 1) {

                    if (Common::Maze::maze[point.imag()][0] == 1) {

                        Location::west_wall = 1;
                        Location::west_wall_point = std::complex<int>(0, point.imag());
                    }
                }

            skip:
                if (Location::west_wall != 1) {

                    update_west(current_seg->get_positive_side(), point, rows, cols, in_sibling);
                    update_west(current_seg->get_negative_side(), point, rows, cols, in_sibling);
                }
            }

            void BSP::Segment::update_location_walls(BSP::Segment* current_seg, std::complex<int> pt, int ros, int cls) {

                BSP::Segment* north_sibling = nullptr;
                BSP::Segment* south_sibling = nullptr;
                BSP::Segment* east_sibling = nullptr;
                BSP::Segment* west_sibling = nullptr;

                std::complex<int> point = pt;
                int rows = ros;
                int cols = cls;

                int current_north = current_seg->get_top_left().imag();
                int current_south = current_seg->get_bottom_right().imag();
                int current_east = current_seg->get_bottom_right().real();
                int current_west = current_seg->get_top_left().real();

                int last_north = current_north;
                int last_south = current_south;
                int last_east = current_east;
                int last_west = current_west;

                std::function<void()> initialize_maze_to_north = [&point, &rows]() {

                    Location::north_wall = 1;
                    Location::north_wall_point = std::complex<int>(point.real(), rows - 1);
                };

                std::function<void()> initialize_maze_to_south = [&point]() {

                    Location::south_wall = 1;
                    Location::south_wall_point = std::complex<int>(point.real(), 0);
                };

                std::function<void()> initialize_maze_to_east = [&point, &cols]() {

                    Location::east_wall = 1;
                    Location::east_wall_point = std::complex<int>(cols - 1, point.imag());
                };

                std::function<void()> initialize_maze_to_west = [&point]() {

                    Location::west_wall = 1;
                    Location::west_wall_point = std::complex<int>(0, point.imag());
                };

                if (current_seg->get_positive_side() == nullptr && current_seg->get_negative_side() == nullptr) {

                    goto run_it;
                }
                else {

                    goto  skip;
                }

            run_it:

                update_north(current_seg, point, rows, cols, false);
                update_south(current_seg, point, rows, cols, false);
                update_east(current_seg, point, rows, cols, false);
                update_west(current_seg, point, rows, cols, false);

                north_sibling = current_seg;
                south_sibling = current_seg;
                east_sibling = current_seg;
                west_sibling = current_seg;

                while (Location::north_wall != 1) {
                    //std::cout << "stuck N" << std::endl;
                    last_north = north_sibling->get_top_left().imag();
                    north_sibling = find_north_sibling(north_sibling->get_parent(), last_north);

                    if (north_sibling == nullptr) {

                        initialize_maze_to_north();
                        break;
                    }

                    if (north_sibling->get_negative_side() == nullptr)
                        update_north(north_sibling, point, rows, cols, true);
                    else
                        update_north(north_sibling->get_negative_side(), point, rows, cols, true);

                    if (north_sibling->get_positive_side() == nullptr)
                        update_north(north_sibling, point, rows, cols, true);
                    else
                        update_north(north_sibling->get_positive_side(), point, rows, cols, true);
                }

                while (Location::south_wall != 1) {
                    //std::cout << "stuck S" << std::endl;
                    last_south = south_sibling->get_bottom_right().imag();
                    south_sibling = find_south_sibling(south_sibling->get_parent(), last_south);

                    if (south_sibling == nullptr) {

                        initialize_maze_to_south();
                        break;
                    }

                    if (south_sibling->get_positive_side() == nullptr)
                        update_south(south_sibling, point, rows, cols, true);
                    else
                        update_south(south_sibling->get_positive_side(), point, rows, cols, true);

                    if (south_sibling->get_negative_side() == nullptr)
                        update_south(south_sibling, point, rows, cols, true);
                    else
                        update_south(south_sibling->get_negative_side(), point, rows, cols, true);
                }

                while (Location::east_wall != 1) {
                    //std::cout << "stuck E" << std::endl;
                    last_east = east_sibling->get_bottom_right().real();
                    east_sibling = find_east_sibling(east_sibling->get_parent(), last_east);

                    if (east_sibling == nullptr) {

                        initialize_maze_to_east();
                        break;
                    }

                    if (east_sibling->get_negative_side() == nullptr)
                        update_east(east_sibling, point, rows, cols, true);
                    else
                        update_east(east_sibling->get_negative_side(), point, rows, cols, true);

                    if (east_sibling->get_positive_side() == nullptr)
                        update_east(east_sibling, point, rows, cols, true);
                    else
                        update_east(east_sibling->get_positive_side(), point, rows, cols, true);
                }

                while (Location::west_wall != 1) {
                    //std::cout << "stuck W" << std::endl;
                    last_west = west_sibling->get_top_left().real();
                    west_sibling = find_west_sibling(west_sibling->get_parent(), last_west);

                    if (west_sibling == nullptr) {

                        initialize_maze_to_west();
                        break;
                    }

                    if (west_sibling->get_positive_side() == nullptr)
                        update_west(west_sibling, point, rows, cols, true);
                    else
                        update_west(west_sibling->get_positive_side(), point, rows, cols, true);

                    if (west_sibling->get_negative_side() == nullptr)
                        update_west(west_sibling, point, rows, cols, true);
                    else
                        update_west(west_sibling->get_negative_side(), point, rows, cols, true);
                }


            skip:
                return;
            }

            void BSP::Segment::upddate_parents(BSP::Segment* root) {

                BSP::Segment* positive_side = root->get_positive_side();
                BSP::Segment* negative_side = root->get_negative_side();
                BSP::Segment* parent_node = root;

                if (positive_side == nullptr && negative_side == nullptr) return;

                if (positive_side->get_positive_side() == nullptr && positive_side->get_negative_side() == nullptr) {

                    positive_side->set_parent(parent_node);
                }
                else {

                    BSP::Segment::upddate_parents(positive_side);
                    positive_side->set_parent(parent_node);
                }

                if (negative_side->get_positive_side() == nullptr && negative_side->get_negative_side() == nullptr) {

                    negative_side->set_parent(parent_node);
                }
                else {

                    BSP::Segment::upddate_parents(negative_side);
                    negative_side->set_parent(parent_node);
                }

                return;
            }

			BSP::Segment::Segment():positive_side(nullptr), negative_side(nullptr),parent(nullptr), top_left(-1, -1), bottom_right(-1, -1){	}

            BSP::Segment::~Segment() {
            
                Segment* current_node = this;
                this->clean_children(current_node);
            }
			
            void Segment::clean_children(BSP::Segment* parent) {

                Segment* current_node = parent;

                //return if this is a child
                if (current_node->get_positive_side() == nullptr && current_node->get_negative_side() == nullptr) 
                    return; 

                else {

                    if (current_node->get_positive_side() != nullptr) Segment::clean_children(current_node->get_positive_side());

                    if (current_node->get_negative_side() != nullptr) Segment::clean_children(current_node->get_negative_side());

                    current_node = parent->get_positive_side();
                    if (current_node!=nullptr) {

                        delete current_node;
                        parent->set_positive_side(nullptr);
                    }
                  
                    current_node = parent->get_negative_side();
                    if (current_node != nullptr) {

                        delete current_node;
                        parent->set_negative_side(nullptr);
                    }
                }
            }

			std::pair<bool, BSP::Segment*> Segment::build_tree(const std::complex<int> top_left, const std::complex<int> bottom_right) {

				
				std::pair<bool, BSP::Segment*> rst = std::pair<bool, BSP::Segment*>(false, nullptr);
				std::pair<bool, BSP::Segment*> positive_side = std::pair<bool, BSP::Segment*>(false, nullptr);
				std::pair<bool, BSP::Segment*> negative_side = std::pair<bool, BSP::Segment*>(false, nullptr);
				
				//true if it's found a 1 on this periphery
				bool found = false; 
				int direction = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
				//int direction = 1;
				direction %= 2;

				std::complex<int> positive_top_left(-1, -1);
				std::complex<int> positive_bottom_right(-1, -1);
				std::complex<int> negative_top_left(-1, -1);
				std::complex<int> negative_bottom_right(-1, -1);

				std::function<BSP::Segment* ()> make_segment = [ &top_left, &bottom_right]() {
																			
					BSP::Segment* parent_node = nullptr;
					parent_node = new BSP::Segment();
												
					parent_node->set_top_left(top_left);
					parent_node->set_bottom_right(bottom_right);

					parent_node->set_positive_side(nullptr);
					parent_node->set_negative_side(nullptr);
																		
					return parent_node;
				};
								
				if (top_left == bottom_right) {
																
					rst = Common::Maze::maze[top_left.imag()][top_left.real()] == 1 ?
						std::pair<bool, BSP::Segment*>(true, nullptr) : std::pair<bool, BSP::Segment*>(false, nullptr);
															
					return rst;
				}

				//to left-right search
				direction = 0 == top_left.imag() - bottom_right.imag() ? 1 : direction;
				//to top-bottom search 
				direction = 0 == bottom_right.real() - top_left.real() ? 0 : direction;

				//checking whether this periphery has 1
				//non-0 (i.e., 1) for horizontal (left-right) search 
				if (direction) {
									
					//scanning through the edge
					left_right:
						for (int i = 0; i < bottom_right.real() - top_left.real() + 1; ++i){
							if (Common::Maze::maze[(int)((top_left.imag() - bottom_right.imag()) / 2) + bottom_right.imag()][i + top_left.real()]) {
																				
								found = true;
								break;
							}
						}															
												
						if (0 == top_left.imag() - bottom_right.imag()) {
							//1 row goes here 
							positive_top_left = std::complex<int>(1+top_left.real() + (int)((bottom_right.real() - top_left.real()) / 2), top_left.imag());
							positive_bottom_right = bottom_right;
							positive_side = build_tree(positive_top_left, positive_bottom_right);
										
							negative_top_left = top_left;
							negative_bottom_right = std::complex<int>(top_left.real() + (int)((bottom_right.real() - top_left.real()) / 2), bottom_right.imag());
							negative_side = build_tree(negative_top_left, negative_bottom_right);
						}
						else {
												
							positive_top_left = top_left;
							positive_bottom_right = std::complex<int>(bottom_right.real(), 1+ bottom_right.imag() + (int)((top_left.imag() - bottom_right.imag()) / 2));
							positive_side = build_tree(positive_top_left, positive_bottom_right);
						
							negative_top_left = std::complex<int>(top_left.real(), bottom_right.imag() + (int)((top_left.imag() - bottom_right.imag()) / 2));
							negative_bottom_right = bottom_right;
							negative_side = build_tree(negative_top_left, negative_bottom_right);
						}
										
											
								
						
				}
				//0 for vertical (top-bottom) search 
				else {
																								
					//scanning thru the edge
					bottom_top:						
						for (int i = 0; i < top_left.imag() - bottom_right.imag() + 1; ++i){
							if (Common::Maze::maze[i + bottom_right.imag()][(int)((bottom_right.real() - top_left.real()) / 2) + top_left.real()]) {
																										
								found = true;
								break;
							}
						}
														
						if (0 == bottom_right.real() - top_left.real()) {
							//1 column goes here
							positive_top_left = top_left;
							positive_bottom_right = std::complex<int>(bottom_right.real(), 1+ bottom_right.imag() + (int)((top_left.imag() - bottom_right.imag()) / 2));
							positive_side = build_tree(positive_top_left, positive_bottom_right);
																
							negative_top_left = std::complex<int>(top_left.real(), bottom_right.imag() + (int)((top_left.imag() - bottom_right.imag()) / 2));
							negative_bottom_right = bottom_right;
							negative_side = build_tree(negative_top_left, negative_bottom_right);
						}
						else {
																	
							positive_top_left = std::complex<int>( 1+ (int)((bottom_right.real() - top_left.real()) / 2) + top_left.real(), top_left.imag());
							positive_bottom_right = bottom_right;
							positive_side = build_tree(positive_top_left, positive_bottom_right);

							negative_top_left = top_left;
							negative_bottom_right = std::complex<int>((int)((bottom_right.real() - top_left.real()) / 2) + top_left.real(), bottom_right.imag());
							negative_side = build_tree(negative_top_left, negative_bottom_right);
						}				
				}

				//two of first ones and found must be same cuz pigeon hole
				if (positive_side.first == found && found == negative_side.first && positive_side.second==nullptr && negative_side.second==nullptr) {
					
					return std::pair<bool, BSP::Segment*>(found, nullptr);
				}
				
					BSP::Segment* parent_node = nullptr;	

					if (positive_side.second != nullptr && negative_side.second==nullptr) {

						parent_node = make_segment();
						parent_node->set_positive_side(positive_side.second);

						BSP::Segment* negative_node = nullptr;
						negative_node = new BSP::Segment();
									
						negative_node->set_top_left(negative_top_left);
						negative_node->set_bottom_right(negative_bottom_right);
											
						parent_node->set_negative_side(negative_node);
					}

					if (positive_side.second == nullptr && negative_side.second != nullptr) {

						parent_node = make_segment();
						parent_node->set_negative_side(negative_side.second);

						BSP::Segment* positive_node = nullptr;
						positive_node = new BSP::Segment();

						positive_node->set_top_left(positive_top_left);
						positive_node->set_bottom_right(positive_bottom_right);

						parent_node->set_positive_side(positive_node);
					}

					if (positive_side.second != nullptr && negative_side.second != nullptr) {

						parent_node = make_segment();
						parent_node->set_positive_side(positive_side.second);
						parent_node->set_negative_side(negative_side.second);
					}
					
					if(positive_side.second == nullptr && negative_side.second == nullptr) {
					
						parent_node = make_segment();
					
						BSP::Segment* positive_node = nullptr;
						positive_node = new BSP::Segment();
					
						positive_node->set_top_left(positive_top_left);
						positive_node->set_bottom_right(positive_bottom_right);
					
						BSP::Segment* negative_node = nullptr;
						negative_node = new BSP::Segment();
					
						negative_node->set_top_left(negative_top_left);
						negative_node->set_bottom_right(negative_bottom_right);
					
						parent_node->set_positive_side(positive_node);
						parent_node->set_negative_side(negative_node);
					}

					return std::pair<bool, BSP::Segment*>(found, parent_node);
				 
				//make_tree ends here
			}

			void Segment::set_top_left(std::complex<int> top) { this->top_left = top; }
			void Segment::set_bottom_right(std::complex<int> bottom) { this->bottom_right = bottom; }

			void Segment::set_parent(Segment* parent) {this->parent = parent;}
			void Segment::set_positive_side(Segment* pos) {this->positive_side = pos;}
			void Segment::set_negative_side(Segment* neg) {this->negative_side = neg;}

			Segment* BSP::Segment::get_parent() { return this->parent; }
			Segment* BSP::Segment::get_positive_side() { return this->positive_side; }
			Segment* BSP::Segment::get_negative_side() { return this->negative_side; }

			std::complex<int> BSP::Segment::get_top_left() { return this->top_left; }
			std::complex<int> BSP::Segment::get_bottom_right() { return this->bottom_right; }

			//class ends below
}

namespace Common {

	boost::multi_array<int, 2> Maze::maze = boost::multi_array<int, 2>(boost::extents[10240][10240]);
    int Maze::rows = -1;
    int Maze::cols = -1;

	Maze::Maze(){}

	int Maze::load_maze(std::string input) {

		//std::string input = argv;
		std::string path = "";
		std::ifstream x(input);
		std::string current_line = "1";
		int maze_point = 0;

		std::getline(x, current_line, ' ');

		if (0 != current_line.compare("P1")) {

			x.close();
			return 1;
		}

		std::getline(x, current_line, ' ');
		Common::Maze::cols = stoi(current_line);
		std::getline(x, current_line);
        Common::Maze::rows = stoi(current_line);

		for (int i = 0; i < rows; ++i) {

			std::getline(x, current_line, '\n');
			int k = 0;
			for (auto j : current_line) {

				maze_point = j == '1' ? 1 : 0;
				Common::Maze::maze[rows - 1 - i][k++] = maze_point;
			}
		}

		x.close();
		return 0;
	}

	void Location::reset() {

		Location::north_wall = 0;
		Location::south_wall = 0;
		Location::east_wall = 0;
		Location::west_wall = 0;
	}

    Naive::Naive() {  }

    void Naive::update_walls() {

        Location::reset();

        boost::multi_array<std::complex<int>, 1> a = boost::multi_array<std::complex<int>, 1>(boost::extents[Common::Maze::rows * Common::Maze::cols]);

        int k = 0;
        for (int i = 0; i < Common::Maze::rows; ++i) {

            for (int j = 0; j < Common::Maze::cols; ++j) {

                if(Common::Maze::maze[i][j] == 1)
                    a[k++] = std::complex<int>(j, i);
            }
        }

        for (std::complex<int> item:a) {

            //north or south
            if (Location::point.real() == item.real()) {

                if ( Location::north_wall == 0 || Location::north_wall_point.imag() > item.imag()) {

                    if (item.imag() > Location::point.imag()) {

                        Location::north_wall = 1;
                        Location::north_wall_point = item;
                    }
                }

                if (Location::south_wall == 0 ||  Location::south_wall_point.imag() < item.imag()) {

                    if (item.imag() < Location::point.imag()) {

                        Location::south_wall = 1;
                        Location::south_wall_point = item;
                    }
                }
            }

            //east or west
            if (Location::point.imag() == item.imag()) {

                if (Location::east_wall == 0 || Location::east_wall_point.real() > item.real()) {

                    if (item.real() > Location::point.real()) {

                        Location::east_wall = 1;
                        Location::east_wall_point = item;
                    }
                }

                if (Location::west_wall == 0 || Location::west_wall_point.real() < item.real()) {

                    if (item.real() < Location::point.real()) {

                        Location::west_wall = 1;
                        Location::west_wall_point = item;
                    }
                }
            }
        }

        a.resize(boost::extents[0]);
    }
}
