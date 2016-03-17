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
 * file: GtkCRNColorAction.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNColorAction.h>
#include <CRNIO/CRNIO.h>
#include <GtkCRNApp.h>
#include <gtkmm/scalebutton.h>
#include <CRNi18n.h>

using namespace GtkCRN;

/*! Default constructor */
ColorAction::ColorAction()
	:Gtk::Action()
{
}

/*! Constructor 
 * \param[in]	name	the id of the action
 * \param[in]	stock_id	a Gtk::Stock item
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 */
ColorAction::ColorAction(const Glib::ustring& name, const Gtk::StockID& stock_id, const Glib::ustring& label, const Glib::ustring& tooltip):
	Gtk::Action(name, stock_id, label, tooltip){
}

/*! Constructor 
 * \param[in]	name	the id of the action
 * \param[in]	icon_name	an icon in the default path
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 */
ColorAction::ColorAction(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label, const Glib::ustring& tooltip):
	Gtk::Action(name, icon_name, label, tooltip){
}

/*! Creates a blank ColorAction 
 * \return	a new ColorAction
 */
Glib::RefPtr<ColorAction> ColorAction::create()
{
	return Glib::RefPtr<ColorAction>(new ColorAction());
}

/*! Creates a ColorAction
 * \param[in]	name	the id of the action
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 * \return	a new ColorAction
 */
Glib::RefPtr<ColorAction> ColorAction::create(const Glib::ustring& name, const Glib::ustring& label, const Glib::ustring& tooltip)
{
	return Glib::RefPtr<ColorAction>(new ColorAction(name, Gtk::StockID(), label, tooltip));
}

/*! Creates a ColorAction
 * \param[in]	name	the id of the action
 * \param[in]	stock_id	a Gtk::Stock item
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 * \return	a new ColorAction
 */
Glib::RefPtr<ColorAction> ColorAction::create(const Glib::ustring& name, const Gtk::StockID& stock_id, const Glib::ustring& label, const Glib::ustring& tooltip)
{
	return Glib::RefPtr<ColorAction>(new ColorAction(name, stock_id, label, tooltip));
}

/*! Creates a ColorAction
 * \param[in]	name	the id of the action
 * \param[in]	icon_name	an icon in the default path
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 * \return	a new ColorAction
 */
Glib::RefPtr<ColorAction> ColorAction::create_with_icon_name(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label, const Glib::ustring& tooltip)
{
	return Glib::RefPtr<ColorAction>(new ColorAction(name, icon_name, label, tooltip));
}

/*! Customizes menu items 
 * \return	a menu item
 */
Gtk::Widget* ColorAction::create_menu_item_vfunc()
{
	Gtk::MenuItem *it = new Gtk::ImageMenuItem;
	it->signal_activate().connect(sigc::mem_fun(this, &ColorAction::dialog));
	update_color.connect(sigc::bind(sigc::ptr_fun(&ColorAction::change_menu_color), it));
	change_menu_color(color, it);
	return it;
}

/*! Customizes tool items 
 * \return	a tool item
 */
Gtk::Widget* ColorAction::create_tool_item_vfunc()
{
	Gtk::ToolItem *it = new Gtk::ToolItem;
	Gtk::ColorButton *cb = Gtk::manage(new Gtk::ColorButton(color));
	cb->show();
	cb->signal_color_set().connect(sigc::bind(sigc::mem_fun(this, &ColorAction::on_button), cb));
#ifdef CRN_USING_GTKMM3
	update_color.connect(sigc::mem_fun(cb, &Gtk::ColorButton::set_rgba));
#else /* CRN_USING_GTKMM3 */
	update_color.connect(sigc::mem_fun(cb, &Gtk::ColorButton::set_color));
#endif /* CRN_USING_GTKMM3 */
	it->add(*cb);
	return it;
}

/*! Asks for a color */
void ColorAction::dialog()
{
#ifdef CRN_USING_GTKMM3
	Gtk::ColorChooserDialog dial;
#else /* CRN_USING_GTKMM3 */
	Gtk::ColorSelectionDialog dial;
#endif /* CRN_USING_GTKMM3 */
	Gtk::Window *mainwin = App::get_main_window();
	if (mainwin)
	{
		dial.set_transient_for (*mainwin);
		dial.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	}
	else
	{
		dial.set_position(Gtk::WIN_POS_CENTER);
	}
	dial.set_modal(true);	
#ifdef CRN_USING_GTKMM3
	dial.set_rgba(color);
#else /* CRN_USING_GTKMM3 */
	dial.get_color_selection()->set_current_color(color);
#endif /* CRN_USING_GTKMM3 */
	dial.show();
	if (dial.run() == Gtk::RESPONSE_OK)
	{
#ifdef CRN_USING_GTKMM3
		set_color(dial.get_rgba());
#else /* CRN_USING_GTKMM3 */
		set_color(dial.get_color_selection()->get_current_color());
#endif /* CRN_USING_GTKMM3 */
	}
}

/*! Button click 
 * \param[in]	but	the button that was clicked
 */
void ColorAction::on_button(Gtk::ColorButton* but)
{
#ifdef CRN_USING_GTKMM3
	set_color(but->get_rgba());
#else /* CRN_USING_GTKMM3 */
	set_color(but->get_color());
#endif /* CRN_USING_GTKMM3 */
}

/*! Customizes a menu item
 * \param[in]	col	the new color
 * \param[in]	mit	the menu item to customize
 */
#ifdef CRN_USING_GTKMM3
void ColorAction::change_menu_color(const Gdk::RGBA &col, Gtk::MenuItem* mit)
#else /* CRN_USING_GTKMM3 */
void ColorAction::change_menu_color(const Gdk::Color &col, Gtk::MenuItem* mit)
#endif /* CRN_USING_GTKMM3 */
{
	Gtk::ImageMenuItem *mi = dynamic_cast<Gtk::ImageMenuItem*>(mit);
	if (mi)
	{
		int w, h;
		Gtk::IconSize::lookup(Gtk::ICON_SIZE_MENU, w, h);
		Gtk::DrawingArea *da = Gtk::manage(new Gtk::DrawingArea);
#ifdef CRN_USING_GTKMM3
		da->override_background_color(col, Gtk::STATE_FLAG_NORMAL);
#else /* CRN_USING_GTKMM3 */
		da->modify_bg(Gtk::STATE_NORMAL, col);
#endif /* CRN_USING_GTKMM3 */
		da->set_size_request(w, h);
		da->show();
		mi->set_image(*da);
	}
}

/*! Sets the selected color 
 * \param[in]	col the new color
 */
#ifdef CRN_USING_GTKMM3
void ColorAction::set_color(const Gdk::RGBA &col)
#else /* CRN_USING_GTKMM3 */
void ColorAction::set_color(const Gdk::Color &col)
#endif /* CRN_USING_GTKMM3 */
{
	color = col;
	update_color.emit(color);
	changed.emit();
}
