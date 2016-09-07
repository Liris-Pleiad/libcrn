/* Copyright 2011-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: nimrod.cpp
 * \author Yann LEYDIER
 */

#define GETTEXT_PACKAGE "nimrod"
#include <GtkCRNAltoDocument.h>
#include <GtkCRNApp.h>
#include <GtkCRNMain.h>
#include <GtkCRNFileSelecterDialog.h>
#include <CRNi18n.h>

class Viewer: public GtkCRN::App
{
	public:
		Viewer():
			actions(Gtk::ActionGroup::create("nimrod"))
		{
			Glib::ustring title("Nimrod Alto viewer");
			title += " © CoReNum";
			set_title(title);

			actions->add(Gtk::Action::create("nimrod-new", Gtk::Stock::NEW), sigc::mem_fun(this, &Viewer::new_project));
			actions->add(Gtk::Action::create("nimrod-open", Gtk::Stock::OPEN), sigc::mem_fun(this, &Viewer::open_project));

			ui_manager->insert_action_group(actions);
			ui_manager->insert_action_group(alto.get_image_actions());

			Glib::ustring ui_info = 
				"<ui>"
				"	<menubar name='MenuBar'>"
				"		<menu action='app-file-menu'>"
				"			<menuitem action='nimrod-new'/>"
				"			<menuitem action='nimrod-open'/>"
				"			<menuitem action='app-quit'/>"
				"		</menu>"
				"		<menu action='app-help-menu'>"
				"			<menuitem action='app-about'/>"
				"		</menu>"
				"	</menubar>"
				"	<toolbar name='ToolBar'>"
				"		<toolitem	action='nimrod-open'/>"
				"		<separator/>"
				"		<toolitem action='image-zoom-in'/>"
				"		<toolitem action='image-zoom-out'/>"
				"		<toolitem action='image-zoom-100'/>"
				"		<toolitem action='image-zoom-fit'/>"
				"	</toolbar>"
				"</ui>";

			ui_manager->add_ui_from_string(ui_info);
			Gtk::VBox *vbox = Gtk::manage(new Gtk::VBox());
			vbox->show();
			add(*vbox);

			vbox->pack_start(*ui_manager->get_widget("/MenuBar"), false, true, 0);
			vbox->pack_start(*ui_manager->get_widget("/ToolBar"), false, true, 0);

			alto.show();
			vbox->pack_start(alto, true, true, 0);

		}
		virtual ~Viewer() override {}
	private:
		class NewDialog: public Gtk::Dialog
		{
			public:
				NewDialog(Gtk::Window *parent):
					Gtk::Dialog(_("New project"), *this, true),
					xmlmode(_("Read image names in _XML Altos"), true),
					filemode(_("Match images and XML Altos _filenames"), true),
					xmlpath(_("Path to the XML Alto files"), Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER),
					imgpath(_("Path to the images"), Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER),
					showimgpath(_("_Images are in a different folder"), true),
					imgpathlab(_("Images path"))
				{
					add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
					okbut = add_button(Gtk::Stock::OK, Gtk::RESPONSE_ACCEPT);
					okbut->set_sensitive(false);
					std::vector<int> altbut;
					altbut.push_back(Gtk::RESPONSE_ACCEPT);
					altbut.push_back(Gtk::RESPONSE_CANCEL);
					set_alternative_button_order_from_array(altbut);	
					set_default_response(Gtk::RESPONSE_ACCEPT);

					Gtk::Table *tab = Gtk::manage(new Gtk::Table(6, 2));
					get_vbox()->pack_start(*tab, true, true);
					tab->attach(*Gtk::manage(new Gtk::Label(_("Project name"))), 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);
					tab->attach(projectname, 1, 2, 0, 1, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
					projectname.signal_changed().connect(sigc::mem_fun(this, &NewDialog::on_name_changed));
					projectname.set_activates_default(true);
					Gtk::RadioButton::Group g;
					xmlmode.set_group(g);
					tab->attach(xmlmode, 0, 2, 1, 2, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
					filemode.set_group(g);
					tab->attach(filemode, 0, 2, 2, 3, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
					xmlmode.set_active(false);
					xmlmode.signal_toggled().connect(sigc::mem_fun(this, &NewDialog::on_mode_changed));
					tab->attach(*Gtk::manage(new Gtk::Label(_("XML Altos path"))), 0, 1, 3, 4, Gtk::FILL, Gtk::FILL);
					tab->attach(xmlpath, 1, 2, 3, 4, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
					xmlpath.signal_current_folder_changed().connect(sigc::mem_fun(this, &NewDialog::on_xml_folder_changed));
					tab->attach(showimgpath, 0, 2, 4, 5, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
					showimgpath.set_active(false);
					showimgpath.signal_toggled().connect(sigc::mem_fun(this, &NewDialog::on_show_img_path));
					show_all();
					tab->attach(imgpathlab, 0, 1, 5, 6, Gtk::FILL, Gtk::FILL);
					tab->attach(imgpath, 1, 2, 5, 6, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
				}
				crn::xml::SAltoWrapper run()
				{
					if (Gtk::Dialog::run() == Gtk::RESPONSE_ACCEPT)
					{
						try
						{
							if (xmlmode.get_active())
							{
								// TODO add progress bar
								return crn::xml::AltoWrapper::NewFromDir(xmlpath.get_current_folder().c_str(), projectname.get_text().c_str(), showimgpath.get_active() ? imgpath.get_current_folder().c_str() : "", nullptr, false);
							}
							else
							{
								// TODO add progress bar
								return crn::xml::AltoWrapper::NewFromDirs(imgpath.get_current_folder().c_str(), xmlpath.get_current_folder().c_str(), projectname.get_text().c_str(), nullptr, false);
							}
						}
						catch (crn::Exception &ex)
						{
							GtkCRN::App::show_exception(ex, false);
							return nullptr;
						}
						catch (std::exception &ex)
						{
							GtkCRN::App::show_exception(ex, false);
							return nullptr;
						}
						catch (...)
						{
							GtkCRN::App::show_message(_("Cannot create project"), Gtk::MESSAGE_ERROR);
							return nullptr;
						}
					}
					else
						return nullptr;
				};
			private:
				void on_name_changed()
				{
					okbut->set_sensitive(!projectname.get_text().empty());
				}
				void on_show_img_path()
				{
					bool show = showimgpath.get_active();
					imgpathlab.set_visible(show);
					imgpath.set_visible(show);
				}
				void on_mode_changed()
				{
					showimgpath.set_visible(xmlmode.get_active());
					bool show = showimgpath.get_active();
					imgpathlab.set_visible(show || filemode.get_active());
					imgpath.set_visible(show || filemode.get_active());
				}
				void on_xml_folder_changed()
				{
					imgpath.set_current_folder(xmlpath.get_current_folder());
				}
				Gtk::Entry projectname;
				Gtk::RadioButton xmlmode, filemode;
				Gtk::FileChooserButton xmlpath, imgpath;
				Gtk::CheckButton showimgpath;
				Gtk::Label imgpathlab;
				Gtk::Button *okbut;
		};
		void new_project()
		{
			NewDialog dial(this);
			crn::xml::SAltoWrapper w(dial.run());
			if (w)
				alto.set_alto(w);
		}

		void open_project()
		{
			GtkCRN::FileSelecterDialog dial(crn::Document::GetDefaultDirName(), this);
			if (dial.run() == Gtk::RESPONSE_ACCEPT)
			{
				alto.set_alto(crn::xml::AltoWrapper::NewFromDocument(dial.get_selection()));
			}
		}

		virtual void about() override
		{
		}

		GtkCRN::AltoDocument alto;
		Glib::RefPtr<Gtk::ActionGroup> actions;
};

int main(int argc, char *argv[])
{
	GtkCRN::Main kit(argc, argv);
	Viewer app;
	app.show();
	GtkCRN::App::set_main_window(&app);
	GtkCRN::Main::SetDefaultExceptionHandler();
	kit.run_thread_safe();
	return 0;
}


