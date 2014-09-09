add_subdirectory(userman)

# Add a refman target if Doxygen is installed here
find_package(Doxygen)
if(DOXYGEN_FOUND)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/refman)
	add_custom_target(refman
		${DOXYGEN_EXECUTABLE}
		${CMAKE_CURRENT_BINARY_DIR}/../src/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)

# Set up the doc target based on above refman/userman tool checks
if(DOXYGEN_FOUND AND DOCBOOK_FOUND)
	add_custom_target(doc DEPENDS refman userman)
elseif(DOXYGEN_FOUND)
	add_custom_target(doc DEPENDS refman)
elseif(DOXYGEN_FOUND)
	add_custom_target(doc DEPENDS userman)
endif()
