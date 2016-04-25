/* Copyright 2010-2014 CoReNum
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
 * file: GtkCRNApp.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNApp_HEADER
#define GtkCRNApp_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNException.h>
#include <vector>

namespace GtkCRN
{
	/*! \brief Base class to create an application
	 * \ingroup gtkcrn
	 */
	class App: 
#ifdef CRN_USING_GTKMM3
		public Gtk::ApplicationWindow
#else
		public Gtk::Window
#endif
	{
		public:
			/*! \brief Constructor. Creates Actions and adds them to UI manager.
			 *
			 * Available actions are:
			 * 	- app-file-menu
			 * 	- app-quit
			 * 	- app-help-menu
			 * 	- app-help (overload virtual void help() to customize)
			 * 	- app-about (overload virtual void about() to customize)
			 */
			App();
			/*! \brief Destructor */
			virtual ~App() {}

			/*! \brief Gets a pointer to the main window */
			static Gtk::Window* get_main_window() { return internal_main_window(); }
			/*! \brief Sets the main window of the application to be used as parent for dialogs and messages */
			static void set_main_window(Gtk::Window *win) { internal_main_window() = win; }

			/*! \brief Displays a message */
			static void show_message(const Glib::ustring &message, Gtk::MessageType mtype);
			/*! \brief Displays an exception */
			static void show_exception(crn::Exception &ex, bool kill_app = true);
			/*! \brief Displays an exception */
			static void show_exception(std::exception &ex, bool kill_app = true);
			/*! \brief Displays an exception */
			static void show_exception(bool kill_app = true);

		protected:
			/*! \brief Callback for application quit event (overloadable) */
			virtual bool ask_for_quit(GdkEventAny* event);
			/*! \brief Displays help (overloadable) */
			virtual void help() {}
			/*! \brief Displays credits (overloadable) */
			virtual void about() {}
			/*! \brief Shows a dialog asking for a single string */
			Glib::ustring ask_for_string(const Glib::ustring &msg, const Glib::ustring &defval = "");

#ifndef CRN_USING_GTKMM3
			Glib::RefPtr<Gtk::UIManager> ui_manager; /*!< add your action groups to the ui manager to easily create menus and toolbars */
			Glib::RefPtr<Gtk::ActionGroup> actions; /*!< default actions */
#endif

			static Gtk::Window*& internal_main_window();
	};
};

#endif


