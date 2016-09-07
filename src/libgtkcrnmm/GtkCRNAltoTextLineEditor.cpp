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
 * file: GdkCRNAltoTextLineEditor.cpp
 * \author Yann LEYDIER
 */

#ifndef CRN_USING_GTKMM3
#include <GtkCRNAltoTextLineEditor.h>
#include <GtkCRNApp.h>
#include <GtkCRNAltoStyleRefList.h>
#include <CRNi18n.h>

using namespace GtkCRN;

AltoTextLineEditor::AltoTextLineEditor(crn::xml::Alto &alto, crn::xml::AltoTextLine &l, Gtk::Window *parent):
	Gtk::Dialog(_("Text line information"), true),
	line(l),
	corrected(Gtk::ICON_SIZE_BUTTON),
	has_baseline(_("Baseline"))
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
	Gtk::Table *tab = Gtk::manage(new Gtk::Table(7, 2));
	get_vbox()->pack_start(*tab, true, true, 2);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Horizontal position"))), 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);
	crn::StringUTF8 s(line.GetHPos());
	tab->attach(*Gtk::manage(new Gtk::Label(s.CStr())), 1, 2, 0, 1, Gtk::FILL, Gtk::FILL);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Vertical position"))), 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
	s = line.GetVPos();
	tab->attach(*Gtk::manage(new Gtk::Label(s.CStr())), 1, 2, 1, 2, Gtk::FILL, Gtk::FILL);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Width"))), 0, 1, 2, 3, Gtk::FILL, Gtk::FILL);
	s = line.GetWidth();
	tab->attach(*Gtk::manage(new Gtk::Label(s.CStr())), 1, 2, 2, 3, Gtk::FILL, Gtk::FILL);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Height"))), 0, 1, 3, 4, Gtk::FILL, Gtk::FILL);
	s = line.GetHeight();
	tab->attach(*Gtk::manage(new Gtk::Label(s.CStr())), 1, 2, 3, 4, Gtk::FILL, Gtk::FILL);

	baseline.set_range(line.GetVPos(), line.GetVPos() + line.GetHeight());
	baseline.set_increments(1.0, 1.0);
	baseline.set_digits(2);
	const crn::Option<double> bl(line.GetBaseline());
	if (bl)
	{
		has_baseline.set_active(true);
		baseline.set_value(*bl);
	}
	else
	{
		has_baseline.set_active(false);
		baseline.set_value(line.GetVPos() + line.GetHeight() - 1);
	}
	has_baseline.signal_toggled().connect(sigc::mem_fun(this, &AltoTextLineEditor::on_toggle));
	tab->attach(has_baseline, 0, 1, 4, 5, Gtk::FILL, Gtk::FILL);
	tab->attach(baseline, 1, 2, 4, 5, Gtk::FILL, Gtk::FILL);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Manually corrected"))), 0, 1, 5, 6, Gtk::FILL, Gtk::FILL);
	const crn::Option<bool> cor(line.GetManuallyCorrected());
	if (cor)
	{
		if (*cor)
			corrected.set_value(crn::Prop3::True);
		else
			corrected.set_value(crn::Prop3::False);
	}
	else
		corrected.set_value(crn::Prop3::Unknown);
	tab->attach(corrected, 1, 2, 5, 6, Gtk::FILL, Gtk::FILL);

	tab->attach(*Gtk::manage(new Gtk::Label(_("Line styles"))), 0, 1, 6, 7, Gtk::FILL, Gtk::FILL);
	tab->attach(*Gtk::manage(new GtkCRN::AltoStyleRefList(alto, line)), 1, 2, 6, 7, Gtk::FILL, Gtk::FILL);

	tab->show_all();
	on_toggle();

	signal_response().connect(sigc::mem_fun(this, &AltoTextLineEditor::on_close));
}

void AltoTextLineEditor::on_toggle()
{
	baseline.set_sensitive(has_baseline.get_active());
}

void AltoTextLineEditor::on_close(int resp)
{
	hide();
	if (resp == Gtk::RESPONSE_ACCEPT)
	{
		if (has_baseline.get_active())
		{
			line.SetBaseline(baseline.get_value(), false);
		}
		else
		{
			line.UnsetBaseline();
		}
		switch (corrected.get_value().GetValue())
		{
			case TRUE:
				line.SetManuallyCorrected(true);
				break;
			case FALSE:
				line.SetManuallyCorrected(false);
				break;
			default:
				line.UnsetManuallyCorrected();
				break;
		}
	}
}

#endif

