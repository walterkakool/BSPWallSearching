#define _USE_MATH_DEFINES
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
#include <boost/chrono.hpp>
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
#include <ncurses.h>
#include <fstream>
#include "Components.hpp"

using namespace std;
using namespace BSP;
using namespace Common;

int Common::Location::north_wall = 0;
int Common::Location::south_wall = 0;
int Common::Location::east_wall = 0;
int Common::Location::west_wall = 0;

std::complex<int> Location::north_wall_point(0, 0);
std::complex<int> Location::south_wall_point(0, 0);
std::complex<int> Location::east_wall_point(0, 0);
std::complex<int> Location::west_wall_point(0, 0);

std::complex<int> Location::point(0, 0);

void run(int a, int b) {

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::string time_cost_str = "0";
    char* time_cost_char = &time_cost_str[0];
    
    Common::Naive* naive_obj = new Common::Naive();

    int use_BSP = 1; 
    BSP::Segment* current_segment= new BSP::Segment();
    std::pair<bool, BSP::Segment*> root = current_segment->build_tree(std::complex<int>(1, Common::Maze::rows-2), std::complex<int>(Common::Maze::cols-2, 1));
    BSP::Segment::upddate_parents(root.second);

    BSP::Segment* leaf_node = BSP::Segment::to_leaf(root.second, std::complex<int>(1, 1));

    int input = 0;
    int locay = Common::Maze::rows-b; 
    int lasty = locay;
    int locax = a; 
    int lastx = locax;

    initscr();
    start_color();
    //for context
    init_pair(9, 0, 255);

    //light purple
    init_pair(10, 147, 147);
    //purple
    init_pair(11, 128, 128);
    //black
    init_pair(12, 0, 0);
    //white
    init_pair(13, 255, 255);

    //red
    init_pair(1, 160, 160);
    //blue
    init_pair(2, 39, 39);
    //green
    init_pair(3, 83, 83);
    //brown
    init_pair(4, 130, 130);

    WINDOW* wBSP = newwin(1, 200, 0, 0);
    WINDOW* wNaive = newwin(1, 200, 0, 0);

    WINDOW* location_point_naive = newwin(1, 1, locay, locax);
    WINDOW* location_point = newwin(1, 1, locay, locax);
    WINDOW* wall = newwin(1, 1, 0, 0);
    WINDOW* stand_available = newwin(1, 1, 0, 0);

    WINDOW* wnorth = newwin(1, 1, locay, locax);
    WINDOW* wsouth = newwin(1, 1, locay, locax);
    WINDOW* weast = newwin(1, 1, locay, locax);
    WINDOW* wwest = newwin(1, 1, locay, locax);

    wbkgdset(location_point_naive, COLOR_PAIR(10));
    box(location_point_naive, 0, 0);
    wbkgdset(location_point, COLOR_PAIR(11));
    box(location_point, 0, 0);
    wbkgdset(wall, COLOR_PAIR(12));
    box(wall, 0, 0);
    wbkgdset(stand_available, COLOR_PAIR(13));
    box(stand_available, 0, 0);

    wbkgdset(wnorth, COLOR_PAIR(1));
    box(wnorth, 0, 0);
    wbkgdset(wsouth, COLOR_PAIR(2));
    box(wnorth, 0, 0);
    wbkgdset(weast, COLOR_PAIR(3));
    box(wnorth, 0, 0);
    wbkgdset(wwest, COLOR_PAIR(4));
    box(wnorth, 0, 0);

    wbkgdset(wBSP, COLOR_PAIR(9));
    box(wBSP, 0, 0);

    wbkgdset(wNaive, COLOR_PAIR(9));
    box(wNaive, 0, 0);

    std::function<void(char*)> BSP_context = [&wBSP](char* time_ms) {

        delwin(wBSP);
        wBSP = newwin(1, 200, 0, 0);
        wbkgdset(wBSP, COLOR_PAIR(9));
        box(wBSP, 0, 0);

        wattron(wBSP, A_STANDOUT);
        mvwprintw(wBSP, 0,4, "Method: BSP");

        mvwprintw(wBSP, 0,55, "time cost: ");
        mvwprintw(wBSP, 0,85, time_ms);
        mvwprintw(wBSP, 0,105, "ms");

        wattroff(wBSP, A_STANDOUT);
        wrefresh(wBSP);
    };

    std::function<void(char*)> Naive_context = [&wNaive](char* time_ms) {

        delwin(wNaive);
        wNaive = newwin(1, 200, 0, 0);
        wbkgdset(wNaive, COLOR_PAIR(9));
        box(wNaive, 0, 0);

        wattron(wNaive, A_STANDOUT);
        mvwprintw(wNaive, 0,4, "Method: Naive");

        mvwprintw(wNaive, 0,55, "time cost: ");
        mvwprintw(wNaive, 0,85, time_ms);
        mvwprintw(wNaive, 0,105, "ms");

        wattroff(wNaive, A_STANDOUT);
        wrefresh(wNaive);
    };

        //light purple
    std::function<void(int y, int x)> location_point_naive_mod = [&location_point_naive](int y, int x) {

        delwin(location_point_naive);
        location_point_naive = newwin(1, 1, y+2, x);
        wbkgdset(location_point_naive, COLOR_PAIR(10));
        box(location_point_naive, 0, 0);
        wrefresh(location_point_naive);
    };

    //purple
    std::function<void(int y, int x)> location_point_mod = [&location_point](int y, int x) {

        delwin(location_point);
        location_point = newwin(1, 1, y+2, x);
        wbkgdset(location_point, COLOR_PAIR(11));
        box(location_point, 0, 0);
        wrefresh(location_point);
    };

    std::function<void(int y, int x)> wall_mod = [&wall](int y, int x) {

        delwin(wall);
        wall = newwin(1, 1, y+2, x);
        wbkgdset(wall, COLOR_PAIR(12));
        box(wall, 0, 0);
        wrefresh(wall);
    };

    std::function<void(int y, int x)> stand_available_mod = [&stand_available](int y, int x) {

        delwin(stand_available);
        stand_available = newwin(1, 1, y+2, x);
        wbkgdset(stand_available, COLOR_PAIR(13));
        box(stand_available, 0, 0);
        wrefresh(stand_available);
    };

    std::function<void()> north_mod = [&wnorth]() {

        delwin(wnorth);
        wnorth = newwin(1, 1, Common::Maze::rows-Location::north_wall_point.imag()+2, Location::north_wall_point.real());
        wbkgdset(wnorth, COLOR_PAIR(1));
        box(wnorth, 0, 0);
        wrefresh(wnorth);
    };

    std::function<void()> south_mod = [&wsouth]() {

        delwin(wsouth);
        wsouth = newwin(1, 1, Common::Maze::rows-Location::south_wall_point.imag()+2, Location::south_wall_point.real());
        wbkgdset(wsouth, COLOR_PAIR(2));
        box(wsouth, 0, 0);
        wrefresh(wsouth);
    };

    std::function<void()> east_mod = [&weast]() {

        delwin(weast);
        weast = newwin(1, 1, Common::Maze::rows-Location::east_wall_point.imag()+2, Location::east_wall_point.real());
        wbkgdset(weast, COLOR_PAIR(3));
        box(weast, 0, 0);
        wrefresh(weast);
    };

    std::function<void()> west_mod = [&wwest]() {

        delwin(wwest);
        wwest = newwin(1, 1, Common::Maze::rows-Location::west_wall_point.imag()+2, Location::west_wall_point.real());
        wbkgdset(wwest, COLOR_PAIR(4));
        box(wwest, 0, 0);
        wrefresh(wwest);
    };

   if(Common::Maze::rows*Common::Maze::cols>1024*1024){
   
        std::cout<<"Your maze is too big; make sure that your maze is not bigger than 1024*1024. This program is quitting...\n";
        goto exits;
   }
    
   bkgd(COLOR_BLACK);
   refresh();
   
   for (int i = 0; i < 256; ++i)
       for (int j = 0; j < 256; ++j)
            stand_available_mod(i,j);

    for (int i = 0; i <= Common::Maze::rows; ++i)
       for (int j = 0; j <= Common::Maze::cols; ++j)
           if(Common::Maze::maze[Common::Maze::rows-i][j]==1)
                wall_mod(i,j);

    curs_set(0);
    noecho();

prints:

    Common::Location::reset();
    if(use_BSP){

        start = std::chrono::system_clock::now();
        leaf_node = BSP::Segment::to_leaf(root.second, std::complex<int>(locax, Common::Maze::rows-locay));
        BSP::Segment::update_location_walls(leaf_node, std::complex<int>(locax, Common::Maze::rows-locay), Common::Maze::rows,Common::Maze::cols);
        end = std::chrono::system_clock::now();

        time_cost_str = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        time_cost_char = &time_cost_str[0];

        BSP_context(time_cost_char);

        location_point_mod(locay,locax);
    }else{
        
        start = std::chrono::system_clock::now();
        Location::point = std::complex<int>(locax, Common::Maze::rows-locay);
        naive_obj->update_walls();
        end = std::chrono::system_clock::now();

        time_cost_str = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        time_cost_char = &time_cost_str[0];

        Naive_context(time_cost_char);

        location_point_naive_mod(locay,locax);
    }
        
    north_mod();
    south_mod();
    east_mod();
    west_mod();

    keypad(location_point, true);

    input = wgetch(location_point);
    input = input==ERR?27:input;  	

    switch (input) {

    case KEY_UP:
        lasty = locay; 
        lastx = locax;   
        
        if(Common::Maze::maze[Common::Maze::rows-lasty+1][lastx]!=1){

            locay = 1 > --locay ? 1 : locay;
        }
       
        break;

    case KEY_DOWN:
        lasty = locay; 
        lastx = locax;
        
        if(Common::Maze::maze[Common::Maze::rows-lasty-1][lastx]!=1){
        
            locay = Common::Maze::rows-1 < ++locay ? Common::Maze::rows-1 : locay;
        }

        break;

    case KEY_LEFT:
        lastx = locax; 
        lasty = locay;

        if(Common::Maze::maze[Common::Maze::rows-lasty][lastx-1]!=1){

           locax = 1 > --locax ? 1 : locax;
        }
        
        break;

    case KEY_RIGHT:
        lastx = locax; 
        lasty = locay;

        if(Common::Maze::maze[Common::Maze::rows-lasty][lastx+1]!=1){

           locax = Common::Maze::cols-2 < ++locax ? Common::Maze::cols-2 : locax;
        }

        break;

    default:
        lastx = locax; 
        lasty = locay;
        break;
    }   

    if (input == 27) {
    
        exits:
            delwin(location_point_naive);
            delwin(location_point);
            delwin(wall);
            delwin(stand_available);

            delwin(wnorth);
            delwin(wsouth);
            delwin(weast);
            delwin(wwest);

            delwin(wBSP);
            delwin(wNaive);

            delete current_segment;
            delete naive_obj;
            delete root.second;

            endwin();
    }
    else {

        if(input == 9) {
    
            use_BSP = ++use_BSP%2;
            goto prints;
        }
        wall_mod(Common::Maze::rows-Location::north_wall_point.imag(), Location::north_wall_point.real());
        wall_mod(Common::Maze::rows-Location::south_wall_point.imag(), Location::south_wall_point.real());
        wall_mod(Common::Maze::rows-Location::east_wall_point.imag(), Location::east_wall_point.real());
        wall_mod(Common::Maze::rows-Location::west_wall_point.imag(), Location::west_wall_point.real());
        stand_available_mod(lasty,lastx);

        goto prints;
    }

    
    endwin();
}


int main(int argc, char** argv )
{
    std::string valve="1";
    int no_maze = 0;
    std::string input_path = "./";
    input_path += argv[1];
    input_path += "/maze.pnm";

    char* argv_chars = argv[2];
    std::string stra = argv_chars;
    std::string strb = "a";

    if(!stra.empty()){

        argv_chars = argv[3];
        strb = argv_chars;
    }

    std::function<std::complex<int>()> initial_point = [](){
    
        int random_num = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();

        if(Common::Maze::maze[random_num%3][random_num%7]==0)    
            return std::complex<int> (random_num%7, random_num%3);        

        for(int i=0; i< Common::Maze::rows; ++i)              
            for(int j=0; j< Common::Maze::cols; ++j)
                 if(Common::Maze::maze[i][j]==0)  return std::complex<int> (j,i);

        return std::complex<int> (0,0);
    };

    no_maze = Common::Maze::load_maze(input_path);
    if(no_maze){
        
        std::cout<<"no maze.pnm found"<<std::endl;
        return 0;   
    }
    
    try{
        
        int a = stoi(stra);
        int b = stoi(strb);

        if(a<1||a>Common::Maze::cols-2 || b<1 || b>Common::Maze::cols-2) goto initial;

        if(Common::Maze::maze[b][a]==0)
            run(a,b);
        else{
        
            initial:
                std::complex<int> point = initial_point();
                if(point.real() == 0 && point.imag() ==0 ){
            
                    std::cout<<"the input maze is bad cuz no standable point; exiting..."<<std::endl;
                    return 1;
                }
                else{
            
                    run(point.real(), point.imag());
                }                
        }
        
    }
    catch(exception& e) {
    
        std::complex<int> point = initial_point();
        if(point.real() == 0 && point.imag() ==0 ){
            
            std::cout<<"the input maze is bad cuz no standable point; exiting..."<<std::endl;
            return 1;
        }
        else{
            
            run(point.real(), point.imag());
        }              
    }
    
    std::cout<<"Exited with 0."<<std::endl;
    return 0;
}
