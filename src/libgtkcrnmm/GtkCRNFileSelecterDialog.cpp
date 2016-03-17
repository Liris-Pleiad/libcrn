/* Copyright 2010-2013 CoReNum, INSA-Lyon
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
 * file: GtkCRNFileSelecterDialog.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <GtkCRNFileSelecterDialog.h>
#include <GtkCRNApp.h>
#include <CRNi18n.h>

using namespace GtkCRN;

/*! Constructor 
 * \param[in]	p	the path to the directory to display
 * \param[in]	parent	a pointer to the parent window or nullptr for the application main window
 */
FileSelecterDialog::FileSelecterDialog(const crn::Path &p, Gtk::Window *parent):
	Gtk::Dialog(_("File selection"), true),
	fsel(p)
{
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
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	add_button(Gtk::Stock::OK, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	set_alternative_button_order_from_array(altbut);	
	set_default_response(Gtk::RESPONSE_ACCEPT);
	fsel.show();
	fsel.signal_selection_activated().connect(sigc::mem_fun(this, &FileSelecterDialog::on_file_activated));
	get_vbox()->pack_start(fsel, true, true, 0);
	set_default_size(384, 512);
}

/*! Autocloses the dialog
 * \param[in]	filename	ignored
 */
void FileSelecterDialog::on_file_activated(const crn::Path filename)
{
	response(Gtk::RESPONSE_ACCEPT);
}

/*! Sets the selected file
 * \param	p	the (relative) filename to select
 * \return	true if the file exists, false else
 */
bool FileSelecterDialog::set_selection(const crn::Path &p)
{
	return fsel.set_selection(p);
}
