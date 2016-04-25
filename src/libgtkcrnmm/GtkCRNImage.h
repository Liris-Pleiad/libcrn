/* Copyright 2010-2016 CoReNum, INSA-Lyon, ZHAO Xiaojuan, ENS-Lyon
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
 * file: GtkCRNImage.h
 * \author Yann LEYDIER, ZHAO Xiaojuan
 */

#ifndef GtkCRNImage_HEADER
#define GtkCRNImage_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNGeometry/CRNPoint2DInt.h>
#include <CRNGeometry/CRNPoint2DDouble.h>
#include <CRNGeometry/CRNRect.h>
#include <CRNStringUTF8.h>
#include <vector>
#include <iostream>

namespace GtkCRN
{
	/*! \brief A widget that holds an image
	 *
	 * An image widget with scrollbars, optional rulers, zoom facilities and multiple selections.
	 *
	 * Selections are combinations of a label and/or a rectangle. They are stored in selection lists that define the colors and the behaviour of the selections.
	 * \ingroup gtkcrn
	 */
	class Image:
#ifdef CRN_USING_GTKMM3
		public Gtk::Grid
#else
		public Gtk::Table
#endif
	{
		public:
			/*! \brief Constructor */
			Image();
			/*! \brief Destructor */
			virtual ~Image() override;

			/*! \brief Are the rulers visible? */
#ifndef CRN_USING_GTKMM3
			void set_rulers_visible(bool is_visible);
#endif /* !CRN_USING_GTKMM3 */

			/*! \brief Returns to offset of the image (at scale 1:1) */
			const crn::Point2DInt& get_offset() { return pos; }
			/*! \brief Sets the new image to display */
			void set_pixbuf(Glib::RefPtr<Gdk::Pixbuf> pb);
			/*! \brief Gets the zoom level */
			double get_zoom() const { return zoom; }
			/*! \brief Sets the zoom level */
			void set_zoom(double z);
			/*! \brief Increments the zoom level by 10% */
			void zoom_in();
			/*! \brief Decrements the zoom level by 10% */
			void zoom_out();
			/*! \brief Sets the zoom level to 100% */
			void zoom_100();
			/*! \brief Sets the zoom level to fit the image's size */
			void zoom_fit();

			/*! \brief Focus the image on a point */
			void focus_on(int x, int y);

			/*! \brief Force the image to redraw */
			void force_redraw() { need_recompute = true; }

			/*! \brief Sets the cursor in user mouse mode */
#ifdef CRN_USING_GTKMM3
			void set_user_cursor(const Gdk::CursorType &cur);
#else /* CRN_USING_GTKMM3 */
			void set_user_cursor(const Gdk::Cursor &cur);
#endif /* CRN_USING_GTKMM3 */

#ifdef CRN_USING_GTKMM3
			/*! \brief Creates a tool button connected to the zoom_fit method.
			 *
			 * Creates a tool button connected to the zoom_fit method. Valid names are:
			 *   - image-zoom-in
			 *   - image-zoom-out
			 *   - image-zoom-100
			 *   - image-zoom-fit
			 *   - image-clear-user-selection
			 */
			Glib::RefPtr<Gio::SimpleActionGroup> get_actions() { return image_actions; }
#else
			/*! \brief Creates a tool button connected to the zoom_fit method.
			 *
			 * Creates a tool button connected to the zoom_fit method. Valid names are:
			 *   - image-zoom-in
			 *   - image-zoom-out
			 *   - image-zoom-100
			 *   - image-zoom-fit
			 *   - image-clear-user-selection
			 */
			Glib::RefPtr<Gtk::ActionGroup> get_actions() { return image_actions; }
#endif

			/*! \brief State of the mouse activity */
			enum class MouseMode { NONE, SCROLL, DRAW, MOVE, STRETCH_LEFT, STRETCH_BOTTOM_LEFT, STRETCH_BOTTOM, STRETCH_BOTTOM_RIGHT, STRETCH_RIGHT, STRETCH_TOP_RIGHT, STRETCH_TOP, STRETCH_TOP_LEFT, MOVEPOINT, USER };
			/*! \brief Returns the signal associated to changes in selections. Binds to void on_overlay_changed(crn::String overlay_id, crn::String overlay_item_id, GtkCRN::Image::MouseMode mm). */
			sigc::signal<void, crn::String, crn::String, MouseMode> signal_overlay_changed() { return overlay_changed; }
			/*! \brief Returns a signal that can be used to generate pop-up menus. Binds to void on_rmb_clicked(guint mouse_button, guint32 time, std::vector<std::pair<crn::String, crn::String> > overlay_items_under_mouse, int x_on_image, int y_on_image). */
			sigc::signal<void, guint, guint32, std::vector<std::pair<crn::String, crn::String> >, int, int > signal_rmb_clicked() { return rmb_clicked; }
			/*! \brief Returns a signal that indicates a user mouse click. Binds to void on_mouse_clicked(int x, int y). */
			sigc::signal<void, int, int> signal_user_mouse() { return user_mouse; }
			/*! \brief Returns a signal that indicates that the image will be redrawn. Binds to void on_redraw(Glib::RefPtr<Gdk::Pixbuf> new_image). */
			sigc::signal<void, Glib::RefPtr<Gdk::Pixbuf> > signal_drawing() { return drawing; }
			/*! \brief Returns a signal that indicates that the zoom was changed. Binds to void on_zoom_changed(). */
			sigc::signal<void> signal_zoom_changed() { return zoom_changed; }
			/*! \brief Returns a signal emitted when the image is scrolled. Binds to void on_scroll(int x, int y). */
			sigc::signal<void, int, int> signal_scrolled_event() { return scrolled_event; }

			/*! \brief Gets the horizontal adjustment of the scrollbars */
#ifdef CRN_USING_GTKMM3
			Glib::RefPtr<Gtk::Adjustment> get_hadjustment() { return hscrollbar.get_adjustment(); }
#else /* CRN_USING_GTKMM3 */
			Gtk::Adjustment& get_hadjustment() { return *hscrollbar.get_adjustment(); }
#endif /* CRN_USING_GTKMM3 */
			/*! \brief Gets the vertical adjustment of the scrollbars */
#ifdef CRN_USING_GTKMM3
			Glib::RefPtr<Gtk::Adjustment> get_vadjustment() { return vscrollbar.get_adjustment(); }
#else /* CRN_USING_GTKMM3 */
			Gtk::Adjustment& get_vadjustment() { return *vscrollbar.get_adjustment(); }
#endif /* CRN_USING_GTKMM3 */

			/*! \brief Overlay types and mouse selection modes */
			enum class Overlay { None = 0, Rectangle, Point, Line, Text, User, Polygon };
			/*! \brief An overlay item */
			struct OverlayItem
			{
				OverlayItem(const crn::StringUTF8& lab = ""):label(lab) {}
				OverlayItem(const OverlayItem&) = delete;
				OverlayItem(OverlayItem&&) = default;
				OverlayItem& operator=(const OverlayItem&) = delete;
				OverlayItem& operator=(OverlayItem&&) = default;
				virtual ~OverlayItem() {}
				crn::StringUTF8 label; /*!< text to display */
			};
			struct Line: public OverlayItem
			{
				Line(const crn::Point2DInt &p1_, const crn::Point2DInt &p2_, const crn::StringUTF8& lab = ""): OverlayItem(lab), p1(p1_), p2(p2_) { }
				Line(const Line&) = delete;
				Line(Line&&) = default;
				Line& operator=(const Line&) = delete;
				Line& operator=(Line&&) = default;
				crn::Point2DInt p1, p2;
			};
			struct Rectangle: public OverlayItem
			{
				Rectangle(const crn::Rect & r, const crn::StringUTF8& lab = ""): OverlayItem(lab), rect(r) { }
				Rectangle(const Rectangle&) = delete;
				Rectangle(Rectangle&&) = default;
				Rectangle& operator=(const Rectangle&) = delete;
				Rectangle& operator=(Rectangle&&) = default;
				crn::Rect rect;
			};
			struct Point: public OverlayItem
			{
				Point(const crn::Point2DInt &p, const crn::StringUTF8& lab = ""): OverlayItem(lab), point(p) { }
				Point(const Point&) = delete;
				Point(Point&&) = default;
				Point& operator=(const Point&) = delete;
				Point& operator=(Point&&) = default;
				crn::Point2DInt point;
			};
			struct Polygon: public OverlayItem
			{
				Polygon(const std::vector<crn::Point2DInt>& po, const crn::StringUTF8& lab = ""):OverlayItem(lab),points(po) { }
				Polygon(std::vector<crn::Point2DInt> &&po, const crn::StringUTF8& lab = ""):OverlayItem(lab),points(std::move(po)) { }
				Polygon(const Polygon&) = delete;
				Polygon(Polygon&&) = delete;
				Polygon& operator=(const Polygon&) = delete;
				Polygon& operator=(Polygon&&) = delete;
				std::vector<crn::Point2DInt> points;
			};
			struct Text: public OverlayItem
			{
				Text(const crn::Point2DInt &p, const crn::StringUTF8& lab = ""): OverlayItem(lab), pos(p) { }
				Text(const Text&) = delete;
				Text(Text&&) = delete;
				Text& operator=(const Text&) = delete;
				Text& operator=(Text&&) = delete;
				crn::Point2DInt pos;
			};
			/*! \brief An overlay configuration */
			struct OverlayConfig
			{
				/*! \brief Default constructor */
				OverlayConfig();
#ifdef CRN_USING_GTKMM3
				Gdk::RGBA color1; /*!< color of the line */
				Gdk::RGBA color2; /*!< color of the background or handle points */
				Gdk::RGBA text_color; /*!< color of the label */
#else
				Gdk::Color color1; /*!< color of the line */
				Gdk::Color color2; /*!< color of the background or handle points */
				Gdk::Color text_color; /*!< color of the label */
#endif
				bool show_labels; /*!< shall the labels be displayed? */
				unsigned int text_size; /*!< size of the labels in pixels */
				bool editable; /*!< can the items be modificated? */
				bool moveable; /*!< can the items be moved? */
				bool can_jut_out; /*!< can the items jut out of the image? */
				bool fill; /*!< shall the rectangles and polygons be filled or double-framed? */
				double fill_alpha; /*!< transparency if rectangles are filled */
				unsigned int cross_size; /*!< size of the cross to draw points */
				bool draw_arrows; /*!< shall an arrow be drawn at the end of lines? */
				unsigned int arrow_size; /*!< size of the arrows */
				bool absolute_text_size; /*!< are the labels absolute or are they zoomed in with the image? */
				Glib::ustring font_family; /*!< font family for the labels */
				bool closed_polygons; /*!< draw polylines or polygons */
				bool show; /*!< is the overlay displayed? */
			};
			/*! \brief Gets the configuration of an overlay */
			OverlayConfig& get_overlay_config(const crn::String &id) { return overlays[id].config; }
			/*! \brief Shows an hidden overlay */
			void show_overlay(const crn::String &id) { set_overlay_visible(id, true); }
			/*! \brief Hides an overlay */
			void hide_overlay(const crn::String &id) { set_overlay_visible(id, false); }
			/*! \brief Shows or hides an overlay */
			void set_overlay_visible(const crn::String &id, bool visible);
			/*! \brief Clears all overlays */
			void clear_overlays();
			/*! \brief Clears an overlay */
			void clear_overlay(const crn::String &id);
			/*! \brief Adds a rectangle to an overlay */
			void add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::Rect &r, const crn::StringUTF8 &label = "");
			/*! \brief Adds a point to an overlay */
			void add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::Point2DInt &point, const crn::StringUTF8 &label = "");
			/*! \brief Adds a line to an overlay */
			void add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::Point2DInt &p1, const crn::Point2DInt &p2, const crn::StringUTF8 &label = "");
			/*! \brief Adds a polygon to an overlay */
			void add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const std::vector<crn::Point2DInt>& p, const crn::StringUTF8 &label="");
			/*! \brief Adds a polygon to an overlay */
			void add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, std::vector<crn::Point2DInt> &&p, const crn::StringUTF8 &label="");
			/*! \brief Adds a text to an overlay */
			void add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::StringUTF8 &label, const crn::Point2DInt &position);
			/*! \brief Gets an overlay item */
			OverlayItem& get_overlay_item(const crn::String &overlay_id, const crn::String &item_id);
			/*! \brief Gets an overlay item */
			const OverlayItem& get_overlay_item(const crn::String &overlay_id, const crn::String &item_id) const;
			/*! \brief Removes an overlay item */
			void remove_overlay_item(const crn::String &overlay_id, const crn::String &item_id);

			/*! \brief Sets the mouse selection type */
			void set_selection_type(Overlay typ);
			/*! \brief Gets the mouse selection type */
			Overlay get_selection_type() const { return selection_type; }
			/*! \brief Gets the mouse selection configuration */
			OverlayConfig& get_selection_config() { return get_overlay_config(selection_overlay()); }
			/*! \brief Is there a mouse selection? */
			bool has_selection() const;
			/*! \brief Clears the mouse selection */
			void clear_selection();
			/*! \brief Returns the name of the mouse selection overlay */
			static const crn::String& selection_overlay();
			/*! \brief Gets the mouse selection as a rectangle */
			const crn::Rect& get_selection_as_rect() const;
			/*! \brief Gets the mouse selection as a point */
			const crn::Point2DInt& get_selection_as_point() const;
			/*! \brief Gets the mouse selection as a line */
			std::pair<crn::Point2DInt, crn::Point2DInt> get_selection_as_line() const;
			/*! \brief Sets the mouse selection */
			void set_selection(const crn::Rect &r);
			/*! \brief Sets the mouse selection */
			void set_selection(const crn::Point2DInt &p);
			/*! \brief Sets the mouse selection */
			void set_selection(const crn::Point2DInt &p1, const crn::Point2DInt &p2);

		private:
			/*! \brief Asks for a refresh */
#ifdef CRN_USING_GTKMM3
			bool expose(const Cairo::RefPtr<Cairo::Context> &cc);
#else /* CRN_USING_GTKMM3 */
			bool expose(GdkEventExpose *ev);
#endif /* CRN_USING_GTKMM3 */
			/*! \brief Asks for a full redraw */
			bool configure(GdkEventConfigure *ev);
			/*! \brief Handles mouse motion */
			bool mouse_motion(GdkEventMotion *ev);
			/*! \brief Handles mouse click */
			bool button_clicked(GdkEventButton *ev);
			/*! \brief Scroll with mouse wheel */
			bool mouse_wheel(GdkEventScroll *ev);
			MouseMode mouse_mode; /*!< current mode */
			crn::Point2DDouble click_ref; /*!< where the last click occurred */
			/*! \brief Asks for a full redraw */
			void scrolled();
			/*! \brief Refreshes or redraws the image */
			bool refresh();
			sigc::connection refresher; /*!< handle to the timed event that redraws the image */

			bool need_redraw; /*!< the image and selections shall be redrawn */
			bool need_recompute; /*!< the image shall be recomputed (e.g.: was scrolled or resized) */
			Glib::RefPtr<Gdk::Pixbuf> image; /*!< original image*/
			crn::Rect image_bounds; /*!< the original image bounds */
			crn::Point2DInt pos; /*!< position in the original image */
			int dispw, disph; /*!< the display's width and height */
			double zoom; /*!< zoom factor (1.0 = 100%) */
			Glib::RefPtr<Gdk::Pixbuf> buffer; /*!< cropped and scaled image to be displayed */
			static int selection_margin; /*!< space around the selections border that is clickable */
#ifdef CRN_USING_GTKMM3
			Glib::RefPtr<Gio::SimpleActionGroup> image_actions; /*!< Public UI elements */
#else
			Glib::RefPtr<Gtk::ActionGroup> image_actions; /*!< Public UI elements */
#endif
			crn::Point2DInt *movePoint;

			/*! \internal Overlay */

			struct Overlay_internal
			{
				OverlayConfig config;
				std::map<crn::String, std::unique_ptr<OverlayItem> > items;
			};

			std::map<crn::String, Overlay_internal> overlays; /*!< named list of overlays */

			Overlay selection_type; /*!< selection type (point, line or rectangle) */
			/*! \brief Finds overlay under the mouse and the adequate mouse mode */
			MouseMode find_selection_at(double mouse_x, double mouse_y, crn::String &overlay_id, crn::String &overlay_item_id);
			/*! \brief Updates the cursor from a mouse mode */
			void set_cursor_from_mode(MouseMode m);
			crn::String selected_overlay; /*!< probably useless */
			crn::String selected_overlay_item; /*!< the user selection if it exists */

#ifndef CRN_USING_GTKMM3
			Gtk::HRuler hruler;
			Gtk::VRuler vruler;
#endif /* !CRN_USING_GTKMM3 */
			Gtk::DrawingArea da;
#ifndef CRN_USING_GTKMM3
			Glib::RefPtr<Gdk::GC> da_gc;
#endif /* !CRN_USING_GTKMM3 */
#ifdef CRN_USING_GTKMM3
			Gtk::Scrollbar hscrollbar;
			Gtk::Scrollbar vscrollbar;
#else
			Gtk::HScrollbar hscrollbar;
			Gtk::VScrollbar vscrollbar;
#endif
#ifdef CRN_USING_GTKMM3
			Gdk::CursorType scroll_cursor;
			Gdk::CursorType select_cursor;
			Gdk::CursorType move_cursor;
			Gdk::CursorType move_1_cursor;
			Gdk::CursorType drag_left_cursor;
			Gdk::CursorType drag_bottom_left_cursor;
			Gdk::CursorType drag_bottom_cursor;
			Gdk::CursorType drag_bottom_right_cursor;
			Gdk::CursorType drag_right_cursor;
			Gdk::CursorType drag_top_right_cursor;
			Gdk::CursorType drag_top_cursor;
			Gdk::CursorType drag_top_left_cursor;
			Gdk::CursorType user_cursor;
#else
			Gdk::Cursor scroll_cursor;
			Gdk::Cursor select_cursor;
			Gdk::Cursor move_cursor;
			Gdk::Cursor move_1_cursor;
			Gdk::Cursor drag_left_cursor;
			Gdk::Cursor drag_bottom_left_cursor;
			Gdk::Cursor drag_bottom_cursor;
			Gdk::Cursor drag_bottom_right_cursor;
			Gdk::Cursor drag_right_cursor;
			Gdk::Cursor drag_top_right_cursor;
			Gdk::Cursor drag_top_cursor;
			Gdk::Cursor drag_top_left_cursor;
			Gdk::Cursor user_cursor;
#endif /* CRN_USING_GTKMM3 */
			sigc::signal<void, crn::String, crn::String, MouseMode> overlay_changed; /*!< signals that an overlay item was modified */
			sigc::signal<void, guint, guint32, std::vector<std::pair<crn::String, crn::String> >, int, int > rmb_clicked; /*!< signals that the right mouse button was clicked */
			sigc::signal<void, int, int> user_mouse; /*!< signals that the mouse was clicked in custom user mode */
			sigc::signal<void, Glib::RefPtr<Gdk::Pixbuf> > drawing; /*!< signals that the image will be refreshed */
			sigc::signal<void> zoom_changed; /*!< signals that the scale changed */
			sigc::signal<void, int, int> scrolled_event; /*!< signals that the scrollbars moved */

	};
}

#endif


