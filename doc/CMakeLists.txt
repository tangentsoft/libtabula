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

# Add a userman target if the DocBook XSL tools are installed here
find_package(DocBook)
if(DOCBOOK_FOUND)
	set(USERMAN_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/userman)
	set(USERMAN_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/userman)
	set(USERMAN_WEBDIR ${CMAKE_CURRENT_BINARY_DIR}/html/userman)

	set(DBX_BASE userman)

	set(USERMAN_DBX ${USERMAN_BINDIR}/${DBX_BASE}.dbx)
	set(USERMAN_PDF ${USERMAN_BINDIR}/${DBX_BASE}.pdf)
	set(USERMAN_FO  ${USERMAN_SRCDIR}/${DBX_BASE}.fo)

	set(COMMON_SS ${USERMAN_SRCDIR}/common.xsl)
	set(FO_SS     ${USERMAN_SRCDIR}/fo.xsl)
	set(HTML_SS   ${USERMAN_SRCDIR}/html.xsl)

	file(MAKE_DIRECTORY ${USERMAN_WEBDIR})
	add_custom_target(userman
		${DOCBOOK_XSLT_PROCESSOR} --xinclude --path ${USERMAN_SRCDIR} 
			${FO_SS} ${USERMAN_DBX} > ${USERMAN_FO}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating user manual from DocBook" VERBATIM
	)
endif(DOCBOOK_FOUND)

# Set up the doc target based on above refman/userman tool checks
if(DOXYGEN_FOUND AND DOCBOOK_FOUND)
	add_custom_target(doc DEPENDS refman userman)
elseif(DOXYGEN_FOUND)
	add_custom_target(doc DEPENDS refman)
elseif(DOXYGEN_FOUND)
	add_custom_target(doc DEPENDS userman)
endif()
