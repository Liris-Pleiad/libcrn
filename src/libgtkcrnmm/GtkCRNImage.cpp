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
 * file: GtkCRNImage.cpp
 * \author Yann LEYDIER, ZHAO Xiaojuan
 */

#include <libgtkcrnmm_config.h>
#include <CRNi18n.h>
#include <GtkCRNImage.h>
#include <CRNIO/CRNIO.h>
#include <CRNMath/CRNMath.h>

using namespace GtkCRN;

#ifdef CRN_USING_GTKMM3
#	define get_red_p get_red
#	define get_green_p get_green
#	define get_blue_p get_blue
#endif

int Image::selection_margin(5);

/*! Constructor */
Image::Image():
	//Grid(3, 3, false),
	mouse_mode(MouseMode::NONE),
	need_redraw(true),
	need_recompute(false),
	dispw(0),
	disph(0),
	zoom(1.0),
	image_actions(Gtk::ActionGroup::create("image")),
	selection_type(Overlay::None)//,
#ifdef CRN_USING_GTKMM3/*
	scroll_cursor(Gdk::Cursor::create(Gdk::FLEUR)),
	select_cursor(Gdk::Cursor::create(Gdk::CROSS)),
	move_cursor(Gdk::Cursor::create(Gdk::HAND1)),
	move_1_cursor(Gdk::Cursor::create(Gdk::FLEUR)),
	drag_left_cursor(Gdk::Cursor::create(Gdk::LEFT_SIDE)),
	drag_bottom_left_cursor(Gdk::Cursor::create(Gdk::BOTTOM_LEFT_CORNER)),
	drag_bottom_cursor(Gdk::Cursor::create(Gdk::BOTTOM_SIDE)),
	drag_bottom_right_cursor(Gdk::Cursor::create(Gdk::BOTTOM_RIGHT_CORNER)),
	drag_right_cursor(Gdk::Cursor::create(Gdk::RIGHT_SIDE)),
	drag_top_right_cursor(Gdk::Cursor::create(Gdk::TOP_RIGHT_CORNER)),
	drag_top_cursor(Gdk::Cursor::create(Gdk::TOP_SIDE)),
	drag_top_left_cursor(Gdk::Cursor::create(Gdk::TOP_LEFT_CORNER)),
	user_cursor(Gdk::Cursor::create(Gdk::TARGET))*/
#else /* CRN_USING_GTKMM3 */
	/*scroll_cursor(Gdk::FLEUR),
	select_cursor(Gdk::CROSS),
	move_cursor(Gdk::HAND1),
	move_1_cursor(Gdk::FLEUR),
	drag_left_cursor(Gdk::LEFT_SIDE),
	drag_bottom_left_cursor(Gdk::BOTTOM_LEFT_CORNER),
	drag_bottom_cursor(Gdk::BOTTOM_SIDE),
	drag_bottom_right_cursor(Gdk::BOTTOM_RIGHT_CORNER),
	drag_right_cursor(Gdk::RIGHT_SIDE),
	drag_top_right_cursor(Gdk::TOP_RIGHT_CORNER),
	drag_top_cursor(Gdk::TOP_SIDE),
	drag_top_left_cursor(Gdk::TOP_LEFT_CORNER),
	user_cursor(Gdk::TARGET)*/
#endif /* CRN_USING_GTKMM3 */
{
#ifdef CRN_USING_GTKMM3
	//attach(hruler, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL, 0, 0);
	//hruler.show();
#else /* CRN_USING_GTKMM3 */
	attach(hruler, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL, 0, 0);
	hruler.show();
#endif /* CRN_USING_GTKMM3 */
	//hruler.set_sensitive(false);
#ifdef CRN_USING_GTKMM3
	//attach(vruler, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 0, 0);
	//vruler.show();
#else /* CRN_USING_GTKMM3 */
	attach(vruler, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 0, 0);
	vruler.show();
#endif /* CRN_USING_GTKMM3 */
	//vruler.set_sensitive(false);
#ifdef CRN_USING_GTKMM3
	attach(da, 1, 2, 1, 2);
#else
	attach(da, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND | Gtk::SHRINK, Gtk::FILL | Gtk::EXPAND | Gtk::SHRINK, 0, 0);
#endif
	da.add_events(Gdk::EXPOSURE_MASK | Gdk::STRUCTURE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK);
#ifdef CRN_USING_GTKMM3
	da.signal_draw().connect(sigc::mem_fun(this, &Image::expose)); // redraw
#else /* CRN_USING_GTKMM3 */
	da.signal_expose_event().connect(sigc::mem_fun(this, &Image::expose)); // redraw
#endif /* CRN_USING_GTKMM3 */
	da.signal_configure_event().connect(sigc::mem_fun(this, &Image::configure)); // size changed
	da.signal_motion_notify_event().connect(sigc::mem_fun(this, &Image::mouse_motion)); // mouse motion
	da.signal_button_press_event().connect(sigc::mem_fun(this, &Image::button_clicked));
	da.signal_button_release_event().connect(sigc::mem_fun(this, &Image::button_clicked));
	da.signal_scroll_event().connect(sigc::mem_fun(this, &Image::mouse_wheel));
	da.show();
	//da.set_sensitive(false);
#ifdef CRN_USING_GTKMM3
	hscrollbar.set_orientation(Gtk::Orientation::ORIENTATION_HORIZONTAL);
#endif
	hscrollbar.get_adjustment()->set_lower(0);
	hscrollbar.get_adjustment()->set_step_increment(10);
	hscrollbar.get_adjustment()->set_page_increment(100);
#ifdef CRN_USING_GTKMM3
	attach(hscrollbar, 1, 2, 2, 3);
#else
	attach(hscrollbar, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL, 0, 0);
#endif
	hscrollbar.signal_value_changed().connect(sigc::mem_fun(this, &Image::scrolled));
	hscrollbar.show();
	//hscrollbar.set_sensitive(false);
#ifdef CRN_USING_GTKMM3
	vscrollbar.set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
#endif
	vscrollbar.get_adjustment()->set_lower(0);
	vscrollbar.get_adjustment()->set_step_increment(10);
	vscrollbar.get_adjustment()->set_page_increment(100);
#ifdef CRN_USING_GTKMM3
	attach(vscrollbar, 2, 3, 1, 2);
#else
	attach(vscrollbar, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL, 0, 0);
#endif
	vscrollbar.signal_value_changed().connect(sigc::mem_fun(this, &Image::scrolled));
	vscrollbar.show();
	//vscrollbar.set_sensitive(false);

	set_sensitive(false);
	refresher = Glib::signal_timeout().connect(sigc::mem_fun(this, &Image::refresh), 100);

	// actions
	/*image_actions->add(Gtk::Action::create("image-zoom-in", Gtk::Stock::ZOOM_IN), sigc::mem_fun(this, &Image::zoom_in));
	image_actions->add(Gtk::Action::create("image-zoom-out", Gtk::Stock::ZOOM_OUT), sigc::mem_fun(this, &Image::zoom_out));
	image_actions->add(Gtk::Action::create("image-zoom-100", Gtk::Stock::ZOOM_100), sigc::mem_fun(this, &Image::zoom_100));
	image_actions->add(Gtk::Action::create("image-zoom-fit", Gtk::Stock::ZOOM_FIT), sigc::mem_fun(this, &Image::zoom_fit));
	image_actions->add(Gtk::Action::create("image-clear-user-selection", Gtk::Stock::CLEAR, _("_Clear User Selection"), _("Clear User Selection")), sigc::mem_fun(this, &Image::clear_selection));
	*/
	overlays[selection_overlay()].config.moveable = true;
	overlays[selection_overlay()].config.editable = true;
}

/*! Destructor */
Image::~Image()
{
	refresher.disconnect();
}

#ifndef CRN_USING_GTKMM3
/*!
 * Are the rulers visible?
 *
 * \param[in]  is_visible  are the ruler visible?
 */
void Image::set_rulers_visible(bool is_visible)
{
	if (is_visible)
	{
		hruler.show();
		vruler.show();
	}
	else
	{
		hruler.hide();
		vruler.hide();
	}
}
#endif /* ! CRN_USING_GTKMM3 */

/*!
 * Sets the new image to display
 *
 * \param[in]  pb  the new image (nullptr to unset the image)
 */
void Image::set_pixbuf(Glib::RefPtr<Gdk::Pixbuf> pb)
{
	image = pb;
	if (!image)
	{
		set_sensitive(false);
	}
	else
	{
		pos.X = pos.Y = 0;
		hscrollbar.get_adjustment()->set_upper(image->get_width());
		vscrollbar.get_adjustment()->set_upper(image->get_height());
		image_bounds = crn::Rect(0, 0, image->get_width() - 1, image->get_height() - 1);
		need_recompute = true;
		set_sensitive(true);
	}
}

/*!
 * Sets the zoom level
 *
 * \throws	Glib::OptionError	null or negative zoom
 *
 * \param[in]  z  the new zoom level (must be > 0)
 */
void Image::set_zoom(double z)
{
	if (z <= 0.0)
		throw Glib::OptionError(Glib::OptionError::BAD_VALUE, Glib::ustring("void Image::set_zoom(double z): ") + _("null or negative zoom value."));
	zoom = z;
	need_recompute = true;
	zoom_changed.emit();
	scrolled_event.emit(int(hscrollbar.get_value()), int(vscrollbar.get_value()));
}

/*!
 * Increments the zoom level by 10%
 */
void Image::zoom_in()
{
	zoom += 0.1;
	need_recompute = true;
	zoom_changed.emit();
	scrolled_event.emit(int(hscrollbar.get_value()), int(vscrollbar.get_value()));
}

/*!
 * Decrements the zoom level by 10%
 */
void Image::zoom_out()
{
	zoom -= 0.1;
	if (zoom <= 0.0)
		zoom = 0.1;
	need_recompute = true;
	zoom_changed.emit();
	scrolled_event.emit(int(hscrollbar.get_value()), int(vscrollbar.get_value()));
}

/*!
 * Sets the zoom level to 100%
 */
void Image::zoom_100()
{
	zoom = 1.0;
	need_recompute = true;
	zoom_changed.emit();
	scrolled_event.emit(int(hscrollbar.get_value()), int(vscrollbar.get_value()));
}

/*!
 * Sets the zoom level to fit the image's size
 */
void Image::zoom_fit()
{
	if (image)
		zoom = crn::Min(double(dispw) / image_bounds.GetWidth(), double(disph) / image_bounds.GetHeight());
	else
		zoom = 1.0;
	need_recompute = true;
	zoom_changed.emit();
	scrolled_event.emit(int(hscrollbar.get_value()), int(vscrollbar.get_value()));
}

/*!
 * Focuses the image on a point. If the point is outside of the image, the focus is made on the nearest point in the image.
 *
 * \param[in]  x  the abscissa of the point to focus on
 * \param[in]  y  the ordinate of the point to focus on
 */
void Image::focus_on(int x, int y)
{
	if (image_bounds.IsValid())
	{
		if (image_bounds.GetLeft() > x) x = image_bounds.GetLeft();
		if (image_bounds.GetRight() < x) x = image_bounds.GetRight();
		if (image_bounds.GetTop() > y) y = image_bounds.GetTop();
		if (image_bounds.GetBottom() < y) y = image_bounds.GetBottom();
		pos.X = x - int(dispw / zoom / 2.0);
		hscrollbar.set_value(pos.X);
		pos.Y = y - int(disph / zoom / 2.0);
		vscrollbar.set_value(pos.Y);
		need_recompute = true;
	}
}

/*! Sets the cursor in user mouse mode
 * \param[in]	cur	the cursor to use in user mouse mode
 */
#ifdef CRN_USING_GTKMM3
void Image::set_user_cursor(const Glib::RefPtr<Gdk::Cursor> &cur)
#else /* CRN_USING_GTKMM3 */
void Image::set_user_cursor(const Gdk::Cursor &cur)
#endif /* CRN_USING_GTKMM3 */
{
	user_cursor = cur;
}

/*! Asks for a full redraw */
void Image::scrolled()
{
	need_recompute = true;
	scrolled_event.emit(int(hscrollbar.get_value()), int(vscrollbar.get_value()));
}


/*! Asks for a refresh
 * \param[in]	cc	the cairo context
 * \param[in]	ev	the Gdk event
 * \return	false
 */
#ifdef CRN_USING_GTKMM3
bool Image::expose(const Cairo::RefPtr<Cairo::Context> &cc)
#else /* CRN_USING_GTKMM3 */
bool Image::expose(GdkEventExpose *ev)
#endif /* CRN_USING_GTKMM3 */
{
	need_redraw = true;
	return false;
}

/*! Asks for a full redraw
 * \param[in]	ev	the Gdk event
 * \return	false
 */
bool Image::configure(GdkEventConfigure *ev)
{
	dispw = ev->width;
	disph = ev->height;
	need_recompute = true;
	return false;
}

/*! Handles mouse motion
 * \param[in]	ev	the Gdk event
 * \return	false
 */
bool Image::mouse_motion(GdkEventMotion* ev)
{
	// update rulers
#ifdef CRN_USING_GTKMM3
	//hruler.set_range(pos.X, pos.X + int(dispw / zoom), pos.X + int(ev->x / zoom), pos.X + int(dispw / zoom));
	//vruler.set_range(pos.Y, pos.Y + int(disph / zoom), pos.Y + int(ev->y / zoom), pos.Y + int(disph / zoom));
#else /* CRN_USING_GTKMM3 */
	hruler.set_range(pos.X, pos.X + int(dispw / zoom), pos.X + int(ev->x / zoom), pos.X + int(dispw / zoom));
	vruler.set_range(pos.Y, pos.Y + int(disph / zoom), pos.Y + int(ev->y / zoom), pos.Y + int(disph / zoom));
#endif /* CRN_USING_GTKMM3 */

	// check for a click
	switch (mouse_mode)
	{
		case MouseMode::NONE:
			{
				crn::String s1, s2;
				MouseMode mode = find_selection_at(ev->x, ev->y, s1, s2);
				set_cursor_from_mode(mode);
			}
			break;
		case MouseMode::SCROLL:
			{
				double dx = (click_ref.X - ev->x) / zoom;
				double dy = (click_ref.Y - ev->y) / zoom;
				hscrollbar.set_value(hscrollbar.get_value() + dx);
				vscrollbar.set_value(vscrollbar.get_value() + dy);
				click_ref.X = ev->x;
				click_ref.Y = ev->y;
			}
			break;
		case MouseMode::DRAW:
			switch (selection_type)
			{
				case Overlay::Rectangle:
					{
						crn::Rect newsel(
								int(crn::Min(click_ref.X, ev->x) / zoom) + pos.X,
								int(crn::Min(click_ref.Y, ev->y) / zoom) + pos.Y,
								int(crn::Max(click_ref.X, ev->x) / zoom) + pos.X,
								int(crn::Max(click_ref.Y, ev->y) / zoom) + pos.Y);
						if (!overlays[selection_overlay()].config.can_jut_out)
							newsel &= image_bounds;
						auto r = dynamic_cast<Rectangle*>(overlays[selection_overlay()].items[selection_overlay()].get());
						if (r)
							r->rect = newsel;
						else
							overlays[selection_overlay()].items[selection_overlay()].reset(new Rectangle(newsel));
						need_redraw = true;
					}
					break;
				case Overlay::Point:
					{
						int x = int(ev->x / zoom + pos.X);
						int y = int(ev->y / zoom + pos.Y);
						if (!overlays[selection_overlay()].config.can_jut_out)
						{
							x = crn::Cap(x, 0, image_bounds.GetRight());
							y = crn::Cap(y, 0, image_bounds.GetBottom());
						}
						auto pt = dynamic_cast<crn::Point2DInt*>(overlays[selection_overlay()].items[selection_overlay()].get());
						if (pt)
						{
							pt->X = x;
							pt->Y = y;
						}
						else
							overlays[selection_overlay()].items[selection_overlay()].reset(new Point(crn::Point2DInt(x, y)));
						need_redraw = true;
					}
					break;
				case Overlay::Line:
					{
						int x = int(ev->x / zoom + pos.X);
						int y = int(ev->y / zoom + pos.Y);
						if (!overlays[selection_overlay()].config.can_jut_out)
						{
							x = crn::Cap(x, 0, image_bounds.GetRight());
							y = crn::Cap(y, 0, image_bounds.GetBottom());
						}
						auto li = dynamic_cast<Line*>(overlays[selection_overlay()].items[selection_overlay()].get());
						if (li)
						{
							li->p2.X = x;
							li->p2.Y = y;
						}
						else
							overlays[selection_overlay()].items[selection_overlay()].reset(new Line(crn::Point2DInt(x,y), crn::Point2DInt(x,y)));
						need_redraw = true;
					}
					break;
			}
			break;
		case MouseMode::MOVE:
			{

				double dx = (ev->x - click_ref.X) / zoom;
				double dy = (ev->y - click_ref.Y) / zoom;
				int ox = int(dx);
				int oy = int(dy);

				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);
				std::cout<<selected_overlay<<std::endl;
				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
					{
						if (ox >= 0)
							ox = crn::Min(image_bounds.GetRight() - crn::Max(li->p1.X, li->p2.X), ox);
						else
							ox = crn::Max(-crn::Min(li->p1.X, li->p2.X), ox);
						if (oy >= 0)
							oy = crn::Min(image_bounds.GetBottom() - crn::Max(li->p1.Y, li->p2.Y), oy);
						else
							oy = crn::Max(-crn::Min(li->p1.Y, li->p2.Y), oy);
					}
					li->p1.X += ox;
					li->p1.Y += oy;
					li->p2.X += ox;
					li->p2.Y += oy;
				}


				Rectangle* rec=dynamic_cast<Rectangle*>(item.get());
				if(rec != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
					{
						if (ox >= 0)
							ox = crn::Min(image_bounds.GetRight() - crn::Max(rec->rect.GetTopLeft().X, rec->rect.GetBottomRight().X), ox);
						else
							ox = crn::Max(-crn::Min(rec->rect.GetTopLeft().X, rec->rect.GetBottomRight().X), ox);
						if (oy >= 0)
							oy = crn::Min(image_bounds.GetBottom() - crn::Max(rec->rect.GetTopLeft().Y, rec->rect.GetBottomRight().Y), oy);
						else
							oy = crn::Max(-crn::Min(rec->rect.GetTopLeft().Y, rec->rect.GetBottomRight().Y), oy);
					}
					rec->rect.SetLeft(rec->rect.GetTopLeft().X + ox);
					rec->rect.SetTop(rec->rect.GetTopLeft().Y + oy);
					rec->rect.SetRight(rec->rect.GetBottomRight().X + ox);
					rec->rect.SetBottom(rec->rect.GetBottomRight().Y + oy);
				}
				Polygon* pol=dynamic_cast<Polygon*>(item.get());
				if(pol != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
					{
						int x_maximum=pol->points[0].X;
						int y_maximum=pol->points[0].Y;
						int x_minimum=pol->points[0].X;
						int y_minimum=pol->points[0].Y;
						for (size_t nb = 0; nb < pol->points.size(); ++nb)
						{
							if(x_maximum < pol->points[nb].X)
								x_maximum = pol->points[nb].X;
							if(y_maximum < pol->points[nb].Y)
								y_maximum = pol->points[nb].Y;
							if(x_minimum > pol->points[nb].X)
								x_minimum = pol->points[nb].X;
							if(y_minimum > pol->points[nb].Y)
								y_minimum = pol->points[nb].Y;
						}
						if (ox >= 0)
							ox = crn::Min(image_bounds.GetRight() - x_maximum, ox);
						else
							ox = crn::Max(-x_minimum, ox);
						if (oy >= 0)
							oy = crn::Min(image_bounds.GetBottom() - y_maximum, oy);
						else
							oy = crn::Max(-y_minimum, oy);
					}
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						pol->points[nb].X += ox;
						pol->points[nb].Y += oy;
					}
				}

				click_ref.X = ev->x + (ox - dx) * zoom; // don't forget to add the residue to keep the cursor at the same place in the box!
				click_ref.Y = ev->y + (oy - dy) * zoom;
				need_redraw = true;

			}
			break;

			// case MouseMode::MOVE1:
		case MouseMode::MOVEPOINT:
			{
				double dx = (ev->x - click_ref.X) / zoom;
				double dy = (ev->y - click_ref.Y) / zoom;
				int ox = int(dx);
				int oy = int(dy);
				if(movePoint != nullptr)
				{

					std:: unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);
					Line* li = dynamic_cast<Line*>(item.get());
					if (li != nullptr)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
						{
							if (ox >= 0)
								ox = crn::Min(image_bounds.GetRight() - li->p1.X, ox);
							else
								ox = crn::Max(-li->p1.X, ox);
							if (oy >= 0)
								oy = crn::Min(image_bounds.GetBottom() - li->p1.Y, oy);
							else
								oy = crn::Max(-li->p1.Y, oy);
						}
						movePoint->X += int(ox * zoom);
						movePoint->Y += int(oy * zoom);
					}
					Point* po = dynamic_cast<Point*>(item.get());
					if (po != nullptr)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
						{
							if (ox >= 0)
								ox = crn::Min(image_bounds.GetRight() - po->point.X, ox);
							else
								ox = crn::Max(-po->point.X, ox);
							if (oy >= 0)
								oy = crn::Min(image_bounds.GetBottom() - po->point.Y, oy);
							else
								oy = crn::Max(-po->point.Y, oy);
						}
						movePoint->X += int(ox * zoom);
						movePoint->Y += int(oy * zoom);
					}
					Polygon* pol = dynamic_cast<Polygon*>(item.get());
					if(pol != nullptr)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
						{
							int x_maximum=pol->points[0].X;
							int y_maximum=pol->points[0].Y;
							int x_minimum=pol->points[0].X;
							int y_minimum=pol->points[0].Y;
							for (size_t nb = 0; nb < pol->points.size(); ++nb)
							{
								if(x_maximum < pol->points[nb].X)
									x_maximum = pol->points[nb].X;
								if(y_maximum < pol->points[nb].Y)
									y_maximum = pol->points[nb].Y;
								if(x_minimum > pol->points[nb].X)
									x_minimum = pol->points[nb].X;
								if(y_minimum > pol->points[nb].Y)
									y_minimum = pol->points[nb].Y;
							}
							if (ox >= 0)
								ox = crn::Min(image_bounds.GetRight() - x_maximum, ox);
							else
								ox = crn::Max(-x_minimum, ox);
							if (oy >= 0)
								oy = crn::Min(image_bounds.GetBottom() - y_maximum, oy);
							else
								oy = crn::Max(-y_minimum, oy);
						}
						movePoint->X += ox;
						movePoint->Y += oy;

					}
					if(ev->type == GDK_BUTTON_RELEASE)
						movePoint = nullptr;
				}

				click_ref.X = ev->x + (ox - dx) * zoom; // don't forget to add the residue to keep the cursor at the same place in the box!
				click_ref.Y = ev->y + (oy - dy) * zoom;
				need_redraw = true;
			}
			break;

		case MouseMode::STRETCH_LEFT:
			{
				int x = pos.X + int(ev->x / zoom);


				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{
					if (x < li->p2.X)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						li->p1.X = x;
						need_redraw = true;
					}
				}

				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{
					if (x < rec->rect.GetRight())
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						rec->rect.SetLeft(x);
						need_redraw = true;
					}
				}

				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (x < 0) x = 0;
					po->point.X = x;
					need_redraw = true;

				}

				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{
					int x_maximum=pol->points[0].X;
					int x_minimum=pol->points[0].X;
					size_t id_minimim = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if(x_maximum < pol->points[nb].X)
							x_maximum = pol->points[nb].X;
						if(x_minimum > pol->points[nb].X)
						{
							x_minimum = pol->points[nb].X;
							id_minimim = nb;
						}
					}

					if (x < x_maximum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						pol->points[id_minimim].X = x;
						need_redraw = true;
					}
				}

			}
			break;
		case MouseMode::STRETCH_RIGHT:
			{
				int x = pos.X + int(ev->x / zoom);

				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{
					if (x > li->p1.X)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						li->p2.X = x;
						need_redraw = true;
					}
				}

				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{

					if (x > rec->rect.GetLeft())
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						rec->rect.SetRight(x);//x2 = x;
						need_redraw = true;
					}
				}

				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
					po->point.X = x;
					need_redraw = true;

				}

				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{

					int x_minimum=pol->points[0].X;
					int x_maximum=pol->points[0].X;
					size_t id_maximum = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if(x_minimum > pol->points[nb].X)
							x_minimum = pol->points[nb].X;
						if(x_maximum < pol->points[nb].X)
						{
							x_maximum = pol->points[nb].X;
							id_maximum = nb;
						}
					}

					if (x > x_minimum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						pol->points[id_maximum].X = x;
						need_redraw = true;
					}
				}

			}
			break;
		case MouseMode::STRETCH_TOP:
			{
				int y = pos.Y + int(ev->y / zoom);
				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{

					if (y < li->p2.Y)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						li->p1.Y = y;
						need_redraw = true;
					}
				}

				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{

					if (y < rec->rect.GetBottom())//y2)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						rec->rect.SetTop(y);
						need_redraw = true;
					}
				}

				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (y < 0) y = 0;
					po->point.Y = y;
					need_redraw = true;

				}

				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{
					int y_maximum=pol->points[0].Y;
					int y_minimum=pol->points[0].Y;
					size_t id_minimim = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if(y_maximum < pol->points[nb].Y)
							y_maximum = pol->points[nb].Y;
						if(y_minimum > pol->points[nb].Y)
						{
							y_minimum = pol->points[nb].Y;
							id_minimim = nb;
						}
					}

					if (y < y_maximum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						pol->points[id_minimim].Y = y;
						need_redraw = true;
					}
				}

			}
			break;
		case MouseMode::STRETCH_BOTTOM:
			{
				int y = pos.Y + int(ev->y / zoom);
				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{
					if (y > li->p1.Y)//y1)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						li->p2.Y = y;
						need_redraw = true;
					}
				}

				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{
					if (y > rec->rect.GetTop())//y1)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						rec->rect.SetBottom(y);//y2 = y;
						need_redraw = true;
					}
				}

				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
					po->point.Y = y;
					need_redraw = true;

				}

				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{
					int y_minimum = pol->points[0].Y;
					int y_maximum = pol->points[0].Y;
					size_t id_maximum = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if (y_minimum > pol->points[nb].Y)
							y_minimum = pol->points[nb].Y;
						if (y_maximum < pol->points[nb].Y)
						{
							y_maximum = pol->points[nb].Y;
							id_maximum = nb;
						}
					}

					if (y > y_minimum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						pol->points[id_maximum].Y = y;
						need_redraw = true;
					}
				}

			}
			break;
		case MouseMode::STRETCH_TOP_LEFT:
			{
				int x = pos.X + int(ev->x / zoom);

				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{

					if (x < li->p2.X)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						li->p1.X = x;
						need_redraw = true;
					}
				}

				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{

					if (x < rec->rect.GetRight())//x2)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						rec->rect.SetLeft(x);//x1 = x;
						need_redraw = true;
					}
				}

				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (x < 0) x = 0;
					po->point.X = x;
					need_redraw = true;

				}

				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{
					int x_maximum=pol->points[0].X;
					int x_minimum=pol->points[0].X;
					size_t id_minimim = 0;
					for(size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if(x_maximum < pol->points[nb].X)
							x_maximum = pol->points[nb].X;
						if(x_minimum > pol->points[nb].X)
						{
							x_minimum = pol->points[nb].X;
							id_minimim = nb;
						}
					}

					if (x < x_maximum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						pol->points[id_minimim].X = x;
						need_redraw = true;
					}
				}

				int y = pos.Y + int(ev->y / zoom);

				if (li != nullptr)
				{

					if (y < li->p2.Y)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						li->p1.Y = y;
						need_redraw = true;
					}
				}

				if (rec != nullptr)
				{

					if (y < rec->rect.GetBottom())//y2)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						rec->rect.SetTop(y);//y1 = y;
						need_redraw = true;
					}
				}

				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (y < 0) y = 0;
					po->point.Y = y;
					need_redraw = true;

				}

				if (pol != nullptr)
				{

					int y_maximum = pol->points[0].Y;
					int y_minimum = pol->points[0].Y;
					size_t id_minimim = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if (y_maximum < pol->points[nb].Y)
							y_maximum = pol->points[nb].Y;
						if (y_minimum > pol->points[nb].Y)
						{
							y_minimum = pol->points[nb].Y;
							id_minimim = nb;
						}
					}
					if (y < y_maximum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						pol->points[id_minimim].Y = y;
						need_redraw = true;
					}
				}
			}
			break;
		case MouseMode::STRETCH_TOP_RIGHT:
			{
				int x = pos.X + int(ev->x / zoom);

				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{
					if (x > li->p1.X)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						li->p2.X = x;
						need_redraw = true;
					}
				}

				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{

					if (x > rec->rect.GetLeft())//x1)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						rec->rect.SetRight(x);//x2 = x;
						need_redraw = true;
					}
				}

				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
					po->point.X = x;
					need_redraw = true;

				}

				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{
					int x_minimum = pol->points[0].X;
					int x_maximum = pol->points[0].X;
					size_t id_maximum = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if (x_minimum > pol->points[nb].X)
							x_minimum = pol->points[nb].X;
						if (x_maximum < pol->points[nb].X)
						{
							x_maximum = pol->points[nb].X;
							id_maximum = nb;
						}
					}

					if (x > x_minimum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						pol->points[id_maximum].X = x;
						need_redraw = true;
					}
				}

				int y = pos.Y + int(ev->y / zoom);

				if (li != nullptr)
				{

					if (y < li->p2.Y)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						li->p1.Y = y;
						need_redraw = true;
					}
				}

				if (rec != nullptr)
				{

					if (y < rec->rect.GetBottom())//y2)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						rec->rect.SetTop(y);//y1 = y;
						need_redraw = true;
					}
				}

				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (y < 0) y = 0;
					po->point.Y= y;
					need_redraw = true;

				}

				if (pol != nullptr)
				{
					int y_maximum = pol->points[0].Y;
					int y_minimum = pol->points[0].Y;
					size_t id_minimim = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if (y_maximum < pol->points[nb].Y)
							y_maximum = pol->points[nb].Y;
						if (y_minimum > pol->points[nb].Y)
						{
							y_minimum = pol->points[nb].Y;
							id_minimim = nb;
						}
					}

					if (y < y_maximum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y < 0) y = 0;
						pol->points[id_minimim].Y = y;
						need_redraw = true;
					}
				}
			}
			break;
		case MouseMode::STRETCH_BOTTOM_LEFT:
			{
				int x = pos.X + int(ev->x / zoom);

				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{

					if (x < li->p2.X)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						li->p1.X = x;
						need_redraw = true;
					}
				}
				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{

					if (x < rec->rect.GetRight())
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						rec->rect.SetLeft(x);
						need_redraw = true;
					}
				}
				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (x < 0) x = 0;
					po->point.X = x;
					need_redraw = true;
				}
				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{
					int x_maximum = pol->points[0].X;
					int x_minimum = pol->points[0].X;
					size_t id_minimim = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if (x_maximum < pol->points[nb].X)
							x_maximum = pol->points[nb].X;
						if (x_minimum > pol->points[nb].X)
						{
							x_minimum = pol->points[nb].X;
							id_minimim = nb;
						}
					}

					if (x < x_maximum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x < 0) x = 0;
						pol->points[id_minimim].X = x;
						need_redraw = true;
					}
				}

				int y = pos.Y + int(ev->y / zoom);
				if (li != nullptr)
				{
					if (y > li->p1.Y)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						li->p2.Y = y;
						need_redraw = true;
					}
				}

				if (rec != nullptr)
				{
					if (y > rec->rect.GetTop())
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						rec->rect.SetBottom(y);
						need_redraw = true;
					}
				}

				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
					po->point.Y = y;
					need_redraw = true;

				}

				if (pol != nullptr)
				{
					int y_minimum = pol->points[0].Y;
					int y_maximum = pol->points[0].Y;
					size_t id_maximum = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if (y_minimum > pol->points[nb].Y)
							y_minimum = pol->points[nb].Y;
						if (y_maximum < pol->points[nb].Y)
						{
							y_maximum = pol->points[nb].Y;
							id_maximum = nb;
						}
					}

					if (y > y_minimum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						pol->points[id_maximum].Y = y;
						need_redraw = true;
					}
				}

			}
			break;
		case MouseMode::STRETCH_BOTTOM_RIGHT:
			{
				int x = pos.X + int(ev->x / zoom);

				std::unique_ptr<OverlayItem> &item(overlays[selected_overlay].items[selected_overlay_item]);

				Line* li = dynamic_cast<Line*>(item.get());
				if (li != nullptr)
				{
					if (x > li->p1.X)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						li->p2.X = x;
						need_redraw = true;
					}
				}

				Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
				if (rec != nullptr)
				{
					if (x > rec->rect.GetLeft())
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						rec->rect.SetRight(x);
						need_redraw = true;
					}
				}

				Point* po = dynamic_cast<Point*>(item.get());
				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
					po->point.X = x;
					need_redraw = true;

				}

				Polygon* pol = dynamic_cast<Polygon*>(item.get());
				if (pol != nullptr)
				{
					int x_minimum = pol->points[0].X;
					int x_maximum = pol->points[0].X;
					size_t id_maximum = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if(x_minimum > pol->points[nb].X)
							x_minimum = pol->points[nb].X;
						if(x_maximum < pol->points[nb].X)
						{
							x_maximum = pol->points[nb].X;
							id_maximum = nb;
						}
					}

					if (x > x_minimum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (x > image_bounds.GetRight()) x = image_bounds.GetRight();
						pol->points[id_maximum].X = x;
						need_redraw = true;
					}
				}

				int y = pos.Y + int(ev->y / zoom);
				if (li != nullptr)
				{
					if (y > li->p1.Y)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						li->p2.Y = y;
						need_redraw = true;
					}
				}

				if (rec != nullptr)
				{
					if (y > rec->rect.GetTop())
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						rec->rect.SetBottom(y);
						need_redraw = true;
					}
				}

				if (po != nullptr)
				{
					if (!overlays[selected_overlay].config.can_jut_out)
						if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
					po->point.Y = y;
					need_redraw = true;

				}

				if (pol != nullptr)
				{
					int y_minimum = pol->points[0].Y;
					int y_maximum = pol->points[0].Y;
					size_t id_maximum = 0;
					for (size_t nb = 0; nb < pol->points.size(); ++nb)
					{
						if (y_minimum > pol->points[nb].Y)
							y_minimum = pol->points[nb].Y;
						if (y_maximum < pol->points[nb].Y)
						{
							y_maximum = pol->points[nb].Y;
							id_maximum = nb;
						}
					}

					if (y > y_minimum)
					{
						if (!overlays[selected_overlay].config.can_jut_out)
							if (y > image_bounds.GetBottom()) y = image_bounds.GetBottom();
						pol->points[id_maximum].Y = y;
						need_redraw = true;
					}
				}

			}
			break;
		case MouseMode::USER:
			{
				int x = int(ev->x / zoom) + pos.X;
				int y = int(ev->y / zoom) + pos.Y;
				user_mouse.emit(x, y);
			}
			break;
	}
	return false;
}

/*! Handles mouse click
 * \param[in]	ev	the Gdk event
 * \return	false
 */
bool Image::button_clicked(GdkEventButton *ev)
{
	switch (mouse_mode)
	{
		case MouseMode::NONE:
			if (ev->type == GDK_BUTTON_PRESS)
			{
				switch (ev->button)
				{
					case 1:
						{
							// left button
							// look for a selection under the mouse pointer and use it if found
							int x = int(ev->x / zoom) + pos.X;
							int y = int(ev->y / zoom) + pos.Y;
							if (selection_type == Overlay::User)
							{
								mouse_mode = MouseMode::USER;
								user_mouse.emit(x, y);
								break;
							}
							MouseMode mode = find_selection_at(ev->x, ev->y, selected_overlay, selected_overlay_item);
							click_ref.X = ev->x;
							click_ref.Y = ev->y;
							switch (mode)
							{
								case MouseMode::NONE:
									{
										// no selection under the mouse pointer, so we start a new user selection
										if (!overlays[selection_overlay()].config.can_jut_out && ((x > image_bounds.GetRight() || (y > image_bounds.GetBottom()))))
											break; // cannot select out of the image
										switch (selection_type)
										{
											case Overlay::Rectangle:
												add_overlay_item(selection_overlay(), selection_overlay(), crn::Rect(x, y));
												mouse_mode = MouseMode::DRAW;
												selected_overlay = selection_overlay();
												selected_overlay_item = selection_overlay();
												need_redraw = true;
												break;
											case Overlay::Point:
												add_overlay_item(selection_overlay(), selection_overlay(), crn::Point2DInt(x, y));
												mouse_mode = MouseMode::DRAW;
												selected_overlay = selection_overlay();
												selected_overlay_item = selection_overlay();
												need_redraw = true;
												break;
											case Overlay::Line:
												add_overlay_item(selection_overlay(), selection_overlay(), crn::Point2DInt(x, y), crn::Point2DInt(x, y));
												mouse_mode = MouseMode::DRAW;
												selected_overlay = selection_overlay();
												selected_overlay_item = selection_overlay();
												need_redraw = true;
												break;
										}
									}
									break;
								default: // selection found, stretch or move
									mouse_mode = mode;
							}
						}
						break;
					case 2:
						// middle button: scroll
						click_ref.X = ev->x;
						click_ref.Y = ev->y;
						mouse_mode = MouseMode::SCROLL;
						break;
					case 3:
						{
							// send signal
							int realx = int(ev->x / zoom) + pos.X;
							int realy = int(ev->y / zoom) + pos.Y;
							std::vector<std::pair<crn::String, crn::String> > res;
							for (std::map<crn::String, Overlay_internal>::const_iterator lit = overlays.begin(); lit != overlays.end(); ++lit)
							{
								if (!lit->second.config.show)
									continue; // do not include overlay items that are invisible
								for (const auto &overlay : lit->second.items)
								{
									const std::unique_ptr<OverlayItem>& item(overlay.second);
									Line* li = dynamic_cast<Line*>(item.get());
									if(li != nullptr)
									{
										crn::Rect r(li->p1.X - selection_margin, li->p1.Y - selection_margin, li->p2.X + selection_margin, li->p2.Y + selection_margin);
										if (r.Contains(realx, realy))
										{
											double s = (li->p1.Y - li->p2.Y) * (realx - li->p1.X) + (li->p2.X - li->p1.X) * (realy - li->p1.Y); // dot product of p1-mouse by the orthogonal of p1-p2
											s /= sqrt(double(crn::Sqr(li->p1.X -  li->p2.X) + crn::Sqr(li->p1.Y - li->p2.Y))); // normalize
											if (crn::Abs(s) < selection_margin)
												res.push_back(std::make_pair(lit->first, overlay.first));
										}
									}

									Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
									if(rec != nullptr)
									{
										crn::Rect r=rec->rect;//()rec->x1, rec->y1, rec->x2, rec->y2);
										if (r.Contains(realx, realy))
											res.push_back(std::make_pair(lit->first, overlay.first));
									}
									Point* p = dynamic_cast<Point*>(item.get());
									if(p != nullptr)
									{

										crn::Rect r(p->point.X - selection_margin, p->point.Y - selection_margin, p->point.X + selection_margin, p->point.Y + selection_margin);
										if (r.Contains(realx, realy))
											res.push_back(std::make_pair(lit->first, overlay.first));
									}

									Polygon* poly = dynamic_cast<Polygon*>(item.get());
									if(poly != nullptr)
									{
										// if (poly->contains(crn::Point2DInt(realx, realy),zoom))
										//  res.push_back(std::make_pair(lit->first, overlay.first));
										int max_x = poly->points[0].X;
										int min_x = poly->points[0].X;
										int max_y = poly->points[0].Y;
										int min_y = poly->points[0].Y;
										for (size_t i = 1; i< poly->points.size(); ++i)
										{
											int x1 = poly->points[i].X;
											int y1 = poly->points[i].Y;
											if(min_x > x1)
												min_x = x1;
											if(max_x < x1)
												max_x = x1;
											if(min_y > y1)
												min_y = y1;
											if(max_y < y1)
												max_y = y1;
#ifdef CRN_USING_GTKMM3
											crn::Rect rec(min_x - selection_margin, min_y - selection_margin, max_x + selection_margin, max_y + selection_margin);
											if(rec.Contains(realx,realy))
												res.push_back(std::make_pair(lit->first, overlay.first));
#endif /* ! CRN_USING_GTKMM3 */
										}
#ifdef CRN_USING_GTKMM3
										crn::Rect rec(min_x - selection_margin, min_y - selection_margin, max_x + selection_margin ,max_y + selection_margin);
										if(rec.Contains(realx,realy))
											res.push_back(std::make_pair(lit->first, overlay.first));
#endif /* CRN_USING_GTKMM3 */
									}
								}
							}
							rmb_clicked.emit(ev->button, ev->time, res, realx, realy);
						}
						break;
				}
			}
			else if (ev->type == GDK_BUTTON_RELEASE)
			{
				switch (ev->button)
				{
					case 3:
						clear_selection();
						break;
				}
			}
			break;
		case MouseMode::SCROLL:
			if ((ev->type == GDK_BUTTON_RELEASE) && (ev->button == 2))
			{
				mouse_mode = MouseMode::NONE;
			}
			break;
		case MouseMode::USER:
			if ((ev->type == GDK_BUTTON_RELEASE) && (ev->button == 1))
			{
				mouse_mode = MouseMode::NONE;
			}
			break;
		default:
			if ((ev->type == GDK_BUTTON_RELEASE) && (ev->button == 1))
			{
				overlay_changed.emit(selected_overlay, selected_overlay_item, mouse_mode);
				mouse_mode = MouseMode::NONE;
			}
			break;
	}
	set_cursor_from_mode(mouse_mode);
	return FALSE;
}

/*! Scroll with mouse wheel
 * \param[in]	ev	the scroll event
 * \return	false
 */
bool Image::mouse_wheel(GdkEventScroll *ev)
{
	switch (ev->direction)
	{
		case GDK_SCROLL_UP:
			if (ev->state & GDK_CONTROL_MASK)
			{
				zoom_in();
			}
			else
			{
				pos.Y -= int(vscrollbar.get_adjustment()->get_page_increment());
				if (pos.Y < 0) pos.Y = 0;
				vscrollbar.set_value(pos.Y);
			}
			break;
		case GDK_SCROLL_DOWN:
			if (ev->state & GDK_CONTROL_MASK)
			{
				zoom_out();
			}
			else
			{
				pos.Y += int(vscrollbar.get_adjustment()->get_page_increment());
				if (pos.Y + disph / zoom > image_bounds.GetHeight())
					pos.Y = crn::Max(0, image_bounds.GetHeight() - int(disph / zoom));
				vscrollbar.set_value(pos.Y);
			}
			break;
		case GDK_SCROLL_LEFT:
			pos.X -= int(hscrollbar.get_adjustment()->get_page_increment());
			if (pos.X < 0) pos.X = 0;
			hscrollbar.set_value(pos.X);
			break;
		case GDK_SCROLL_RIGHT:
			pos.X += int(hscrollbar.get_adjustment()->get_page_increment());
			if (pos.X + dispw / zoom > image_bounds.GetWidth())
				pos.X = crn::Max(0, image_bounds.GetWidth() - int(dispw / zoom));
			hscrollbar.set_value(pos.X);
			break;
	}
	return false;
}

/*! Finds overlay under the mouse and the adequate mouse mode
 * \param[in]	mouse_x	the abscissa in screen pixels
 * \param[in]	mouse_y	the ordinate in screen pixels
 * \param[out]	overlay_id	the overlay's name for the item under the mouse
 * \param[out]	overlay_item_id	the overlay item's name
 * \return	the adequate mouse mode depending on the overlay item and the position of the cursor, or NONE if no overlay is present under the mouse
 */
Image::MouseMode Image::find_selection_at(double mouse_x, double mouse_y, crn::String &overlay_id, crn::String &overlay_item_id)
{
	// coordinates are in screen pixels
	int x = int(pos.X * zoom + mouse_x);
	int y = int(pos.Y * zoom + mouse_y);

	for (std::map<crn::String, Overlay_internal>::const_iterator lit = overlays.begin(); lit != overlays.end(); ++lit)
	{
		if (!lit->second.config.show)
			continue; // skip invisible overlays

		for (const auto &overlay : lit->second.items)
		{
			const std::unique_ptr<OverlayItem>& item(overlay.second);
			Rectangle* rec = dynamic_cast<Rectangle*>(item.get());
			if(rec != nullptr)
			{
				if (!rec->rect.IsValid())
					continue;
				int x1 = int(rec->rect.GetLeft() * zoom);
				int y1 = int(rec->rect.GetTop() * zoom);
				int x2 = int(rec->rect.GetRight() * zoom);
				int y2 = int(rec->rect.GetBottom() * zoom);
				crn::Rect r(x1 - selection_margin, y1 - selection_margin, x2 + selection_margin, y2 + selection_margin);
				if (r.Contains(x, y))
				{
					overlay_id = lit->first;
					overlay_item_id = overlay.first;
					if (lit->second.config.editable)
					{
						// bottom right?
						r = crn::Rect(crn::Max(x1, x2 - selection_margin), crn::Max(y1, y2 - selection_margin), x2 + selection_margin, y2 + selection_margin);
						if (r.Contains(x, y))
							return MouseMode::STRETCH_BOTTOM_RIGHT;
						// top left?
						r = crn::Rect(x1 - selection_margin, y1 - selection_margin, crn::Min(x2, x1 + selection_margin), crn::Min(y2, y1 + selection_margin));
						if (r.Contains(x, y))
							return MouseMode::STRETCH_TOP_LEFT;
						// bottom left?
						r = crn::Rect(x1 - selection_margin, crn::Max(y1, y2 - selection_margin), crn::Min(x2, x1 + selection_margin), y2 + selection_margin);
						if (r.Contains(x, y))
							return MouseMode::STRETCH_BOTTOM_LEFT;
						// top right?
						r = crn::Rect(crn::Max(x1, x2 - selection_margin), y1 - selection_margin, x2 + selection_margin, crn::Min(y2, y1 + selection_margin));
						if (r.Contains(x, y))
							return MouseMode::STRETCH_TOP_RIGHT;
						// right?
						r = crn::Rect(crn::Max(x1, x2 - selection_margin), y1, x2 + selection_margin, y2);
						if (r.Contains(x, y))
							return MouseMode::STRETCH_RIGHT;
						// bottom?
						r = crn::Rect(x1, crn::Max(y1, y2 - selection_margin), x2, y2 + selection_margin);
						if (r.Contains(x, y))
							return MouseMode::STRETCH_BOTTOM;;
						// left?
						r = crn::Rect(x1 - selection_margin, y1, crn::Min(x2, x1 + selection_margin), y2);
						if (r.Contains(x, y))
							return MouseMode::STRETCH_LEFT;
						// top?
						r = crn::Rect(x1, y1 - selection_margin, x2, crn::Min(y2, y1 + selection_margin));
						if (r.Contains(x, y))
							return MouseMode::STRETCH_TOP;;
					}
					if (lit->second.config.moveable)
						return MouseMode::MOVE;
					else
						continue;
				}
			}

			Point* p=dynamic_cast<Point*>(item.get());
			if(p != nullptr)
			{
				int x1 = int(p->point.X * zoom);
				int y1 = int(p->point.Y * zoom);
				crn::Rect r(x1 - selection_margin, y1 - selection_margin, x1 + selection_margin, y1 + selection_margin);
				if (r.Contains(x, y))
				{
					overlay_id = lit->first;
					overlay_item_id = overlay.first;
					movePoint = &(p->point);
					if (lit->second.config.moveable)
						return MouseMode::MOVEPOINT;
					else
						continue;
				}
			}

			Text* t=dynamic_cast<Text*>(item.get());
			if(t != nullptr)
			{
				int x1 = int(t->pos.X * zoom);
				int y1 = int(t->pos.Y * zoom);
				crn::Rect r(x1 - selection_margin, y1 - selection_margin, x1 + selection_margin, y1 + selection_margin);
				if (r.Contains(x, y))
				{
					overlay_id = lit->first;
					overlay_item_id = overlay.first;
					movePoint = &(t->pos);
					if (lit->second.config.moveable)
						return MouseMode::MOVEPOINT;
					else
						continue;
				}
			}

			Line* li = dynamic_cast<Line*>(item.get());
			if(li != nullptr)
			{
				int x1 = int(li->p1.X * zoom);
				int y1 = int(li->p1.Y * zoom);
				int x2 = int(li->p2.X * zoom);
				int y2 = int(li->p2.Y * zoom);
				crn::Rect r(crn::Min(x1, x2) - selection_margin, crn::Min(y1, y2) - selection_margin, crn::Max(x1, x2) + selection_margin, crn::Max(y1, y2) + selection_margin);
				if (r.Contains(x, y))
				{
					if (lit->second.config.editable)
					{
						// point 1?
						r = crn::Rect (x1 - selection_margin, y1 - selection_margin, x1 + selection_margin, y1 + selection_margin);
						if (r.Contains(x, y))
						{
							movePoint=&(li->p1);
							overlay_id = lit->first;
							overlay_item_id = overlay.first;
							return MouseMode::MOVEPOINT;
						}
						// point 2?
						r = crn::Rect (x2 - selection_margin, y2 - selection_margin, x2 + selection_margin, y2 + selection_margin);
						if (r.Contains(x, y))
						{
							movePoint=&(li->p2);
							overlay_id = lit->first;
							overlay_item_id = overlay.first;
							return MouseMode::MOVEPOINT;
						}
					}
					// move the line?
					double s = (y1 - y2) * (x - x1) + (x2 - x1) * (y - y1); // dot product of p1-mouse by the orthogonal of p1-p2
					s /= sqrt(double(crn::Sqr(x1 -  x2) + crn::Sqr(y1 - y2))); // normalize
					if (crn::Abs(s) < selection_margin)
					{
						overlay_id = lit->first;
						overlay_item_id = overlay.first;
						if (lit->second.config.moveable)
							return MouseMode::MOVE;
						else
							continue;
					}
				}
			}

			Polygon* poly = dynamic_cast<Polygon*>(item.get());
			if(poly != nullptr)
			{
				if (lit->second.config.editable)
				{
					for (size_t j = 0; j < poly->points.size(); ++j)
					{
						int x1 = int(poly->points[j].X * zoom);
						int y1 = int(poly->points[j].Y * zoom);

						crn::Rect r(x1 - selection_margin, y1 - selection_margin, x1 + selection_margin, y1 + selection_margin);
						if (r.Contains(x, y))
						{
							movePoint=&(poly->points[j]);
							overlay_id = lit->first;
							overlay_item_id = overlay.first;
							return MouseMode::MOVEPOINT;
						}
					}
				}
				if (lit->second.config.moveable)
					for (size_t i = 0; i < poly->points.size(); ++i)
					{
						int x1 = int(poly->points[i].X * zoom);
						int y1 = int(poly->points[i].Y * zoom);
						int x2;
						int y2;
						if(i == poly->points.size()-1)
						{
							if( !lit->second.config.closed_polygons)
								continue;
							else
							{
								x2 = int(poly->points[0].X * zoom);
								y2 = int(poly->points[0].Y * zoom);
							}
						}
						else
						{
							x2 = int(poly->points[i+1].X * zoom);
							y2 = int(poly->points[i+1].Y * zoom);
						}
						crn::Rect r(crn::Min(x1, x2) - selection_margin, crn::Min(y1, y2) - selection_margin, crn::Max(x1, x2) + selection_margin, crn::Max(y1, y2) + selection_margin);
						if (r.Contains(x, y))
						{
							double s = (y1 - y2) * (x - x1) + (x2 - x1) * (y - y1); // dot product of p1-mouse by the orthogonal of p1-p2
							s /= sqrt(double(crn::Sqr(x1 -  x2) + crn::Sqr(y1 - y2))); // normalize
							if (crn::Abs(s) < selection_margin)
							{
								overlay_id = lit->first;
								overlay_item_id = overlay.first;

								return MouseMode::MOVE;

							}
						}
					}
				else
					continue;

			}

		} // for each overlay item
	} // for each overlay
	return MouseMode::NONE;
}

/*! Updates the cursor from a mouse mode
 * \param[in]	m	the mouse mode
 */
void Image::set_cursor_from_mode(Image::MouseMode m)
{
	switch (m)
	{
		case MouseMode::SCROLL:
			da.get_window()->set_cursor(scroll_cursor);
			break;
		case MouseMode::DRAW:
			da.get_window()->set_cursor(select_cursor);
			break;
		case MouseMode::MOVE:
			da.get_window()->set_cursor(move_cursor);
			break;
			//    case MouseMode::MOVE1:
		case MouseMode::MOVEPOINT:
			da.get_window()->set_cursor(move_1_cursor);
			break;
		case MouseMode::STRETCH_LEFT:
			da.get_window()->set_cursor(drag_left_cursor);
			break;
		case MouseMode::STRETCH_BOTTOM_LEFT:
			da.get_window()->set_cursor(drag_bottom_left_cursor);
			break;
		case MouseMode::STRETCH_BOTTOM:
			da.get_window()->set_cursor(drag_bottom_cursor);
			break;
		case MouseMode::STRETCH_BOTTOM_RIGHT:
			da.get_window()->set_cursor(drag_bottom_right_cursor);
			break;
		case MouseMode::STRETCH_RIGHT:
			da.get_window()->set_cursor(drag_right_cursor);
			break;
		case MouseMode::STRETCH_TOP_RIGHT:
			da.get_window()->set_cursor(drag_top_right_cursor);
			break;
		case MouseMode::STRETCH_TOP:
			da.get_window()->set_cursor(drag_top_cursor);
			break;
		case MouseMode::STRETCH_TOP_LEFT:
			da.get_window()->set_cursor(drag_top_left_cursor);
			break;
		case MouseMode::USER:
			da.get_window()->set_cursor(user_cursor);
			break;
		case MouseMode::NONE:
		default:
			if (selection_type == Overlay::User)
				da.get_window()->set_cursor(user_cursor);
			else
				da.get_window()->set_cursor();
	}
}


/*! Default constructor */
Image::OverlayConfig::OverlayConfig():
	color1("black"),
	color2("white"),
	text_color("black"),
	show_labels(false),
	text_size(15),
	editable(false),
	moveable(false),
	can_jut_out(false),
	fill(true),
	fill_alpha(0.5),
	cross_size(15),
	draw_arrows(true),
	arrow_size(10),
	absolute_text_size(true),
	font_family("sans"),
	closed_polygons(false),
	show(true)
{ }

/*! Sets the mouse selection type
 * \throws	ExceptionInvalidArgument	Cannot set selection type to Text or Polygon
 * \param[in]	typ	the type of selection. Can be Overlay::Rectangle, Overlay::Line, Overlay::Point, Overlay::None, Overlay::User
 */
void Image::set_selection_type(Overlay typ)
{
	if (typ == Overlay::Text)
		throw crn::ExceptionInvalidArgument(_("Cannot set selection type to Text."));
	if (typ == Overlay::Polygon)
		throw crn::ExceptionInvalidArgument(_("Cannot set selection type to Polygon."));
	selection_type = typ;
	if (typ == Overlay::User)
	{
		switch (mouse_mode)
		{
			case MouseMode::MOVE:
			case MouseMode::STRETCH_LEFT:
			case MouseMode::STRETCH_RIGHT:
			case MouseMode::STRETCH_TOP:
			case MouseMode::STRETCH_BOTTOM:
			case MouseMode::STRETCH_TOP_LEFT:
			case MouseMode::STRETCH_TOP_RIGHT:
			case MouseMode::STRETCH_BOTTOM_LEFT:
			case MouseMode::STRETCH_BOTTOM_RIGHT:
			case MouseMode::DRAW:
				overlay_changed.emit(selected_overlay, selected_overlay_item, mouse_mode);
		}
		mouse_mode = MouseMode::NONE;
	}
	need_redraw = true;
}

/*! Adds a rectangle to an overlay
 * \param[in]	overlay_id	the id of the overlay in which the rectangle will be added
 * \param[in]	item_id	the id of the item to add
 * \param[in]	r	the rectangle to add
 * \param[in]	label	the label of the rectangle (optional)
 */

void Image::add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::Rect &r, const crn::StringUTF8 &label)
{
	overlays[overlay_id].items[item_id].reset(new Rectangle(r, label));
	need_redraw = true;
	movePoint = nullptr;
}

/*! Adds a point to an overlay
 * \param[in]	overlay_id	the id of the overlay in which the point will be added
 * \param[in]	item_id	the id of the item to add
 * \param[in]	point	the point to add
 * \param[in]	label	the label of the point (optional)
 */
void Image::add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::Point2DInt &point, const crn::StringUTF8 &label)
{
	overlays[overlay_id].items[item_id].reset(new Point(point,label));
	need_redraw = true;
	movePoint = nullptr;
}

/*! Adds a line to an overlay
 * \param[in]	overlay_id	the id of the overlay in which the line will be added
 * \param[in]	item_id	the id of the item to add
 * \param[in]	p1	the first point of the line
 * \param[in]	p2	the second point of the line
 * \param[in]	label	the label of the line (optional)
 */
void Image::add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::Point2DInt &p1, const crn::Point2DInt &p2, const crn::StringUTF8 &label)
{
	overlays[overlay_id].items[item_id].reset(new Line(p1,p2,label));
	need_redraw = true;
	movePoint = nullptr;
}
/*! Adds a text to an overlay
 * \param[in]	overlay_id	the id of the overlay in which the text will be added
 * \param[in]	item_id	the id of the item to add
 * \param[in]	label	the label of the point (optional)
 * \param[in]	position	the position of the text on the image
 */
void Image::add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const crn::StringUTF8 &label, const crn::Point2DInt &position)
{
	overlays[overlay_id].items[item_id].reset(new Text(position,label));
	need_redraw = true;
	movePoint = nullptr;
}

/*! Adds a polygon to an overlay
 * \param[in]	overlay_id	the id of the overlay in which the line will be added
 * \param[in]	item_id	the id of the item to add
 * \param[in]	p1	the first point of the line
 * \param[in]	p2	the second point of the line
 * \param[in]	label	the label of the line (optional)
 * \throws	ExceptionDimension	the polygon must have more than two points
 */
void Image::add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, const std::vector<crn::Point2DInt>& p, const crn::StringUTF8 &label)
{
	if(p.size() < 2)
		throw crn::ExceptionDimension(crn::StringUTF8("Image::add_overlay_item(polygon): ") + _("The polygon must have more than two points."));

	overlays[overlay_id].items[item_id].reset(new Polygon(p,label));
	need_redraw = true;
	movePoint = nullptr;
}

/*! Adds a polygon to an overlay
 * \param[in]	overlay_id	the id of the overlay in which the line will be added
 * \param[in]	item_id	the id of the item to add
 * \param[in]	p1	the first point of the line
 * \param[in]	p2	the second point of the line
 * \param[in]	label	the label of the line (optional)
 * \throws	ExceptionDimension	the polygon must have more than two points
 */
void Image::add_overlay_item(const crn::String &overlay_id, const crn::String &item_id, std::vector<crn::Point2DInt> &&p, const crn::StringUTF8 &label)
{
	if(p.size() < 2)
		throw crn::ExceptionDimension(crn::StringUTF8("Image::add_overlay_item(polygon): ") + _("The polygon must have more than two points."));

	overlays[overlay_id].items[item_id].reset(new Polygon(std::move(p),label));
	need_redraw = true;
	movePoint = nullptr;
}

/*! Gets an overlay item
 * \throws	crn::ExceptionDomain	Overlay not found
 * \throws	crn::ExceptionNotFound	Overlay item not found
 * \param[in]	overlay_id	the id of the overlay
 * \param[in]	item_id	the id of the item
 * \return	a reference to the item
 */
Image::OverlayItem& Image::get_overlay_item(const crn::String &overlay_id, const crn::String &item_id)
{
	std::map<crn::String, Overlay_internal>::iterator lit(overlays.find(overlay_id));
	if (lit == overlays.end())
		throw crn::ExceptionDomain(_("Overlay not found."));
	std::map<crn::String, std::unique_ptr<OverlayItem>>::iterator it(lit->second.items.find(item_id));
	if (it == lit->second.items.end())
		throw crn::ExceptionNotFound(_("Overlay item not found."));
	return *it->second;
}

/*! Gets an overlay item
 * \throws	crn::ExceptionDomain	Overlay not found
 * \throws	crn::ExceptionNotFound	Overlay item not found
 * \param[in]	overlay_id	the id of the overlay
 * \param[in]	item_id	the id of the item
 * \return	a const reference to the item
 */
const Image::OverlayItem& Image::get_overlay_item(const crn::String &overlay_id, const crn::String &item_id) const
{
	std::map<crn::String, Overlay_internal>::const_iterator lit(overlays.find(overlay_id));
	if (lit == overlays.end())
		throw crn::ExceptionDomain(_("Overlay not found."));
	std::map<crn::String, std::unique_ptr<OverlayItem>>::const_iterator it(lit->second.items.find(item_id));
	if (it == lit->second.items.end())
		throw crn::ExceptionNotFound(_("Overlay item not found."));
	return *it->second;
}

/*! Removes an overlay item
 * \throws	crn::ExceptionDomain	Overlay not found
 * \throws	crn::ExceptionNotFound	Overlay item not found
 * \param[in]	overlay_id	the id of the overlay
 * \param[in]	item_id	the id of the item
 */
void Image::remove_overlay_item(const crn::String &overlay_id, const crn::String &item_id)
{
	std::map<crn::String, Overlay_internal>::iterator lit(overlays.find(overlay_id));
	if (lit == overlays.end())
		throw crn::ExceptionDomain(_("Overlay not found."));
	std::map<crn::String, std::unique_ptr<OverlayItem>>::iterator it(lit->second.items.find(item_id));
	if (it == lit->second.items.end())
		throw crn::ExceptionNotFound(_("Overlay item not found."));
	lit->second.items.erase(it);

	if (selected_overlay == overlay_id)
	{
		if (selected_overlay_item == item_id)
			selected_overlay_item = "";
	}
	need_redraw = true;
	movePoint = nullptr;
}

/*! Clears all overlays */
void Image::clear_overlays()
{
	for (std::map<crn::String, Overlay_internal>::iterator it = overlays.begin(); it != overlays.end(); ++it)
		if (it->first != selection_overlay())
			clear_overlay(it->first);
	movePoint = nullptr;
}

/*! Clears an overlay
 * \param[in]	id	the id of the overlay
 */
void Image::clear_overlay(const crn::String &id)
{
	overlays[id].items.clear();
	need_redraw = true;
	if (selected_overlay == id)
	{
		selected_overlay_item = "";
	}
	overlay_changed.emit(id, "", MouseMode::NONE);
}

/*! Is there a mouse selection?
 * \return	true if there is a mouse selection, false else
 */
bool Image::has_selection() const
{
	std::map<crn::String, Overlay_internal>::const_iterator it(overlays.find(selection_overlay()));
	if (it != overlays.end())
		return !it->second.items.empty();
	else
		return false;
}

/*! Clears the mouse selection */
void Image::clear_selection()
{
	clear_overlay(selection_overlay());
}

const crn::String& Image::selection_overlay()
{
	static const crn::String s(U" 月 سِمسِم coucou");
	return s;
}

/*! Gets the mouse selection as a rectangle
 * \throws	crn::ExceptionNotFound	No selection found
 * \throws	crn::ExceptionDomain	The selection is not a rectangle
 * \return	the selection as a rectangle
 */
const crn::Rect& Image::get_selection_as_rect() const
{
	std::map<crn::String, Overlay_internal>::const_iterator lit(overlays.find(selection_overlay()));
	if (lit == overlays.end())
		throw crn::ExceptionNotFound(_("No selection found."));
	std::map<crn::String, std::unique_ptr<OverlayItem>>::const_iterator it(lit->second.items.find(selection_overlay()));
	if (it == lit->second.items.end())
		throw crn::ExceptionNotFound(_("No selection found."));
	const std::unique_ptr<OverlayItem>& item(it->second);

	Rectangle* rec=dynamic_cast<Rectangle*>(item.get());
	if (rec != nullptr)
		return rec->rect;
	else
		throw crn::ExceptionDomain(_("The selection is not a rectangle."));
}

/*! Gets the mouse selection as a point
 * \throws	crn::ExceptionNotFound	No selection found
 * \throws	crn::ExceptionDomain	The selection is not a point
 * \return	the selection as a point
 */
const crn::Point2DInt& Image::get_selection_as_point() const
{
	std::map<crn::String, Overlay_internal>::const_iterator lit(overlays.find(selection_overlay()));
	if (lit == overlays.end())
		throw crn::ExceptionNotFound(_("No selection found."));
	std::map<crn::String, std::unique_ptr<OverlayItem>>::const_iterator it(lit->second.items.find(selection_overlay()));
	if (it == lit->second.items.end())
		throw crn::ExceptionNotFound(_("No selection found."));
	const std::unique_ptr<OverlayItem>& item(it->second);

	Point* po=dynamic_cast<Point*>(item.get());
	if (po != nullptr)
		return po->point;
	else
		throw crn::ExceptionDomain(_("The selection is not a point."));
}

/*! Gets the mouse selection as a line
 * \throws	crn::ExceptionNotFound	No selection found
 * \throws	crn::ExceptionDomain	The selection is not a line
 * \return	the selection as a line
 */
std::pair<crn::Point2DInt, crn::Point2DInt> Image::get_selection_as_line() const
{
	std::map<crn::String, Overlay_internal>::const_iterator lit(overlays.find(selection_overlay()));
	if (lit == overlays.end())
		throw crn::ExceptionNotFound(_("No selection found."));
	std::map<crn::String, std::unique_ptr<OverlayItem>>::const_iterator it(lit->second.items.find(selection_overlay()));
	if (it == lit->second.items.end())
		throw crn::ExceptionNotFound(_("No selection found."));
	const std::unique_ptr<OverlayItem>& item(it->second);

	Line* li=dynamic_cast<Line*>(item.get());
	if (li != nullptr)
		return std::make_pair(li->p1, li->p2);
	else
		throw crn::ExceptionDomain(_("The selection is not a line."));
}

/*! Sets the mouse selection
 * \throws	crn::ExceptionInvalidArgument	The selection is not a rectangle
 * \param[in]	r	the new selection
 */
void Image::set_selection(const crn::Rect &r)
{
	std::unique_ptr<OverlayItem>& item( overlays[selection_overlay()].items[selection_overlay()]);

	Rectangle * rec = dynamic_cast<Rectangle*>(item.get());
	if(rec != nullptr)
		rec->rect = r;
	else
		throw crn::ExceptionInvalidArgument(_("The selection is not a rectangle."));

	overlay_changed.emit(selection_overlay(), selection_overlay(), mouse_mode);
}

/*! Sets the mouse selection
 * \throws	crn::ExceptionInvalidArgument	The selection is not a point
 * \param[in]	p	the new selection
 */
void Image::set_selection(const crn::Point2DInt &p)
{
	std::unique_ptr<OverlayItem>& item( overlays[selection_overlay()].items[selection_overlay()]);

	Point * po = dynamic_cast<Point*>(item.get());
	if(po != nullptr)
		po->point = p;
	else
		throw crn::ExceptionInvalidArgument(_("The selection is not a point."));

	overlay_changed.emit(selection_overlay(), selection_overlay(), mouse_mode);
}

/*! Sets the mouse selection
 * \throws	crn::ExceptionInvalidArgument	The selection is not a line
 * \param[in]	p1	the first point of the new selection
 * \param[in]	p2	the second point of the new selection
 */
void Image::set_selection(const crn::Point2DInt &p1, const crn::Point2DInt &p2)
{
	std::unique_ptr<OverlayItem>& item( overlays[selection_overlay()].items[selection_overlay()]);

	Line * li = dynamic_cast<Line*>(item.get());
	if(li != nullptr)
	{
		li->p1 = p1;
		li->p2 = p2;
	}
	else
		throw crn::ExceptionInvalidArgument(_("The selection is not a line."));

	overlay_changed.emit(selection_overlay(), selection_overlay(), mouse_mode);
}

/*! Shows or hides an overlay
 * \param[in]	id	the id of the overlay
 * \param[in]	visible	shall the overlay be displayed?
 */
void Image::set_overlay_visible(const crn::String &id, bool visible)
{
	overlays[id].config.show = visible;
	need_redraw = true;
}

/*! Refreshes or redraws the image
 * \return	true
 */

bool Image::refresh()
{
	if (!image)
		return true;

	Glib::RefPtr<Gdk::Window> win = da.get_window();
	if (win)
	{
		// if the drawing area is fully created
		if (need_recompute && dispw && disph)
		{
			// need to recompute the buffer

			// set the scrollbars' scale
			hscrollbar.get_adjustment()->set_page_size(dispw / zoom);
			vscrollbar.get_adjustment()->set_page_size(disph / zoom);
			pos.X = int(hscrollbar.get_value());
			pos.Y = int(vscrollbar.get_value());
			if (pos.X + dispw / zoom > image_bounds.GetWidth())
			{
				hscrollbar.set_value(crn::Max(0.0, image_bounds.GetWidth() - dispw / zoom));
				pos.X = int(hscrollbar.get_value());
			}
			if (pos.Y + disph / zoom > image_bounds.GetHeight())
			{
				vscrollbar.set_value(crn::Max(0.0, image_bounds.GetHeight() - disph / zoom));
				pos.Y = int(vscrollbar.get_value());
			}

			// compute size
			int srcw = crn::Min(int(dispw / zoom), image->get_width() - pos.X);
			int srch = crn::Min(int(disph / zoom), image->get_height() - pos.Y);
			Glib::RefPtr<Gdk::Pixbuf> crop = Gdk::Pixbuf::create_subpixbuf(image, pos.X, pos.Y, srcw, srch);
			if (!crop)
				return true;
			buffer = crop->scale_simple(int(crop->get_width() * zoom), int(crop->get_height() * zoom), Gdk::INTERP_BILINEAR);
			// allows the user to modify the image
			drawing.emit(buffer);

			// set the rulers' scale
#ifdef CRN_USING_GTKMM3
			//hruler.set_range(pos.X, pos.X + int(dispw / zoom), pos.X, pos.X + int(dispw / zoom));
			//vruler.set_range(pos.Y, pos.Y + int(disph / zoom), pos.Y, pos.Y + int(disph / zoom));
#else /* CRN_USING_GTKMM3 */
			hruler.set_range(pos.X, pos.X + int(dispw / zoom), pos.X, pos.X + int(dispw / zoom));
			vruler.set_range(pos.Y, pos.Y + int(disph / zoom), pos.Y, pos.Y + int(disph / zoom));
#endif /* CRN_USING_GTKMM3 */

			// update the GC
#ifdef CRN_USING_GTKMM3
			//da_gc = Gdk::GC::create(win);
			//da_gc->set_rgb_fg_color(Gdk::Color("white"));
#else /* CRN_USING_GTKMM3 */
			da_gc = Gdk::GC::create(win);
			da_gc->set_rgb_fg_color(Gdk::Color("white"));
#endif /* CRN_USING_GTKMM3 */

			need_recompute = false;
			need_redraw = true;
		}
		if (need_redraw)
		{
			// need to redraw the image
			// create a buffer
#ifdef CRN_USING_GTKMM3
			//Glib::RefPtr<Gdk::Pixmap> pm = Gdk::Pixmap::create(win, dispw, disph);
#else /* CRN_USING_GTKMM3 */
			Glib::RefPtr<Gdk::Pixmap> pm = Gdk::Pixmap::create(win, dispw, disph);
#endif /* CRN_USING_GTKMM3 */
			// clear the buffer
#ifdef CRN_USING_GTKMM3
			//pm->draw_rectangle(da_gc, true, 0, 0, dispw, disph);
#else /* CRN_USING_GTKMM3 */
			pm->draw_rectangle(da_gc, true, 0, 0, dispw, disph);
#endif /* CRN_USING_GTKMM3 */
			// copy the image
#ifdef CRN_USING_GTKMM3
			//pm->draw_pixbuf(da_gc, buffer, 0, 0, 0, 0, buffer->get_width(), buffer->get_height(), Gdk::RGB_DITHER_NONE, 0, 0);
#else /* CRN_USING_GTKMM3 */
			pm->draw_pixbuf(da_gc, buffer, 0, 0, 0, 0, buffer->get_width(), buffer->get_height(), Gdk::RGB_DITHER_NONE, 0, 0);
#endif /* CRN_USING_GTKMM3 */

			// now draw the rectangles!
#ifdef CRN_USING_GTKMM3
			//Cairo::RefPtr<Cairo::Context> cc = pm->create_cairo_context();
			auto cc = win->create_cairo_context();
			cc->set_source_rgb(1, 1, 1);
			cc->paint();
			Gdk::Cairo::set_source_pixbuf(cc, buffer);
			cc->paint();
#else /* CRN_USING_GTKMM3 */
			Cairo::RefPtr<Cairo::Context> cc = pm->create_cairo_context();
			if (!cc)
			{
				std::cerr << "cannot create cairo context to refresh image" << std::endl; // XXX
				return true;
			}
#endif /* CRN_USING_GTKMM3 */
			crn::Rect screen(0, 0, dispw, disph);
			if (!screen.IsValid())
				return true;

			for (std::map<crn::String, Overlay_internal>::reverse_iterator lit = overlays.rbegin(); lit != overlays.rend(); ++lit) //dynamic_cast
			{
				if (!lit->second.config.show)
					continue;
				double fontsize = lit->second.config.text_size * (lit->second.config.absolute_text_size ? 1.0 : zoom);
				Pango::FontDescription fdesc;
				fdesc.set_family(lit->second.config.font_family);
				fdesc.set_absolute_size(fontsize * Pango::SCALE);
				for (std::map<crn::String, std::unique_ptr<OverlayItem>>::const_iterator it = lit->second.items.begin(); it != lit->second.items.end(); ++it)
				{
					const std::unique_ptr<OverlayItem>& item(it->second);
					const Point* p = dynamic_cast<const Point*>(item.get());
					if(p != nullptr)
					{
						int x = int((p->point.X - pos.X) * zoom);
						int y = int((p->point.Y - pos.Y) * zoom);
						if (!screen.Contains(x, y)) // clipping
							continue;
						cc->set_source_rgb(lit->second.config.color2.get_red_p(), lit->second.config.color2.get_green_p(), lit->second.config.color2.get_blue_p());
						cc->rectangle(x - 1, y - 1, 2, 2);
						cc->stroke();
						cc->set_source_rgb(lit->second.config.color1.get_red_p(), lit->second.config.color1.get_green_p(), lit->second.config.color1.get_blue_p());
						cc->move_to(x - lit->second.config.cross_size / 2, y);
						cc->line_to(x + lit->second.config.cross_size / 2, y);
						cc->move_to(x, y - lit->second.config.cross_size / 2);
						cc->line_to(x, y + lit->second.config.cross_size / 2);
						cc->stroke();
						if (lit->second.config.show_labels && it->second->label.IsNotEmpty())
						{
							Glib::RefPtr<Pango::Layout> pl(Pango::Layout::create(cc));
							pl->set_font_description(fdesc);
							pl->set_text(p->label.Std());
							cc->move_to(x, y);
							cc->set_source_rgb(lit->second.config.text_color.get_red_p(), lit->second.config.text_color.get_green_p(), lit->second.config.text_color.get_blue_p());
							pl->show_in_cairo_context(cc);
						}
						continue;
					}
					Rectangle* rec=dynamic_cast<Rectangle*> (item.get());
					if(rec != nullptr)
					{
						crn::Rect z=rec->rect;//(rec->x1, rec->y1, rec->x2, rec->y2);
						if (!z.IsValid())
							continue;
						z.Translate(-pos.X, -pos.Y);
						z = z * zoom;
						if (!(screen & z).IsValid()) // clipping
							continue;
						cc->rectangle(z.GetLeft(), z.GetTop(), z.GetWidth(), z.GetHeight());
						cc->set_source_rgb(lit->second.config.color1.get_red_p(), lit->second.config.color1.get_green_p(), lit->second.config.color1.get_blue_p());
						if (lit->second.config.fill)
						{
							cc->stroke_preserve();
							cc->set_source_rgba(lit->second.config.color2.get_red_p(), lit->second.config.color2.get_green_p(), lit->second.config.color2.get_blue_p(), lit->second.config.fill_alpha);
							cc->fill();
						}
						else
						{
							cc->stroke();
							z.Translate(1, 1);
							cc->rectangle(z.GetLeft(), z.GetTop(), z.GetWidth(), z.GetHeight());
							cc->set_source_rgb(lit->second.config.color2.get_red_p(), lit->second.config.color2.get_green_p(), lit->second.config.color2.get_blue_p());
							cc->stroke();
						}
						if (lit->second.config.show_labels && it->second->label.IsNotEmpty())
						{
							Glib::RefPtr<Pango::Layout> pl(Pango::Layout::create(cc));
							pl->set_font_description(fdesc);
							pl->set_text(it->second->label.Std());
							cc->move_to(z.GetLeft(), z.GetTop());
							cc->set_source_rgb(lit->second.config.text_color.get_red_p(), lit->second.config.text_color.get_green_p(), lit->second.config.text_color.get_blue_p());
							pl->show_in_cairo_context(cc);
						}
						continue;
					}
					Line* li=dynamic_cast<Line*> (item.get());
					if(li != nullptr)
					{
						int x1 = int((li->p1.X - pos.X) * zoom);
						int y1 = int((li->p1.Y - pos.Y) * zoom);
						int x2 = int((li->p2.X - pos.X) * zoom);
						int y2 = int((li->p2.Y - pos.Y) * zoom);
						if (!(screen & crn::Rect(crn::Min(x1, x2), crn::Min(y1, y2), crn::Max(x1, x2), crn::Max(y1, y2))).IsValid()) // clipping
							continue;
						cc->set_source_rgb(lit->second.config.color2.get_red_p(), lit->second.config.color2.get_green_p(), lit->second.config.color2.get_blue_p());
						cc->rectangle(x1 - 1, y1 - 1, 2, 2);
						cc->rectangle(x2 - 1, y2 - 1, 2, 2);
						cc->stroke();
						cc->set_source_rgb(lit->second.config.color1.get_red_p(), lit->second.config.color1.get_green_p(), lit->second.config.color1.get_blue_p());
						cc->move_to(x1, y1);
						cc->line_to(x2, y2);
						int w = x2 - x1;
						int h = y2 - y1;
						double l = sqrt(double(w * w + h * h));
						if (lit->second.config.draw_arrows)
						{
							double l2 = l - lit->second.config.arrow_size;
							int w2 = int(w * l2 / l);
							int h2 = int(h * l2 / l);
							int dx = w - w2;
							int dy = h - h2;
							cc->move_to(x1 + w2 - dy / 2, y1 + h2 + dx /2);
							cc->line_to(x2, y2);
							cc->move_to(x1 + w2 + dy / 2, y1 + h2 - dx /2);
							cc->line_to(x2, y2);
						}
						cc->stroke();
						if (lit->second.config.show_labels && it->second->label.IsNotEmpty())
						{
							Glib::RefPtr<Pango::Layout> pl(Pango::Layout::create(cc));
							pl->set_font_description(fdesc);
							crn::Angle<crn::Degree> a(crn::Angle<crn::Degree>::Atan(h, w));
							pl->set_text(it->second->label.Std());
							pl->set_alignment(Pango::ALIGN_CENTER);
							pl->set_width(int(l));
							if ((a.value > 90) && (a.value < 270))
							{
								a.value += 180;
							}
							cc->move_to(x1 + w / 2, y1 + h / 2);
							cc->rotate_degrees(a.value);
							cc->set_source_rgb(lit->second.config.text_color.get_red_p(), lit->second.config.text_color.get_green_p(), lit->second.config.text_color.get_blue_p());
							pl->show_in_cairo_context(cc);
							cc->rotate_degrees(-a.value);
						}
						continue;
					}
					Text* te = dynamic_cast<Text*> (item.get());
					if(te != nullptr)
					{
						Glib::RefPtr<Pango::Layout> pl(Pango::Layout::create(cc));
						pl->set_font_description(fdesc);
						pl->set_text(it->second->label.Std());
						int x = int((te->pos.X - pos.X) * zoom);
						int y = int((te->pos.Y - pos.Y) * zoom);
						if (!screen.Contains(x, y)) // clipping
							continue;
						cc->move_to(x, y);
						cc->set_source_rgb(lit->second.config.text_color.get_red_p(), lit->second.config.text_color.get_green_p(), lit->second.config.text_color.get_blue_p());
						pl->show_in_cairo_context(cc);
						continue;
					}
					Polygon * po = dynamic_cast<Polygon*> (item.get());
					if(po != nullptr)
					{
						double x_centre = po->points[0].X;
						double y_centre =  po->points[0].Y;
						int  x_big =  po->points[0].X;
						int x_small = po->points[0].X;
						int y_big = po->points[0].Y;
						int y_small = po->points[0].Y;


						for (size_t i = 1; i < po->points.size(); ++ i)
						{
							x_centre += po->points[i].X;
							y_centre += po->points[i].Y;

							if(po->points[i].X > x_big)
								x_big = po->points[i].X;
							if(po->points[i].X < x_small)
								x_small = po->points[i].X;

							if(po->points[i].Y > y_big)
								y_big = po->points[i].Y;
							if(po->points[i].Y < y_small)
								y_small = po->points[i].Y;
						}

						if (!(screen & crn::Rect(int((x_small- pos.X) * zoom), int((y_small- pos.Y) * zoom), int((x_big- pos.X) * zoom), int((y_big- pos.Y) * zoom))).IsValid()) // clipping
							continue;

						cc->set_source_rgb(lit->second.config.color1.get_red_p(), lit->second.config.color1.get_green_p(), lit->second.config.color1.get_blue_p());
						cc->move_to( int(po->points[0].X - pos.X) * zoom, int( po->points[0].Y - pos.Y) * zoom);

						for (size_t i = 1; i < po->points.size(); ++ i)
						{
							int x1 = int((po->points[i].X - pos.X) * zoom);
							int y1 = int((po->points[i].Y - pos.Y) * zoom);
							cc->line_to(x1, y1);
						}


						if(lit->second.config.closed_polygons)
						{
							cc->line_to(int((po->points[0].X - pos.X) * zoom), int(( po->points[0].Y - pos.Y) * zoom));
						}
						if (lit->second.config.closed_polygons && lit->second.config.fill)
						{
							cc->stroke_preserve();
							cc->set_source_rgba(lit->second.config.color2.get_red_p(), lit->second.config.color2.get_green_p(), lit->second.config.color2.get_blue_p(), lit->second.config.fill_alpha);
							cc->fill();
						}
						else
							cc->stroke();


						for (size_t i = 0; i < po->points.size(); ++ i)
						{
							int x1 = int((po->points[i].X - pos.X) * zoom);
							int y1 = int((po->points[i].Y - pos.Y) * zoom);
							cc->set_source_rgb(lit->second.config.color2.get_red_p(), lit->second.config.color2.get_green_p(), lit->second.config.color2.get_blue_p());
							cc->rectangle(x1 - 1, y1 - 1, 2, 2);
						}
						cc->stroke();
						if (lit->second.config.show_labels && it->second->label.IsNotEmpty())
						{
							int h = int((y_big - y_small) * zoom);
							int w = int ((x_big - x_small)* zoom);

							double l = sqrt(double(w * w + h * h));

							Glib::RefPtr<Pango::Layout> pl(Pango::Layout::create(cc));
							pl->set_font_description(fdesc);
							pl->set_text(it->second->label.Std());
							//  cc->move_to(x_small*zoom, y_small*zoom);
							pl->set_alignment(Pango::ALIGN_CENTER);
							pl->set_width(int(l));

							cc->move_to((x_centre/int(po->points.size()))*zoom, (y_centre/int(po->points.size()))*zoom);
							cc->set_source_rgb(lit->second.config.text_color.get_red_p(), lit->second.config.text_color.get_green_p(), lit->second.config.text_color.get_blue_p());
							pl->show_in_cairo_context(cc);
						}
						continue;
					}
				}// for each overlay item
			} //for each overlay

			// copy pixmap to drawing area
#ifdef CRN_USING_GTKMM3
			//win->draw_drawable(da_gc, pm, 0, 0, 0, 0);
#else /* CRN_USING_GTKMM3 */
			win->draw_drawable(da_gc, pm, 0, 0, 0, 0);
#endif /* CRN_USING_GTKMM3 */

			// done!
			need_redraw = false;
		}
	}
	return true;
}

