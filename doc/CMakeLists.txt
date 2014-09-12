# doc/CMakeLists.txt - Tells CMake how to build the libtabula docs.
#    See ../CMakeLists.txt for system-wide matters.
#
# Created 2014.09.09 by Warren Young
#
# Copyright © 2014 by Educational Technology Resources, Inc.
#
# Others may also hold copyrights on code in this file.  See the
# CREDITS.md file in the top directory of the distribution for details.
#
# This file is part of libtabula.
#
# libtabula is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# libtabula is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with libtabula; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
# USA

# Add a refman target if Doxygen is installed here
find_package(Doxygen)
if(DOXYGEN_FOUND)
	file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc/html/refman)
	add_custom_target(refman
		${DOXYGEN_EXECUTABLE}
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
else()
	message(STATUS "Did not find Doxygen, so refman cannot be built!")
endif(DOXYGEN_FOUND)

# Is xsltproc here?
find_program(XSLTPROC
  NAMES xsltproc
  DOC   "path to xsltproc from libxslt >= 1.1.27")
if(XSLTPROC)
	# It's here, but is it new enough?  xsltproc from libxslt 1.1.26 and
	# earlier can't handle XIncludes that ping-ping between the source
	# and build directories.  A single include across the boundary is
	# fine, but we have cases where an XIncluded file then XIncludes
	# something back in the other directory, whereupon older xsltproc
	# versions fail because they assume all further XIncludes will be
	# in the directory it found the first XIncluded file in.
	execute_process(COMMAND ${XSLTPROC} --version
		OUTPUT_VARIABLE XSLTPROC_VERSION
		ERROR_QUIET)
	if(XSLTPROC_VERSION)
		string(REGEX MATCH "libxslt ([0-9]+)" DUMMY ${XSLTPROC_VERSION})
		if(CMAKE_MATCH_1 AND (CMAKE_MATCH_1 GREATER 10126))
			message(STATUS "xstlproc found, and new enough")
			set(WORKING_XSLTPROC 1)
		else()
			message(STATUS "xsltproc missing or too old (${CMAKE_MATCH_1}); need 1.1.27+!")
		endif()
	endif(XSLTPROC_VERSION)
endif(XSLTPROC)

# Add a userman target if xsltproc checks succeeded.
# TODO: Explicitly test for the DocBook XSL stylesheets, too.
if(WORKING_XSLTPROC)
	# Definitions
	set(USERMAN_BINDIR ${CMAKE_CURRENT_BINARY_DIR}/userman)
	set(USERMAN_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/userman)
	set(XSLT_PATH      "${USERMAN_SRCDIR}:${USERMAN_BINDIR}")

	set(USERMAN_PDFDIR ${CMAKE_BINARY_DIR}/doc/pdf)
	set(USERMAN_WEBDIR ${CMAKE_BINARY_DIR}/doc/html/userman)

	set(DBX_BASE userman)

	set(USERMAN_DBX  ${USERMAN_BINDIR}/${DBX_BASE}.dbx)
	set(USERMAN_HTML ${USERMAN_WEBDIR}/index.html)
	set(USERMAN_PDF  ${USERMAN_PDFDIR}/${DBX_BASE}.pdf)
	set(USERMAN_FO   ${USERMAN_BINDIR}/${DBX_BASE}.fo)
	file(GLOB USERMAN_CHAPTERS ${USERMAN_SRCDIR} "*.dbx")

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
		set(outFileFull ${USERMAN_BINDIR}/${outFile})
		set(mktxt       ${USERMAN_SRCDIR}/mktxt)

		add_custom_command(OUTPUT ${outFileFull}
			COMMAND ${mktxt} ${inFileFull} ${outFileFull}
			DEPENDS ${inFileFull}
			COMMENT "Generating ${inFile} from ${outFile}")

		set(outTxtFiles ${outTxtFiles} ${outFileFull})
	endforeach(outFile)

	# TODO: Add ${EX_TXT} to clean.  Can't just remove *.txt, because that
	# would remove *this* file, too.

	# Define the high-level userman target, which builds PDF and HTML versions
	file(MAKE_DIRECTORY ${USERMAN_PDFDIR})
	file(MAKE_DIRECTORY ${USERMAN_WEBDIR})
	add_custom_target(userman
		DEPENDS ${USERMAN_HTML} ${USERMAN_PDF}
		COMMENT "Generating user manual from DocBook" VERBATIM
	)

	# Build userman.pdf from the DocBook inputs (*.dbx and *.txt)
	add_custom_command(
		OUTPUT ${USERMAN_PDF} ${USERMAN_FO}
		COMMAND ${XSLTPROC} --xinclude --path "${XSLT_PATH}" --nonet
			${FO_SS} ${USERMAN_DBX} > ${USERMAN_FO}
		COMMAND 
			${USERMAN_SRCDIR}/fo2pdf ${USERMAN_FO} ${USERMAN_PDF}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		MAIN_DEPENDENCY ${USERMAN_DBX}
		DEPENDS ${USERMAN_CHAPTERS} ${outTxtFiles}
		COMMENT "Generating PDF version of the user manual" VERBATIM
	)

	# Build HTML version of userman from the same inputs
	add_custom_command(
		OUTPUT ${USERMAN_HTML}
		COMMAND
			${XSLTPROC} --xinclude --path "${XSLT_PATH}" --nonet
			-o ${USERMAN_WEBDIR}/ ${HTML_SS} ${USERMAN_DBX}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		MAIN_DEPENDENCY ${USERMAN_DBX}
		DEPENDS ${USERMAN_CHAPTERS} ${outTxtFiles}
		COMMENT "Generating HTML version of the user manual" VERBATIM
	)
endif(WORKING_XSLTPROC)

# Set up the doc target based on the refman/userman tool checks above
if(DOXYGEN_FOUND AND WORKING_XSLTPROC)
	message(STATUS "doc target will build refman and userman")
	add_custom_target(doc DEPENDS refman userman)
elseif(DOXYGEN_FOUND)
	message(STATUS "doc target will build refman only")
	add_custom_target(doc DEPENDS refman)
elseif(WORKING_XSLTPROC)
	message(STATUS "doc target will build userman only")
	add_custom_target(doc DEPENDS userman)
else()
	message(STATUS "Not generating a doc target: ${DOXYGEN_FOUND}")
endif()
