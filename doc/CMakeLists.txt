add_subdirectory(userman)

# Set up the doc target based on the refman/userman tool checks
if(DOXYGEN_FOUND AND DOCBOOK_FOUND)
	add_custom_target(doc DEPENDS refman userman)
elseif(DOXYGEN_FOUND)
	add_custom_target(doc DEPENDS refman)
elseif(DOXYGEN_FOUND)
	add_custom_target(doc DEPENDS userman)
endif()
