# Create and install translation files

set(Gettext_FOUND ON CACHE BOOL "Are Gettext tools available?")

function(REQUIRE_BINARY varname)
	if ("${${varname}}" STREQUAL "${varname}-NOTFOUND" OR "${${varname}}" STREQUAL "")
		set(Gettext_FOUND OFF CACHE BOOL "Are Gettext tools available?" FORCE)
	endif()
endfunction()

find_program(Gettext_XGETTEXT_EXECUTABLE xgettext
	HINTS ${GETTEXT_DIR} ${XGETTEXT} ${BINARIES}
	)
REQUIRE_BINARY(Gettext_XGETTEXT_EXECUTABLE)

find_program(Gettext_MSGINIT_EXECUTABLE msginit
	HINTS ${GETTEXT_DIR} ${MSGINIT} ${BINARIES}
	)
REQUIRE_BINARY(Gettext_MSGINIT_EXECUTABLE)

find_program(Gettext_MSGMERGE_EXECUTABLE msgmerge
	HINTS ${GETTEXT_DIR} ${MSGMERGE} ${BINARIES}
	)
REQUIRE_BINARY(Gettext_MSGMERGE_EXECUTABLE)

find_program(Gettext_MSGFMT_EXECUTABLE msgfmt
	HINTS ${GETTEXT_DIR} ${MSGFMT} ${BINARIES}
	)
REQUIRE_BINARY(Gettext_MSGFMT_EXECUTABLE)

mark_as_advanced(
	Gettext_MSGFMT_EXECUTABLE 
	Gettext_MSGINIT_EXECUTABLE 
	Gettext_MSGMERGE_EXECUTABLE 
	Gettext_XGETTEXT_EXECUTABLE
	)

function(TRANSLATE srcdir domainname)
	message("Adding targets to generate translations for ${domainname}")
	# create directory for po files
	if (NOT EXISTS "${srcdir}/po")
		file(MAKE_DIRECTORY "${srcdir}/po")
	endif()

	file(GLOB_RECURSE SOURCES "${srcdir}/*.cpp")

	# create pot file
	set(POT_FILE "${CMAKE_BINARY_DIR}/${domainname}.pot")
	add_custom_command(OUTPUT "${POT_FILE}"
		COMMAND "${Gettext_XGETTEXT_EXECUTABLE}"
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
				COMMAND "${Gettext_MSGINIT_EXECUTABLE}"
				--input="${POT_FILE}"
				--output-file="${POFILE}"
				--locale=${LANG}
				WORKING_DIRECTORY "${srcdir}/po"
				)
		else()
			# update po file
			set(MO_DEP "${POFILE}.update")
			add_custom_command(OUTPUT "${POFILE}.update"
				COMMAND "${Gettext_MSGMERGE_EXECUTABLE}"
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
			COMMAND "${Gettext_MSGFMT_EXECUTABLE}"
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

