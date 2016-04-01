# This is the CMake script for compiling libcrn
# Martial TOLA - March 2016

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
set(CRN_PACKAGE_VERSION_PATCH 0)
set(CRN_PACKAGE_VERSION "${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}.${CRN_PACKAGE_VERSION_PATCH}")
message("${PRJ_NAME} ${CRN_PACKAGE_VERSION}")

project(${PRJ_NAME})
#set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/${PRJ_NAME}_deploy_${CRN_PACKAGE_VERSION}")

# set Release as default build target
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: Debug, Release."
		FORCE)
endif ()
message("Build type : ${CMAKE_BUILD_TYPE}")

set (BUILD_SHARED_LIBS on)

set(RUNTIME_INSTALL_PATH "bin")
set(LIBRARY_INSTALL_PATH "lib")
set(ARCHIVE_INSTALL_PATH "lib")
set(CRN_DATA_PATH "share/crn" CACHE PATH "Path to the data files")

#############################
if(UNIX)
	set(CRN_PF_UNIX ON)
endif(UNIX)

if(APPLE)
	set(CRN_PF_APPLE ON)
endif(APPLE)

if(ANDROID)
	if (ANDROID_NATIVE_API_LEVEL LESS 9)
		message(FATAL_ERROR "Android API level must be >=9.")
	endif (ANDROID_NATIVE_API_LEVEL LESS 9)
	set(CRN_PF_ANDROID ON)
	#set(ICONV_INCLUDE_DIR "${ANDROID_NDK}/sources/android/support/include") # need to include a homebuilt version of iconv

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
endif(ANDROID)
#############################

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

#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11} -Qunused-arguments")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ENABLE_CXX11} -Qunused-arguments")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11} -Wfatal-errors -pthread")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ENABLE_CXX11}")
	message("Clang compiler detected (with ${ENABLE_CXX11} activated)")
endif()
#------------------------------------------------------------------------------
# MSVC
#------------------------------------------------------------------------------
# /MP for multiple compilation units (cl.exe) ; without any value, it is set automatically depending on your number of main threads
if(MSVC)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
	set(CMAKE_STATIC_LIBRARY_PREFIX "lib")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

	set (BUILD_SHARED_LIBS off)

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

	# set default search path to the binary kit
	set(CMAKE_FIND_ROOT_PATH "${CMAKE_SOURCE_DIR}/gtkmm2formsvc14")
	set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/gtkmm2formsvc14")

endif()
#------------------------------------------------------------------------------
# APPLE OS X
#------------------------------------------------------------------------------
if(APPLE)
	set(CMAKE_MACOSX_RPATH ON) # see cmake POLICY CMP0042
endif(APPLE)
#------------------------------------------------------------------------------

# Find Iconv
find_package(Iconv)
if(ICONV_FOUND)
	include_directories(${ICONV_INCLUDE_DIR})
else(ICONV_FOUND)
	message(FATAL_ERROR "Iconv not found. Please set ICONV_INCLUDE_DIR and ICONV_LIBRARIES.")
endif(ICONV_FOUND)

# Find GDIPLUS
if(MSVC)
	OPTION(CRN_USING_GDIPLUS "Use Gdi+ to load and save images." on)
	if (CRN_USING_GDIPLUS)
		set(GDIPLUS_LIBRARIES "gdiplus.lib")
	else (CRN_USING_GDIPLUS)
		set(GDIPLUS_LIBRARIES "")
	endif (CRN_USING_GDIPLUS)
endif(MSVC)

# gettext
set(LANGUAGES "en_GB;fr")
include(Gettext)
if (Gettext_FOUND)
	TRANSLATE("${PROJECT_SOURCE_DIR}/src/libcrn" "libcrn")
	TRANSLATE("${PROJECT_SOURCE_DIR}/src/libgtkcrnmm" "libgtkcrnmm")
	TRANSLATE("${PROJECT_SOURCE_DIR}/src/titus" "titus")
	INSTALL_TRANSLATIONS()
endif()

# libintl
find_package(Intl)
if(Intl_FOUND AND Gettext_FOUND)
	message("Using libintl")
	set(CRN_USING_LIBINTL on)
endif()

# Find Gtkmm2
find_package(GTK2 2.22 COMPONENTS gtk gtkmm)
if(GTK2_FOUND)
	if(Intl_FOUND AND Gettext_FOUND)
		message("Using Glibintl")
		set(CRN_USING_GLIB_INTL on)
		set(CRN_USING_LIBINTL off)
	endif()
	message("Using Gtkmm2")
	set(CRN_USING_GTKMM on)
	message("Using GdkPixbuf")
	set(CRN_USING_GDKPB on)
endif()

find_package(GTK3 3.10 COMPONENTS gtk gtkmm)
if(GTK3_FOUND)
	if(Intl_FOUND AND Gettext_FOUND)
		message("Using Glibintl")
		set(CRN_USING_GLIB_INTL on)
		set(CRN_USING_LIBINTL off)
	endif()
	message("Using Gtkmm3")
	set(CRN_USING_GTKMM on)
	message("Using GdkPixbuf")
	set(CRN_USING_GDKPB on)
endif()

if(NOT MSVC)
# libpng
	find_package(PNG)
	if(PNG_FOUND)
		set(CRN_USING_LIBPNG on)
		message("Using libpng")
	endif()

# jpeglib
	find_package(JPEG)
	if(JPEG_FOUND)
		set(CRN_USING_LIBJPEG on)
		message("Using jpeglib")
	endif()

endif()

# Build
add_subdirectory(src/libcrn)
add_subdirectory(src/demos)
if(GTK2_FOUND)
	add_subdirectory(src/libgtkcrnmm)
	add_subdirectory(src/titus)
endif()

# Tests
enable_testing()
add_subdirectory(src/tests)

# Pack
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_FILE_NAME "libcrn-${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}.${CRN_PACKAGE_VERSION_PATCH}${CRN_PACK_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR ${CRN_PACKAGE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CRN_PACKAGE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CRN_PACKAGE_VERSION_PATCH})
set(CPACK_BINARY_TZ OFF)
if(WIN32)
	set(CPACK_BINARY_NSIS OFF) # tmp
	set(CPACK_NSIS_DISPLAY_NAME "libcrn ${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}.${CRN_PACKAGE_VERSION_PATCH}")
	set(CPACK_BINARY_TBZ2 OFF)
	set(CPACK_BINARY_ZIP ON)
	set(CPACK_NSIS_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
else(WIN32)
	set(CPACK_BINARY_TBZ2 ON)
endif(WIN32)
if(UNIX)
	set(CPACK_DEBIAN_PACKAGE_VERSION ${CRN_PACKAGE_VERSION})
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Yann LEYDIER <yann.leydier@corenum.com>")
	set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "The CoReNum image processing library.")
	set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) # auto depends
	set(CPACK_DEB_COMPONENT_INSTALL ON)
	set(CPACK_RPM_COMPONENT_INSTALL ON)
endif(UNIX)
set(CPACK_COMPONENTS_ALL bin dev app)
set(CPACK_COMPONENT_BIN_DISPLAY_NAME "Runtime files")
set(CPACK_COMPONENT_BIN_DESCRIPTION "Libraries and configuration files")
set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_DEV_DESCRIPTION "Headers and various library-related files")
set(CPACK_COMPONENT_DEV_DEPENDS bin)
set(CPACK_COMPONENT_APP_DISPLAY_NAME "Applications")
set(CPACK_COMPONENT_APP_DESCRIPTION "Demonstration applications")
set(CPACK_COMPONENT_APP_DEPENDS bin)
include(CPack)

