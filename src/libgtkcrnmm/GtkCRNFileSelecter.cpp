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
 * file: GtkCRNFileSelecter.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <CRNi18n.h>
#include <GtkCRNFileSelecter.h>
#include <CRNIO/CRNIO.h>
#include <CRNData/CRNForeach.h>

using namespace GtkCRN;

/*! Constructor 
 * \param[in]	p	the path to the directory to display
 */
FileSelecter::FileSelecter(const crn::Path &p):
	path(p)
{
	pathdisplay.show();
	pathdisplay.set_sensitive(false);
	pack_start(pathdisplay, false, true, 0);

	Gtk::ScrolledWindow *sw = Gtk::manage(new Gtk::ScrolledWindow);
	sw->show();
	pack_start(*sw, true, true, 0);
	
	path.ToLocal();
	files = Gtk::ListStore::create(columns);
	tv.set_model(files);
	tv.append_column(_("Filename"), columns.name);
	Gtk::TreeView::Column *col = tv.get_column(0);
	if (col)
	  col->set_sort_column(columns.name);
	tv.signal_row_activated().connect(sigc::mem_fun(this, &FileSelecter::on_row_activated));
	tv.show();
	sw->add(tv);
	update_files();
}

/*! Sets the path of the directory to display
 * \param[in]	p	the path to the directory to display
 */
void FileSelecter::set_path(const crn::Path &p)
{
	path = p;
	path.ToLocal();
	update_files();
}

/*! Sets the selected file
 * \param	p	the (relative) filename to select
 * \return	true if the file exists, false else
 */
bool FileSelecter::set_selection(const crn::Path &p)
{
	for (Gtk::TreeIter it : files->children())
	{
		if (it->get_value(columns.name).c_str() == p)
		{
			tv.get_selection()->select(it);
			return true;
		}
	}
	return false;
}

/*! Gets the selected file
 * \return	a void string if nothing is selected, the selected (relative) filename else
 */
crn::Path FileSelecter::get_selection() const
{
	if (tv.get_selection()->count_selected_rows() == 0)
		return "";
	Gtk::TreeIter it(tv.get_selection()->get_selected());
	return it->get_value(columns.name).c_str();
}

/*! Populates the file list */
void FileSelecter::update_files()
{
	pathdisplay.set_text(path.CStr());
	files->clear();
	crn::IO::Directory dir(path);
	std::vector<crn::Path> dirfiles(dir.GetFiles());
	for (const crn::Path &file : dirfiles)
	{
		Gtk::TreeIter it(files->append());
		it->set_value(columns.name, Glib::ustring(file.GetFilename().CStr()));
	}
	tv.get_selection()->select(files->children()[0]);
}

/*! Signals that the selection changed */
void FileSelecter::on_selection_changed()
{
	selection_changed.emit(get_selection());
}

/*! Signals that a file was double clicked
 * \param[in]	path	the selected row
 * \param[in]	column	the selected column
 */
void FileSelecter::on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
	selection_activated.emit(get_selection());
}
