# Find the MySQL headers and library
# Based on http://www.cmake.org/Wiki/CMakeUserFindMySQL
#
#    MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#    MYSQL_LIBRARIES   - List of libraries when using MySQL.
#    MYSQL_FOUND       - True if MySQL found.

IF (MYSQL_INCLUDE_DIR)
    # Already in cache, be silent
    SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
	PATHS
		/sw/include
		/opt/mysql/include
		/usr/local/mysql/include
	PATH_SUFFIXES
		mysql
)

SET(MYSQL_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MYSQL_LIBRARY
    NAMES ${MYSQL_NAMES}
    PATHS
		/sw/lib
		/opt/mysql/lib
		/usr/lib/x86_64-linux-gnu
    PATH_SUFFIXES
		mysql
)

IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
    SET(MYSQL_FOUND TRUE)
    SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
    SET(MYSQL_FOUND FALSE)
    SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF (MYSQL_FOUND)
    IF (NOT MYSQL_FIND_QUIETLY)
        MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
    ENDIF (NOT MYSQL_FIND_QUIETLY)
ELSE (MYSQL_FOUND)
    IF (MYSQL_FIND_REQUIRED)
        MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
        MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
    ENDIF (MYSQL_FIND_REQUIRED)
ENDIF (MYSQL_FOUND)

MARK_AS_ADVANCED(
    MYSQL_LIBRARY
    MYSQL_INCLUDE_DIR
)
