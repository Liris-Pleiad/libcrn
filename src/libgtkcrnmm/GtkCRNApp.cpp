/* Copyright 2010-2016 CoReNum, ENS-Lyon
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
 * file: GtkCRNApp.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <GtkCRNApp.h>
#include <CRN.h>
#include <CRNString.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

#ifndef CRN_USING_GTKMM3
#	define get_content_area get_vbox
#endif

using namespace crn;
using namespace GtkCRN;

Gtk::Window*& App::internal_main_window() { static Gtk::Window *main = nullptr; return main; }

#ifdef CRN_USING_GTKMM3
/*! Constructor. Creates Actions and adds them to UI manager. */
App::App()
{
	add_action("app-quit", sigc::bind(sigc::hide_return(sigc::mem_fun(this, &App::ask_for_quit)), (GdkEventAny*)nullptr));
	add_action("app-help", sigc::mem_fun(this, &App::help));
	add_action("app-about", sigc::mem_fun(this, &App::about));

	signal_delete_event().connect(sigc::mem_fun(this, &App::ask_for_quit));
}
#else
/*! Constructor. Creates Actions and adds them to UI manager. */
App::App() :
	ui_manager(Gtk::UIManager::create()),
	actions(Gtk::ActionGroup::create("GtkCRN::App actions"))	
{
	actions->add(Gtk::Action::create("app-file-menu", _("_File"), _("File")));
	actions->add(Gtk::Action::create("app-quit", Gtk::Stock::QUIT), sigc::bind(sigc::hide_return(sigc::mem_fun(this, &App::ask_for_quit)), (GdkEventAny*)nullptr));
	actions->add(Gtk::Action::create("app-help-menu", Gtk::StockID("corenum-icon-circle"), _("_?"), _("?")));
	actions->add(Gtk::Action::create("app-help", Gtk::Stock::HELP), sigc::mem_fun(this, &App::help));
	actions->add(Gtk::Action::create("app-about", Gtk::Stock::ABOUT), sigc::mem_fun(this, &App::about));
	
	ui_manager->insert_action_group(actions);

	signal_delete_event().connect(sigc::mem_fun(this, &App::ask_for_quit));
}
#endif

#include <iostream>
/*! Callback for application quit event (overloadable)
 * \param[in]	event	only valid if the callback was called by the window manager
 * \return	true to continue app, false after call to Gtk::Main::Quit
 */
bool App::ask_for_quit(GdkEventAny* event)
{
	Gtk::MessageDialog dial(*this, _("Are you sure you want to quit?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
	if (dial.run() == Gtk::RESPONSE_YES)
	{
#ifdef CRN_USING_GTKMM3
		auto app = get_application();
		if (app)
			app->quit();
		else
			exit(0);
#else
		Gtk::Main::quit();
#endif
		return false;
	}
	return true;
}

/*! Shows a dialog asking for a single string
 * \param[in]	msg	the message to display in the dialog
 * \param[in]	defval	the default value of the entry
 * \return	the entered string or empty string if the dialog was canceled
 */
Glib::ustring App::ask_for_string(const Glib::ustring &msg, const Glib::ustring &defval)
{
#ifdef CRN_USING_GTKMM3
	Gtk::Dialog dial("", *this, true);
#else /* CRN_USING_GTKMM3 */
	Gtk::Dialog dial("", *this, true, false);
#endif /* CRN_USING_GTKMM3 */
	dial.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	Gtk::Label lab(msg);
	lab.show();
	dial.get_content_area()->pack_start(lab, false, true, 2);
	Gtk::Entry ent;
	ent.set_activates_default();
	ent.set_text(defval);
	ent.show();
	dial.get_content_area()->pack_start(ent, false, true, 2);
	//dial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_REJECT);
	//dial.add_button(Gtk::Stock::OK, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	//dial.set_alternative_button_order_from_array(altbut);	
	dial.set_default_response(Gtk::RESPONSE_ACCEPT);
	if (dial.run() == Gtk::RESPONSE_ACCEPT)
		return ent.get_text();
	else
		return "";
}

/*! \cond */
struct mesg
{
	Glib::ustring message;
	Gtk::MessageType mtype;
};
static gboolean do_show_message(mesg *message)
{
	Gtk::MessageDialog md(message->message, false, message->mtype, Gtk::BUTTONS_OK, true);
	md.run();
	delete message;
	return false;
}
/*! \endcond */

/*! Displays a message 
 * \param[in]	message	the message to display
 * \param[in]	mtype	the icon to display next to the text
 */
void App::show_message(const Glib::ustring &message, Gtk::MessageType mtype)
{
	mesg *msg = new mesg;
	msg->message = message;
	msg->mtype = mtype;
	g_idle_add(GSourceFunc(do_show_message), msg);
}

/*! \cond */
struct exc_data
{
	StringUTF8 message;
	StringUTF8 stack;
	bool kill;
};
gboolean display_exception(exc_data *ex)
{
	Gtk::MessageDialog md(ex->message.CStr(), false, ex->kill ? Gtk::MESSAGE_ERROR : Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);
	if (ex->stack.IsNotEmpty())
	{
		Gtk::Expander *expd = Gtk::manage(new Gtk::Expander(_("Advanced details")));
		md.get_content_area()->pack_start(*expd, true, true, 0);
		Gtk::ScrolledWindow *sw = Gtk::manage(new Gtk::ScrolledWindow);
		expd->add(*sw);
		Gtk::TextView *lab = Gtk::manage(new Gtk::TextView);
		lab->get_buffer()->set_text(ex->stack.CStr());
		sw->add(*lab);
		expd->set_expanded(false);
		expd->show_all();
	}
	md.set_resizable(true);
	md.set_default_size(500, -1);
	md.run();
	bool kill = false;
	if (ex->kill)
		kill = true;
	delete ex;
	if (kill)
		std::terminate();
	return false;
}
/*! \endcond */

/*! Displays an unknown exception 
 * \param[in]	kill_app	shall the application be killed after the exceptions was displayed?
 */
void App::show_exception(bool kill_app)
{
	exc_data *msg = new exc_data;
	msg->message = _("Unhandled exception caught.");
	msg->kill = kill_app;
	g_idle_add(GSourceFunc(display_exception), msg);
}

/*! Displays an unknown exception 
 * \param[in]	ex	the exception to display
 * \param[in]	kill_app	shall the application be killed after the exceptions was displayed?
 */
void App::show_exception(crn::Exception &ex, bool kill_app)
{
	exc_data *msg = new exc_data;
	msg->message = ex.GetMessage();
	msg->stack = ex.GetContext();
	msg->kill = kill_app;
	g_idle_add(GSourceFunc(display_exception), msg);
}

/*! Displays an unknown exception 
 * \param[in]	ex	the exception to display
 * \param[in]	kill_app	shall the application be killed after the exceptions was displayed?
 */
void App::show_exception(std::exception &ex, bool kill_app)
{
	exc_data *msg = new exc_data;
	msg->message = ex.what();
	msg->kill = kill_app;
	g_idle_add(GSourceFunc(display_exception), msg);
}


