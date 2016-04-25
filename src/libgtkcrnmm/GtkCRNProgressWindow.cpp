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
 * file: GtkCRNProgressWindow.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNProgressWindow.h>
#include <GtkCRNMain.h>
#include <GtkCRNApp.h>
#include <CRNi18n.h>
#include <CRNIO/CRNIO.h>

using namespace GtkCRN;

/*! Constructor
 * \param[in]	title	the title of the window
 * \param[in]	parent	a pointer to the parent window, or nullptr to use the application's default window
 * \param[in]	auto_close	shall the window close when the processing reached 100%?
 */
ProgressWindow::ProgressWindow(const Glib::ustring title, Gtk::Window *parent, bool auto_close):
	autoclose(auto_close),
	terminate_on_exception(true),
#ifdef CRN_USING_GTKMM3
	vbox(Gtk::ORIENTATION_VERTICAL),
	closebut(_("_Close")) // TODO check if translation is needed of if the Gtk uses the "icon name"
#else
	closebut(Gtk::Stock::CLOSE)
#endif
{
	set_title(title);
	if (parent)
	{
		set_transient_for (*parent);
		set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	}
	else
	{
		Gtk::Window *mainwin = App::get_main_window();
		if (mainwin)
		{
			set_transient_for (*mainwin);
			set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
		}
		else
			set_position(Gtk::WIN_POS_CENTER);
	}
	set_modal();
	set_deletable(false);
	set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);

	signal_delete_event().connect(sigc::mem_fun(this, &ProgressWindow::no_close));

	closebut.signal_clicked().connect(sigc::mem_fun(this, &ProgressWindow::on_close));
	closebut.set_sensitive(false);
	
	vbox.show();
	if (!autoclose)
	{
#ifdef CRN_USING_GTKMM3
		Gtk::Alignment *al = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_END));
		vbox.pack_end(*al, false, false, 0);
		Gtk::Box *hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
#else /* CRN_USING_GTKMM3 */
		Gtk::Alignment *al = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_RIGHT));
		vbox.pack_end(*al, false, false, 0);
		Gtk::HBox *hbox = Gtk::manage(new Gtk::HBox());
#endif /* CRN_USING_GTKMM3 */
		al->add(*hbox);
		hbox->pack_end(closebut, false, false, 0);
		al->show_all();
	}
	add(vbox);
}

/*! Adds a progress bar to the window
 * \param[in]	name	the label of the progress bar
 * \param[in]	maxcount	the number of calls to Advance() needed for the progress to reach 100%
 * \return	the id of the progress bar
 */
size_t ProgressWindow::add_progress_bar(const crn::String &name, size_t maxcount)
{
	progbars.push_back(std::make_shared<Progress>(name, maxcount));
	vbox.pack_start(progbars.back()->GetProgressBar());
	return progbars.size() - 1;
}

/*
 * Returns a progress object
 *
 * \throws	crn::ExceptionDomain	index out of bounds
 *
 * \param[in]	id	the id of the progress bar
 * \return	a pointer to a crn::Progress object
 */
crn::Progress* ProgressWindow::get_crn_progress(size_t id)
{
	if (id >= progbars.size())
		throw crn::ExceptionDomain(crn::StringUTF8("crn::Progress* ProgressWindow::get_crn_progress(size_t id): ") + _("index out of bounds."));
	return progbars[id].get();
}

/*
 * Returns a Gtk progress bar
 *
 * \throws	crn::ExceptionDomain	index out of bounds
 *
 * \param[in]	id	the id of the progress bar
 * \return	a pointer to a Gtk::ProgressBar
 */
Gtk::ProgressBar& ProgressWindow::get_gtk_progressbar(size_t id)
{
	if (id >= progbars.size())
		throw crn::ExceptionDomain(crn::StringUTF8("Gtk::ProgressBar& ProgressWindow::get_gtk_progressbar(size_t id): ") + _("index out of bounds."));
	return progbars[id]->GetProgressBar();
}

/*! Executes a processing
 * \param[in]	func	the code to execute
 */
void ProgressWindow::run(sigc::slot<void> func)
{
	show();
	Glib::Thread *thread = Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &ProgressWindow::do_run), func), true);
	working = true;
	Glib::Thread::create(sigc::bind(sigc::mem_fun(this, &ProgressWindow::wait), thread), false);
	while (working)
	{
		Gtk::Main::iteration();
	}
}

bool ProgressWindow::no_close(GdkEventAny *ev)
{
	return true;
}

void ProgressWindow::do_run(sigc::slot<void> func)
{
	try
	{
		func();
	}
	catch (crn::Exception &ex)
	{
		App::show_exception(ex, terminate_on_exception);
	}
	catch (std::exception &ex)
	{
		App::show_exception(ex, terminate_on_exception);
	}
	catch (...)
	{
		App::show_exception(terminate_on_exception);
	}
}

void ProgressWindow::wait(Glib::Thread *thread)
{
	thread->join();
	g_idle_add(GSourceFunc(ProgressWindow::end), this);
}

void ProgressWindow::on_close()
{
	hide();
	working = false;
}

gboolean ProgressWindow::end(ProgressWindow *pw)
{
	pw->closebut.set_sensitive(true);
	pw->done.emit();
	if (pw->autoclose)
	{
		pw->hide();
		pw->working = false;
	}
	return false;
}

