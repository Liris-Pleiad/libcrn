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

namespace GtkCRN
{
#ifdef CRN_USING_GTKMM3
	void set_enable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp, bool enabled);
	void enable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp);
	void disable_action_group(const Glib::RefPtr<Gio::SimpleActionGroup> &grp);

	void set_enable_action(const Glib::RefPtr<Gio::SimpleActionGroup> &grp, const Glib::ustring &action, bool enabled);
	void enable_action(const Glib::RefPtr<Gio::SimpleActionGroup> &grp, const Glib::ustring &action);
	void disable_action(const Glib::RefPtr<Gio::SimpleActionGroup> &grp, const Glib::ustring &action);
#else
	void set_enable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp, bool enabled);
	void enable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp);
	void disable_action_group(const Glib::RefPtr<Gtk::ActionGroup> &grp);

	void set_enable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action, bool enabled);
	void enable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action);
	void disable_action(const Glib::RefPtr<Gtk::ActionGroup> &grp, const Glib::ustring &action);
#endif
};

#endif


