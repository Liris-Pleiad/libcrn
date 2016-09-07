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
 * file: GdkCRNAltoStyleRefList.cpp
 * \author Yann LEYDIER
 */

#ifndef CRN_USING_GTKMM3
#include <GtkCRNAltoStyleRefList.h>
#include <GtkCRNAltoTextStyleEditor.h>
#include <GtkCRNAltoParagraphStyleEditor.h>
#include <GtkCRNApp.h>
#include <CRNi18n.h>

using namespace GtkCRN;

AltoStyleRefList::AltoStyleRefList(crn::xml::Alto &a, crn::xml::Element &el):
	alto(a),
	element(el)
{
	Gtk::Button *but = Gtk::manage(new Gtk::Button(Gtk::Stock::ADD));
	but->signal_clicked().connect(sigc::mem_fun(this, &AltoStyleRefList::add_style));
	pack_start(*but, false, true, 2);
	const std::vector<crn::xml::Id> slist(GetStyleRefs(element));
	for (const crn::xml::Id &sid : slist)
	{
		styles.push_back(std::make_unique<AltoStyleButton>(alto, sid));
		pack_start(*styles.back(), false, true, 2);
		Gtk::Button *but = Gtk::manage(new Gtk::Button);
		but->set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON)));
		styles.back()->pack_start(*but, false, true, 0);
		but->signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &AltoStyleRefList::rem_style), styles.back()->get_id()));
	}
	show_all_children();
}

void AltoStyleRefList::add_style()
{
	Gtk::Dialog dial(_("Add style"), *dynamic_cast<Gtk::Window*>(get_toplevel()), true);
	dial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dial.add_button(Gtk::Stock::ADD, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	dial.set_alternative_button_order_from_array(altbut);	
	dial.set_default_response(Gtk::RESPONSE_ACCEPT);
	std::map<crn::xml::Id, std::unique_ptr<Gtk::CheckButton> > buts;
	Gtk::HBox hbox;
	dial.get_vbox()->pack_start(hbox, false, true, 2);
	Gtk::Button addtbut(_("Add text style"));
	addtbut.set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_BUTTON)));
	addtbut.signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &AltoStyleRefList::create_style), true, dial.get_vbox(), &buts));
	hbox.pack_start(addtbut, true, true, 2);
	Gtk::Button addpbut(_("Add paragraph style"));
	addpbut.set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::INDENT, Gtk::ICON_SIZE_BUTTON)));
	addpbut.signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &AltoStyleRefList::create_style), false, dial.get_vbox(), &buts));
	hbox.pack_start(addpbut, true, true, 2);
	std::vector<crn::xml::Id> allstyles(alto.GetStyles().GetTextStyles());
	const std::vector<crn::xml::Id> ps(alto.GetStyles().GetParagraphStyles());
	allstyles.insert(allstyles.end(), ps.begin(), ps.end());
	for (const crn::xml::Id &id : allstyles)
	{
		bool ignore = false;
		for (const std::unique_ptr<AltoStyleButton> &s : styles)
		{
			if (s->get_id() == id)
			{
				ignore = true;
				break;
			}
		}
		if (ignore)
			continue; // do not show styles that are already referenced by the element
		buts[id].reset(new Gtk::CheckButton);
		AltoStyleButton *sb = Gtk::manage(new AltoStyleButton(alto, id));
		sb->get_front().pack_start(*buts[id], false, true, 2);
		dial.get_vbox()->pack_start(*sb, false, true, 2);
	}
	dial.get_vbox()->show_all();
	if (dial.run() == Gtk::RESPONSE_ACCEPT)
	{
		dial.hide();
		for (std::map<crn::xml::Id, std::unique_ptr<Gtk::CheckButton> >::iterator it = buts.begin(); it != buts.end(); ++it)
		{
			if (it->second->get_active())
			{
				AddStyleRef(element, it->first);
				styles.push_back(std::make_unique<AltoStyleButton>(alto, it->first));
				pack_start(*styles.back(), false, true, 2);
				styles.back()->show();
				Gtk::Button *but = Gtk::manage(new Gtk::Button);
				but->set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::REMOVE, Gtk::ICON_SIZE_BUTTON)));
				styles.back()->pack_start(*but, false, true, 0);
				but->signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &AltoStyleRefList::rem_style), it->first));
				but->show();
			}
		}
	}
}

void AltoStyleRefList::rem_style(crn::xml::Id id)
{
	for (std::vector<std::unique_ptr<AltoStyleButton>>::iterator it = styles.begin(); it != styles.end(); ++it)
	{
		if ((*it)->get_id() == id)
		{
			remove(**it);
			styles.erase(it);
			RemoveStyleRef(element, id);
			return;
		}
	}
}

void AltoStyleRefList::create_style(bool text, Gtk::VBox *vbox, std::map<crn::xml::Id, std::unique_ptr<Gtk::CheckButton> > *buts)
{
	Gtk::Dialog dial(_("Create style"), *dynamic_cast<Gtk::Window*>(get_toplevel()), true);
	dial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dial.add_button(Gtk::Stock::ADD, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	dial.set_alternative_button_order_from_array(altbut);	
	dial.set_default_response(Gtk::RESPONSE_ACCEPT);
	Gtk::Entry ent;
	dial.get_vbox()->pack_start(ent, false, true, 2);
	ent.show();
	ent.set_activates_default();
	bool ok = false;
	bool add = false;
	while (!ok)
	{ // ask until a valid id is entered
		if (dial.run() == Gtk::RESPONSE_ACCEPT)
		{
			add = ok = alto.CheckId(ent.get_text().c_str());
			if (!ok)
				GtkCRN::App::show_message(_("This id already exists in the document."), Gtk::MESSAGE_ERROR);
		}
		else
		{
			ok = true;
			add = false;
		}
	}
	dial.hide();
	if (add)
	{ // add and edit style
		crn::xml::Id newid(ent.get_text().c_str());
		if (text)
		{
			crn::xml::Alto::Styles::Text &s(alto.GetStyles().AddTextStyle(newid, 10));
			AltoTextStyleEditor ed(s, dynamic_cast<Gtk::Window*>(get_toplevel()));
			ed.run();
		}
		else
		{
			crn::xml::Alto::Styles::Paragraph &s(alto.GetStyles().AddParagraphStyle(newid));
			AltoParagraphStyleEditor ed(s, dynamic_cast<Gtk::Window*>(get_toplevel()));
			ed.run();
		}
		// add to gui
		(*buts)[newid].reset(new Gtk::CheckButton);
		AltoStyleButton *sb = Gtk::manage(new AltoStyleButton(alto, newid));
		sb->get_front().pack_start(*(*buts)[newid], false, true, 2);
		sb->show_all();
		vbox->pack_start(*sb, false, true, 2);
	}
}

#endif

