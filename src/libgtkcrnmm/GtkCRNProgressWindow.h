/* Copyright 2010-2015 CoReNum, INSA-Lyon
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
 * file: GtkCRNProgressWindow.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNProgressWindow_HEADER
#define GtkCRNProgressWindow_HEADER

#include <libgtkcrnmm_config.h>
#include <GtkCRNProgress.h>
#include <gtkmm.h>

namespace GtkCRN
{
	/*! \brief Splash window with progress bars
	 * \ingroup gtkcrn
	 */
	class ProgressWindow : public Gtk::Window
	{
		public:
			/*! \brief Constructor */
			ProgressWindow(const Glib::ustring title, Gtk::Window *parent = nullptr, bool auto_close = false);
			virtual ~ProgressWindow() override {}

			/*! \brief Adds a progress bar to the window */
			size_t add_progress_bar(const crn::String &name, size_t maxcount = 100);
			/*! \brief Gets an existing progress bar */
			crn::Progress* get_crn_progress(size_t id);
			/*! \brief Gets an existing progress bar widget */
			Gtk::ProgressBar& get_gtk_progressbar(size_t id);
			
			/*! \brief Shall the application be terminated if the processing throws an exception? */
			void set_terminate_on_exception(bool term) { terminate_on_exception = term; }

			/*! \brief Executes a processing */
			void run(sigc::slot<void> func);
			/*! \brief Executes a processing */
			template<typename T> inline T run(sigc::slot<T> func)
			{
				T retval;
				run(sigc::bind(sigc::mem_fun(this, &ProgressWindow::wrap_run<T>), func, &retval));
				return retval;
			}

			/*! \brief Signals that the progress reached 100%. Connect to void on_done(). */
			sigc::signal<void> signal_done() { return done; }

		private:
			template<typename T> void wrap_run(sigc::slot<T> func, T *retval)
			{
				*retval = func();
			}
			bool no_close(GdkEventAny *ev);
			void do_run(sigc::slot<void> func);
			void wait(Glib::Thread *thread);
			void on_close();
			static gboolean end(ProgressWindow *pw);
			bool working;

			bool autoclose;
			std::vector<std::shared_ptr<Progress> > progbars;

			bool terminate_on_exception;

#ifdef CRN_USING_GTKMM3
			Gtk::Box vbox;
#else
			Gtk::VBox vbox;
#endif
			Gtk::Button closebut;

			sigc::signal<void> done;
	};
}

#endif

