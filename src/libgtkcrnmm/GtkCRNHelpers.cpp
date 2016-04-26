/* Copyright 2016 CoReNum
 * 
 * This file is part of libcrn.
 * 
 * libcrn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libcrn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcrn.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * file: GtkCRNHelpers.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNHelpers.h>
#include <CRNStringUTF8.h>
#include <CRNi18n.h>

using namespace GtkCRN;

#ifdef CRN_USING_GTKMM3
void GtkCRN::set_enable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp, bool enabled)
{
	const auto actions = grp->list_actions();
	for (const auto &act : actions)
		set_enable_action(grp, act, enabled);
}

void GtkCRN::enable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp)
{
	set_enable_action_group(grp, true);
}

void GtkCRN::disable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp)
{
	set_enable_action_group(grp, false);
}
#else
void GtkCRN::set_enable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp, bool enabled)
{
	grp->set_sensitive(enabled);
}

void GtkCRN::enable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp)
{
	set_enable_action_group(grp, true);
}

void GtkCRN::disable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp)
{
	set_enable_action_group(grp, false);
}

void GtkCRN::set_enable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action, bool enabled)
{
	grp->get_action(action)->set_sensitive(enabled);
}

void GtkCRN::enable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action)
{
	set_enable_action(grp, action, true);
}

void GtkCRN::disable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action)
{
	set_enable_action(grp, action, false);
}

bool GtkCRN::is_toggle_action_active(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action)
{
	auto act = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(grp->get_action(action));
	if (!act)
		throw crn::ExceptionRuntime(crn::StringUTF8("GtkCRN::is_toggle_action_active(): ") + _("not a ToggleAction."));
	return act->get_active();
}

#endif

