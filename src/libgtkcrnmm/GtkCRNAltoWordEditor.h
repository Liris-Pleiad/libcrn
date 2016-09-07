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
 * file: GdkCRNAltoWordEditor.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoWordEditor_HEADER
#define GtkCRNAltoWordEditor_HEADER

#include <CRNXml/CRNAlto.h>
#include <gtkmm.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	/*! \brief A widget to edit a word's properties
	 * \ingroup	gtkcrn
	 */
	class AltoWordEditor: public Gtk::Dialog
	{
		public:
			AltoWordEditor(crn::xml::Alto &alto, crn::xml::AltoWord &w, Gtk::Window *parent = nullptr);
			virtual ~AltoWordEditor() override {}

		private:
			void on_toggle();
			void on_close(int resp);

			Gtk::Entry text;
			Gtk::CheckButton haswc;
			Gtk::SpinButton wc;
			Gtk::CheckButton bold, italics, subscript, superscript, smallcaps, underline;

			crn::xml::AltoWord &word;
	};
}

#endif

#endif

