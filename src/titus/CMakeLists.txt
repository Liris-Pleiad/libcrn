# titus build script

#############################
# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/include/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/src/libcrn")
# add path to libgtkcrnmm
include_directories("${PROJECT_SOURCE_DIR}/src/libgtkcrnmm")
#############################

# sources
file(
	GLOB_RECURSE
	TITUS_SRC
	*.cpp
)
file(
	GLOB_RECURSE
	TITUS_HEADER
	*.h
	*.hpp
)
if (MSVC)
	GroupSources(src/titus)
	add_definitions("/wd4250")
endif(MSVC)

# For Windows set the .ico file with the .rc file
if( MSVC )
	set( resource_files ${resource_files}
	  "${PROJECT_SOURCE_DIR}/src/titus/titus.rc" )
endif()

# For Apple set the .icns file containing icons
if(APPLE)
	# set how it shows up in the Info.plist file
	set(MACOSX_BUNDLE_ICON_FILE titus.icns)

	# set where in the bundle to put the icns file
	set_source_files_properties(
	  "${PROJECT_SOURCE_DIR}/src/titus/titus.icns"
	  PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	# include the icns file in the target
	set( resource_files ${resource_files}
	  "${PROJECT_SOURCE_DIR}/src/titus/titus.icns" )
endif()

if(GTK2_FOUND)
	add_executable(titus2 MACOSX_BUNDLE ${TITUS_SRC} ${TITUS_HEADER} ${resource_files})
	target_include_directories(titus2 PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm2")
	target_include_directories(titus2 PRIVATE "${GTK2_INCLUDE_DIRS}")
	target_link_libraries(titus2 gtkcrnmm2)
	if(CRN_USING_LIBINTL OR CRN_USING_GLIB_INTL)
		include_directories(${Intl_INCLUDE_DIRS})
		target_link_libraries(titus2	${Intl_LIBRARIES})
	endif()
	install(TARGETS titus2 DESTINATION bin COMPONENT app)
endif()

if(GTK3_FOUND)
	add_executable(titus3 MACOSX_BUNDLE ${TITUS_SRC} ${TITUS_HEADER} ${resource_files})
	target_include_directories(titus3 PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm3")
	target_include_directories(titus3 PRIVATE "${GTK3_INCLUDE_DIRS}")
	target_link_libraries(titus3 gtkcrnmm3)
	if(CRN_USING_LIBINTL OR CRN_USING_GLIB_INTL)
		include_directories(${Intl_INCLUDE_DIRS})
		target_link_libraries(titus3	${Intl_LIBRARIES})
	endif()
	install(TARGETS titus3 DESTINATION bin COMPONENT app)
endif()

install(FILES "titus.png" DESTINATION ${CRN_DATA_PATH} COMPONENT app)

###############################################################################

if(GTK2_FOUND)
	set(APP_NAME titus2)
elseif(GTK3_FOUND)
	set(APP_NAME titus3)
endif()

set(APPS "\${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_PATH}/${APP_NAME}")
if(APPLE)
	set(APPS "\${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_PATH}/${APP_NAME}.app")
endif(APPLE)
if(WIN32)
	set(APPS "\${CMAKE_INSTALL_PREFIX}/${RUNTIME_INSTALL_PATH}/${APP_NAME}.exe")
endif(WIN32)

install(CODE "
    include(BundleUtilities)
    fixup_bundle(${APPS} \"\" \"\")
    " COMPONENT app)

# ---

if(APPLE)
	INSTALL(DIRECTORY "${CMAKE_INSTALL_PREFIX}/${CRN_DATA_PATH}" DESTINATION "${RUNTIME_INSTALL_PATH}/${APP_NAME}.app/Contents/MacOS/share" COMPONENT app)
endif(APPLE)
