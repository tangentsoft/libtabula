# Add a userman target if the DocBook XSL tools are installed here
find_package(DocBook)
if(DOCBOOK_FOUND)
	# Definitions
	set(USERMAN_BINDIR ${CMAKE_CURRENT_BINARY_DIR})
	set(USERMAN_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR})
	set(USERMAN_WEBDIR ${CMAKE_BINARY_DIR}/doc/html/userman)

	set(DBX_BASE userman)

	set(USERMAN_DBX ${USERMAN_BINDIR}/${DBX_BASE}.dbx)
	set(USERMAN_PDF ${USERMAN_BINDIR}/${DBX_BASE}.pdf)
	set(USERMAN_FO  ${USERMAN_SRCDIR}/${DBX_BASE}.fo)

	set(COMMON_SS ${USERMAN_SRCDIR}/common.xsl)
	set(FO_SS     ${USERMAN_SRCDIR}/fo.xsl)
	set(HTML_SS   ${USERMAN_SRCDIR}/html.xsl)

	# Build *.txt from the subset of examples/*.{cpp,h} needed for
	# textual inclusion into the userman.
	set(EX_TXT cgi_jpeg.txt cpool.txt deadlock.txt fieldinf.txt
			for_each.txt load_jpeg.txt multiquery.txt
			resetdb.txt simple1.txt simple2.txt simple3.txt
			ssqls1.txt ssqls2.txt ssqls3.txt ssqls4.txt
			ssqls5.txt ssqls6.txt stock.txt store_if.txt
			tquery1.txt transaction.txt)
	set(outTxtFiles)
	
	# Convert the list of *.txt files to a set of commands for building
	# them from the input example files.
	#
	# TODO: Make this conditional on whether mktxt can run here.  Be
	# sure the condition doesn't prevent txtFiles target from working
	# in dist trees, where *.txt will already exist.
	foreach(outFile ${EX_TXT})
		string(REPLACE ".txt" ".cpp" inFile ${outFile})
		if(NOT EXISTS ${CMAKE_SOURCE_DIR}/examples/${inFile})
			string(REPLACE ".txt" ".h" inFile ${outFile})
		endif()

		set(inFileFull  ${CMAKE_SOURCE_DIR}/examples/${inFile})
		set(outFileFull ${CMAKE_CURRENT_BINARY_DIR}/${outFile})
		set(mktxt       ${CMAKE_CURRENT_SOURCE_DIR}/mktxt)

		add_custom_command(OUTPUT ${outFileFull}
			COMMAND ${mktxt} ${inFileFull} ${outFileFull}
			DEPENDS ${inFileFull}
			COMMENT "Generating ${inFile} from ${outFile}")

		set(outTxtFiles ${outTxtFiles} ${outFile})
	endforeach(outFile)

	# TODO: Add ${EX_TXT} to clean.  Can't just remove *.txt, because that
	# would remove *this* file, too.

	# Build the actual userman from the *.dbx and *.txt inputs
	file(MAKE_DIRECTORY ${USERMAN_WEBDIR})
	add_custom_target(userman
		${DOCBOOK_XSLT_PROCESSOR} --xinclude 
			--path "${USERMAN_SRCDIR}:${USERMAN_BINDIR}"
			${FO_SS} ${USERMAN_DBX} > ${USERMAN_FO}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS ${outTxtFiles}
		COMMENT "Generating user manual from DocBook" VERBATIM
	)
endif(DOCBOOK_FOUND)
