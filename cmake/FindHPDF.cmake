# - Find HPDF
# Find the native HPDF includes and library
# This module defines
#  HPDF_INCLUDE_DIR, where to find jpeglib.h, etc.
#  HPDF_LIBRARIES, the libraries needed to use HPDF.
#  HPDF_FOUND, If false, do not try to use HPDF.
# also defined, but not for general use are
#  HPDF_LIBRARY, where to find the HPDF library.

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

find_package(PNG)

if(PNG_FOUND)
	find_path(HPDF_INCLUDE_DIR hpdf.h)

	set(HPDF_NAMES ${HPDF_NAMES} hpdf libhpdf)
	find_library(HPDF_LIBRARY NAMES ${HPDF_NAMES} )

	# handle the QUIETLY and REQUIRED arguments and set HPDF_FOUND to TRUE if
	# all listed variables are TRUE
	include(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(HPDF DEFAULT_MSG HPDF_LIBRARY HPDF_INCLUDE_DIR)

	if(HPDF_FOUND)
		set(HPDF_INCLUDE_DIRS ${HPDF_INCLUDE_DIR} ${PNG_INCLUDE_DIR} )
		set(HPDF_LIBRARIES ${HPDF_LIBRARY} ${PNG_LIBRARY})
	endif()

endif(ZLIB_FOUND)

