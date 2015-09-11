# This subproject extracts out some of the more complex tests from the
# top-level file, such as those that require attempted compilation.

project("cmake_compile_tests")

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

# Define a macro to check for post-C++98 features, called from the top level
#
# Original version by Rolf Eike Beer
#
# Modifications by Andreas Weis: http://pageant.ghulbus.eu/?p=664
# Explicitly placed into the public domain
#
# Modified for libtabula, primarily by removing unneeded functionality.
MACRO(CXX_CHECK_FEATURE FEATURE_NAME RESULT_VAR)
	IF (NOT DEFINED ${RESULT_VAR})
		SET(LOG_NAME "\"${FEATURE_NAME}\"")
		string(REPLACE " " "_" BASE_NAME "c++-test-${FEATURE_NAME}")
		SET(BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME}")
		SET(SRC_FILE_BASE ${CMAKE_SOURCE_DIR}/cmake/${BASE_NAME})
		MESSAGE(STATUS "Checking for C++ ${LOG_NAME} feature...")

		SET(SRC_FILE "${SRC_FILE_BASE}.cpp")
		SET(SRC_FILE_FAIL "${SRC_FILE_BASE}_fail.cpp")
		SET(SRC_FILE_FAIL_COMPILE "${SRC_FILE_BASE}_fail_compile.cpp")

		IF (CROSS_COMPILING)
			try_compile(${RESULT_VAR} "${BIN_DIR}" "${SRC_FILE}")
			IF (${RESULT_VAR} AND EXISTS ${SRC_FILE_FAIL})
				try_compile(${RESULT_VAR} "${BIN_DIR}_fail" "${SRC_FILE_FAIL}")
			ENDIF (${RESULT_VAR} AND EXISTS ${SRC_FILE_FAIL})
		ELSE (CROSS_COMPILING)
			try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR
					"${BIN_DIR}" "${SRC_FILE}")
			IF (_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
				SET(${RESULT_VAR} TRUE)
			ELSE (_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
				SET(${RESULT_VAR} FALSE)
			ENDIF (_COMPILE_RESULT_VAR AND NOT _RUN_RESULT_VAR)
			IF (${RESULT_VAR} AND EXISTS ${SRC_FILE_FAIL})
				try_run(_RUN_RESULT_VAR _COMPILE_RESULT_VAR
						"${BIN_DIR}_fail" "${SRC_FILE_FAIL}")
				IF (_COMPILE_RESULT_VAR AND _RUN_RESULT_VAR)
					SET(${RESULT_VAR} TRUE)
				ELSE (_COMPILE_RESULT_VAR AND _RUN_RESULT_VAR)
					SET(${RESULT_VAR} FALSE)
				ENDIF (_COMPILE_RESULT_VAR AND _RUN_RESULT_VAR)
			ENDIF (${RESULT_VAR} AND EXISTS ${SRC_FILE_FAIL})
		ENDIF (CROSS_COMPILING)
		IF (${RESULT_VAR} AND EXISTS ${SRC_FILE_FAIL_COMPILE})
			try_compile(_TMP_RESULT "${BIN_DIR}_fail_compile" "${SRC_FILE_FAIL_COMPILE}")
			IF (_TMP_RESULT)
				SET(${RESULT_VAR} FALSE)
			ELSE (_TMP_RESULT)
				SET(${RESULT_VAR} TRUE)
			ENDIF (_TMP_RESULT)
		ENDIF (${RESULT_VAR} AND EXISTS ${SRC_FILE_FAIL_COMPILE})

		IF (${RESULT_VAR})
			MESSAGE(STATUS "C++ ${LOG_NAME} feature supported")
		ELSE (${RESULT_VAR})
			MESSAGE(STATUS "C++ ${LOG_NAME} feature NOT supported")
		ENDIF (${RESULT_VAR})
		SET(${RESULT_VAR} ${${RESULT_VAR}} CACHE INTERNAL "C++ feature ${LOG_NAME}")
	ENDIF (NOT DEFINED ${RESULT_VAR})
ENDMACRO(CXX_CHECK_FEATURE)
