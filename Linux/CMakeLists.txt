cmake_minimum_required(VERSION 2.8)
project(BSPSearchingWalls)

add_definitions(-w)

set(HOME_PATH_ROOT ${CMAKE_CURRENT_SOURCE_DIR}) 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(THREADS_PREFER_PTHREAD_FLAG true)
set(CMAKE_VERBOSE_MAKEFILE true)
set(Boost_USE_MULTITHREADED ON) 

option(CMAKE_INSTALL_PREFIX "${HOME_PATH_ROOT}")
option(ENABLE_ASAN  false)
option(ENABLE_LSAN  false)

find_path(NCURSESPATH ncurses REQUIRED)
find_path(CATCHPATH catch2 REQUIRED) 
find_package(Curses REQUIRED)

include(Sanitizers.cmake)

if (ENABLE_ASAN)

	APPEND_ASAN_COMPILER_LINKER()
endif()

if (ENABLE_LSAN)

	APPEND_LSAN_COMPILER_LINKER()
endif()

find_package(Threads REQUIRED) 

find_package(Boost REQUIRED) 

add_library(ProjLibs  ${HOME_PATH_ROOT}/Components.hpp 
                     ${HOME_PATH_ROOT}/Components.cpp
                     ${HOME_PATH_ROOT}/runner.cpp 
)

# Project sources
add_executable(runner ${HOME_PATH_ROOT}/runner.cpp )

target_include_directories(ProjLibs PUBLIC include "${HOME_PATH_ROOT}" "${Boost_INCLUDE_DIRS}" ${CURSES_INCLUDE_DIRS}) 
target_compile_definitions(ProjLibs PUBLIC "-DBOOST_LOG_DYN_LINK")
install(TARGETS ProjLibs DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${HOME_PATH_ROOT}/Components.hpp ${HOME_PATH_ROOT}/Components.cpp  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin) 

target_link_libraries(ProjLibs ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CURSES_INCLUDE_DIR} ${CURSES_LIBRARIES})
target_link_libraries(runner ProjLibs) 
install(TARGETS runner DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(PROGRAMS ${HOME_PATH_ROOT}/demo DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(PROGRAMS ${HOME_PATH_ROOT}/maze.pnm DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
