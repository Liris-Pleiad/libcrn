/* Copyright 2011-2016 CoReNum, ENS-Lyon
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
 * file: GtkCRNConfigElement.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNConfigElement.h>
#include <GtkCRNProp3.h>
#include <CRNi18n.h>
#include <limits>

using namespace GtkCRN;

/*! Constructor
 * \param[in]	el	the configuration element to display
 * \param[in]	differ	shall the changes be applied later using apply_changes()?
 */
ConfigElement::ConfigElement(crn::ConfigElement &el, bool differ):
	value(el.GetValue()),
	tmpvalue(el.GetValue())
{
	if (!value)
		throw crn::ExceptionUninitialized(_("The element was not initialized."));
	if (differ)
		tmpvalue = crn::Clone(*value);

	if (std::dynamic_pointer_cast<crn::Int>(tmpvalue))
		typ = U"Int";
	else if (std::dynamic_pointer_cast<crn::Real>(tmpvalue))
		typ = U"Real";
	else if (std::dynamic_pointer_cast<crn::Prop3>(tmpvalue))
		typ = U"Prop3";
	else if (std::dynamic_pointer_cast<crn::String>(tmpvalue))
		typ = U"String";
	else if (std::dynamic_pointer_cast<crn::StringUTF8>(tmpvalue))
		typ = U"StringUTF8";
	else if (std::dynamic_pointer_cast<crn::Path>(tmpvalue))
		typ = U"Path";

	Gtk::Label *lab = Gtk::manage(new Gtk::Label(el.GetName().CStr()));
	lab->show();
	pack_start(*lab, false, true, 2);

	if (typ == U"Prop3")
	{
		Prop3 *p = Gtk::manage(new Prop3(Gtk::ICON_SIZE_BUTTON, el.GetValue<crn::Prop3>()));
		p->signal_value_changed().connect(sigc::mem_fun(this, &ConfigElement::on_p3_changed));
		p->show();
		pack_start(*p, true, true, 2);
	}
	else if (!el.GetAllowedValues().IsEmpty())
	{ // any Int, Real, String, StringUTF8 or Path
		Gtk::ComboBoxText *cb = Gtk::manage(new Gtk::ComboBoxText);
		const std::vector<crn::StringUTF8> values(el.GetAllowedValues<crn::StringUTF8>());
		for (const crn::StringUTF8 &val : values)
#ifdef CRN_USING_GTKMM3
			cb->append(val.CStr());
#else /* CRN_USING_GTKMM3 */
			cb->append_text(val.CStr());
#endif /* CRN_USING_GTKMM3 */
		cb->set_active_text(el.GetValue<crn::StringUTF8>().CStr());
		cb->signal_changed().connect(sigc::bind(sigc::mem_fun(this, &ConfigElement::on_combo_changed), cb));
		cb->show();
		pack_start(*cb, true, true, 2);
	}
	else if (typ == U"Int")
	{
		if (el.HasMinValue() && el.HasMaxValue())
		{ // has finite range, use a slider
			Gtk::HScale *s = Gtk::manage(new Gtk::HScale(el.GetMinValue<int>(), el.GetMaxValue<int>() + 1, 1));
			s->set_value(el.GetValue<int>());
			s->signal_value_changed().connect(sigc::bind(sigc::mem_fun(this, &ConfigElement::on_range_changed), s));
			s->show();
			pack_start(*s, true, true, 2);
		}
		else
		{ // has at least one infinite (maxed) bound, use a spin button
			Gtk::SpinButton *s = Gtk::manage(new Gtk::SpinButton);
			int m = std::numeric_limits<int>::min(), M = std::numeric_limits<int>::max();
			if (el.HasMinValue())
				m = el.GetMinValue<int>();
			if (el.HasMaxValue())
				M = el.GetMaxValue<int>();
			s->set_range(m, M);
			s->set_increments(1, 10);
			s->set_value(el.GetValue<int>());
			s->signal_value_changed().connect(sigc::bind(sigc::mem_fun(this, &ConfigElement::on_spin_changed), s));
			s->show();
			pack_start(*s, true, true, 2);
		}
	}
	else if (typ == U"Real")
	{
		if (el.HasMinValue() && el.HasMaxValue())
		{ // has finite range, use a slider
			Gtk::HScale *s = Gtk::manage(new Gtk::HScale(el.GetMinValue<double>(), el.GetMaxValue<double>() + 0.01, 0.01));
			s->set_digits(2);
			s->set_value(el.GetValue<double>());
			s->signal_value_changed().connect(sigc::bind(sigc::mem_fun(this, &ConfigElement::on_range_changed), s));
			s->show();
			pack_start(*s, true, true, 2);
		}
		else
		{ // has at least one infinite (maxed) bound, use a spin button
			Gtk::SpinButton *s = Gtk::manage(new Gtk::SpinButton(0, 2));
			double m = -std::numeric_limits<double>::max(), M = std::numeric_limits<double>::max();
			if (el.HasMinValue())
				m = el.GetMinValue<double>();
			if (el.HasMaxValue())
				M = el.GetMaxValue<double>();
			s->set_range(m, M);
			s->set_increments(0.01, 1);
			s->set_value(el.GetValue<double>());
			s->signal_value_changed().connect(sigc::bind(sigc::mem_fun(this, &ConfigElement::on_spin_changed), s));
			s->show();
			pack_start(*s, true, true, 2);
		}
	}
	else // string types
	{
		Gtk::Entry *e = Gtk::manage(new Gtk::Entry);
		e->set_text(el.GetValue<crn::StringUTF8>().CStr());
		e->signal_changed().connect(sigc::bind(sigc::mem_fun(this, &ConfigElement::on_entry_changed), e));
		e->show();
		pack_start(*e, true, true, 2);
	}
	lab = Gtk::manage(new Gtk::Label("?"));
	lab->show();
	lab->set_tooltip_text(el.GetDescription().CStr());
	pack_start(*lab, false, true, 2);
}

/*! In differ mode, validates the changes to the value */
void ConfigElement::apply_changes()
{
	if (typ == U"Int")
	{
		*std::static_pointer_cast<crn::Int>(value) = *std::static_pointer_cast<crn::Int>(tmpvalue);
	}
	else if (typ == U"Real")
	{
		*std::static_pointer_cast<crn::Real>(value) = *std::static_pointer_cast<crn::Real>(tmpvalue);
	}
	else if (typ == U"Prop3")
	{
		*std::static_pointer_cast<crn::Prop3>(value) = *std::static_pointer_cast<crn::Prop3>(tmpvalue);
	}
	else if (typ == U"String")
	{
		*std::static_pointer_cast<crn::String>(value) = *std::static_pointer_cast<crn::String>(tmpvalue);
	}
	else if (typ == U"StringUTF8")
	{
		*std::static_pointer_cast<crn::StringUTF8>(value) = *std::static_pointer_cast<crn::StringUTF8>(tmpvalue);
	}
	else if (typ == U"Path")
	{
		*std::static_pointer_cast<crn::Path>(value) = *std::static_pointer_cast<crn::Path>(tmpvalue);
	}
}

void ConfigElement::on_p3_changed(crn::Prop3 p3)
{
	*std::static_pointer_cast<crn::Prop3>(tmpvalue) = p3;
}

void ConfigElement::on_combo_changed(Gtk::ComboBoxText *combo)
{
	const crn::StringUTF8 val(combo->get_active_text().c_str());
	if (typ == U"Int")
	{
		*std::static_pointer_cast<crn::Int>(tmpvalue) = val.ToInt();
	}
	else if (typ == U"Real")
	{
		*std::static_pointer_cast<crn::Real>(tmpvalue) = val.ToDouble();
	}
	else if (typ == U"String")
	{
		*std::static_pointer_cast<crn::String>(tmpvalue) = val;
	}
	else if (typ == U"StringUTF8")
	{
		*std::static_pointer_cast<crn::StringUTF8>(tmpvalue) = val;
	}
	else if (typ == U"Path")
	{
		*std::static_pointer_cast<crn::Path>(tmpvalue) = val;
	}
}

void ConfigElement::on_range_changed(Gtk::Range *range)
{
	if (typ == U"Int")
	{
		*std::static_pointer_cast<crn::Int>(tmpvalue) = int(range->get_value());
	}
	else if (typ == U"Real")
	{
		*std::static_pointer_cast<crn::Real>(tmpvalue) = range->get_value();
	}
}

void ConfigElement::on_spin_changed(Gtk::SpinButton *spin)
{
	if (typ == U"Int")
	{
		*std::static_pointer_cast<crn::Int>(tmpvalue) = spin->get_value_as_int();
	}
	else if (typ == U"Real")
	{
		*std::static_pointer_cast<crn::Real>(tmpvalue) = spin->get_value();
	}
}

void ConfigElement::on_entry_changed(Gtk::Entry *entry)
{
	if (typ == U"String")
	{
		*std::static_pointer_cast<crn::String>(tmpvalue) = entry->get_text().c_str();
	}
	else if (typ == U"StringUTF8")
	{
		*std::static_pointer_cast<crn::StringUTF8>(tmpvalue) = entry->get_text().c_str();
	}
	else if (typ == U"Path")
	{
		*std::static_pointer_cast<crn::Path>(tmpvalue) = entry->get_text().c_str();
	}
}

