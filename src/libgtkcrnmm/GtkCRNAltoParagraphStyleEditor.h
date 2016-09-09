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
 * file: GdkCRNAltoParagraphStyleEditor.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoParagraphStyleEditor_HEADER
#define GtkCRNAltoParagraphStyleEditor_HEADER

#include <CRNXml/CRNAlto.h>
#include <GtkCRNProp3.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	/*! \brief Paragraph style edition widget
	 * 
	 * A class to display and modify a paragraph style
	 * \ingroup	gtkcrn
	 */
	class AltoParagraphStyleEditor: public Gtk::Dialog
	{
		public:
			AltoParagraphStyleEditor(const crn::xml::Alto::Styles::Paragraph &ps, Gtk::Window *parent = nullptr);
			virtual ~AltoParagraphStyleEditor() override {}

		private:
			//////////////////////////////////////////////////
			// Callbacks
			//////////////////////////////////////////////////
			void on_close(int resp);
			void on_switch();

			//////////////////////////////////////////////////
			// Widgets
			//////////////////////////////////////////////////
			Gtk::Table tab;
			static const crn::StringUTF8 aUndef;
			static const crn::StringUTF8 aRight;
			static const crn::StringUTF8 aLeft;
			static const crn::StringUTF8 aCenter;
			Gtk::ComboBoxText align;
			Gtk::CheckButton hasleft, hasright, haslinespace, hasfirstindent;
			Gtk::SpinButton left, right, linespace, firstindent;

			//////////////////////////////////////////////////
			// Attributes
			//////////////////////////////////////////////////
			crn::xml::Alto::Styles::Paragraph style;
	};
}

#endif

#endif

