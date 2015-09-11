project("c++11_test")

# Does the compiler need special options to use post-C++98 features?
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" FOUND_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" FOUND_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" FOUND_CXX0X)
if (FOUND_CXX14)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif (FOUND_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()
