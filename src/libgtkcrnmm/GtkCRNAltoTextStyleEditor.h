/* Copyright 2012-2016 CoReNum, INSA-Lyon
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
 * file: GdkCRNAltoTextStyleEditor.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoTextStyleEditor_HEADER
#define GtkCRNAltoTextStyleEditor_HEADER

#include <CRNXml/CRNAlto.h>
#include <GtkCRNProp3.h>
#include <gtkmm.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	/*! \brief Text style edition widget
	 * 
	 * A class to display and modify a text style
	 */
	class AltoTextStyleEditor: public Gtk::Dialog
	{
		public:
			AltoTextStyleEditor(const crn::xml::Alto::Styles::Text &ts, Gtk::Window *parent = nullptr);
			virtual ~AltoTextStyleEditor() override {}

		private:
			//////////////////////////////////////////////////
			// Callbacks
			//////////////////////////////////////////////////
			void on_color();
			void on_close(int resp);

			//////////////////////////////////////////////////
			// Widgets
			//////////////////////////////////////////////////
			Gtk::Table tab;
			Gtk::Entry fontfamily;
			GtkCRN::Prop3 serif, fixedwidth;
			Gtk::CheckButton hascolor, bold, italics, subscript, superscript, smallcaps, underline;
			Gtk::SpinButton fontsize;
			Gtk::ColorButton fontcolor;

			//////////////////////////////////////////////////
			// Attributes
			//////////////////////////////////////////////////
			crn::xml::Alto::Styles::Text style;
	};
}

#endif

#endif

