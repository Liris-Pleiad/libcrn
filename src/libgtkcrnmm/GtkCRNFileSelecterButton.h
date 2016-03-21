/* Copyright 2010-2014 CoReNum, INSA-Lyon
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
 * file: GtkCRNFileSelecterButton.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNFileSelecterButton_HEADER
#define GtkCRNFileSelecterButton_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <GtkCRNFileSelecterDialog.h>

namespace GtkCRN
{
	/*! \brief A widget that displays the files in a directory and allows to select one
	 *
	 * A widget that displays the files in a directory and allows to select one.
	 * \ingroup	gtkcrn
*/
	class FileSelecterButton: public Gtk::Button
	{
		public:
			/*! \brief Constructor */
			FileSelecterButton(const crn::Path &p);
			virtual ~FileSelecterButton() override {}

			/*! \brief Sets the path of the directory to display */
			void set_path(const crn::Path &p) { fsel.set_path(p); }
			/*! \brief Gets the path of the displayed directory */
			const crn::Path& get_path() const { return fsel.get_path(); }

			/*! \brief Sets the selected file */
			bool set_selection(const crn::Path &p);
			/*! \brief Gets the selected file */
			crn::Path get_selection() const;

			/*! \brief Signals when a file was selected. Connect to void on_new_file_selected(crn::Path relative_filename) */
			sigc::signal<void, crn::Path> signal_selection_changed() { return selection_changed; }

		private:
			/*! \brief Displays the dialog */
			void on_click();

			sigc::signal<void, crn::Path> selection_changed; /*!< signals that the selection changed*/
			FileSelecterDialog fsel; /*!< dialog */
			Gtk::Label fname; /*!< displays the selected filename */
	};
}
#endif

