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
 * file: GdkCRNAltoImage.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNAltoImage_HEADER
#define GtkCRNAltoImage_HEADER

#include <GtkCRNImage.h>
#include <CRNXml/CRNAltoWrapper.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	class AltoImage: public sigc::trackable
	{
		public:
			AltoImage(Image *image = nullptr);
			virtual ~AltoImage();
			/*! \brief Sets the Image widget to wrap around */
			void set_image(Image *image);
			/*! \brief Sets the alto view */
			void set_view(const crn::xml::AltoWrapper::View &v, bool display_image);
			/*! \brief Unsets the alto view */
			void unset_view(bool clear_image);

			void set_can_edit_pages(bool e) { can_edit_pages = e; }
			void set_can_edit_spaces(bool e) { can_edit_spaces = e; }
			void set_can_edit_blocks(bool e) { can_edit_blocks = e; }
			void set_can_edit_lines(bool e) { can_edit_lines = e; }
			void set_can_edit_words(bool e) { can_edit_words = e; }

			/*! \brief Gets the action group associated to the alto wrapper
			 *
			 * Gets the action group associated to the alto wrapper. Valid names are:
			 * 	- alto-pages
			 * 	- alto-spaces
			 * 	- alto-textblocks
			 * 	- alto-textlines
			 * 	- alto-words
			 * 	- alto-edit
			 */
			Glib::RefPtr<Gtk::ActionGroup> get_actions() { return actions; }

			sigc::signal<void, crn::xml::WordPath> signal_word_resized() { return word_resized; }
			sigc::signal<void, crn::xml::WordPath> signal_word_deleted() { return word_deleted; }
			sigc::signal<void, crn::xml::WordPath> signal_word_added() { return word_added; }
		private:
			void show_hide_pages();
			void show_hide_spaces();
			void show_hide_blocks();
			void show_hide_lines();
			void show_hide_words();
			void toggle_edit();
			void on_rmb_clicked(guint mouse_button, guint32 time, std::vector<std::pair<crn::String, crn::String> > overlay_items_under_mouse, int x, int y);
			void on_overlay_changed(crn::String overlay_id, crn::String overlay_item_id, GtkCRN::Image::MouseMode mm);
			void set_overlays();
			void delete_word(crn::String spath);
			void add_word();
			
			static const crn::String pageList;
			static const crn::String spaceList;
			static const crn::String blockList;
			static const crn::String lineList;
			static const crn::String wordList;

			Glib::RefPtr<Gtk::ActionGroup> actions;
			Image *img;
			sigc::connection rmb_connect, overlay_connect;
			std::unique_ptr<Gtk::Menu> popup;

			bool showpages, showspaces, showtextblocks, showtextlines, showwords;
			bool edit_mode;
			std::unique_ptr<crn::xml::AltoWrapper::View> view;
			sigc::signal<void, crn::xml::WordPath> word_resized;
			sigc::signal<void, crn::xml::WordPath> word_deleted;
			sigc::signal<void, crn::xml::WordPath> word_added;
			bool can_edit_words, can_edit_lines, can_edit_blocks, can_edit_spaces, can_edit_pages;
	};
}

#endif

#endif

