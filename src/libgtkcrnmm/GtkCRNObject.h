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
 * file: GtkCRNObject.h
 * \author Yann LEYDIER
 */

#ifndef GtkSObject_HEADER
#define GtkSObject_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNObject.h>

namespace GtkCRN
{
	/*! \brief Creates a widget to display an object 
	 * \ingroup gtkcrn
	 */
	 Gtk::Widget* create_widget_from_object(const crn::Object *obj);
}

#endif



