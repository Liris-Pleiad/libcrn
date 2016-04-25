/* Copyright 2010-2016 CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: GtkCRNSelectionBox.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNSelectionBox_HEADER
#define GtkCRNSelectionBox_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <vector>
#include <CRNMath/CRNMath.h>
#include <memory>
#include <CRNStringUTF8.h>
#include <CRNString.h>
#include <set>

namespace GtkCRN
{
	/*! \brief A Gtk::Box-like widget with multiple selection, reordering and drag'n drop features
	 * \ingroup gtkcrn
	 */
	class SelectionBox: public Gtk::EventBox
	{
		public:
			/*! \brief Constructor */
			SelectionBox(crn::Orientation ori);
			/*! \brief Destructor */
			virtual ~SelectionBox() override;
			
			////////////////////////////////////////////////////////////////////////////////////////////////
			// Gtk::Box methods
			////////////////////////////////////////////////////////////////////////////////////////////////

			/*! \brief Return value: true if the box is homogeneous. */
			bool get_homogeneous() const { return box->get_homogeneous(); }
			/*! \brief Sets the Gtk::Box:homogeneous property of box, controlling whether or not all children of box are given equal space in the box. */
			void set_homogeneous(bool homogeneous = true) { box->set_homogeneous(homogeneous); }
			/*! \brief Gets the value set by set_spacing(). */
			int get_spacing() const { return box->get_spacing(); }
			/*! \brief Sets the Gtk::Box:spacing property of box, which is the number of pixels to place between children of box. */
			void set_spacing(int spacing) { box->set_spacing(spacing); }

			/*! \brief Left/top side insert a widget to a box. */
			void pack_start(Gtk::Widget& child, Gtk::PackOptions options = Gtk::PACK_EXPAND_WIDGET, guint padding = 0);
			/*! \brief Left/top side insert a widget to a box. */
			void pack_start(Gtk::Widget& child, bool expand, bool fill, guint padding = 0);
			/*! \brief Right/bottom side insert a widget to a box. */
			void pack_end(Gtk::Widget& child, Gtk::PackOptions options = Gtk::PACK_EXPAND_WIDGET, guint padding = 0);
			/*! \brief Right/bottom side insert a widget to a box. */
			void pack_end(Gtk::Widget& child, bool expand, bool fill, guint padding = 0);

			/*! \brief Erase all elements */
			void clear();

			////////////////////////////////////////////////////////////////////////////////////////////////
			// Gtk::ScrolledWindow methods
			////////////////////////////////////////////////////////////////////////////////////////////////

			/*! \brief Sets the scrolling policy */
			void set_policy(Gtk::PolicyType hscrollbar_policy, Gtk::PolicyType vscrollbar_policy) { sw.set_policy(hscrollbar_policy, vscrollbar_policy); }

			////////////////////////////////////////////////////////////////////////////////////////////////
			// Specific methods
			////////////////////////////////////////////////////////////////////////////////////////////////

			/*! \brief Returns the orientation of the box */
			crn::Orientation get_orientation() const { return orientation; }

			/*! \brief Sets if the elements can be reordered by the user */
			void set_can_reorder(bool reorder = true) { can_reorder = reorder; }
			/*! \brief Returns if the elements can be reordered by the user */
			bool get_can_reorder() const { return can_reorder; }

			/*! \brief Returns the list of selected widgets */
			std::vector<Gtk::Widget*> get_selection() const;
			/*! \brief Returns the last widget that was selected */
			Gtk::Widget* get_last_selected() const;

			/*! \brief Returns the list of widgets inside the box */
			std::vector<Gtk::Widget*> get_content() const;
			/*! \brief Returns the number of widgets inside the box */
			size_t get_nb_children() const { return content.size(); }

			/*! \brief Sets the selection (one element) */
			void set_selection(size_t index);
			/*! \brief Adds or remove an element from the selection */
			void set_selected(size_t index, bool selected = true, bool silent = false);
			/*! \brief Is an element selected? */
			bool is_selected(size_t index);

			/*! \brief Selects all elements */
			void select_all();
			/*! \brief Deselects all elements */
			void deselect_all();
			/*! \brief Selects even elements (1st, 3rd…) */
			void select_odd();
			/*! \brief Selects odd elements (2nd, 4th…) */
			void select_even();
			/*! \brief Inverts the view selection */
			void invert_selection();

			/*! \brief If selection size is <= 1, then select the first item, else move last_selected to the first selected element */
			void select_first();
			/*! \brief If selection size is 0, the select the first element, if selection size is 1, then select the previous item, else move last_selected to the previous selected element */
			void select_previous();
			/*! \brief If selection size is 0, the select the first element, if selection size is 1, then select the next item, else move last_selected to the next selected element */
			void select_next();
			/*! \brief If selection size is <= 1, then select the last item, else move last_selected to the last selected element */
			void select_last();

			/*! \brief Signals when a widget was moved. Connect to void on_moved(moved_to, moved_from). */
			sigc::signal<void, std::vector<size_t>, std::vector<size_t>> signal_moved() { return moved; }

			/*! \brief Signals when something was dropped from another application. Connect to void on_droppedin(int position, crn::StringUTF8 data). */
			sigc::signal<void, int, crn::StringUTF8> signal_droppedin() { return droppedin; }
			/*! \brief Signal when the selection has changed. Connect to void on_selection_changed(Gtk::Widget *last_selected_widget, const std::vector<Gtk::Widget*> selection).*/
			sigc::signal<void, Gtk::Widget*, const std::vector<Gtk::Widget*>> signal_selection_changed() { return selection_changed; } 

		private:
			crn::Orientation orientation; /*!< horizontal or vertical stack? */
			bool can_reorder; /*!< is it possible to reorder elements? */
			Gtk::Box *box; /*!< the contained box */
			Gtk::ScrolledWindow sw; /*!< scrollbars */

			class Element;
			/*! \internal A widget to receive the drag'n drop */
			class DropZone: public Gtk::DrawingArea
			{
				public:
					DropZone(const sigc::slot7<void, Element*, const Glib::RefPtr<Gdk::DragContext>&, int, int, const Gtk::SelectionData&, guint, guint> &dropfun, SelectionBox *sb, Element *el);
				private:
					bool drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
					void drag_leave(const Glib::RefPtr<Gdk::DragContext>& context, guint time);
#ifdef CRN_USING_GTKMM3
					bool expose(const Cairo::RefPtr<Cairo::Context>&);
#else /* CRN_USING_GTKMM3 */
					bool expose(GdkEventExpose *ev);
#endif /* CRN_USING_GTKMM3 */
					bool in;
#ifdef CRN_USING_GTKMM3
					Gdk::RGBA bg;
#endif /* CRN_USING_GTKMM3 */
			};
			DropZone first_drop_zone; /*!< a small rectangle zone at the beginning */
			DropZone last_drop_zone; /*!< a small rectangle zone at the end */

			static const crn::String reorderKey; /*!< a keyword to indicate a reorder */
			static const int reorderId; /*!< a key to indicate a reorder */
			static const int dropId; /*!< a key to indicate a drag'n drop */

			/*! \internal A container to display a widget in the list */
			class Element: public Gtk::Frame
			{
				public:
					Element(Gtk::Widget &w, SelectionBox *sb);

					void Select() { tb.set_active(true); }
					void Deselect() { tb.set_active(false); }

					Gtk::Widget& GetWidget() { return widget; }
					const Gtk::Widget& GetWidget() const { return widget; }

				private:
					void toggled(SelectionBox *const sb);
					void drag_begin(const Glib::RefPtr<Gdk::DragContext>&) { Select(); }
					void drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& seldata, guint info, guint time) { seldata.set_text(reorderKey.CStr()); }

					std::shared_ptr<Gtk::Box> box;
					Gtk::Widget& widget;
					Gtk::ToggleButton tb;
					DropZone dz;

			};
			friend class Element;

			std::vector<std::shared_ptr<Element>> content; /*!< the content of the box */
			std::set<std::shared_ptr<Element>> selection; /*!< the objects that are selected */
			std::weak_ptr<Element> last_selected; /*!< the last element that was selected */

			/*! \brief a key was pressed or released */
			bool keyevents(GdkEventKey *ev);
			/*! \brief Cursor enters or leaves */
			bool cursor_cross(GdkEventCrossing *ev);
			bool shift_key; /*!< is the shift key pressed? */
			bool control_key; /*!< is the control key pressed? */
			bool selecting; /*!< used to prevent events from being sent when selecting a range of elements */

			/*! \brief An object has been dropped on a target, we send the data */
			void drop(Element *dropon, const Glib::RefPtr<Gdk::DragContext>& context, int, int, const Gtk::SelectionData& selection_data, guint, guint time);
			/*! \brief An object has been dropped on a target, we send the data */
#ifdef CRN_USING_GTKMM3
			void dodrop(Element *dropon, const Gtk::SelectionData *selection_data, int info);
#else /* CRN_USING_GTKMM3 */
			void dodrop(Element *dropon, const GtkSelectionData *selection_data, int info);
#endif /* CRN_USING_GTKMM3 */

			/*! \brief An object is being dragged, we check if we must scroll */
			bool drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
			/*! \brief Automatic scrolling */
			bool autoscroll();

			Gtk::Widget *drag_item; /*!< the widget being dragged */
			int vscrolldiv, hscrolldiv; /*!< automatic scrolling divisors */
			sigc::signal<void, std::vector<size_t>, std::vector<size_t>> moved; /*!< signals that an object has moved */
			sigc::signal<void, int, crn::StringUTF8> droppedin; /*!< signals that something was dropped from another application */
			sigc::signal<void, Gtk::Widget*, const std::vector<Gtk::Widget*>> selection_changed; /*!< signal when the selection has changed */
	};
}

#endif


