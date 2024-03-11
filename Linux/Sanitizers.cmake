function(APPEND_ASAN_COMPILER_LINKER)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address" PARENT_SCOPE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address" PARENT_SCOPE)
        set(ENV{ASAN_OPTIONS} "check_initialization_order=1" PARENT_SCOPE) 
        #message(${CMAKE_CXX_FLAGS})
        #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}" PARENT_SCOPE)
        #message(STATUS "Appending code coverage compiler flags: ${COVERAGE_COMPILER_FLAGS}")
endfunction() 


function(APPEND_LSAN_COMPILER_LINKER)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak" PARENT_SCOPE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=leak" PARENT_SCOPE)
        set(ENV{LSAN_OPTIONS} "detect_leaks=1" PARENT_SCOPE)
endfunction() 
