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
 * file: GtkCRNColorAction.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNColorAction_HEADER
#define GtkCRNColorAction_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>

#ifndef CRN_USING_GTKMM3
namespace GtkCRN
{
	/*! \brief A proxy action for a color chooser
	 *
	 * A proxy action for a color chooser
	 * \ingroup gtkcrn
	 */
	class ColorAction
		: public Gtk::Action
	{
		public:
			/*! \brief Creates a blank ColorAction */
			static Glib::RefPtr<ColorAction> create();
			/*! \brief Creates ColorAction */
			static Glib::RefPtr<ColorAction> create(const Glib::ustring& name, const Glib::ustring& label=Glib::ustring(), const Glib::ustring& tooltip=Glib::ustring());
			/*! \brief Creates ColorAction */
			static Glib::RefPtr<ColorAction> create(const Glib::ustring& name, const Gtk::StockID& stock_id, const Glib::ustring& label=Glib::ustring(), const Glib::ustring& tooltip=Glib::ustring());
			/*! \brief Creates ColorAction */
			static Glib::RefPtr<ColorAction> create_with_icon_name(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label, const Glib::ustring& tooltip);

			/*! \brief	Gets the selected color */
			Gdk::Color get_color() const { return color; }
			/*! \brief	Sets the selected color */
			void set_color(const Gdk::Color &col);

			/*! \brief Signals when the color was changed. Connect to void on_color_changed(); */
			sigc::signal<void> signal_changed() { return changed; }

			/*! \brief Destructor */
			virtual ~ColorAction() override {}

		protected:
			/*! \brief Default constructor */
			ColorAction();

			/*! \brief Constructor */
			ColorAction(const Glib::ustring& name, const Gtk::StockID& stock_id=Gtk::StockID(), const Glib::ustring& label=Glib::ustring(), const Glib::ustring& tooltip=Glib::ustring());
			/*! \brief Constructor */
			ColorAction(const Glib::ustring& name, const Glib::ustring& icon_name, const Glib::ustring& label=Glib::ustring(), const Glib::ustring& tooltip=Glib::ustring());

			/*! \brief Customizes menu items */
			virtual Gtk::Widget* create_menu_item_vfunc() override;
			/*! \brief Customizes tool items */
			virtual Gtk::Widget* create_tool_item_vfunc() override;

			/*! \brief Asks for a color */
			void dialog();
			/*! \brief Button click */
			void on_button(Gtk::ColorButton* but);
			/*! \brief Customizes a menu item */
			static void change_menu_color(const Gdk::Color &col, Gtk::MenuItem* mit);

			sigc::signal<void, const Gdk::Color&> update_color; /*!< need to update the displayed color */
			sigc::signal<void> changed; /*!< the selected color changed */

			Gdk::Color color; /*!< the selected color*/
	};
}
#endif

#endif

