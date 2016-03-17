/* Copyright 2011-2014 CoReNum, INSA-Lyon
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
 * file: GtkCRNConfigElement.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNConfigElement_HEADER
#define GtkCRNConfigElement_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNUtils/CRNConfigElement.h>

namespace GtkCRN
{
	/*! \brief Representation for a configuration element
	 * \ingroup gtkcrn
	 */
	class ConfigElement: public Gtk::HBox
	{
		public:
			/*! \brief Constructor */
			ConfigElement(crn::ConfigElement &el, bool differ = false);
			/*! \brief Destructor */
			virtual ~ConfigElement() override { }

			/*! \brief In differ mode, validates the changes to the value */
			void apply_changes();
		private:
			void on_p3_changed(crn::Prop3 p3);
			void on_combo_changed(Gtk::ComboBoxText *combo);
			void on_range_changed(Gtk::Range *range);
			void on_spin_changed(Gtk::SpinButton *spin);
			void on_entry_changed(Gtk::Entry *entry);
			crn::String typ;
			crn::SObject value, tmpvalue;
	};
}

#endif



