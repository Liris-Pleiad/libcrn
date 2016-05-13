# MSVC - KIT VS 2015 x64 - V01

if( DEFINED ENV{MSVC_KIT_ROOT} )
	set( MSVC_KIT_ROOT $ENV{MSVC_KIT_ROOT} )
endif()
if( NOT DEFINED MSVC_KIT_ROOT )
	message(FATAL_ERROR "MSVC_KIT_ROOT not set.  Please set MSVC_KIT_ROOT.")
endif()

### mandatory :

set(ICONV_DIR										${MSVC_KIT_ROOT}/libiconv-1.14)

### optional 1 : png, jpeg, intl

set(ZLIB_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/zlib)
set(ZLIB_LIBRARY									${MSVC_KIT_ROOT}/3rdparty/zlib/lib/Release/zlib.lib)

set(PNG_PNG_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/libpng)
set(PNG_LIBRARY										${MSVC_KIT_ROOT}/3rdparty/libpng/lib/Release/libpng.lib)

set(JPEG_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/libjpeg)
set(JPEG_LIBRARY									${MSVC_KIT_ROOT}/3rdparty/libjpeg/lib/Release/jpeg.lib)

# ---

# TEMP !!! : juste here to test 3 different versions of intl...

if(0)
set(Intl_INCLUDE_DIR								${MSVC_KIT_ROOT}/gtkmm2/include)
set(Intl_LIBRARY									${MSVC_KIT_ROOT}/gtkmm2/lib/intl.lib)
endif()

if(0)
set(Intl_INCLUDE_DIR								${MSVC_KIT_ROOT}/gtkmm3_debug/include/gettext/intl)
set(Intl_LIBRARY									${MSVC_KIT_ROOT}/gtkmm3_debug/lib/intl.dll.lib)
endif()

if(0)
set(Intl_INCLUDE_DIR								${MSVC_KIT_ROOT}/gtkmm3_release/include/gettext/intl)
set(Intl_LIBRARY									${MSVC_KIT_ROOT}/gtkmm3_release/lib/intl.dll.lib)
endif()

### optional 2 : zip, haru(pdf)

# TODO

#set(XXXX_INCLUDE_DIR								${MSVC_KIT_ROOT}/libzip-1.1.2/include)
#set(XXXX_LIBRARY									${MSVC_KIT_ROOT}/libzip-1.1.2/lib/Release/zip.lib)

#set(XXXX_INCLUDE_DIR								${MSVC_KIT_ROOT}/libharu-2.3.0/include)
#set(XXXX_LIBRARY									${MSVC_KIT_ROOT}/libharu-2.3.0/lib/Release/libhpdf.lib)

### optional 3 : gtkmm2, gtkmm3

#set(WITH_GTKMM2										TRUE)
#set(WITH_GTKMM3_DEBUG								TRUE)
#set(WITH_GTKMM3_RELEASE								TRUE)

# ---

if(WITH_GTKMM2)
	set(GTK2_HINTS									${MSVC_KIT_ROOT}/gtkmm2)

	set(FREETYPE_INCLUDE_DIR_ft2build				${GTK2_HINTS}/include)
	set(FREETYPE_INCLUDE_DIR_freetype2				${GTK2_HINTS}/include/freetype2)

	set(Intl_INCLUDE_DIR							${GTK2_HINTS}/include)
	set(Intl_LIBRARY								${GTK2_HINTS}/lib/intl.lib)
else()
	if(WITH_GTKMM3_DEBUG)
		set(GTK3_HINTS								${MSVC_KIT_ROOT}/gtkmm3_debug)
	elseif(WITH_GTKMM3_RELEASE)
		set(GTK3_HINTS								${MSVC_KIT_ROOT}/gtkmm3_release)
	endif()

	if(WITH_GTKMM3_DEBUG OR WITH_GTKMM3_RELEASE)
		set(FREETYPE_INCLUDE_DIR_ft2build			${GTK3_HINTS}/include/freetype)
		set(FREETYPE_INCLUDE_DIR_freetype2			${GTK3_HINTS}/include/freetype/freetype)

		set(Intl_INCLUDE_DIR						${GTK3_HINTS}/include/gettext/intl)
		set(Intl_LIBRARY							${GTK3_HINTS}/lib/intl.dll.lib)
	endif()
endif()

### optional 4 : qt4, qt5

###