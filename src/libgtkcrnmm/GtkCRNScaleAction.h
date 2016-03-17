/* Copyright 2010-2016 CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: GtkCRNScaleAction.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNScaleAction_HEADER
#define GtkCRNScaleAction_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>

namespace GtkCRN
{
	/*! \brief A proxy action for a scale widget
	 *
	 * A proxy action for a scale widget
	 *
	 * \ingroup gtkcrn
	 */
	class ScaleAction
		: public Gtk::Action
	{
		public:
			/*! \brief Creates a blank ScaleAction */
			static Glib::RefPtr<ScaleAction> create();
			/*! \brief Creates a ScaleAction */
			static Glib::RefPtr<ScaleAction> create(const Glib::ustring& name, const Glib::ustring& label = Glib::ustring(), const Glib::ustring& tooltip = Glib::ustring());
			/*! \brief Creates a ScaleAction */
			static Glib::RefPtr<ScaleAction> create(const Glib::ustring& name, const Gtk::StockID& stock_id, const Glib::ustring& label = Glib::ustring(), const Glib::ustring& tooltip = Glib::ustring());
			/*! \brief Creates a ScaleAction */
			static Glib::RefPtr<ScaleAction> create_with_icon_name(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label, const Glib::ustring& tooltip);

			/*! \brief Gets the Adjustment object to configure the scale */
#ifdef CRN_USING_GTKMM3
			Glib::RefPtr<Gtk::Adjustment> get_adjustment() { return adj; }
#else /* CRN_USING_GTKMM3 */
			Gtk::Adjustment& get_adjustment() { return adj; }
#endif /* CRN_USING_GTKMM3 */
			/*! \brief Sets the icons to display depending of the value of the scale */
			void set_icons(const Glib::StringArrayHandle& icons);

			/*! \brief Signals when the value was changed. Connect to void on_value_changed(); */
#ifdef CRN_USING_GTKMM3
			Glib::SignalProxy0<void> signal_changed() { return adj->signal_value_changed(); }
#else /* CRN_USING_GTKMM3 */
			Glib::SignalProxy0<void> signal_changed() { return adj.signal_value_changed(); }
#endif /* CRN_USING_GTKMM3 */

			/*! \brief Destructor */
			virtual ~ScaleAction() override { }

		protected:
			ScaleAction();
			ScaleAction(const Glib::ustring& name, const Gtk::StockID& stock_id = Gtk::StockID(), const Glib::ustring& label = Glib::ustring(), const Glib::ustring& tooltip = Glib::ustring());
			ScaleAction(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label = Glib::ustring(), const Glib::ustring& tooltip = Glib::ustring());

			virtual Gtk::Widget* create_menu_item_vfunc() override;
			virtual Gtk::Widget* create_tool_item_vfunc() override;
			void dialog();

#ifdef CRN_USING_GTKMM3
			Glib::RefPtr<Gtk::Adjustment> adj;
#else /* CRN_USING_GTKMM3 */
			Gtk::Adjustment adj;
#endif /* CRN_USING_GTKMM3 */
			Glib::ustring lab;
			std::vector<Glib::ustring> iconlist;
	};
}
#endif

