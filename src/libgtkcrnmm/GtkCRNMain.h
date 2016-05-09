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
 * file: GtkCRNMain.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNMain_HEADER
#define GtkCRNMain_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNUtils/CRNConfigurationFile.h>

/*! \defgroup gtkcrn UI library */
namespace GtkCRN
{
	/*! \brief Main loop and context
	 * \ingroup gtkcrn
	 */
#ifdef CRN_USING_GTKMM3
	class Main : public Gtk::Application
#else
	class Main: public Gtk::Main
#endif
	{

		public:
			/*! \brief You have to create a Main object in the main function. */
			Main(int &argc, char **&argv);
			//virtual ~Main() override {}
			/*! \brief Launches the application. */
			void run_thread_safe();

			/*! \brief Sets the default exception handler to print message and context to the standard error */
			static void SetDefaultExceptionHandler();

		private:
			static bool init_done; /*!< is Gtk initialized? */
#ifdef CRN_USING_GTKMM3
#else /* CRN_USING_GTKMM3 */
			Glib::RefPtr<Gtk::IconFactory> iconfac; /*!< icon factory to handle libcrn's icons */
			std::vector<Gtk::IconSet> icons; /*!< store for libcrn's icons */
#endif /* CRN_USING_GTKMM3 */
	};
}

#endif


