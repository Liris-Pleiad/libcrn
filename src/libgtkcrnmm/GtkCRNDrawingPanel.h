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
 * file: GtkCRNDrawingPanel.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNDrawingPanel_HEADER
#define GtkCRNDrawingPanel_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNGeometry/CRNPoint2DDouble.h>
#include <GtkCRNColorAction.h>
#include <GtkCRNScaleAction.h>

namespace GtkCRN
{
	/*! \brief A widget that allows to draw on it
	 *
	 * A widget for drawing. The user can adjust the colors of back- and foreground and the stroke thickness. The sketch can be exported to an image.
	 *
	 * \ingroup	gtkcrn
	 */
	class DrawingPanel: public Gtk::DrawingArea
	{
		public:
			/*! \brief Constructor */
			DrawingPanel();
			virtual ~DrawingPanel() override {}

			/*! \brief Returns actions associated to the drawing panel. Valid actions are:
			 * 	- drawing-panel-background
			 * 	- drawing-panel-foreground
			 * 	- drawing-panel-clear
			 * 	- drawing-panel-thickness-set
			 * 	- drawing-panel-thickness-plus
			 * 	- drawing-panel-thickness-minus
			 */
			Glib::RefPtr<Gtk::ActionGroup> get_actions() { return actions; }
			/*! \brief Sets the zoom level */
			void set_zoom(double z);

			/*! \brief Clears the panel */
			void clear();

			/*! \brief Gets the background color */
#ifdef CRN_USING_GTKMM3
			Gdk::RGBA get_background() const;
#else /* CRN_USING_GTKMM3 */
			Gdk::Color get_background() const;
#endif /* CRN_USING_GTKMM3 */
			/*! \brief Sets the background color */
#ifdef CRN_USING_GTKMM3
			void set_background(const Gdk::RGBA& col);
#else /* CRN_USING_GTKMM3 */
			void set_background(const Gdk::Color& col);
#endif /* CRN_USING_GTKMM3 */
			/*! \brief Gets the foreground color */
#ifdef CRN_USING_GTKMM3
			Gdk::RGBA get_foreground() const;
#else /* CRN_USING_GTKMM3 */
			Gdk::Color get_foreground() const;
#endif /* CRN_USING_GTKMM3 */
			/*! \brief Sets the foreground color */
#ifdef CRN_USING_GTKMM3
			void set_foreground(const Gdk::RGBA& col);
#else /* CRN_USING_GTKMM3 */
			void set_foreground(const Gdk::Color& col);
#endif /* CRN_USING_GTKMM3 */

			/*! \brief Renders the panel to a Pixbuf */
			Glib::RefPtr<Gdk::Pixbuf> create_image();

		private:
			/*! \brief Refreshes the image */
			bool draw();
			/*! \brief Starts a new stroke */
			bool on_button_press(GdkEventButton *ev);
			/*! \brief Adds a point to the stroke */
			bool on_motion(GdkEventMotion *ev);
			/*! \brief Asks for a refresh */
#ifdef CRN_USING_GTKMM3
			bool on_expose(const Cairo::RefPtr<Cairo::Context> &);
#else /* CRN_USING_GTKMM3 */
			bool on_expose(GdkEventExpose *ev);
#endif /* CRN_USING_GTKMM3 */
			/*! \brief Changes the thickness of the strokes */
			void modify_thickness(double val);
			/*! \brief Changes the thickness of the strokes */
			void on_thickness_set();

			double thickness; /*!< the thickness of the strokes in pixels */
			double zoom; /*!< the current scale */
			std::vector<std::vector<crn::Point2DDouble>> stroke_list; /*!< the strokes to display*/

			Glib::RefPtr<Gtk::ActionGroup> actions; /*!< public UI elements */
#ifndef CRN_USING_GTKMM3
			Glib::RefPtr<GtkCRN::ColorAction> bg_act, fg_act;
			Glib::RefPtr<GtkCRN::ScaleAction> thick_act;
#endif
	};
}

#endif

