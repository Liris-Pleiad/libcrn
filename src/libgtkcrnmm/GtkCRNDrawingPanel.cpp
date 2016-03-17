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
 * file: GtkCRNDrawingPanel.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <CRNi18n.h>
#include <GtkCRNDrawingPanel.h>
#include <CRNIO/CRNIO.h>
#include <GdkCRNPixbuf.h>

using namespace GtkCRN;

/*! Constructor */
DrawingPanel::DrawingPanel():
	thickness(2.0),
	zoom(1.0),
	actions(Gtk::ActionGroup::create("drawing panel"))
{
	bg_act = GtkCRN::ColorAction::create("drawing-panel-background", Gtk::Stock::SELECT_COLOR, _("_Background Color…"), _("Background Color"));
	actions->add(bg_act);
	actions->get_action("drawing-panel-background")->set_short_label(_("Background"));
	actions->get_action("drawing-panel-background")->set_is_important();
#ifdef CRN_USING_GTKMM3
	bg_act->set_color(Gdk::RGBA("white"));
#else /* CRN_USING_GTKMM3 */
	bg_act->set_color(Gdk::Color("white"));
#endif /* CRN_USING_GTKMM3 */
	bg_act->signal_changed().connect(sigc::mem_fun(this, &Gtk::Widget::queue_draw));

	fg_act = GtkCRN::ColorAction::create("drawing-panel-foreground", Gtk::Stock::SELECT_COLOR, _("_Foreground Color…"), _("Foreground Color"));
	actions->add(fg_act);
	actions->get_action("drawing-panel-foreground")->set_short_label(_("Foreground"));
	actions->get_action("drawing-panel-foreground")->set_is_important();
#ifdef CRN_USING_GTKMM3
	fg_act->set_color(Gdk::RGBA("black"));
#else /* CRN_USING_GTKMM3 */
	fg_act->set_color(Gdk::Color("black"));
#endif /* CRN_USING_GTKMM3 */
	fg_act->signal_changed().connect(sigc::mem_fun(this, &Gtk::Widget::queue_draw));

	actions->add(Gtk::Action::create("drawing-panel-clear", Gtk::Stock::CLEAR, _("_Clear"), _("Clear")), sigc::mem_fun(this, &DrawingPanel::clear));
	actions->add(Gtk::Action::create("drawing-panel-thickness-plus", Gtk::StockID("gtk-crn-size-plus"), _("_Increase Thickness"), _("Increase Thickness")), sigc::bind(sigc::mem_fun(this, &DrawingPanel::modify_thickness), 0.1));
	actions->add(Gtk::Action::create("drawing-panel-thickness-minus", Gtk::StockID("gtk-crn-size-minus"), _("_Decrease Thickness"), _("Decrease Thickness")), sigc::bind(sigc::mem_fun(this, &DrawingPanel::modify_thickness), -0.1));
	thick_act = GtkCRN::ScaleAction::create("drawing-panel-thickness-set", Gtk::StockID(), _("Set _Thickness"), _("Set Thickness"));
	actions->add(thick_act);
#ifdef CRN_USING_GTKMM3
	thick_act->get_adjustment()->configure(thickness, 0.2, 20, 0.1, 1, 1);
#else /* CRN_USING_GTKMM3 */
	thick_act->get_adjustment().configure(thickness, 0.2, 20, 0.1, 1, 1);
#endif /* CRN_USING_GTKMM3 */
	thick_act->signal_changed().connect(sigc::mem_fun(this, &DrawingPanel::on_thickness_set));

	std::vector<Glib::ustring> icons;
	icons.push_back("line05");
	icons.push_back("line3");
	icons.push_back("line1");
	icons.push_back("line2");
	icons.push_back("line3");
	thick_act->set_icons(icons);

	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::BUTTON_MOTION_MASK);
	signal_button_press_event().connect(sigc::mem_fun(this, &DrawingPanel::on_button_press));
	signal_motion_notify_event().connect(sigc::mem_fun(this, &DrawingPanel::on_motion));
#ifdef CRN_USING_GTKMM3
	signal_draw().connect(sigc::mem_fun(this, &DrawingPanel::on_expose));
#else /* CRN_USING_GTKMM3 */
	signal_expose_event().connect(sigc::mem_fun(this, &DrawingPanel::on_expose));
#endif /* CRN_USING_GTKMM3 */
}

/*!
 * Changes the zoom
 *
 * \throws	Glib::OptionError	null or negative zoom
 *
 * \param[in]	z	the new zoom value
 */
void DrawingPanel::set_zoom(double z)
{
	if (z <= 0)
	{
		throw Glib::OptionError(Glib::OptionError::BAD_VALUE, Glib::ustring("void DrawingPanel::set_zoom(double z): ") + _("null or negative zoom value."));
	}
	zoom = z;
	queue_draw();
}

/*! Clears the panel */
void DrawingPanel::clear()
{
	stroke_list.clear();
	queue_draw();
}

/*! Gets the background color
 * \return	the background color
 */
#ifdef CRN_USING_GTKMM3
Gdk::RGBA DrawingPanel::get_background() const
#else /* CRN_USING_GTKMM3 */
Gdk::Color DrawingPanel::get_background() const
#endif /* CRN_USING_GTKMM3 */
{
	return bg_act->get_color();
}

/*! Sets the background color
 * \param[in]	col	the new background color
 */
#ifdef CRN_USING_GTKMM3
void DrawingPanel::set_background(const Gdk::RGBA& col)
#else /* CRN_USING_GTKMM3 */
void DrawingPanel::set_background(const Gdk::Color& col)
#endif /* CRN_USING_GTKMM3 */
{
	bg_act->set_color(col);
}

/*! Gets the foreground color
 * \return	the foreground color
 */
#ifdef CRN_USING_GTKMM3
Gdk::RGBA DrawingPanel::get_foreground() const
#else /* CRN_USING_GTKMM3 */
Gdk::Color DrawingPanel::get_foreground() const
#endif /* CRN_USING_GTKMM3 */
{
	return fg_act->get_color();
}

/*! Sets the foreground color
 * \param[in]	col	the new foreground color
 */
#ifdef CRN_USING_GTKMM3
void DrawingPanel::set_foreground(const Gdk::RGBA& col)
#else /* CRN_USING_GTKMM3 */
void DrawingPanel::set_foreground(const Gdk::Color& col)
#endif /* CRN_USING_GTKMM3 */
{
	fg_act->set_color(col);
}


/*! Renders the panel to a Pixbuf
 * \return	a Pixbuf containing what was drawn on the panel
 */
Glib::RefPtr<Gdk::Pixbuf> DrawingPanel::create_image()
{
	int l = std::numeric_limits<int>::max(), t = std::numeric_limits<int>::max(), r = 0, b = 0;
	for (const std::vector<crn::Point2DDouble> &stroke : stroke_list)
	{
		for (const crn::Point2DDouble &pnt : stroke)
		{
			int pl = int(floor(pnt.X));
			int pt = int(floor(pnt.Y));
			int pr = int(ceil(pnt.X));
			int pb = int(ceil(pnt.Y));
			if (pl < l) l = pl;
			if (pt < t) t = pt;
			if (pr > r) r = pr;
			if (pb > b) b = pb;
		}
	}
	if ((r == 0) || (b == 0))
	{
		return Glib::RefPtr<Gdk::Pixbuf>(nullptr);
	}
	int border = int(ceil(thickness));
	r += border;
	b += border;
	l -= border; if (l < 0) l = 0;
	t -= border; if (t < 0) t = 0;
	Cairo::RefPtr<Cairo::ImageSurface> surf(Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, r, b));
	Cairo::RefPtr<Cairo::Context> cr(Cairo::Context::create(surf));
#ifdef CRN_USING_GTKMM3
	Gdk::Cairo::set_source_rgba(cr, bg_act->get_color());
#else /* CRN_USING_GTKMM3 */
	cr->set_source_rgb(bg_act->get_color().get_red_p(), bg_act->get_color().get_green_p(), bg_act->get_color().get_blue_p());
#endif /* CRN_USING_GTKMM3 */
	cr->move_to(0, 0);
	cr->paint();
#ifdef CRN_USING_GTKMM3
	Gdk::Cairo::set_source_rgba(cr, fg_act->get_color());
#else /* CRN_USING_GTKMM3 */
	cr->set_source_rgb(fg_act->get_color().get_red_p(), fg_act->get_color().get_green_p(), fg_act->get_color().get_blue_p());
#endif /* CRN_USING_GTKMM3 */
	cr->set_line_join(Cairo::LINE_JOIN_ROUND);
	cr->set_line_cap(Cairo::LINE_CAP_ROUND);
	cr->set_line_width(thickness);
	for (const std::vector<crn::Point2DDouble> &stroke : stroke_list)
	{
		if (stroke.size())
		{
			cr->move_to(stroke.front().X, stroke.front().Y);
			for (size_t i = 1; i < stroke.size(); ++i)
			{
				cr->line_to(stroke[i].X, stroke[i].Y);
			}
			cr->stroke();
		}
	}
	crn::Path tmpname(tmpnam(nullptr));
	surf->write_to_png(tmpname.CStr());
	Glib::RefPtr<Gdk::Pixbuf> pb = GdkCRN::PixbufFromFile(tmpname);
	try
	{
		crn::IO::Rm(tmpname);
	} catch (...) { }
	Glib::RefPtr<Gdk::Pixbuf> fpb(Gdk::Pixbuf::create_subpixbuf(pb, l, t, r - l, b - t));
	return fpb;
}

/*! Refreshes the image
 * \return	true
 */
bool DrawingPanel::draw()
{
	Cairo::RefPtr<Cairo::Context> cr = get_window()->create_cairo_context();
#ifdef CRN_USING_GTKMM3
	Gdk::Cairo::set_source_rgba(cr, bg_act->get_color());
#else /* CRN_USING_GTKMM3 */
	cr->set_source_rgb(bg_act->get_color().get_red_p(), bg_act->get_color().get_green_p(), bg_act->get_color().get_blue_p());
#endif /* CRN_USING_GTKMM3 */
	cr->move_to(0, 0);
	cr->paint();
#ifdef CRN_USING_GTKMM3
	Gdk::Cairo::set_source_rgba(cr, fg_act->get_color());
#else /* CRN_USING_GTKMM3 */
	cr->set_source_rgb(fg_act->get_color().get_red_p(), fg_act->get_color().get_green_p(), fg_act->get_color().get_blue_p());
#endif /* CRN_USING_GTKMM3 */
	cr->set_line_join(Cairo::LINE_JOIN_ROUND);
	cr->set_line_cap(Cairo::LINE_CAP_ROUND);
	cr->set_line_width(thickness * zoom);
	for (const std::vector<crn::Point2DDouble> &stroke : stroke_list)
	{
		if (stroke.size())
		{
			cr->move_to(stroke.front().X * zoom, stroke.front().Y * zoom);
			for (size_t i = 1; i < stroke.size(); ++i)
			{
				cr->line_to(stroke[i].X * zoom, stroke[i].Y * zoom);
			}
			cr->stroke();
		}
	}
	return true;
}

/*! Starts a new stroke
 * \param[in]	ev	the Gdk event
 * \return	true
 */
bool DrawingPanel::on_button_press(GdkEventButton *ev)
{
	stroke_list.push_back(std::vector<crn::Point2DDouble>());
	stroke_list.back().push_back(crn::Point2DDouble(ev->x / zoom, ev->y / zoom));
	return true;
}

/*! Adds a point to the stroke
 * \param[in]	ev	the Gdk event
 * \return	true
 */
bool DrawingPanel::on_motion(GdkEventMotion *ev)
{
	stroke_list.back().push_back(crn::Point2DDouble(ev->x / zoom, ev->y / zoom));
	queue_draw();
	return true;
}

/*! Asks for a refresh
 * \param[in]	ev	the Gdk event
 * \return	true
 */
#ifdef CRN_USING_GTKMM3
bool DrawingPanel::on_expose(const Cairo::RefPtr<Cairo::Context> &)
#else /* CRN_USING_GTKMM3 */
bool DrawingPanel::on_expose(GdkEventExpose *ev)
#endif /* CRN_USING_GTKMM3 */
{
	draw();
	return true;
}

/*! Changes the thickness of the strokes
 * \param[in]	val	the increment
 */
void DrawingPanel::modify_thickness(double val)
{
	thickness += val;
	if (thickness <= 0)
		thickness = 0.2;
#ifdef CRN_USING_GTKMM3
	thick_act->get_adjustment()->set_value(thickness);
#else /* CRN_USING_GTKMM3 */
	thick_act->get_adjustment().set_value(thickness);
#endif /* CRN_USING_GTKMM3 */
	queue_draw();
}

/*! Changes the thickness of the strokes */
void DrawingPanel::on_thickness_set()
{
#ifdef CRN_USING_GTKMM3
	thickness = thick_act->get_adjustment()->get_value();
#else /* CRN_USING_GTKMM3 */
	thickness = thick_act->get_adjustment().get_value();
#endif /* CRN_USING_GTKMM3 */
	queue_draw();
}

