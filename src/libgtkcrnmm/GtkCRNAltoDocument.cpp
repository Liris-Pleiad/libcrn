/* Copyright 2012-2016 CoReNum, INSA-Lyon
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
 * file: GdkCRNAltoDocument.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNAltoDocument.h>

#ifndef CRN_USING_GTKMM3
using namespace GtkCRN;

AltoDocument::AltoDocument():
	doc(true, false)
{
	add(doc);
	doc.signal_view_selection_changed().connect(sigc::mem_fun(this, &AltoDocument::on_view_selection_changed));
	doc.show();
	imgwrapper.set_image(&doc.get_image());
	toolbar.set_orientation(Gtk::ORIENTATION_VERTICAL);
	toolbar.set_toolbar_style(Gtk::TOOLBAR_ICONS);
	toolbar.append(*Gtk::manage(imgwrapper.get_actions()->get_action("alto-pages")->create_tool_item()));
	toolbar.append(*Gtk::manage(imgwrapper.get_actions()->get_action("alto-spaces")->create_tool_item()));
	toolbar.append(*Gtk::manage(imgwrapper.get_actions()->get_action("alto-textblocks")->create_tool_item()));
	toolbar.append(*Gtk::manage(imgwrapper.get_actions()->get_action("alto-textlines")->create_tool_item()));
	toolbar.append(*Gtk::manage(imgwrapper.get_actions()->get_action("alto-words")->create_tool_item()));
	toolbar.append(*Gtk::manage(imgwrapper.get_actions()->get_action("alto-edit")->create_tool_item()));
	doc.get_left_panel().pack_start(toolbar, true, true, 2);
	toolbar.show_all();
}

void AltoDocument::set_alto(const crn::xml::SAltoWrapper &aw)
{
	alto = aw;
	if (alto)
		doc.set_document(alto->GetDocument());
	else
		doc.set_document(nullptr);
}

void AltoDocument::set_show_toolbar(bool show)
{
	toolbar.set_visible(show);
}

void AltoDocument::on_view_selection_changed(const crn::String last_selected_view_id, const std::vector<crn::String> selected_views_ids)
{
	if (last_selected_view_id.IsNotEmpty() && alto)
	{
		imgwrapper.set_view(alto->GetView(last_selected_view_id), false);
	}
	else
	{
		imgwrapper.unset_view(false);
	}
}

#endif
