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

SET(CRN_PACKAGE_VERSION_MAJOR 3)
SET(CRN_PACKAGE_VERSION_MINOR 9)
SET(CRN_PACKAGE_VERSION_PATCH 0)
SET(CRN_PACKAGE_VERSION "${CRN_PACKAGE_VERSION_MAJOR}.${CRN_PACKAGE_VERSION_MINOR}.${CRN_PACKAGE_VERSION_PATCH}")
message("${PRJ_NAME} ${CRN_PACKAGE_VERSION}")

project(${PRJ_NAME})
set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/${PRJ_NAME}_deploy_${CRN_PACKAGE_VERSION}")

# set Release as default build target
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: Debug, Release."
		FORCE)
endif ()
message("Build type : ${CMAKE_BUILD_TYPE}")

#############################
if(WIN32)
	set(CRN_PF_WIN32 ON)
endif(WIN32)

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
	if (GCC_VERSION LESS 4.7)
		set(ENABLE_CXX11 "-std=c++0x")
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ENABLE_CXX11}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ENABLE_CXX11}")
	message("Clang compiler detected (with ${ENABLE_CXX11} activated)")
endif()
#------------------------------------------------------------------------------
# MSVC
#------------------------------------------------------------------------------
# /MP for multiple compilation units (cl.exe) ; without any value, it is set automatically depending on your number of main threads
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

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

# Find Gtkmm
set(GTK2_HINTS "${CMAKE_SOURCE_DIR}/gtkmm2formsvc14")
find_package(GTK2 2.22 COMPONENTS gtk gtkmm)

# ---
add_subdirectory(src/libcrn)
add_subdirectory(src/demos)
if(GTK2_FOUND)
	add_subdirectory(src/libgtkcrnmm)
	add_subdirectory(src/titus)
endif()
