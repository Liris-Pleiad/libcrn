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
 * file: GdkCRNAltoStyleButton.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoStyleButton_HEADER
#define GtkCRNAltoStyleButton_HEADER

#include <CRNXml/CRNAlto.h>
#include <gtkmm.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	/*! \brief A button that links to a style editor
	 * 
	 * A button that links to a style editor
	 * \ingroup	gtkcrn
	 */
	class AltoStyleButton: public Gtk::HBox
	{
		public:
			AltoStyleButton(crn::xml::Alto &alto, const crn::xml::Id &id);
			virtual ~AltoStyleButton() override {}

			Gtk::HBox& get_front() { return front; }
			const crn::xml::Id get_id() const { return lab.get_text().c_str(); }
		private:
			//////////////////////////////////////////////////
			// Callbacks
			//////////////////////////////////////////////////
			void edit_text_style(crn::xml::Alto::Styles::Text &style);
			void edit_paragraph_style(crn::xml::Alto::Styles::Paragraph &style);
			void set_style(crn::xml::Alto::Styles::Text &style);

			//////////////////////////////////////////////////
			// Widgets
			//////////////////////////////////////////////////
			Gtk::HBox front;
			Gtk::Label lab;
			Gtk::Button edit;

			//////////////////////////////////////////////////
			// Attributes
			//////////////////////////////////////////////////
	};
}

#endif

#endif

