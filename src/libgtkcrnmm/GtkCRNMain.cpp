/* Copyright 2010-2016 CoReNum, INSA-Lyon, ENS-Lyon
 *
 * This file is part of libgtkcrnmm.
 *
 * libgtkcrnmm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libgtkcrnmm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgtkcrnmm.  If not, see <http://www.gnu.org/licenses/>.
 *
 * file: GtkCRNMain.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <CRN.h>
#include <CRNConfig.h>
#include <GtkCRNMain.h>
#include <CRNString.h>
#include <CRNIO/CRNIO.h>
#include <GdkCRNPixbuf.h>
#include <iostream>
#ifdef __GNUG__
#	include <cxxabi.h>
#endif
#include <CRNi18n.h>

#include "logo64trans.hpp"
#include "icon64trans.hpp"
#include "icon64circletrans.hpp"

using namespace crn;
using namespace GtkCRN;

#ifdef CRN_PF_WIN32
const char *HOMEPATH = "APPDATA";
#else
const char *HOMEPATH = "HOME";
#endif

bool Main::init_done(false);

Main::Main(int &argc, char **&argv):
	Gtk::Main(argc, argv),
	iconfac(Gtk::IconFactory::create())
{
	if (!init_done)
	{
		// init
		if (!Glib::thread_supported())
			Glib::thread_init();

		Gio::init();

		// locales
		char *loc = getenv("LANG");
		CRNdout << GETTEXT_PACKAGE << " LANG environment var: " << (loc == nullptr ? "none" : loc) << std::endl;

		loc = setlocale(LC_ALL, "");
		if (!loc)
			CRNdout << GETTEXT_PACKAGE << " setlocale failed" << std::endl;
		else
			CRNdout << GETTEXT_PACKAGE << " locale = " << loc << std::endl;

		CRNdout << GETTEXT_PACKAGE << " gettext package = " << GETTEXT_PACKAGE << std::endl;

		CRNdout << "setting path to " << Config::GetLocalePath().CStr() << std::endl;
		loc = bindtextdomain(GETTEXT_PACKAGE, Config::GetLocalePath().CStr());
		if (!loc)
			CRNdout << GETTEXT_PACKAGE << " no bound path. should be " << Config::GetLocalePath().CStr() << std::endl;
		else
			CRNdout << GETTEXT_PACKAGE << " path = " << loc << std::endl;
		loc = bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
		if (!loc)
			CRNdout << GETTEXT_PACKAGE << " no bound codeset. should be " << "UTF-8" << std::endl;
		else
			CRNdout << GETTEXT_PACKAGE << " Codeset = " << loc << std::endl;
		loc = textdomain(GETTEXT_PACKAGE);
		if (!loc)
			CRNdout << GETTEXT_PACKAGE << " textdomain failed" << std::endl;
		else
			CRNdout << GETTEXT_PACKAGE << " text domain = " << loc << std::endl;

		CRNdout << _("Using default language.") << std::endl;

		init_done = true;
	}

	// icons
	struct {const char *file; const char *name;} iconlist[] =
	{
		{"addblock.png", "gtk-crn-add-block"},
		{"addview.png", "gtk-crn-add-view"},
		{"block.png", "gtk-crn-block"},
		{"document.png", "gtk-crn-document"},
		{"eraser.png", "gtk-crn-eraser"},
		{"even.png", "gtk-crn-even"},
		{"eye.png", "gtk-crn-eye"},
		{"filter.png", "gtk-crn-filter"},
		{"find.png", "gtk-crn-find"},
		{"find_from_file.png", "gtk-crn-find-from-file"},
		{"findpencil.png", "gtk-crn-findpencil"},
		{"image.png", "gtk-crn-image"},
		{"inkpen.png", "gtk-crn-inkpen"},
		{"invert.png", "gtk-crn-invert"},
		{"key.png", "gtk-crn-key"},
		{"keyboard.png", "gtk-crn-keyboard"},
		{"lens.png", "gtk-crn-lens"},
		{"line05.png", "gtk-crn-line-0.5"},
		{"line1.png", "gtk-crn-line-1"},
		{"line2.png", "gtk-crn-line-2"},
		{"line3.png", "gtk-crn-line-3"},
		{"log.png", "gtk-crn-log"},
		{"merge.png", "gtk-crn-merge"},
		{"mouse.png", "gtk-crn-mouse"},
		{"odd.png", "gtk-crn-odd"},
		{"paintbrush.png", "gtk-crn-paintbrush"},
		{"pencil.png", "gtk-crn-pencil"},
		{"remblock.png", "gtk-crn-delete-block"},
		{"remview.png", "gtk-crn-delete-view"},
		{"rotate_180.png", "gtk-crn-rotate-180"},
		{"rotate_left.png", "gtk-crn-rotate-left"},
		{"rotate_right.png", "gtk-crn-rotate-right"},
		{"script.png", "gtk-crn-script"},
		{"size_plus.png", "gtk-crn-size-plus"},
		{"size_minus.png", "gtk-crn-size-minus"},
		{"square1.png", "gtk-crn-square-1"},
		{"square2.png", "gtk-crn-square-2"},
		{"square3.png", "gtk-crn-square-3"},
		{"square4.png", "gtk-crn-square-4"},
		{"view.png", "gtk-crn-view"},
		{"rgb.png", "gtk-crn-rgb"},
		{"gray.png", "gtk-crn-gray"},
		{"bw.png", "gtk-crn-bw"},
		{"cmyk.png", "gtk-crn-cmyk"},
		{"twopages.png", "gtk-crn-two-pages"},
		{"paragraph.png", "gtk-crn-paragraph"},
	};
	crn::Path basedir(Config::GetStaticDataPath() + crn::Path::Separator());
	for (size_t tmp = 0; tmp < sizeof(iconlist) / (2 * sizeof(const char*)); tmp++)
	{
		try
		{
			Glib::RefPtr<Gdk::Pixbuf> pb = GdkCRN::PixbufFromFile(basedir + iconlist[tmp].file);
			if (pb)
			{
#ifdef CRN_USING_GTKMM3
				icons.push_back(Gtk::IconSet::create(pb));
#else /* CRN_USING_GTKMM3 */
				icons.push_back(Gtk::IconSet(pb));
#endif /* CRN_USING_GTKMM3 */
				iconfac->add(Gtk::StockID(iconlist[tmp].name), icons.back());
			}
		}
		catch (...)
		{
			CRNError("libgtkcrnmm: Missing file: " + basedir + iconlist[tmp].file);
		}
	}
	Glib::RefPtr<Gdk::Pixbuf> pb(Gdk::Pixbuf::create_from_inline(sizeof(logo64), logo64));
	if (pb)
	{
#ifdef CRN_USING_GTKMM3
		icons.push_back(Gtk::IconSet::create(pb));
#else /* CRN_USING_GTKMM3 */
		icons.push_back(Gtk::IconSet(pb));
#endif /* CRN_USING_GTKMM3 */
		iconfac->add(Gtk::StockID("corenum-logo"), icons.back());
	}
	pb = Gdk::Pixbuf::create_from_inline(sizeof(icon64), icon64);
	if (pb)
	{
#ifdef CRN_USING_GTKMM3
		icons.push_back(Gtk::IconSet::create(pb));
#else /* CRN_USING_GTKMM3 */
		icons.push_back(Gtk::IconSet(pb));
#endif /* CRN_USING_GTKMM3 */
		iconfac->add(Gtk::StockID("corenum-icon"), icons.back());
	}
	pb = Gdk::Pixbuf::create_from_inline(sizeof(icon64circle), icon64circle);
	if (pb)
	{
#ifdef CRN_USING_GTKMM3
		icons.push_back(Gtk::IconSet::create(pb));
#else /* CRN_USING_GTKMM3 */
		icons.push_back(Gtk::IconSet(pb));
#endif /* CRN_USING_GTKMM3 */
		iconfac->add(Gtk::StockID("corenum-icon-circle"), icons.back());
	}
	iconfac->add_default();
	Gtk::IconTheme::get_default()->append_search_path(Config::GetStaticDataPath().CStr());
}

void Main::run_thread_safe()
{
#ifdef CRN_USING_GTKMM3
	//gdk_threads_enter();
#else /* CRN_USING_GTKMM3 */
	gdk_threads_enter();
#endif /* CRN_USING_GTKMM3 */
	try
	{
		run();
	}
	catch (crn::Exception &ex)
	{
		CRNdout << ex.GetMessage().c_str() << std::endl;
		CRNdout << ex.GetContext().c_str() << std::endl;
	}
	catch (std::exception &ex)
	{
		CRNdout << ex.what() << std::endl;
	}
	catch (...)
	{
		CRNdout << "unknown error" << std::endl;
	}
#ifdef CRN_USING_GTKMM3
	//gdk_threads_leave();
#else /* CRN_USING_GTKMM3 */
	gdk_threads_leave();
#endif /* CRN_USING_GTKMM3 */
}

static void gtkcrnmmterminate()
{
	try
	{
		throw;
	}
	catch (const Exception &ex)
	{
		const char *exname = typeid(ex).name();
#ifdef __GNUG__
		int status = -1;
		char *ret = abi::__cxa_demangle(exname, nullptr, nullptr, &status);
		if (status == 0)
		{
			exname = ret;
		}
#endif
		std::cerr << "Unhandled <" << exname << ">: " << std::endl;
#ifdef __GNUG__
		free(ret);
#endif
		std::cerr << "what: " << ex.what() << std::endl;
		std::cerr << "context: " << ex.GetContext().c_str() << std::endl;
	}
	catch (const Glib::Exception &ex)
	{
		const char *exname = typeid(ex).name();
#ifdef __GNUG__
		int status = -1;
		char *ret = abi::__cxa_demangle(exname, nullptr, nullptr, &status);
		if (status == 0)
		{
			exname = ret;
		}
#endif
		std::cerr << "Unhandled <" << exname << ">: " << std::endl;
#ifdef __GNUG__
		free(ret);
#endif
		std::cerr << "what: " << ex.what().c_str() << std::endl;
	}
	catch (const std::exception &ex)
	{
		const char *exname = typeid(ex).name();
#ifdef __GNUG__
		int status = -1;
		char *ret = abi::__cxa_demangle(exname, nullptr, nullptr, &status);
		if (status == 0)
		{
			exname = ret;
		}
#endif
		std::cerr << "Unhandled <" << exname << ">: " << std::endl;
#ifdef __GNUG__
		free(ret);
#endif
		std::cerr << "what: " << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unexpected exception!" << std::endl;
	}
	abort();
}

/*! Sets the default exception handler to print message and context to the standard error */
void Main::SetDefaultExceptionHandler()
{
	Glib::add_exception_handler(sigc::ptr_fun(gtkcrnmmterminate));
	crn::Exception::SetDefaultHandler();
}


