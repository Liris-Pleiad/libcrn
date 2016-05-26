# libQtcrn build script

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
	LIBQTCRN_SRC
	*.cpp
	)
file(
	GLOB_RECURSE
	LIBQTCRN_HEADER
	*.h
	*.hpp
	)
file(
	GLOB_RECURSE
	LIBQTCRN_HEADER_MOC
	*.hxx
	)
if (MSVC)
	GroupSources(src/Qt/libQtcrn)
	add_definitions("/wd4250")
endif(MSVC)

add_definitions("-DBUILD_LIBQTCRN")

set(LIBQTCRN_INCLUDE_INSTALL "include/libQtcrn-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}")

# Qt4
if(QT4_FOUND)
	# Sources
	#configure_file(libQtcrn_config4.h.in "${PROJECT_BINARY_DIR}/include/libQtcrn4/libQtcrn_config.h") -> TODO : uncomment

	# Moc
	qt4_wrap_cpp(MOC_FILES ${LIBQTCRN_HEADER_MOC})
    #qt4_wrap_ui(UI_FILES ${LIBQTCRN_UI})					// .ui
    #qt4_add_resources(QT_RESOURCE_FILES ${LIBQTCRN_QRC})	// .qrc

	# Build
	add_library(Qtcrn4 STATIC ${LIBQTCRN_SRC} ${LIBQTCRN_HEADER} ${MOC_FILES} ${UI_FILES} ${QT_RESOURCE_FILES})
	target_include_directories(Qtcrn4 PRIVATE "${PROJECT_BINARY_DIR}/include/libQtcrn4")
	target_include_directories(Qtcrn4 PRIVATE "${CNR_QTGUI_INC}")

	# Dependencies
	target_link_libraries(Qtcrn4 crn)
	target_link_libraries(Qtcrn4 ${CNR_QTGUI_LIB})

	# Install -> TODO !!!
endif()

# Qt5
if(QT5_FOUND)
	# Sources
	#configure_file(libQtcrn_config5.h.in "${PROJECT_BINARY_DIR}/include/libQtcrn5/libQtcrn_config.h") -> TODO : uncomment

	# Moc
	qt5_wrap_cpp(MOC_FILES ${LIBQTCRN_HEADER_MOC})
    #qt5_wrap_ui(UI_FILES ${LIBQTCRN_UI})					// .ui
    #qt5_add_resources(QT_RESOURCE_FILES ${LIBQTCRN_QRC})	// .qrc

	# Build
	add_library(Qtcrn5 STATIC ${LIBQTCRN_SRC} ${LIBQTCRN_HEADER} ${MOC_FILES} ${UI_FILES} ${QT_RESOURCE_FILES})
	target_include_directories(Qtcrn5 PRIVATE "${PROJECT_BINARY_DIR}/include/libQtcrn5")
	target_include_directories(Qtcrn5 PRIVATE "${CNR_QTGUI_INC}")

	# Dependencies
	target_link_libraries(Qtcrn5 crn)
	target_link_libraries(Qtcrn5 ${CNR_QTGUI_LIB})

	# Install -> TODO !!!
endif()

# Install -> TODO !!!
