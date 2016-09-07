/* Copyright 2011-2016 CoReNum, INSA-Lyon
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
 * file: GdkCRNAltoDisplay.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoDisplay_HEADER
#define GtkCRNAltoDisplay_HEADER

#include <CRNXml/CRNAltoWrapper.h>
#include <GtkCRNDocument.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	/*! \brief XML Alto display
	 * 
	 * A class to display a document and its alto structure
	 * \ingroup	gtkcrn
	 */
	class Alto: public Gtk::HBox
	{
		public:
			Alto();
			virtual ~Alto() override {}
			
			void set_wrapper(const crn::xml::SAltoWrapper &w);

			/*! \brief Gets the action group associated to the display
			 *
			 * Gets the action group associated to the display. Valid names are:
			 * 	- alto-pages
			 * 	- alto-spaces
			 * 	- alto-textblocks
			 * 	- alto-textlines
			 * 	- alto-words
			 * 	- alto-edit
			 */
			Glib::RefPtr<Gtk::ActionGroup> get_actions() { return actions; }
			
			GtkCRN::Document& get_document() { return doc; }

		private:
			//////////////////////////////////////////////////
			// Callbacks
			//////////////////////////////////////////////////
			void on_view_changed();
			void update_boxes();
			void show_hide_pages();
			void show_hide_spaces();
			void show_hide_blocks();
			void show_hide_lines();
			void show_hide_words();
			void toggle_edit();
			void on_rmb_clicked(guint mouse_button, guint32 time, std::vector<std::pair<crn::String, crn::String> > overlay_items_under_mouse, int x, int y);
			static void add_to_group(std::set<crn::xml::Id> *group, const crn::xml::Id &id) { group->insert(id); }
			static void clear_group(std::set<crn::xml::Id> *group) { group->clear(); }
			void add_page();
			void delete_page(const crn::xml::Id &id);
			void split_page(const crn::xml::Id &id, int x);
			void merge_pages();
			void edit_word(const crn::xml::Id &id);
			void edit_line(const crn::xml::Id &id);
			void on_overlay_changed(crn::String overlay_id, crn::String overlay_item_id, GtkCRN::Image::MouseMode mm);

			//////////////////////////////////////////////////
			// Widgets
			//////////////////////////////////////////////////
			Glib::RefPtr<Gtk::ActionGroup> actions;
			std::unique_ptr<Gtk::Menu> popup;

			//////////////////////////////////////////////////
			// Attributes
			//////////////////////////////////////////////////
			GtkCRN::Document doc;
			crn::xml::SAltoWrapper alto;
			std::shared_ptr</*const*/ crn::xml::Alto> current_alto; // read only alto of the page
			bool showpages, showspaces, showtextblocks, showtextlines, showwords;
			bool edit_mode;
			std::set<crn::xml::Id> pagegroup, spacegroup, textblockgroup, textlinegroup, wordgroup;

			static const crn::String pageList;
			static const crn::String spaceList;
			static const crn::String blockList;
			static const crn::String lineList;
			static const crn::String wordList;
	};
}
#endif

#endif

