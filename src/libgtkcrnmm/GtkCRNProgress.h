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
 * file: GtkCRNProgress.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNProgress_HEADER
#define GtkCRNProgress_HEADER

#include <libgtkcrnmm_config.h>
#include <CRNUtils/CRNProgress.h>
#include <gtkmm.h>

namespace GtkCRN
{
	/*! \brief Progress object associated with a widget
	 * \ingroup	gtkcrn
	 */
	class Progress : public crn::Progress
	{
		public:
			/*! \brief Constructor */
			Progress(const crn::String &name, size_t maxcount = 100);
			/*! \brief Constructor */
			Progress(const crn::String &name, sigc::slot<void> execute_at_end, size_t maxcount = 100);
			/*! \brief Destructor */
			virtual ~Progress() override = default;

			/*! \brief Gets the Gtk progress bar */
			Gtk::ProgressBar& GetProgressBar() { return pb; }
			/*! \brief Gets the Gtk progress bar */
			const Gtk::ProgressBar& GetProgressBar() const { return pb; }

			/*! \brief Signal thrown when the progress reached 100% */
			sigc::signal<void> signal_end() { return progress_end; }
		protected:
			/*! \brief Displays current progress on Gtk progress bar (thread safe) */
			virtual void display(const crn::String &msg) override;

		private:
			struct PM
			{
				PM(Progress *p, const crn::String &m):prog(p),msg(m) {}
				Progress *prog;
				crn::String msg;
			};
			static gboolean ts_display(PM *data);
			Gtk::ProgressBar pb;
			sigc::signal<void> progress_end;
			bool has_ended;
	};

}

#endif



