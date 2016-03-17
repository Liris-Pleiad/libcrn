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
 * file: GtkCRNScaleAction.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNScaleAction.h>
#include <CRNIO/CRNIO.h>
#include <gtkmm/scalebutton.h>
#include <GtkCRNApp.h>
#include <CRNi18n.h>

using namespace GtkCRN;

/*! Default constructor */
ScaleAction::ScaleAction():
	Gtk::Action(),
#ifdef CRN_USING_GTKMM3
	adj(Gtk::Adjustment::create(50, 0, 100))
#else /* CRN_USING_GTKMM3 */
	adj(50, 0, 100)
#endif /* CRN_USING_GTKMM3 */
{
}

/*! Constructor 
 * \param[in]	name	the id of the action
 * \param[in]	stock_id	a Gtk::Stock item
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 */
ScaleAction::ScaleAction(const Glib::ustring& name, const Gtk::StockID& stock_id, const Glib::ustring& label, const Glib::ustring& tooltip):
	Gtk::Action(name, stock_id, label, tooltip),
#ifdef CRN_USING_GTKMM3
	adj(Gtk::Adjustment::create(50, 0, 100)),
#else /* CRN_USING_GTKMM3 */
	adj(50, 0, 100),
#endif /* CRN_USING_GTKMM3 */
	lab(label)
{
}

/*! Constructor 
 * \param[in]	name	the id of the action
 * \param[in]	icon_name	an icon in the default path
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 */
ScaleAction::ScaleAction(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label, const Glib::ustring& tooltip):
	Gtk::Action(name, icon_name, label, tooltip),
#ifdef CRN_USING_GTKMM3
	adj(Gtk::Adjustment::create(50, 0, 100)),
#else /* CRN_USING_GTKMM3 */
	adj(50, 0, 100),
#endif /* CRN_USING_GTKMM3 */
	lab(label)
{
}

/*! Sets the icons to display depending of the value of the scale
 * \param[in]	icons	a std::vector or std::list containing filenames of icons (that must be in the default Gtk path for icons). The first icon corresponds the lowest value, the second to the highest value, the following are distributed in ascending order.
 */
void ScaleAction::set_icons(const Glib::StringArrayHandle& icons)
{
	iconlist = icons;
}

/*! Creates a blank ScaleAction 
 * \return	a new ScaleAction
 */
Glib::RefPtr<ScaleAction> ScaleAction::create()
{
	return Glib::RefPtr<ScaleAction>(new ScaleAction());
}

/*! Creates a ScaleAction
 * \param[in]	name	the id of the action
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 * \return	a new ScaleAction
 */
Glib::RefPtr<ScaleAction> ScaleAction::create(const Glib::ustring& name, const Glib::ustring& label, const Glib::ustring& tooltip)
{
	return Glib::RefPtr<ScaleAction>(new ScaleAction(name, Gtk::StockID(), label, tooltip));
}

/*! Creates a ScaleAction
 * \param[in]	name	the id of the action
 * \param[in]	stock_id	a Gtk::Stock item
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 * \return	a new ScaleAction
 */
Glib::RefPtr<ScaleAction> ScaleAction::create(const Glib::ustring& name, const Gtk::StockID& stock_id, const Glib::ustring& label, const Glib::ustring& tooltip)
{
	return Glib::RefPtr<ScaleAction>(new ScaleAction(name, stock_id, label, tooltip));
}

/*! Creates a ScaleAction
 * \param[in]	name	the id of the action
 * \param[in]	icon_name	an icon in the default path
 * \param[in]	label	the label of the action
 * \param[in]	tooltip	the tooltip of the action
 * \return	a new ScaleAction
 */
Glib::RefPtr<ScaleAction> ScaleAction::create_with_icon_name(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label, const Glib::ustring& tooltip)
{
	return Glib::RefPtr<ScaleAction>(new ScaleAction(name, icon_name, label, tooltip));
}

Gtk::Widget* ScaleAction::create_menu_item_vfunc()
{
	Gtk::MenuItem *it = (Gtk::MenuItem*)Action::create_menu_item_vfunc();
	it->signal_activate().connect(sigc::mem_fun(this, &ScaleAction::dialog));
	return it;
}

Gtk::Widget* ScaleAction::create_tool_item_vfunc()
{
	Gtk::ToolItem *it = new Gtk::ToolItem;
	Gtk::ScaleButton *sb = Gtk::manage(new Gtk::ScaleButton(Gtk::ICON_SIZE_BUTTON, 0, 100, 10, iconlist));
	sb->set_adjustment(adj);
	sb->set_icons(iconlist);
	sb->show();
	it->add(*sb);
	return it;
}

void ScaleAction::dialog()
{
	Gtk::Dialog dial("", true);
	Gtk::Window *mainwin = App::get_main_window();
	if (mainwin)
	{
		dial.set_transient_for (*mainwin);
		dial.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	}
	else
		dial.set_position(Gtk::WIN_POS_CENTER);
	dial.set_position(Gtk::WIN_POS_CENTER);
	dial.add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_ACCEPT);
	dial.set_default_response(Gtk::RESPONSE_ACCEPT);
	Gtk::HBox hbox;
	dial.get_vbox()->pack_start(hbox, false, true, 0);
	Gtk::Label label(lab, true);
	hbox.pack_start(label, false, true, 2);
	Gtk::HScale scale(adj);
	scale.set_value_pos(Gtk::POS_LEFT);
	scale.set_size_request(200);
	hbox.pack_start(scale, true, true, 2);
	hbox.show_all();
	dial.show();
	dial.run();
}

