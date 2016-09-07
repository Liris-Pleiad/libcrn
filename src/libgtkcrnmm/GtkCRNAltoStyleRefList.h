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
 * file: GdkCRNAltoStyleRefList.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoStyleRefList_HEADER
#define GtkCRNAltoStyleRefList_HEADER

#include <GtkCRNAltoStyleButton.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	/*! \brief A button that links to a style editor
	 * 
	 * A button that links to a style editor
	 * \ingroup	gtkcrn
	 */
	class AltoStyleRefList: public Gtk::HBox
	{
		public:
			AltoStyleRefList(crn::xml::Alto &a, crn::xml::Element &el);
			virtual ~AltoStyleRefList() override {}

		private:
			//////////////////////////////////////////////////
			// Callbacks
			//////////////////////////////////////////////////
			void add_style();
			void rem_style(crn::xml::Id id);
			void create_style(bool text, Gtk::VBox *vbox, std::map<crn::xml::Id, std::unique_ptr<Gtk::CheckButton> > *buts);

			//////////////////////////////////////////////////
			// Widgets
			//////////////////////////////////////////////////
			std::vector<std::unique_ptr<AltoStyleButton>> styles;

			//////////////////////////////////////////////////
			// Attributes
			//////////////////////////////////////////////////
			crn::xml::Alto &alto;
			crn::xml::Element element;
	};
}

#endif

#endif

