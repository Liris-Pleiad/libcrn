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
 * file: GdkCRNAltoWordEditor.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNAltoWordEditor.h>
#include <GtkCRNApp.h>
#include <GtkCRNAltoStyleRefList.h>
#include <CRNi18n.h>

#ifndef CRN_USING_GTKMM3
using namespace GtkCRN;

AltoWordEditor::AltoWordEditor(crn::xml::Alto &alto, crn::xml::AltoWord &w, Gtk::Window *parent):
	Gtk::Dialog(_("Word information"), true),
	word(w),
	haswc(_("Word confidence")),
	bold(_("Bold")), 
	italics(_("Italics")), 
	subscript(_("Subscript")), 
	superscript(_("Superscript")), 
	smallcaps(_("Small caps")), 
	underline(_("Underlined"))
{
	if (parent)
	{
		set_transient_for(*parent);
		set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	}
	else
	{
		Gtk::Window *mainwin = GtkCRN::App::get_main_window();
		if (mainwin)
		{
			set_transient_for(*mainwin);
			set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
		}
		else
			set_position(Gtk::WIN_POS_CENTER);
	}
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	set_alternative_button_order_from_array(altbut);	
	set_default_response(Gtk::RESPONSE_ACCEPT);
	Gtk::Table *tab = Gtk::manage(new Gtk::Table(6, 2));
	get_vbox()->pack_start(*tab, true, true, 2);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Content"))), 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);
	text.set_text(w.GetContent().CStr());
	text.set_activates_default();
	tab->attach(text, 1, 2, 0, 1, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	tab->attach(haswc, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
	haswc.set_active(w.GetWC());
	haswc.signal_toggled().connect(sigc::mem_fun(this, &AltoWordEditor::on_toggle));
	wc.set_digits(2);
	wc.set_range(0.0, 1.0);
	wc.set_increments(0.1, 0.1);
	if (w.GetWC())
		wc.set_value(w.GetWC().Get());
	tab->attach(wc, 1, 2, 1, 2, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	if (w.GetFontStyle())
	{
		crn::xml::Alto::Styles::Text::FontStyle s = w.GetFontStyle().Get();
		bold.set_active(!!(s & crn::xml::Alto::Styles::Text::FontStyle::Bold));
		italics.set_active(!!(s & crn::xml::Alto::Styles::Text::FontStyle::Italics));
		subscript.set_active(!!(s & crn::xml::Alto::Styles::Text::FontStyle::Subscript));
		superscript.set_active(!!(s & crn::xml::Alto::Styles::Text::FontStyle::Superscript));
		smallcaps.set_active(!!(s & crn::xml::Alto::Styles::Text::FontStyle::SmallCaps));
		underline.set_active(!!(s & crn::xml::Alto::Styles::Text::FontStyle::Underline));
	}
	tab->attach(bold, 0, 1, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab->attach(italics, 1, 2, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab->attach(subscript, 0, 1, 3, 4, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab->attach(superscript, 1, 2, 3, 4, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab->attach(smallcaps, 0, 1, 4, 5, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab->attach(underline, 1, 2, 4, 5, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Word styles"))), 0, 1, 5, 6, Gtk::FILL, Gtk::FILL);
	tab->attach(*Gtk::manage(new GtkCRN::AltoStyleRefList(alto, w)), 1, 2, 5, 6, Gtk::FILL, Gtk::FILL);

	tab->show_all();
	on_toggle();

	signal_response().connect(sigc::mem_fun(this, &AltoWordEditor::on_close));
}

void AltoWordEditor::on_toggle()
{
	wc.set_sensitive(haswc.get_active());
}

void AltoWordEditor::on_close(int resp)
{
	hide();
	if (resp == Gtk::RESPONSE_ACCEPT)
	{
		word.SetContent(text.get_text().c_str());
		if (haswc.get_active())
			word.SetWC(wc.get_value());
		else
			word.UnsetWC();

		crn::xml::Alto::Styles::Text::FontStyle fs = crn::xml::Alto::Styles::Text::FontStyle::Undef;
		if (bold.get_active())
			fs |= crn::xml::Alto::Styles::Text::FontStyle::Bold;
		if (italics.get_active())
			fs |= crn::xml::Alto::Styles::Text::FontStyle::Italics;
		if (subscript.get_active())
			fs |= crn::xml::Alto::Styles::Text::FontStyle::Subscript;
		if (superscript.get_active())
			fs |= crn::xml::Alto::Styles::Text::FontStyle::Superscript;
		if (smallcaps.get_active())
			fs |= crn::xml::Alto::Styles::Text::FontStyle::SmallCaps;
		if (underline.get_active())
			fs |= crn::xml::Alto::Styles::Text::FontStyle::Underline;
		if (!fs)
			word.UnsetFontStyle();
		else
			word.SetFontStyle(fs);

	}
}

#endif

