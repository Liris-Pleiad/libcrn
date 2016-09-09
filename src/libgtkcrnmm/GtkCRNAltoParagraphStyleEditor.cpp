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
 * file: GdkCRNAltoParagraphStyleEditor.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNAltoParagraphStyleEditor.h>
#include <GtkCRNApp.h>
#include <CRNi18n.h>

#ifndef CRN_USING_GTKMM3
using namespace GtkCRN;

const crn::StringUTF8 AltoParagraphStyleEditor::aUndef(N_("Undefined"));
const crn::StringUTF8 AltoParagraphStyleEditor::aRight(N_("Right"));
const crn::StringUTF8 AltoParagraphStyleEditor::aLeft(N_("Left"));
const crn::StringUTF8 AltoParagraphStyleEditor::aCenter(N_("Center"));

AltoParagraphStyleEditor::AltoParagraphStyleEditor(const crn::xml::Alto::Styles::Paragraph &ps, Gtk::Window *parent):
	Gtk::Dialog(_("Paragraph style"), true),
	style(ps),
	tab(6, 2),
	hasleft(_("Left indentation")), 
	hasright(_("Right indentation")), 
	haslinespace(_("Line space")), 
	hasfirstindent(_("First line indentation"))
{
	if (parent)
	{
		set_transient_for(*parent);
		set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
	}
	else
	{
		Gtk::Window *mainwin = App::get_main_window();
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
	get_vbox()->pack_start(tab, false, false, 2);

	tab.attach(*Gtk::manage(new Gtk::Label(_("Id"))), 0, 1, 0, 1, Gtk::FILL, Gtk::FILL, 2, 2);
	tab.attach(*Gtk::manage(new Gtk::Label(style.GetId().CStr())), 1, 2, 0, 1, Gtk::FILL, Gtk::FILL, 2, 2);

	tab.attach(*Gtk::manage(new Gtk::Label(_("Alignment"))), 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 2, 2);
	align.append_text(_(aUndef.CStr()));
	align.append_text(_(aLeft.CStr()));
	align.append_text(_(aRight.CStr()));
	align.append_text(_(aCenter.CStr()));
	if (!style.GetAlign())
	{
		align.set_active(0);
	}
	else
	{
		switch (style.GetAlign().Get())
		{
			case crn::xml::Alto::Styles::Paragraph::Align::Left:
				align.set_active(1);
				break;
			case crn::xml::Alto::Styles::Paragraph::Align::Right:
				align.set_active(2);
				break;
			case crn::xml::Alto::Styles::Paragraph::Align::Center:
				align.set_active(3);
				break;
			default:
				align.set_active(0);
		}
	}
	tab.attach(align, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL, 2, 2);

	tab.attach(hasleft, 0, 1, 2, 3, Gtk::FILL, Gtk::FILL, 2, 2);
	hasleft.set_active(style.GetLeftIndent());
	hasleft.signal_toggled().connect(sigc::mem_fun(this, &AltoParagraphStyleEditor::on_switch));
	tab.attach(left, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL, 2, 2);
	left.set_range(-1000, 1000);
	left.set_increments(1, 10);
	left.set_digits(2);
	if (style.GetLeftIndent())
		left.set_value(style.GetLeftIndent().Get());

	tab.attach(hasright, 0, 1, 3, 4, Gtk::FILL, Gtk::FILL, 2, 2);
	hasright.set_active(style.GetRightIndent());
	hasright.signal_toggled().connect(sigc::mem_fun(this, &AltoParagraphStyleEditor::on_switch));
	tab.attach(right, 1, 2, 3, 4, Gtk::FILL, Gtk::FILL, 2, 2);
	right.set_range(-1000, 1000);
	right.set_increments(1, 10);
	right.set_digits(2);
	if (style.GetRightIndent())
		right.set_value(style.GetRightIndent().Get());

	tab.attach(haslinespace, 0, 1, 4, 5, Gtk::FILL, Gtk::FILL, 2, 2);
	haslinespace.set_active(style.GetLineSpace());
	haslinespace.signal_toggled().connect(sigc::mem_fun(this, &AltoParagraphStyleEditor::on_switch));
	tab.attach(linespace, 1, 2, 4, 5, Gtk::FILL, Gtk::FILL, 2, 2);
	linespace.set_range(-1000, 1000);
	linespace.set_increments(1, 10);
	linespace.set_digits(2);
	if (style.GetLineSpace())
		linespace.set_value(style.GetLineSpace().Get());

	tab.attach(hasfirstindent, 0, 1, 5, 6, Gtk::FILL, Gtk::FILL, 2, 2);
	hasfirstindent.set_active(style.GetFirstLineIndent());
	hasfirstindent.signal_toggled().connect(sigc::mem_fun(this, &AltoParagraphStyleEditor::on_switch));
	tab.attach(firstindent, 1, 2, 5, 6, Gtk::FILL, Gtk::FILL, 2, 2);
	firstindent.set_range(-1000, 1000);
	firstindent.set_increments(1, 10);
	firstindent.set_digits(2);
	if (style.GetFirstLineIndent())
		firstindent.set_value(style.GetFirstLineIndent().Get());

	tab.show_all();
	on_switch();

	signal_response().connect(sigc::mem_fun(this, &AltoParagraphStyleEditor::on_close));
}

void AltoParagraphStyleEditor::on_close(int resp)
{
	hide();
	if (resp == Gtk::RESPONSE_ACCEPT)
	{
		Glib::ustring al = align.get_active_text();
		if (al == _(aUndef.CStr()))
			style.UnsetAlign();
		else if (al == _(aLeft.CStr()))
			style.SetAlign(crn::xml::Alto::Styles::Paragraph::Align::Left);
		else if (al == _(aRight.CStr()))
			style.SetAlign(crn::xml::Alto::Styles::Paragraph::Align::Right);
		else if (al == _(aCenter.CStr()))
			style.SetAlign(crn::xml::Alto::Styles::Paragraph::Align::Center);
		if (hasleft.get_active())
			style.SetLeftIndent(left.get_value());
		else
			style.UnsetLeftIndent();
		if (hasright.get_active())
			style.SetRightIndent(right.get_value());
		else
			style.UnsetRightIndent();
		if (haslinespace.get_active())
			style.SetLineSpace(linespace.get_value());
		else
			style.UnsetLineSpace();
		if (hasfirstindent.get_active())
			style.SetFirstLineIndent(firstindent.get_value());
		else
			style.UnsetFirstLineIndent();
	}
}

void AltoParagraphStyleEditor::on_switch()
{
	left.set_sensitive(hasleft.get_active());
	right.set_sensitive(hasright.get_active());
	linespace.set_sensitive(haslinespace.get_active());
	firstindent.set_sensitive(hasfirstindent.get_active());
}

#endif
