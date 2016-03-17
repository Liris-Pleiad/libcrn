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
 * file: GtkCRNFileSelecterDialog.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNFileSelecterDialog_HEADER
#define GtkCRNFileSelecterDialog_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <GtkCRNFileSelecter.h>

namespace GtkCRN
{
	/*! \brief A widget that displays the files in a directory and allows to select one
	 *
	 * A widget that displays the files in a directory and allows to select one.
	 * \ingroup	gtkcrn
	 */
	class FileSelecterDialog: public Gtk::Dialog
	{
		public:
			/*! \brief Constructor */
			FileSelecterDialog(const crn::Path &p, Gtk::Window *parent = nullptr);
			virtual ~FileSelecterDialog() override {}

			/*! \brief Sets the path of the directory to display */
			void set_path(const crn::Path &p) { fsel.set_path(p); }
			/*! \brief Gets the path of the displayed directory */
			const crn::Path& get_path() const { return fsel.get_path(); }

			/*! \brief Sets the selected file */
			bool set_selection(const crn::Path &p);
			/*! \brief Gets the selected file */
			crn::Path get_selection() const { return fsel.get_selection(); }

		private:
			/*! \brief Autocloses the dialog */
			void on_file_activated(const crn::Path filename);
			FileSelecter fsel; /*!< the file selecter widget */
	};
}
#endif

