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
 * file: GtkCRNFileSelecter.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNFileSelecter_HEADER
#define GtkCRNFileSelecter_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNIO/CRNPath.h>

namespace GtkCRN
{
	/*! \brief A widget that displays the files in a directory and allows to select one
	 *
	 * A widget that displays the files in a directory and allows to select one.
	 * \ingroup	gtkcrn
	 */
	class FileSelecter: public Gtk::VBox
	{
		public:
			/*! \brief Constructor */
			FileSelecter(const crn::Path &p);
			virtual ~FileSelecter() override {}

			/*! \brief Sets the path of the directory to display */
			void set_path(const crn::Path &p);
			/*! \brief Gets the path of the displayed directory */
			const crn::Path& get_path() const { return path; }

			/*! \brief Sets the selected file */
			bool set_selection(const crn::Path &p);
			/*! \brief Gets the selected file */
			crn::Path get_selection() const;

			/*! \brief Signals when a file was selected. Connect to void on_new_file_selected(crn::Path relative_filename) */
			sigc::signal<void, crn::Path> signal_selection_changed() { return selection_changed; }
			/*! \brief Signals when a file was double clicked. Connect to void on_double_click(crn::Path relative_filename) */
			sigc::signal<void, crn::Path> signal_selection_activated() { return selection_activated; }

		private:
			/*! \brief Populates the file list */
			void update_files();
			/*! \brief Signals that the selection changed */
			void on_selection_changed();
			/*! \brief Signals that a file was double clicked */
			void on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

			Gtk::Entry pathdisplay; /*!< displays the current path */
			/*! \internal Gtk tree model */
			class ModelColumns : public Gtk::TreeModelColumnRecord
			{
				public:
					ModelColumns() { add(name); }
					Gtk::TreeModelColumn<Glib::ustring> name;
			};
			ModelColumns columns; /*!< tree model */
			Glib::RefPtr<Gtk::ListStore> files; /*!< files data */
			mutable Gtk::TreeView tv; /*!< files display */

			sigc::signal<void, crn::Path> selection_changed; /*!< signals that the selection changed */
			sigc::signal<void, crn::Path> selection_activated; /*!< signals that a file was double clicked */
			crn::Path path; /*!< the current path */
	};
}
#endif

