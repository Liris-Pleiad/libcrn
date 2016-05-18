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

if(CRN_DEPENDENCY_PNG)
	set(ZLIB_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/zlib)
	set(ZLIB_LIBRARY									${MSVC_KIT_ROOT}/3rdparty/zlib/lib/Release/zlib.lib)

	set(PNG_PNG_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/libpng)
	set(PNG_LIBRARY										${MSVC_KIT_ROOT}/3rdparty/libpng/lib/Release/libpng.lib)
endif(CRN_DEPENDENCY_PNG)

if(CRN_DEPENDENCY_JPEG)
	set(JPEG_INCLUDE_DIR								${MSVC_KIT_ROOT}/3rdparty/libjpeg)
	set(JPEG_LIBRARY									${MSVC_KIT_ROOT}/3rdparty/libjpeg/lib/Release/jpeg.lib)
endif(CRN_DEPENDENCY_JPEG)

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

if(CRN_DEPENDENCY_ZIP)
	set(ZIP_INCLUDE_DIR								${MSVC_KIT_ROOT}/libzip-1.1.2/include)
	set(ZIP_LIBRARY									${MSVC_KIT_ROOT}/libzip-1.1.2/lib/Release/zip.lib)
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

	set(Intl_INCLUDE_DIR							${GTK2_HINTS}/include)
	set(Intl_LIBRARY								${GTK2_HINTS}/lib/intl.lib)
else()
	if(CRN_DEPENDENCY_GTKMM3_DEBUG)
		set(GTK3_HINTS								${MSVC_KIT_ROOT}/gtkmm3_debug)
	elseif(CRN_DEPENDENCY_GTKMM3_RELEASE)
		set(GTK3_HINTS								${MSVC_KIT_ROOT}/gtkmm3_release)
	endif()

	if(CRN_DEPENDENCY_GTKMM3_DEBUG OR CRN_DEPENDENCY_GTKMM3_RELEASE)
		set(FREETYPE_INCLUDE_DIR_ft2build			${GTK3_HINTS}/include/freetype)
		set(FREETYPE_INCLUDE_DIR_freetype2			${GTK3_HINTS}/include/freetype/freetype)

		if(CRN_DEPENDENCY_GETTEXT_INTL)
			set(Intl_INCLUDE_DIR						${GTK3_HINTS}/include/gettext/intl)
			set(Intl_LIBRARY							${GTK3_HINTS}/lib/intl.dll.lib)
		endif(CRN_DEPENDENCY_GETTEXT_INTL)
	endif()
endif()

### optional 4 : qt4, qt5

###
