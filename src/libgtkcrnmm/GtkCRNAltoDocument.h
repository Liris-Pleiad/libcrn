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
 * file: GdkCRNAltoDocument.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoDocument_HEADER
#define GtkCRNAltoDocument_HEADER

#include <GtkCRNDocument.h>
#include <GtkCRNAltoImage.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	class AltoDocument: public Gtk::Frame
	{
		public:
			AltoDocument();
			virtual ~AltoDocument() override {}

			void set_alto(const crn::xml::SAltoWrapper &aw);

			void set_show_toolbar(bool show);

			Document& get_document() { return doc; }
			AltoImage& get_image_wrapper() { return imgwrapper; }

			Glib::RefPtr<Gtk::ActionGroup> get_image_actions() { return doc.get_image().get_actions(); }
			Glib::RefPtr<Gtk::ActionGroup> get_alto_actions() { return imgwrapper.get_actions(); }
		private:
			void on_view_selection_changed(const crn::String last_selected_view_id, const std::vector<crn::String> selected_views_ids);

			Document doc;
			AltoImage imgwrapper;

			crn::xml::SAltoWrapper alto;
			Gtk::Toolbar toolbar;
	};
}

#endif

#endif

