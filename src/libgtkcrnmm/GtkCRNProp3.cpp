/* Copyright 2010 CoReNum
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
 * file: GtkCRNProp3.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNProp3.h>
#include <CRNi18n.h>

using namespace GtkCRN;

Prop3::Prop3(Gtk::IconSize is, const crn::Prop3 &p):
	prop(p),
	size(is)
{
	img.show();
	add(img);
	set_image();
	signal_button_release_event().connect(sigc::mem_fun(this, &Prop3::click));
}

void Prop3::set_value(const crn::Prop3 &p)
{
	prop = p;
	set_image();
	value_changed.emit(prop);
}

void Prop3::set_icon_size(Gtk::IconSize is)
{
	size = is;
	set_image();
}

/*! Updates the icon */
void Prop3::set_image()
{
	switch (prop.GetValue())
	{
		case TRUE:
			//img.set(Gtk::Stock::YES, size);
			break;
		case FALSE:
			//img.set(Gtk::Stock::NO, size);
			break;
		default:
			//img.set(Gtk::Stock::DIALOG_QUESTION, size);
			;
	}
}

/*!
 * Emulates a mouse button release on the image.
 *
 * \throws	Glib::OptionError	null event
 *
 * \param[in]	ev	ev->button = 1 for LMB, 3 for RMB
 *
 * \return false
 */
bool Prop3::click(GdkEventButton *ev)
{
	if (!ev)
	{
		throw Glib::OptionError(Glib::OptionError::BAD_VALUE, 
				Glib::ustring("bool Prop3::click(GdkEventButton *ev): ") + _("null event."));
	}
	switch (ev->button)
	{
		case 1:
			switch (prop.GetValue())
			{
				case TRUE:
					prop = FALSE;
					break;
				case FALSE:
					prop = crn::Prop3::UNKNOWN;
					break;
				default:
					prop = TRUE;
			}
			break;
		case 3:
			switch (prop.GetValue())
			{
				case FALSE:
					prop = TRUE;
					break;
				case TRUE:
					prop = crn::Prop3::UNKNOWN;
					break;
				default:
					prop = FALSE;
			}
			break;
	}
	set_image();
	value_changed.emit(prop);
	return false;
}

