/* Copyright 2010-2014 CoReNum, INSA-Lyon
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
 * file: GtkCRNProp3.h
 * \author Yann LEYDIER
 */

#ifndef GtkSProp3_HEADER
#define GtkSProp3_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNMath/CRNProp3.h>

namespace GtkCRN
{
	/*! \brief A widget for ternary values representation
	 *
	 * A widget that represents a ternary proposition (True, False, Unknown).
	 *
	 * Clicking on the image changes the value.
	 * 	- LMB: True -> False -> Unknown -> True
	 * 	- RMB: True -> Unknows -> False -> True
	 * \ingroup gtkcrn
	 */
	class Prop3: public Gtk::EventBox
	{
		public:
			/*! \brief Constructor */
			Prop3(Gtk::IconSize is, const crn::Prop3 &p = crn::Prop3::Unknown());
			virtual ~Prop3() override {}

			/*! \brief Sets the value */
			void set_value(const crn::Prop3 &p);
			/*! \brief Gets the value */
			const crn::Prop3& get_value() const { return prop; }

			/*! \brief Sets the icon size */
			void set_icon_size(Gtk::IconSize is);
			/*! \brief Gets the icon size */
			Gtk::IconSize get_icon_size() const { return size; }

			/*! \brief Emulates a mouse button release on the image */
			bool click(GdkEventButton *ev);

			/*! \brief Signals when the value was changed. Connect to void on_value_changed(crn::Prop3). */
			sigc::signal<void, crn::Prop3> signal_value_changed() { return value_changed; }

		private:
			/*! \brief Updates the icon */
			void set_image();

			crn::Prop3 prop; /*!< the value */
			Gtk::IconSize size; /*!< the icon size */
			Gtk::Image img; /*!< the icon */
			sigc::signal<void, crn::Prop3> value_changed; /*!< signals that the value changed*/
	};
}
#endif

