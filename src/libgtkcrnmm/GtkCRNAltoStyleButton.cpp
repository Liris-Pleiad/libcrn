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
 * file: GdkCRNAltoStyleButton.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNAltoStyleButton.h>
#include <CRNException.h>
#include <GtkCRNAltoTextStyleEditor.h>
#include <GtkCRNAltoParagraphStyleEditor.h>
#include <CRNi18n.h>

#ifndef CRN_USING_GTKMM3
using namespace GtkCRN;

AltoStyleButton::AltoStyleButton(crn::xml::Alto &alto, const crn::xml::Id &id)
{
	pack_start(front, false, true, 2);
	Gtk::Image *img;
	bool found = false;
	lab.set_text(id.CStr());
	try
	{
		crn::xml::Alto::Styles::Text &ts(alto.GetStyles().GetTextStyle(id));
		found = true;
		img = new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_BUTTON);
		edit.signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &AltoStyleButton::edit_text_style), ts));
		set_style(ts);
	}
	catch (...) { }
	if (!found)
	{
		try
		{
			crn::xml::Alto::Styles::Paragraph &ps(alto.GetStyles().GetParagraphStyle(id));
			found = true;
			img = new Gtk::Image(Gtk::Stock::INDENT, Gtk::ICON_SIZE_BUTTON);
			edit.signal_clicked().connect(sigc::bind(sigc::mem_fun(this, &AltoStyleButton::edit_paragraph_style), ps));
		}
		catch (...) { }
	}
	if (!found)
		throw crn::ExceptionNotFound(_("Style not found."));
	pack_start(*Gtk::manage(img), false, true, 2);
	pack_start(lab, true, true, 2);
	edit.set_image(*Gtk::manage(new Gtk::Image(Gtk::Stock::EDIT, Gtk::ICON_SIZE_BUTTON)));
	pack_start(edit, false, true, 0);
	show_all_children();
}

void AltoStyleButton::edit_text_style(crn::xml::Alto::Styles::Text &style)
{
	AltoTextStyleEditor ed(style, dynamic_cast<Gtk::Window*>(get_toplevel()));
	ed.run();
	set_style(style);
}

void AltoStyleButton::edit_paragraph_style(crn::xml::Alto::Styles::Paragraph &style)
{
	AltoParagraphStyleEditor ed(style, dynamic_cast<Gtk::Window*>(get_toplevel()));
	ed.run();
}

void AltoStyleButton::set_style(crn::xml::Alto::Styles::Text &style)
{
	Pango::FontDescription fd(lab.get_pango_context()->get_font_description());
	if (style.GetFontFamily())
		fd.set_family(style.GetFontFamily()->CStr());
	else
	{
		if (style.GetFontType())
		{
			if (style.GetFontType().Get() == crn::xml::Alto::Styles::Text::FontType::Serif)
				fd.set_family("serif");
			else
				fd.set_family("sans");
		}
		else
			fd.set_family("sans");
	}
	fd.set_size(int(style.GetFontSize() * Pango::SCALE));
	if (style.GetFontStyle())
	{
		crn::xml::Alto::Styles::Text::FontStyle s = style.GetFontStyle().Get();
		if (!!(s & crn::xml::Alto::Styles::Text::FontStyle::Italics))
			fd.set_style(Pango::STYLE_ITALIC);
		else
			fd.set_style(Pango::STYLE_NORMAL);
		if (!!(s & crn::xml::Alto::Styles::Text::FontStyle::Bold))
			fd.set_weight(Pango::WEIGHT_BOLD);
		else
			fd.set_weight(Pango::WEIGHT_NORMAL);
		if (!!(s & crn::xml::Alto::Styles::Text::FontStyle::SmallCaps))
			fd.set_variant(Pango::VARIANT_SMALL_CAPS);
		else
			fd.set_variant(Pango::VARIANT_NORMAL);
	}
	else
	{
		fd.set_style(Pango::STYLE_NORMAL);
		fd.set_weight(Pango::WEIGHT_NORMAL);
		fd.set_variant(Pango::VARIANT_NORMAL);
	}
	lab.modify_font(fd);
}

#endif

