# nimrod build script

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
	NIMROD_SRC
	*.cpp
)
file(
	GLOB_RECURSE
	NIMROD_HEADER
	*.h
	*.hpp
)
if (MSVC)
	GroupSources(src/nimrod)
	add_definitions("/wd4250")
endif(MSVC)

# For Windows set the .ico file with the .rc file
if( MSVC )
	set( resource_files ${resource_files}
	  "${PROJECT_SOURCE_DIR}/src/nimrod/nimrod.rc" )
endif()

# For Apple set the .icns file containing icons
if(APPLE)
	# set how it shows up in the Info.plist file
	set(MACOSX_BUNDLE_ICON_FILE nimrod.icns)

	# set where in the bundle to put the icns file
	set_source_files_properties(
	  "${PROJECT_SOURCE_DIR}/src/nimrod/nimrod.icns"
	  PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	# include the icns file in the target
	set( resource_files ${resource_files}
	  "${PROJECT_SOURCE_DIR}/src/nimrod/nimrod.icns" )
endif()

add_executable(nimrod MACOSX_BUNDLE ${NIMROD_SRC} ${NIMROD_HEADER} ${resource_files})
target_include_directories(nimrod PRIVATE "${PROJECT_BINARY_DIR}/include/libgtkcrnmm2")
target_include_directories(nimrod PRIVATE "${GTK2_INCLUDE_DIRS}")
target_link_libraries(nimrod gtkcrnmm2)
if(CRN_USING_LIBINTL OR CRN_USING_GLIB_INTL)
	include_directories(${Intl_INCLUDE_DIRS})
	target_link_libraries(nimrod	${Intl_LIBRARIES})
endif()
install(TARGETS nimrod DESTINATION bin COMPONENT app)

#install(FILES "nimrod.png" DESTINATION ${CRN_DATA_PATH} COMPONENT app)

###############################################################################

set(APP_NAME nimrod)

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
