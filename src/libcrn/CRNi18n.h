/* Copyright 2010-2012 CoReNum, INSA Lyon
 *
 * This file is part of libcrn.
 *
 * libcrn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libcrn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcrn.  If not, see <http://www.gnu.org/licenses/>.
 *
 * file: CRNi18n.h
 * \author Yann LEYDIER
 */

#ifndef CRN_I18N_HEADER
#define CRN_I18N_HEADER

#include <CRN.h>

#if defined(CRN_USING_GLIB_INTL)
// libintl through gtk
#		include <gtk/gtk.h>
#		include <glib/gi18n-lib.h>
#		define CRNtextdomain textdomain
#		define CRNbindtextdomain bindtextdomain
#		define CRNbind_textdomain_codeset bind_textdomain_codeset
#else
#   if defined(CRN_USING_LIBINTL)
// directly using libintl
#     include <libintl.h>
#  		define _(String) dgettext(GETTEXT_PACKAGE, String)
#			define N_(String) dngettext(GETTEXT_PACKAGE, String)
#		define CRNtextdomain textdomain
#		define CRNbindtextdomain bindtextdomain
#		define CRNbind_textdomain_codeset bind_textdomain_codeset
#   else
// no i18n
#		if !defined(CRN_PF_ANDROID)
#			ifdef _MSC_VER
#				//pragma message("--> Not using gtk nor libintl. i18n will not be supported.") // no #warning with MSVC
#			else
#				//warning "--> Not using gtk nor libintl. i18n will not be supported."
#			endif
#		endif
#		define _(String) (String)
#		define N_(String) (String)
#		define GETTEXT_PACKAGE ""
inline char* CRNtextdomain(const char*) { return NULL; }
inline char* CRNbindtextdomain(const char*, const char*) { return NULL; }
inline char* CRNbind_textdomain_codeset(const char*, const char*) { return NULL; }
#   endif
#endif

#endif

