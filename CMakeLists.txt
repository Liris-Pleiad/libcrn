# This is the CMake script for compiling libcrn

###############################################################################
# Config
###############################################################################

cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if (COMMAND cmake_policy)
	if(POLICY CMP0042)
		cmake_policy(SET CMP0042 NEW) # https://cmake.org/cmake/help/v3.0/policy/CMP0042.html
	endif(POLICY CMP0042)
endif (COMMAND cmake_policy)

set(DOC_DIR ${PROJECT_BINARY_DIR}/doc)

# project configuration
set(PRJ_NAME libcrn)

set(CRN_PACKAGE_VERSION_MAJOR 3)
set(CRN_PACKAGE_VERSION_MINOR 9)
set(CRN_PACKAGE_VERSION_PATCH 1)
set(CRN_PACKAGE_VERSION "${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}.${CRN_PACKAGE_VERSION_PATCH}")
message("${PRJ_NAME} ${CRN_PACKAGE_VERSION}")

project(${PRJ_NAME})

# set Release as default build target
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: Debug, Release."
		FORCE)
endif ()
message("Build type : ${CMAKE_BUILD_TYPE}")

set(BUILD_SHARED_LIBS on)
set(CRNLIB_EXT "${CMAKE_SHARED_LIBRARY_SUFFIX}") # by default link to the shared library

# install subpaths
set(RUNTIME_INSTALL_PATH "bin")
set(LIBRARY_INSTALL_PATH "lib")
set(ARCHIVE_INSTALL_PATH "lib")
set(CRN_DATA_PATH "share/crn" CACHE PATH "Path to the data files")

# put all binaries in the same directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out")
set(CMAKE_INSTALL_RPATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# platform
if(UNIX)
	set(CRN_PF_UNIX ON)
endif(UNIX)
if(APPLE)
	set(CRN_PF_APPLE ON)
endif(APPLE)
# for windows: _MSC_VER
if(ANDROID)
	if(NOT ANDROID_NATIVE_API_LEVEL)
		set(ANDROID_NATIVE_API_LEVEL 9)
		message(FATAL_ERROR "Android API level set to 9.")
	endif()
	if(ANDROID_NATIVE_API_LEVEL LESS 9)
		message(FATAL_ERROR "Android API level must be >=9.")
	endif(ANDROID_NATIVE_API_LEVEL LESS 9)
	set(CRN_PF_ANDROID ON)

	set(ICONV_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/android-deps/include" CACHE PATH "path to iconv")
	set(ICONV_LIBRARIES "${CMAKE_SOURCE_DIR}/android-deps/lib/${ANDROID_ABI}/libiconv.a" CACHE PATH "path to iconv")
	set(PNG_PNG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/android-deps/include" CACHE PATH "path to libpng")
	set(PNG_LIBRARY_RELEASE "${CMAKE_SOURCE_DIR}/android-deps/lib/${ANDROID_ABI}/libpng.a" CACHE PATH "path to libpng")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
endif(ANDROID)

#------------------------------------------------------------------------------
# Specific compiler options
#------------------------------------------------------------------------------
# GNUCXX (export CXX=g++-4.8 CC=gcc-4.8)
#------------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX)
	set(ENABLE_CXX11 "-std=c++11")

	EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION LESS 4.8)
		message(FATAL_ERROR "g++ >= 4.8 needed.")
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11} -Wfatal-errors -pthread")
	message("GNUCXX compiler detected (with ${ENABLE_CXX11} activated)")
endif()
#------------------------------------------------------------------------------
# Clang (export CXX=clang++ CC=clang)
#------------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_COMPILER_IS_CLANGXX 1)
endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

if(CMAKE_COMPILER_IS_CLANGXX)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	set(ENABLE_CXX11 "-std=c++11")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11} -Wfatal-errors -pthread")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ENABLE_CXX11}")
	message("Clang compiler detected (with ${ENABLE_CXX11} activated)")
endif()
#------------------------------------------------------------------------------
# MSVC
#------------------------------------------------------------------------------
if(MSVC)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
	set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
	set(CMAKE_IMPORT_LIBRARY_PREFIX "lib")
	set(CMAKE_DEBUG_POSTFIX "-d")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /D_MBCS") # multi-process build + multibyte C strings

	set(CRNLIB_EXT "${CMAKE_IMPORT_LIBRARY_SUFFIX}") # link to the import library, not the DLL!

	set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS)
	set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS _SCL_SECURE_NO_WARNINGS)

	# Create filters in VS project
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
			${PROJECT_SOURCE_DIR}/${curdir}/*)
		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "" groupname ${groupname})
				source_group(${groupname} FILES
					${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/msvc/kit.cmake")
		include("${CMAKE_SOURCE_DIR}/cmake/msvc/kit.cmake")
	endif()
endif()
#------------------------------------------------------------------------------
# APPLE OS X
#------------------------------------------------------------------------------
if(APPLE)
	set(CMAKE_MACOSX_RPATH ON) # see cmake POLICY CMP0042
endif(APPLE)
#------------------------------------------------------------------------------

###############################################################################
# Dependencies
###############################################################################
# Image
option(CRN_DEPENDENCY_PNG "Use libpng to load and save images." on)
option(CRN_DEPENDENCY_JPEG "Use jpeglib to load and save images." off)
option(CRN_DEPENDENCY_GDKPIXBUF "Use GdkPixbuf to load and save images." off)
if(MSVC)
	option(CRN_DEPENDENCY_GDIPLUS "Use Gdi+ to load and save images." on)
endif()

# Misc
option(CRN_DEPENDENCY_GETTEXT_INTL "Use Gettext and libintl to translate messages." off)
option(CRN_DEPENDENCY_ZIP "Use libzip to create archives." off)
option(CRN_DEPENDENCY_HARUPDF "Use haru to create PDF files." off)

# GUI
option(CRN_DEPENDENCY_GTKMM2 "Use Gtkmm 2 to build GUIs." off)
if(MSVC)
	option(CRN_DEPENDENCY_GTKMM3_RELEASE "Use Gtkmm 3 (release) to build GUIs." off)
	option(CRN_DEPENDENCY_GTKMM3_DEBUG "Use Gtkmm 3 (debug) to build GUIs." off)
else(MSVC)
	option(CRN_DEPENDENCY_GTKMM3 "Use Gtkmm 3 to build GUIs." off)
endif(MSVC)
option(CRN_DEPENDENCY_QT4 "Use Qt 4 to build GUIs (in development)." off)
option(CRN_DEPENDENCY_QT5 "Use Qt 5 to build GUIs (in development)." off)

# Find Iconv
find_package(Iconv)
if(ICONV_FOUND)
	include_directories(${ICONV_INCLUDE_DIR})
else(ICONV_FOUND)
	message(FATAL_ERROR "Iconv not found. Please set ICONV_INCLUDE_DIR and ICONV_LIBRARIES.")
endif(ICONV_FOUND)

# GDIPLUS
if(CRN_DEPENDENCY_GDIPLUS)
	set(CRN_USING_GDIPLUS ON)
	set(GDIPLUS_LIBRARIES "gdiplus.lib")
else(CRN_DEPENDENCY_GDIPLUS)
	set(CRN_USING_GDIPLUS OFF)
	set(GDIPLUS_LIBRARIES "")
endif(CRN_DEPENDENCY_GDIPLUS)

if(CRN_DEPENDENCY_GETTEXT_INTL)
	# gettext
	set(LANGUAGES "en_GB;fr")
	include(Gettext)
	if(NOT Gettext_FOUND)
		message(FATAL_ERROR "Cannot find Gettext.")
	endif()
	TRANSLATE("${PROJECT_SOURCE_DIR}/src/libcrn" "libcrn")
	TRANSLATE("${PROJECT_SOURCE_DIR}/src/libgtkcrnmm" "libgtkcrnmm")
	TRANSLATE("${PROJECT_SOURCE_DIR}/src/titus" "titus")
	INSTALL_TRANSLATIONS()

	# libintl
	find_package(Intl)
	if(NOT Intl_FOUND)
		message(FATAL_ERROR "Cannot find libintl.")
	endif()
	message("Using libintl")
	set(CRN_USING_LIBINTL on)
endif(CRN_DEPENDENCY_GETTEXT_INTL)

if(CRN_DEPENDENCY_GTKMM2)
	# Find Gtkmm2
	#set(GTK2_DEBUG on)
	find_package(GTK2 2.22 COMPONENTS gtk gtkmm)
	if(NOT GTK2_FOUND)
		message(FATAL_ERROR "Cannot find Gtkmm 2.")
	endif()

	message("Using Gtkmm2")
	set(CRN_USING_GTKMM on)
	if(Intl_FOUND AND Gettext_FOUND)
		message("Using Glibintl instead of libintl")
		set(CRN_USING_GLIB_INTL on)
		set(CRN_USING_LIBINTL off)
	endif()

	if(CRN_DEPENDENCY_GDKPIXBUF)
		message("Using GdkPixbuf")
		set(CRN_USING_GDKPB on)
	endif(CRN_DEPENDENCY_GDKPIXBUF)
endif(CRN_DEPENDENCY_GTKMM2)

if(CRN_DEPENDENCY_GTKMM3)
	#set(GTK3_DEBUG on)
	find_package(GTK3 3.10 COMPONENTS gtk gtkmm)
	if(NOT GTK3_FOUND)
		message(FATAL_ERROR "Cannot find Gtkmm 3.")
	endif()

	message("Using Gtkmm3")
	set(CRN_USING_GTKMM on)
	
	if(Intl_FOUND AND Gettext_FOUND)
		message("Using Glibintl instead of libintl")
		set(CRN_USING_GLIB_INTL on)
		set(CRN_USING_LIBINTL off)
	endif()

	if(CRN_DEPENDENCY_GDKPIXBUF)
		message("Using GdkPixbuf")
		set(CRN_USING_GDKPB on)
	endif(CRN_DEPENDENCY_GDKPIXBUF)
endif(CRN_DEPENDENCY_GTKMM3)

if(CRN_DEPENDENCY_QT4)
	# Find Qt4
	#set(QT4_DEBUG on)
	if (QTDIR)
		set( ENV{QTDIR} ${QTDIR} )
	endif()
	set(QT_USE_QTMAIN TRUE)
	set(QT_USE_QTXML  TRUE)
	find_package(Qt4)
	if(NOT QT4_FOUND)
		message(FATAL_ERROR "Cannot find Qt 4.")
	endif()

	message("Using Qt4")
	set(CRN_USING_QT on)

	include(${QT_USE_FILE})

	set(CNR_QTGUI_INC ${CNR_QTGUI_INC} ${QT_INCLUDE_DIR})
	set(CNR_QTGUI_LIB ${CNR_QTGUI_LIB} ${QT_LIBRARIES})
endif(CRN_DEPENDENCY_QT4)

if(CRN_DEPENDENCY_QT5)
	# Find Qt5
	#set(QT5_DEBUG on)
	if(DEFINED ENV{QT5_DIR})
		set(QT5_DIR $ENV{QT5_DIR})
	endif()
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT5_DIR})

	# Instruct CMake to NOT run moc automatically when needed / IMPORTANT : automoc is off for qt5 because we wan't same behavior with qt4 and/or qt5...
	set(CMAKE_AUTOMOC OFF)

	find_package(Qt5Core)
	if(${Qt5Core_FOUND})
		#message(STATUS "--> Qt5Core found.")
		find_package(Qt5Widgets)
		if(${Qt5Widgets_FOUND})
	  		#message(STATUS "--> Qt5Widgets found.")
	  		find_package(Qt5Xml)
	  		if(${Qt5Xml_FOUND})
				#message(STATUS "--> Qt5Xml found.")
				set(QT5_FOUND TRUE)
	  		else(${Qt5Xml_FOUND})
				message(FATAL_ERROR "Qt5Xml not found.")
	  		endif(${Qt5Xml_FOUND})
		else(${Qt5Widgets_FOUND})
	  		message(FATAL_ERROR "Qt5Widgets not found.")
		endif(${Qt5Widgets_FOUND})
	endif(${Qt5Core_FOUND})
	if(NOT QT5_FOUND)
		message(FATAL_ERROR "Cannot find Qt 5.")
	endif()

	message("Using Qt5")
	set(CRN_USING_QT on)

	set(CNR_QTGUI_INC	${CNR_QTGUI_INC}
	               		${Qt5Widgets_INCLUDES_DIRS}
	               		${Qt5Xml_INCLUDES_DIR})
	set(CNR_QTGUI_LIB	${CNR_QTGUI_LIB}
	               		${Qt5Widgets_LIBRARIES}
	               		${Qt5Xml_LIBRARIES})
endif(CRN_DEPENDENCY_QT5)

if(CRN_DEPENDENCY_PNG)
	# libpng
	find_package(PNG)
	if(NOT PNG_FOUND)
		message(FATAL_ERROR "Cannot find libpng.")
	endif()
	set(CRN_USING_LIBPNG on)
	message("Using libpng")
endif(CRN_DEPENDENCY_PNG)

if(CRN_DEPENDENCY_JPEG)
	# jpeglib
	find_package(JPEG)
	if(NOT JPEG_FOUND)
		message(FATAL_ERROR "Cannot find jpeglib.")
	endif()
	set(CRN_USING_LIBJPEG on)
	message("Using jpeglib")
endif(CRN_DEPENDENCY_JPEG)

if(CRN_DEPENDENCY_ZIP)
	find_package(ZIP)
	if(NOT ZIP_FOUND)
		message(FATAL_ERROR "Cannot find libzip.")
	endif()
	set(CRN_USING_LIBZIP on)
	message("Using libzip")
endif(CRN_DEPENDENCY_ZIP)

if(CRN_DEPENDENCY_HARUPDF)
	find_package(HPDF)
	if(NOT HPDF_FOUND)
		message(FATAL_ERROR "Cannot find haru.")
	endif()
	set(CRN_USING_HARU on)
	message("Using haru")
endif(CRN_DEPENDENCY_HARUPDF)

###############################################################################
# Build
###############################################################################
if(MSVC)
	add_executable(gendef src/gendef/gendef.cc)
endif()
add_subdirectory(src/libcrn)
add_subdirectory(src/demos)
if(GTK2_FOUND OR GTK3_FOUND)
	add_subdirectory(src/libgtkcrnmm)
	add_subdirectory(src/titus)
endif()
if(QT4_FOUND OR QT5_FOUND)
	add_subdirectory(src/Qt/libQtcrn)
	add_subdirectory(src/Qt/titus)
endif()

# Tests
enable_testing()
add_subdirectory(src/tests)

###############################################################################
# Doc
###############################################################################
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
endif(DOXYGEN_FOUND)

###############################################################################
# Pack
###############################################################################
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_FILE_NAME "libcrn-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}.${CRN_PACKAGE_VERSION_PATCH}${CRN_PACK_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "libcrn")
set(CPACK_PACKAGE_VERSION_MAJOR ${CRN_PACKAGE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CRN_PACKAGE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CRN_PACKAGE_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE  "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
if(WIN32)
	option(CPACK_BINARY_DEB OFF)
	option(CPACK_BINARY_NSIS ON)
	set(CPACK_NSIS_DISPLAY_NAME "libcrn ${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}.${CRN_PACKAGE_VERSION_PATCH}")
	set(CPACK_NSIS_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
	option(CPACK_BINARY_RPM OFF)
	option(CPACK_BINARY_STGZ OFF)
	option(CPACK_BINARY_TBZ2 OFF)
	option(CPACK_BINARY_TGZ OFF)
	option(CPACK_BINARY_TZ OFF)
	option(CPACK_BINARY_ZIP ON)
	option(CPACK_SOURCE_TBZ2 OFF)
	option(CPACK_SOURCE_TGZ OFF)
	option(CPACK_SOURCE_TXZ OFF)
	option(CPACK_SOURCE_TZ OFF)
	option(CPACK_SOURCE_ZIP ON)
endif(WIN32)
if(UNIX)
	option(CPACK_BINARY_DEB OFF)
	set(CPACK_DEBIAN_PACKAGE_VERSION ${CRN_PACKAGE_VERSION})
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Yann LEYDIER <yann@leydier.info>")
	set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "libcrn image processing library")
	set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) # auto depends
	set(CPACK_DEB_COMPONENT_INSTALL ON)
	option(CPACK_BINARY_NSIS OFF)
	option(CPACK_BINARY_RPM OFF)
	set(CPACK_RPM_COMPONENT_INSTALL ON)
	option(CPACK_BINARY_STGZ OFF)
	option(CPACK_BINARY_TBZ2 ON)
	option(CPACK_BINARY_TGZ OFF)
	option(CPACK_BINARY_TZ OFF)
	option(CPACK_BINARY_ZIP OFF)
	option(CPACK_SOURCE_TBZ2 ON)
	option(CPACK_SOURCE_TGZ OFF)
	option(CPACK_SOURCE_TXZ OFF)
	option(CPACK_SOURCE_TZ OFF)
	option(CPACK_SOURCE_ZIP OFF)
	option(CPACK_BINARY_TBZ2 OFF)
	option(CPACK_BINARY_ZIP OFF)
endif(UNIX)
if(APPLE)
	option(CPACK_BINARY_DEB OFF)
	option(CPACK_BINARY_NSIS OFF)
	option(CPACK_BINARY_RPM OFF)
	option(CPACK_BINARY_STGZ OFF)
	option(CPACK_BINARY_TBZ2 ON)
	option(CPACK_BINARY_TGZ OFF)
	option(CPACK_BINARY_TZ OFF)
	option(CPACK_BINARY_ZIP OFF)
	option(CPACK_SOURCE_TBZ2 ON)
	option(CPACK_SOURCE_TGZ OFF)
	option(CPACK_SOURCE_TXZ OFF)
	option(CPACK_SOURCE_TZ OFF)
	option(CPACK_SOURCE_ZIP OFF)
	option(CPACK_BINARY_TBZ2 OFF)
	option(CPACK_BINARY_ZIP OFF)
	#CPACK_PACKAGE_FILE_NAME - provides the name of the final compressed disk image (the name of the file that is distributed).
	#CPACK_PACKAGE_ICON - provides the icon for the mounted disk image (appears after the user mounts the disk image).
	#CPACK_BUNDLE_NAME - provides the bundle name (displayed in the finder underneath the bundle icon).
	#CPACK_BUNDLE_ICON - provides the bundle icon (displayed in the /Applications folder, on the dock, etc).
	#CPACK_BUNDLE_PLIST - path to a file that will become the bundle plist.
	#CPACK_BUNDLE_STARTUP_COMMAND - path to a file that will be executed when the user opens the bundle. Could be a shell-script or a binary.
endif(APPLE)
include(CPack)
cpack_add_component(bin
                    DISPLAY_NAME "Runtime files"
                    DESCRIPTION "Libraries and configuration files"
                    REQUIRED)
cpack_add_component(dev
                    DISPLAY_NAME "Development files"
                    DESCRIPTION "Headers and various library-related files"
										DEPENDS bin)
cpack_add_component(app
                    DISPLAY_NAME "Applications"
                    DESCRIPTION "Demonstration applications"
										DEPENDS bin)
