/* Copyright 2016 CoReNum
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
 * file: GtkCRNHelpers.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNHelpers_HEADER
#define GtkCRNHelpers_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNException.h>

namespace GtkCRN
{
#ifdef CRN_USING_GTKMM3
	void set_enable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp, bool enabled);
	void enable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp);
	void disable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp);

	template<typename ACTIONMAPPTR> void set_enable_action(const ACTIONMAPPTR &grp, const Glib::ustring &action, bool enabled)
	{
		auto obj = grp->lookup_action(action);
		auto act = Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(obj);
		if (act)
			act->set_enabled(enabled);
		else
			throw crn::ExceptionNotFound(("GtkCRN::set_enable_action(): action not found: " + action).c_str());
	}

	template<typename ACTIONMAPPTR> void enable_action(const ACTIONMAPPTR &grp, const Glib::ustring &action)
	{
		set_enable_action(grp, action, true);
	}

	template<typename ACTIONMAPPTR> void disable_action(const ACTIONMAPPTR &grp, const Glib::ustring &action)
	{
		set_enable_action(grp, action, false);
	}

	template<typename ACTIONMAPPTR> bool is_toggle_action_active(const ACTIONMAPPTR &grp, const Glib::ustring &action)
	{
		auto ok = false;
		grp->lookup_action(action)->get_state(ok);
		return ok;
	}

#else
	void set_enable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp, bool enabled);
	void enable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp);
	void disable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp);

	void set_enable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action, bool enabled);
	void enable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action);
	void disable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action);

	bool is_toggle_action_active(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action);
#endif
};

#endif


