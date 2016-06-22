# MSVC - KIT VS 2015 x64 - V03

if( DEFINED ENV{LIBCRN_DEPS_PATH} )
	set( MSVC_KIT_ROOT $ENV{LIBCRN_DEPS_PATH} )
endif()
if( NOT DEFINED MSVC_KIT_ROOT )
	message(FATAL_ERROR "MSVC_KIT_ROOT not set.  Please set MSVC_KIT_ROOT.")
endif()

### mandatory :

set(ICONV_DIR										${MSVC_KIT_ROOT}/libiconv-1.14)

### optional 1 : png, jpeg, intl

#if(CRN_DEPENDENCY_PNG)
	set(ZLIB_ROOT										${MSVC_KIT_ROOT}/3rdparty/zlib)
	#set(ZLIB_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/zlib)
	set(ZLIB_LIBRARY									${MSVC_KIT_ROOT}/3rdparty/zlib/lib/Release/zlib.lib)

	set(PNG_PNG_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/libpng)
	set(PNG_LIBRARY										${MSVC_KIT_ROOT}/3rdparty/libpng/lib/Release/libpng.lib)
#endif(CRN_DEPENDENCY_PNG)

if(CRN_DEPENDENCY_JPEG)
	set(JPEG_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/libjpeg)
	set(JPEG_LIBRARY									${MSVC_KIT_ROOT}/3rdparty/libjpeg/lib/Release/jpeg.lib)
endif(CRN_DEPENDENCY_JPEG)

if(CRN_DEPENDENCY_GETTEXT_INTL)
	if(CRN_DEPENDENCY_GTKMM3_DEBUG)
		set(Intl_INCLUDE_DIR								${MSVC_KIT_ROOT}/gtkmm3_debug/include/gettext/intl)
		set(Intl_LIBRARY									${MSVC_KIT_ROOT}/gtkmm3_debug/lib/intl.dll.lib)
	elseif(CRN_DEPENDENCY_GTKMM3_RELEASE)
		set(Intl_INCLUDE_DIR								${MSVC_KIT_ROOT}/gtkmm3_release/include/gettext/intl)
		set(Intl_LIBRARY									${MSVC_KIT_ROOT}/gtkmm3_release/lib/intl.dll.lib)
	else()# by default use Gtkmm2's version of libintl
		set(Intl_INCLUDE_DIR								${MSVC_KIT_ROOT}/gtkmm2/include)
		set(Intl_LIBRARY									${MSVC_KIT_ROOT}/gtkmm2/lib/intl.lib)
	endif()
endif(CRN_DEPENDENCY_GETTEXT_INTL)

### optional 2 : zip, haru(pdf)

if(CRN_DEPENDENCY_ZIP)
	set(ZIP_INCLUDE_DIR									${MSVC_KIT_ROOT}/libzip-1.1.2/include)
	set(ZIP_LIBRARY										${MSVC_KIT_ROOT}/libzip-1.1.2/lib/Release/zip.lib)
endif(CRN_DEPENDENCY_ZIP)

if(CRN_DEPENDENCY_HARUPDF)
	set(HPDF_INCLUDE_DIR								${MSVC_KIT_ROOT}/libharu-2.3.0/include)
	set(HPDF_LIBRARY									${MSVC_KIT_ROOT}/libharu-2.3.0/lib/Release/libhpdf.lib)
endif(CRN_DEPENDENCY_HARUPDF)

### optional 3 : gtkmm2, gtkmm3

if(CRN_DEPENDENCY_GTKMM2)
	set(GTK2_HINTS									${MSVC_KIT_ROOT}/gtkmm2)

	set(FREETYPE_INCLUDE_DIR_ft2build				${GTK2_HINTS}/include)
	set(FREETYPE_INCLUDE_DIR_freetype2				${GTK2_HINTS}/include/freetype2)

else()
	if(CRN_DEPENDENCY_GTKMM3_DEBUG)
		set(GTK3_HINTS								${MSVC_KIT_ROOT}/gtkmm3_debug)
		set(CMAKE_BUILD_TYPE Debug CACHE STRING "Forced by debug version of Gtkmm3." FORCE)
	elseif(CRN_DEPENDENCY_GTKMM3_RELEASE)
		set(GTK3_HINTS								${MSVC_KIT_ROOT}/gtkmm3_release)
		set(CMAKE_BUILD_TYPE Release CACHE STRING "Forced by release version of Gtkmm3." FORCE)
	endif()

	if(CRN_DEPENDENCY_GTKMM3_DEBUG OR CRN_DEPENDENCY_GTKMM3_RELEASE)
		set(FREETYPE_INCLUDE_DIR_ft2build			${GTK3_HINTS}/include/freetype)
		set(FREETYPE_INCLUDE_DIR_freetype2			${GTK3_HINTS}/include/freetype/freetype)
	endif()
endif()

### optional 4 : qt4, qt5

set(QTDIR 				${MSVC_KIT_ROOT}/Qt/qt-4.8.7-x64-msvc2015)

set(QT5_DIR				${MSVC_KIT_ROOT}/Qt/Qt5.6.0/5.6/msvc2015_64)

### optional 5 : opencv

set(OpenCV_DIR			${MSVC_KIT_ROOT}/opencv-3.1.0/build)

###
