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
 * file: GtkCRNFileSelecterButton.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNFileSelecterButton.h>

using namespace GtkCRN;

/*! Constructor 
 * \param[in]	p	the path to the directory to display
 */
FileSelecterButton::FileSelecterButton(const crn::Path &p):
	fsel(p)
{
	Gtk::HBox *hbox = Gtk::manage(new Gtk::HBox);
	add(*hbox);
	//hbox->pack_start(fname, true, true, 0);
	//hbox->pack_start(*Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_BUTTON)), false, true, 0);
	signal_clicked().connect(sigc::mem_fun(this, &FileSelecterButton::on_click));
	hbox->show_all();
}

/*! Sets the selected file
 * \param	p	the (relative) filename to select
 * \return	true if the file exists, false else
 */
bool FileSelecterButton::set_selection(const crn::Path &p)
{
	if (fsel.set_selection(p))
	{
		fname.set_text(fsel.get_selection().GetFilename().CStr());
		selection_changed.emit(fsel.get_selection());
		return true;
	}
	else
		return false;
}

/*! Gets the selected file
 * \return	a void string if nothing is selected, the selected (relative) filename else
 */
crn::Path FileSelecterButton::get_selection() const
{
	crn::Path sel(fsel.get_path());
	sel += crn::Path::Separator();
	sel += fname.get_text().c_str();
	return sel;
}

/*! Displays the dialog */
void FileSelecterButton::on_click()
{
	Gtk::Widget *w = get_ancestor(GTK_TYPE_WINDOW);
	if (w)
		fsel.set_transient_for ((Gtk::Window&)(*w));
	if (fsel.run() == Gtk::RESPONSE_ACCEPT)
	{
		fname.set_text(fsel.get_selection().GetFilename().CStr());
		selection_changed.emit(fsel.get_selection());
	}
	fsel.hide();
}

