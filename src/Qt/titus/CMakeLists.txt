# titus build script

cmake_minimum_required(VERSION 2.8.9) # because 2.8.8 has a bug with QT5_USE_MODULES

if (COMMAND cmake_policy)
  if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
  endif(POLICY CMP0020)
endif (COMMAND cmake_policy)

#############################
# add path to CRN.h
include_directories("${PROJECT_BINARY_DIR}/include/libcrn")
# add path to libcrn
include_directories("${PROJECT_SOURCE_DIR}/src/libcrn")
# add path to libQtcrn
include_directories("${PROJECT_SOURCE_DIR}/src/Qt/libQtcrn")
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
file(
	GLOB_RECURSE
	TITUS_HEADER_MOC
	*.hxx
	)
if (MSVC)
	GroupSources(src/Qt/titus)
	add_definitions("/wd4250")
endif(MSVC)

# For Windows set the .ico file with the .rc file - (https://iconverticons.com/online/)
if(MSVC)
	set(resource_files ${resource_files} "./resources/titus.rc")
endif()

# For Apple   set the .icns file containing icons - (https://iconverticons.com/online/)
if(APPLE)
	# set how it shows up in the Info.plist file
	set(MACOSX_BUNDLE_ICON_FILE titus.icns)

	# set where in the bundle to put the icns file
	SET_SOURCE_FILES_PROPERTIES("./resources/titus.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	# include the icns file in the target
	set(resource_files ${resource_files} "./resources/titus.icns")
endif()

if(QT4_FOUND)
	# Moc
	qt4_wrap_cpp(MOC_FILES ${TITUS_HEADER_MOC})
    #qt4_wrap_ui(UI_FILES ${TITUS_UI})					// .ui
    #qt4_add_resources(QT_RESOURCE_FILES ${TITUS_QRC})	// .qrc

    # Build
	add_executable(titus4 MACOSX_BUNDLE ${TITUS_SRC} ${TITUS_HEADER} ${MOC_FILES} ${UI_FILES} ${QT_RESOURCE_FILES} ${resource_files})
	target_include_directories(titus4 PRIVATE "${PROJECT_BINARY_DIR}/include/libQtcrn4")
	target_include_directories(titus4 PRIVATE "${CNR_QTGUI_INC}")

	# Dependencies
	target_link_libraries(titus4 Qtcrn4)

	#install(TARGETS titus4 DESTINATION bin COMPONENT app) -> TODO !!!
endif()

if(QT5_FOUND)
	# Moc
	qt5_wrap_cpp(MOC_FILES ${TITUS_HEADER_MOC})
    #qt5_wrap_ui(UI_FILES ${TITUS_UI})					// .ui
    #qt5_add_resources(QT_RESOURCE_FILES ${TITUS_QRC})	// .qrc

    # Build
	add_executable(titus5 MACOSX_BUNDLE ${TITUS_SRC} ${TITUS_HEADER} ${MOC_FILES} ${UI_FILES} ${QT_RESOURCE_FILES} ${resource_files})
	target_include_directories(titus5 PRIVATE "${PROJECT_BINARY_DIR}/include/libQtcrn5")
	target_include_directories(titus5 PRIVATE "${CNR_QTGUI_INC}")

	# Dependencies
	target_link_libraries(titus5 Qtcrn5)

	#install(TARGETS titus5 DESTINATION bin COMPONENT app) -> TODO !!!
endif()

# For Windows : write qt.conf and subfolder for QtPlugins
if(MSVC)
	if(QT4_FOUND)
		set(QT_PLUGINS_DIR "${QTDIR}/plugins")
	elseif (QT5_FOUND)
		set(QT_PLUGINS_DIR "${QT5_DIR}/plugins")
	endif()

	#--------------------------------------------------------------------------------
	# Copy Qt plugins to 'Debug and Release' directories and configure qt.conf file
	#--------------------------------------------------------------------------------
	file(GLOB qtplugin_dirs RELATIVE  "${QT_PLUGINS_DIR}"  "${QT_PLUGINS_DIR}/imageformats*"  "${QT_PLUGINS_DIR}/platforms*")
	file(REMOVE_RECURSE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/QtPlugins")
	file(REMOVE_RECURSE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/QtPlugins")
	file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/QtPlugins")
	file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/QtPlugins")
	foreach (qtplugin ${qtplugin_dirs})
	  file(COPY "${QT_PLUGINS_DIR}/${qtplugin}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/QtPlugins")
	  file(COPY "${QT_PLUGINS_DIR}/${qtplugin}" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/QtPlugins")
	endforeach ()
	file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/qt.conf" "[Paths]\nPlugins = QtPlugins")
	file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/qt.conf" "[Paths]\nPlugins = QtPlugins")
endif()

# Install
#install(FILES "titus.png" DESTINATION ${CRN_DATA_PATH} COMPONENT app) -> TODO !!!
