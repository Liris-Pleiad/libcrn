# - Find ZIP
# Find the native ZIP includes and library
# This module defines
#  ZIP_INCLUDE_DIR, where to find jpeglib.h, etc.
#  ZIP_LIBRARIES, the libraries needed to use ZIP.
#  ZIP_FOUND, If false, do not try to use ZIP.
# also defined, but not for general use are
#  ZIP_LIBRARY, where to find the ZIP library.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)
# 
# Modified 2016-05-18 for libcrn

find_package(ZLIB)

if(ZLIB_FOUND)
	find_path(ZIP_INCLUDE_DIR zip.h)

	set(ZIP_NAMES ${ZIP_NAMES} zip libzip)
	find_library(ZIP_LIBRARY NAMES ${ZIP_NAMES} )

	# handle the QUIETLY and REQUIRED arguments and set ZIP_FOUND to TRUE if
	# all listed variables are TRUE
	include(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZIP DEFAULT_MSG ZIP_LIBRARY ZIP_INCLUDE_DIR)

	if(ZIP_FOUND)
		set(ZIP_INCLUDE_DIRS ${ZIP_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} )
		set(ZIP_LIBRARIES ${ZIP_LIBRARY} ${ZLIB_LIBRARY})
	endif()

endif(ZLIB_FOUND)
