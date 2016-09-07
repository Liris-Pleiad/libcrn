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
 * file: GdkCRNAltoTextStyleEditor.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNAltoTextStyleEditor.h>
#include <GtkCRNApp.h>
#include <CRNi18n.h>

#ifndef CRN_USING_GTKMM3
using namespace GtkCRN;

AltoTextStyleEditor::AltoTextStyleEditor(const crn::xml::Alto::Styles::Text &ts, Gtk::Window *parent):
	Gtk::Dialog(_("Text style"), true),
	style(ts),
	tab(9, 2),
	serif(Gtk::ICON_SIZE_BUTTON), 
	fixedwidth(Gtk::ICON_SIZE_BUTTON), 
	hascolor(_("Colored")), 
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
	
	tab.attach(*Gtk::manage(new Gtk::Label(_("Font family"))), 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 2, 2);
	if (style.GetFontFamily())
		fontfamily.set_text(style.GetFontFamily()->CStr());
	tab.attach(fontfamily, 1, 2, 1, 2, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	
	tab.attach(*Gtk::manage(new Gtk::Label(_("Serif"))), 0, 1, 2, 3, Gtk::FILL, Gtk::FILL, 2, 2);
	if (style.GetFontType())
		serif.set_value(style.GetFontType().Get() == crn::xml::Alto::Styles::Text::FontType::Serif ? crn::Prop3::True : crn::Prop3::False);
	else
		serif.set_value(crn::Prop3::Unknown);
	tab.attach(serif, 1, 2, 2, 3, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);

	tab.attach(*Gtk::manage(new Gtk::Label(_("Fixed width"))), 0, 1, 3, 4, Gtk::FILL, Gtk::FILL, 2, 2);
	if (style.GetFontWidth())
		fixedwidth.set_value(style.GetFontWidth().Get() == crn::xml::Alto::Styles::Text::FontWidth::Fixed ? crn::Prop3::True : crn::Prop3::False);
	else
		fixedwidth.set_value(crn::Prop3::Unknown);
	tab.attach(fixedwidth, 1, 2, 3, 4, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	
	tab.attach(*Gtk::manage(new Gtk::Label(_("Font size"))), 0, 1, 4, 5, Gtk::FILL, Gtk::FILL, 2, 2);
	fontsize.set_range(1, 200);
	fontsize.set_increments(1, 10);
	fontsize.set_digits(0);
	fontsize.set_value(style.GetFontSize());
	tab.attach(fontsize, 1, 2, 4, 5, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);

	hascolor.set_active(style.GetFontColor());
	tab.attach(hascolor, 0, 1, 5, 6, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	hascolor.signal_toggled().connect(sigc::mem_fun(this, &AltoTextStyleEditor::on_color));
	if (style.GetFontColor())
	{
		std::string col("#");
		std::stringstream ss;
		ss << std::hex << style.GetFontColor().Get();
		fontcolor.set_color(Gdk::Color(col));
	}
	else
		fontcolor.set_sensitive(false);
	tab.attach(fontcolor, 1, 2, 5, 6, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);

	if (style.GetFontStyle())
	{
		bold.set_active(!!(style.GetFontStyle().Get() & crn::xml::Alto::Styles::Text::FontStyle::Bold));
		italics.set_active(!!(style.GetFontStyle().Get() & crn::xml::Alto::Styles::Text::FontStyle::Italics));
		subscript.set_active(!!(style.GetFontStyle().Get() & crn::xml::Alto::Styles::Text::FontStyle::Subscript));
		superscript.set_active(!!(style.GetFontStyle().Get() & crn::xml::Alto::Styles::Text::FontStyle::Superscript));
		smallcaps.set_active(!!(style.GetFontStyle().Get() & crn::xml::Alto::Styles::Text::FontStyle::SmallCaps));
		underline.set_active(!!(style.GetFontStyle().Get() & crn::xml::Alto::Styles::Text::FontStyle::Underline));
	}
	tab.attach(bold, 0, 1, 6, 7, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab.attach(italics, 1, 2, 6, 7, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab.attach(subscript, 0, 1, 7, 8, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab.attach(superscript, 1, 2, 7, 8, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab.attach(smallcaps, 0, 1, 8, 9, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab.attach(underline, 1, 2, 8, 9, Gtk::EXPAND|Gtk::FILL, Gtk::FILL, 2, 2);
	tab.show_all();

	signal_response().connect(sigc::mem_fun(this, &AltoTextStyleEditor::on_close));
}

void AltoTextStyleEditor::on_color()
{
	fontcolor.set_sensitive(hascolor.get_active());
}

void AltoTextStyleEditor::on_close(int resp)
{
	hide();
	if (resp == Gtk::RESPONSE_ACCEPT)
	{
		if (fontfamily.get_text().empty())
			style.UnsetFontFamily();
		else
			style.SetFontFamily(fontfamily.get_text().c_str());

		if (serif.get_value().IsUnknown())
			style.UnsetFontType();
		else if (serif.get_value().IsTrue())
			style.SetFontType(crn::xml::Alto::Styles::Text::FontType::Serif);
		else
			style.SetFontType(crn::xml::Alto::Styles::Text::FontType::SansSerif);

		if (fixedwidth.get_value().IsUnknown())
			style.UnsetFontWidth();
		else if (fixedwidth.get_value().IsTrue())
			style.SetFontWidth(crn::xml::Alto::Styles::Text::FontWidth::Fixed);
		else
			style.SetFontWidth(crn::xml::Alto::Styles::Text::FontWidth::Proportional);

		style.SetFontSize(fontsize.get_value());

		if (hascolor.get_active())
		{
			Gdk::Color col(fontcolor.get_color());
			uint32_t cval = 0;
			cval += col.get_red() / 256;
			cval <<= 8;
			cval += col.get_green() / 256;
			cval <<= 8;
			cval += col.get_blue() / 256;
			style.SetFontColor(cval);
		}
		else
			style.UnsetFontColor();

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
			style.UnsetFontStyle();
		else
			style.SetFontStyle(fs);
	}
}

#endif

