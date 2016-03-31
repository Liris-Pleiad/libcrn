# Create and install translation files

function(REQUIRE_BINARY binname varname)
	if (defined ${${varname}-NOTFOUND})
		message(FATAL_ERROR "Could not find " binname)
	endif()
endfunction()

find_program(XGETTEXT_EXECUTABLE xgettext
	HINTS ${XGETTEXT} ${BINARIES}
	)
REQUIRE_BINARY(xgettext XGETTEXT_EXECUTABLE)

find_program(MSGINIT_EXECUTABLE msginit
	HINTS ${MSGINIT} ${BINARIES}
	)
REQUIRE_BINARY(msginit MSGINIT_EXECUTABLE)

find_program(MSGFILTER_EXECUTABLE msgfilter
	HINTS ${MSGFILTER} ${BINARIES}
	)
REQUIRE_BINARY(msgfilter MSGFILTER_EXECUTABLE)

find_program(MSGCONV_EXECUTABLE msgconv
	HINTS ${MSGCONV} ${BINARIES}
	)
REQUIRE_BINARY(msgconv MSGCONV_EXECUTABLE)

find_program(MSGMERGE_EXECUTABLE msgmerge
	HINTS ${MSGMERGE} ${BINARIES}
	)
REQUIRE_BINARY(msgmerge MSGMERGE_EXECUTABLE)

find_program(MSGFMT_EXECUTABLE msgfmt
	HINTS ${MSGFMT} ${BINARIES}
	)
REQUIRE_BINARY(msgfmt MSGFMT_EXECUTABLE)

mark_as_advanced(
	MSGCONV_EXECUTABLE 
	MSGFILTER_EXECUTABLE 
	MSGFMT_EXECUTABLE 
	MSGINIT_EXECUTABLE 
	MSGMERGE_EXECUTABLE 
	XGETTEXT_EXECUTABLE
	)

function(TRANSLATE srcdir domainname)
	# create directory for po files
	if (NOT EXISTS "${srcdir}/po")
		file(MAKE_DIRECTORY "${srcdir}/po")
	endif()

	file(GLOB_RECURSE SOURCES "${srcdir}/*.cpp")

	# create pot file
	set(POT_FILE "${CMAKE_BINARY_DIR}/${domainname}.pot")
	add_custom_command(OUTPUT "${POT_FILE}"
		COMMAND "${XGETTEXT_EXECUTABLE}"
			-o "${POT_FILE}"
			-c++
			--from-code=utf-8
			--keyword=_
			--keyword=N_
		${SOURCES}
		DEPENDS ${SOURCES}
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		)

	if(NOT EXISTS "${CMAKE_BINARY_DIR}/locale")
		file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/locale")
	endif()

	set(MO_FILES)
	foreach(LANG ${LANGUAGES})
		# create or update po files
		set(POFILE "${srcdir}/po/${LANG}.po")
		if (NOT EXISTS "${POFILE}")
			# create po file
			set(MO_DEP "${POFILE}")
			add_custom_command(OUTPUT "${POFILE}"
				COMMAND "${MSGINIT_EXECUTABLE}"
				--input="${POT_FILE}"
				--output-file="${POFILE}"
				--locale=${LANG}
				WORKING_DIRECTORY "${srcdir}/po"
				)
		else()
			# update po file
			set(MO_DEP "${POFILE}.update")
			add_custom_command(OUTPUT "${POFILE}.update"
				COMMAND "${MSGMERGE_EXECUTABLE}"
				-U "${POFILE}"
				"${POT_FILE}"
				DEPENDS "${POT_FILE}"
				WORKING_DIRECTORY "${srcdir}/po"
				)
		endif()

		# create or update mo files
		set(MOFILE "${CMAKE_BINARY_DIR}/locale/${LANG}/LC_MESSAGES/${domainname}.mo")
		if(NOT EXISTS "${CMAKE_BINARY_DIR}/locale/${LANG}")
			file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/locale/${LANG}")
		endif()
		if(NOT EXISTS "${CMAKE_BINARY_DIR}/locale/${LANG}/LC_MESSAGES")
			file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/locale/${LANG}/LC_MESSAGES")
		endif()
		list(APPEND MO_FILES "${MOFILE}")
		add_custom_command(OUTPUT "${MOFILE}"
			COMMAND "${MSGFMT_EXECUTABLE}"
			-o "${MOFILE}"
			"${POFILE}"
			DEPENDS "${MO_DEP}"
			WORKING_DIRECTORY "${srcdir}/po"
			)
	endforeach()

	add_custom_target(${domainname}_translate ALL
		DEPENDS ${MO_FILES}
		)

endfunction()

function(INSTALL_TRANSLATIONS)
	install(DIRECTORY "${CMAKE_BINARY_DIR}/locale/" DESTINATION "share/locale" COMPONENT bin)
endfunction()

