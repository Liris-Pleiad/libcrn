/* Copyright 2010-2014 CoReNum, INSA-Lyon
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
 * file: GtkCRNDocument.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNDocument_HEADER
#define GtkCRNDocument_HEADER

#include <gtkmm.h>
#include <GtkCRNImage.h>
#include <CRNDocument.h>
#include <GtkCRNSelectionBox.h>
#include <CRNUtils/CRNProgress.h>

namespace GtkCRN
{
	/*! \brief Displays a crn::Document
	 * \ingroup	gtkcrn
	 */
	class Document//: public Gtk::HPaned
	{
#if 0
		public:
			/*! \brief Constructor */
			Document(bool show_views = true, bool show_tree = true);
			/*! \brief Destructor */
			virtual ~Document() override;

			//////////////////////////////////////////////////////////////////////////////
			// General document management
			//////////////////////////////////////////////////////////////////////////////
			/*! \brief Sets the document to display */
			void set_document(const crn::SDocument &doc);
			/*! \brief Gets the displayed document */
			const crn::SDocument& get_document() { return crndoc; }
			/*! \brief Gets the displayed document */
			crn::SCDocument get_document() const { return crndoc; }

			//////////////////////////////////////////////////////////////////////////////
			// Views
			//////////////////////////////////////////////////////////////////////////////
			/*! \brief Gets the selected views' ids */
			std::vector<crn::String> get_selected_views_ids();
			/*! \brief Gets the displayed views's id */
			crn::String get_selected_view_id();
			/*! \brief Sets the one selected view */
			void set_selected_view(const crn::String &view_id);

			/*! \brief Shall the views show a thumbnail */
			void set_show_thumbnails(bool s);
			/*! \brief Shall the views show a label */
			void set_show_labels(bool s);
			/*! \brief Shall the views show their index */
			void set_show_indexes(bool s);

			/*! \brief Selects even views */
			void select_even();
			/*! \brief Selects odd views */
			void select_odd();
			/*! \brief Selects all views */
			void select_all();
			/*! \brief Selects all views */
			void deselect_all();
			/*! \brief Inverts the view selection */
			void invert_selection();
			/*! \brief Removes all selected views from the document */
			void delete_selection();

			/* !\brief Opens a dialog to add views */
			void append_views_dialog();
			/*! \brief Appends new views at the end of the document */
			void append_views(const std::vector<crn::Path> &filenames);
			/*! \brief Redraws the view list */
			void refresh_views();

			/*! \brief If selection size is <= 1, then select the first view, else move last_selected to the first selected element */
			void select_first();
			/*! \brief If selection size is 0, the select the first element, if selection size is 1, then select the previous view, else move last_selected to the previous selected element */
			void select_previous();
			/*! \brief If selection size is 0, the select the first element, if selection size is 1, then select the next view, else move last_selected to the next selected element */
			void select_next();
			/*! \brief If selection size is <= 1, then select the last view, else move last_selected to the last selected element */
			void select_last();

			/*! \brief Shall we use the default handler for dropped elements? (true by default). 
			 *
			 * Shall we use the default handler for dropped elements? (true by default). 
			 * The default handler tries to insert files in the document. 
			 * Set to false and connect a slot to signal_droppedin() if you want to handle dropped elements by yourself.
			 *
			 * \param[in]  b  true to use the default handler, false to disable the default handler
			 */
			void set_default_drop_in(bool b) { droppedin_handle->block(!b); }
			/*! \brief Signals when something was dropped from another application. Connect to void on_droppedin(int position, crn::StringUTF8 data). */
			sigc::signal<void, int, crn::StringUTF8> signal_droppedin() { return view_box.signal_droppedin(); }

			/*! \brief Signal when the selection has changed. Connect to void on_view_selection_changed(const crn::String last_selected_view_id, const std::vector<crn::String> selected_views_ids).*/
			sigc::signal<void, const crn::String, const std::vector<crn::String> > signal_view_selection_changed() { return selection_changed; } 
			
			/*! \brief Signal when the selection is about to be deleted. Connect to bool on_delete_selection(const std::vector<crn::String> selected_views_ids) that will return true to allow the deletion, false to prevent it. */
			sigc::signal<bool, const std::vector<crn::String> > signal_delete_selection() { return may_delete_selection; }

			/*! \brief Gets the action group associated to views management
			 *
			 * Gets the action group associated to views management. Valid names are:
			 *   - document-views-menu
			 *   - document-views-add
			 *   - document-views-refresh
			 *   - document-views-select-first
			 *   - document-views-select-previous
			 *   - document-views-select-next
			 *   - document-views-select-last
			 *   - document-views-select-all
			 *   - document-views-select-none
			 *   - document-views-select-even
			 *   - document-views-select-odd
			 *   - document-views-invert-selection
			 *   - document-views-remove
			 *   - document-views-export-pdf
			 */
			Glib::RefPtr<Gtk::ActionGroup>& get_views_actions() { return views_actions;}

			//////////////////////////////////////////////////////////////////////////////
			// Subblock tree
			//////////////////////////////////////////////////////////////////////////////
			/*! \brief Gets the displayed block */
			const crn::SBlock& get_selected_block() { return blocksel; }
			/*! \brief Gets the displayed block */
			crn::SCBlock get_selected_block() const { return blocksel; }

			/*! \brief Shows or hides subblocks tree */
			void set_show_subblocks(bool s) { tree_frame.set_visible(s); }
			/*! \brief Is subblocks tree shown? */
			bool get_show_subblocks() const { return tree_frame.get_visible(); }
			/*! \brief Shows or hides subblocks toolbar */
			void set_show_subblocks_toolbar(bool s) { tree_buttons.set_visible(s); }
			/*! \brief Is subblocks toolbar shown? */
			bool get_show_subblocks_toolbar() const { return tree_buttons.get_visible(); }

			/*! \brief Gets the action group associated to subblock management
			 *
			 * Gets the action group associated to subblock management. Valid names are:
			 *   - document-blocks-add
			 *   - document-blocks-remove
			 *   - document-blocks-show
			 *   - document-blocks-configure
			 */
			Glib::RefPtr<Gtk::ActionGroup>& get_subblock_actions() { return tree_actions;}

			//////////////////////////////////////////////////////////////////////////////
			// Displayed image
			//////////////////////////////////////////////////////////////////////////////
			/*! \brief Gets the image widget */
			GtkCRN::Image& get_image() { return img; }
			/*! \brief Gets the image widget */
			const GtkCRN::Image& get_image() const { return img; }

			/*! \brief Refreshes the thumbnail of a view and the Image if needed */
			void reload_image(const crn::String &view_id);
			/*! \brief Refreshes the thumbnail of a view and the Image if needed (thread safe) */
			void thread_safe_reload_image(const crn::String &view_id);

			/*! \brief Sets the colors of the user selection on the image */
			void set_selection_colors(const crn::pixel::RGB8 &col1, const crn::pixel::RGB8 &col2);
			/*! \brief Sets the colors of the subblocks on the image */
			void set_subblocks_colors(const crn::pixel::RGB8 &col1, const crn::pixel::RGB8 &col2, const crn::pixel::RGB8 &textcol);
			/*! \brief Shows or hides subblocks labels on the image */
			void set_show_subblock_labels(bool s);
			/*! \brief Are subblocks labels shown on the image? */
			bool get_show_subblock_labels() const { return show_subblock_labels; }

			//////////////////////////////////////////////////////////////////////////////
			// Custom panels
			//////////////////////////////////////////////////////////////////////////////
			/*!< \brief Return the VBox on the left of the image */
			Gtk::VBox& get_left_panel() { return left_box; }
			/*!< \brief Return the VBox on the right of the image */
			Gtk::VBox& get_right_panel() { return right_box; }

		private:
			//////////////////////////////////////////////////////////////////////////////
			// General document management
			//////////////////////////////////////////////////////////////////////////////
			crn::SDocument crndoc; /*!< the displayed document */
			crn::SBlock current_block; /*!< the currently displayed block */

#ifdef CRN_USING_HARU
			/*! \brief Opens dialog to select a file to export the document as PDF */
			void export_pdf();
#endif

			//////////////////////////////////////////////////////////////////////////////
			// Views
			//////////////////////////////////////////////////////////////////////////////
			/*! \brief Generates the view caches for the document with a progress bar */
			Glib::RefPtr<Gtk::ActionGroup> views_actions;

			/*! \brief Creates the thumbnails */
			void create_view_cache(crn::Progress *pw);

			/*! \internal Data structure for threaded operations */
			struct DVI
			{
				DVI(Document *d, const crn::String &vi):doc(d),id(vi) {}
				Document *doc;
				crn::String id;
			};
			/*! \brief reloads the image (to be used in a thread) */
			static gboolean tsreloadimage(DVI *data);

			sigc::signal<void, const crn::String, const std::vector<crn::String> > selection_changed; /*!< Signals thats the view selection changed*/
			sigc::signal<void, int, crn::StringUTF8>::iterator droppedin_handle; /*!< Signals that the user dragged and dropped something in the view list */
			sigc::signal<bool, const std::vector<crn::String> > may_delete_selection; /*!< Signals thats the users asked to remove the selection from the document */
			/*! \brief Inserts views in the document */
			void droppedin(int pos, crn::StringUTF8 data);
			/*! \brief Reorders the views */
			void moved(std::vector<size_t> to, std::vector<size_t> from);
			/*! \brief Refreshes the display */
			void on_view_selection_changed(Gtk::Widget *last_selected_widget, const std::vector<Gtk::Widget*> selection);
			/*! \brief Handler for keystrokes in the view list */
			bool boxkeyevents(GdkEventKey *ev);

			/*! \internal A widget representing a view */
			class View: public Gtk::VBox
			{
				public:
					View(const crn::String &id, crn::SDocument doc);
					const crn::String& get_view_id() const { return view_id; }
					void set_show_thumbnail(bool s) { if (s) thumb.show(); else thumb.hide(); }
					void set_thumbnail(const crn::Path &fname) { thumb.set(fname.CStr()); }
					void set_show_label(bool s) { if (s) lab.show(); else lab.hide(); }
					void set_show_index(bool s) { if (s) index.show(); else index.hide(); }
					void set_index(size_t i) { index.set_text(crn::StringUTF8(i).CStr()); }
				private:
					Gtk::Image thumb;
					Gtk::Label lab;
					Gtk::Label index;
					crn::String view_id;
			};
			SelectionBox view_box; /*!< the box containing the view objects */
			Gtk::Frame view_frame; /*!< the frame containing the view box */
			std::vector<std::shared_ptr<View> > views; /*!< the list of view objects */
			bool show_thumbnails, show_labels, show_indexes; /*!< options for view display */

			//////////////////////////////////////////////////////////////////////////////
			// Subblock tree
			//////////////////////////////////////////////////////////////////////////////
			Gtk::ScrolledWindow tree_sw; /*!< scrollbars around the subblock tree */
			Gtk::VBox tree_box; /*!< box for the subblock tree */
			Gtk::Toolbar tree_buttons; /*!< buttons to manage the subblocks */
			Glib::RefPtr<Gtk::ActionGroup> tree_actions; /*!< actions on the subblocks that can be added to the UI by the user */

			/*! \internal Gtk tree model to display the subblocks */
			class SubblockColumns : public Gtk::TreeModelColumnRecord
			{
				public:
					SubblockColumns() { add(name); add(block); add(is_tree); add(coords); }

					Gtk::TreeModelColumn<Glib::ustring> name; /*!< name of the block or the tree */
					Gtk::TreeModelColumn<crn::SBlock> block; /*!< pointer to the block or the tree's holder */
					Gtk::TreeModelColumn<bool> is_tree; /*!< is this a block or a tree? */
					Gtk::TreeModelColumn<Glib::ustring> coords; /*!< coordinates of the block if not a tree */
			};
			SubblockColumns block_columns; /*!< column model */
			Glib::RefPtr<Gtk::TreeStore> block_tree_store; /*!< subblocks data */
			Gtk::TreeView block_tree_view; /*!< subblocks display */
			crn::SBlock blocksel; /*!< the selected block */

			/*! \brief Clears the subblock tree */
			void clear_tree();
			/*! \brief Populates the subblock tree */
			void load_tree(const crn::String &view_id);
			/*! \brief Adds a node to the subblock tree */
			void tree_add_children(Gtk::TreeModel::iterator &it, crn::SBlock b);
			/*! \brief Updates the UI and image when subblock selection changed */
			void subblock_selection_changed();
			/*! \brief Adds a subblock with dialog */
			void add_subblock();
			/*! \brief Removes a subblock or a subblock tree */
			void rem_subblock();
			/*! \brief Shows a dialog to personalize the subblock display */
			void configure_subblocks();

			//////////////////////////////////////////////////////////////////////////////
			// Displayed image
			//////////////////////////////////////////////////////////////////////////////
			Image img; /*!< the image */

			/*! \brief Updates the UI when the overlay changed */
			void on_image_overlay_changed(crn::String overlay_id, crn::String overlay_item_id, Image::MouseMode mm);
			/*! \brief Shows or hides the subblocks as overlays */
			void show_hide_subblocks_on_image();
			static const crn::String subblock_list_name; /*!< Overlay id to display subblocks */
			crn::pixel::RGB8 treecol1, treecol2, treetextcol, selcol1, selcol2; /*!< colors to display subblocks*/
			bool show_subblock_labels; /*!< shall we display the subblocks names*/
			bool fill_subblocks; /*!< are the subblocks filled rectangles? */

			//////////////////////////////////////////////////////////////////////////////
			// Layout widgets
			//////////////////////////////////////////////////////////////////////////////
			Gtk::VBox left_box, right_box; /*!< boxes around the image */
			// useless references
			Gtk::VPaned vpan;
			Gtk::Frame tree_frame;
			Gtk::HBox image_box;
#endif
	};
}

#endif


