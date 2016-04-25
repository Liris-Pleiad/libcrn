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
 * file: GtkCRNDocument.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <CRNi18n.h>
#include <GtkCRNDocument.h>
#include <iostream>
#include <CRNUtils/CRNAtScopeExit.h>
#include <GtkCRNPDFAttributes.h>
#include <GtkCRNProgressWindow.h>
#include <GtkCRNApp.h>
#include <CRNIO/CRNFileShield.h>
#include <GdkCRNPixbuf.h>

#ifndef CRN_USING_GTKMM3
#	define get_content_area get_vbox
#endif

using namespace GtkCRN;
//////////////////////////////////////////////////////////////////////////////
// C++ stuff
//////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \param[in]	show_views	shall the view list be displayed?
 * \param[in]	show_tree	shall the subblock tree be displayed?
 */
Document::Document(bool show_views, bool show_tree):
#ifdef CRN_USING_GTKMM3
	views_actions(Gio::SimpleActionGroup::create()),
#else
	views_actions(Gtk::ActionGroup::create("views")),
#endif
	view_box(crn::Orientation::VERTICAL),
	view_frame(_("Views")),
	show_thumbnails(true),
	show_labels(true),
	show_indexes(true),
#ifdef CRN_USING_GTKMM3
	tree_actions(Gio::SimpleActionGroup::create()),
#else
	tree_actions(Gtk::ActionGroup::create("tree")),
#endif
	treecol1(uint8_t(0), 0, 0),
	treecol2(uint8_t(255), 255, 255),
	treetextcol(uint8_t(0), 0, 0),
	selcol1(uint8_t(0), 127, 0),
	selcol2(uint8_t(0), 255, 0),
	show_subblock_labels(true),
	fill_subblocks(true),
	tree_frame(_("Blocks"))
{
	//////////////////////////////////
	// Actions
	//////////////////////////////////
	// views
#ifdef CRN_USING_GTKMM3
	views_actions->add_action("document-views-add", sigc::mem_fun(this, &Document::append_views_dialog));
	views_actions->add_action("document-views-refresh", sigc::mem_fun(this, &Document::refresh_views));
	views_actions->add_action("document-views-select-first", sigc::mem_fun(this, &Document::select_first));
	views_actions->add_action("document-views-select-previous", sigc::mem_fun(this, &Document::select_previous));
	views_actions->add_action("document-views-select-next", sigc::mem_fun(this, &Document::select_next));
	views_actions->add_action("document-views-select-last", sigc::mem_fun(this, &Document::select_last));
	views_actions->add_action("document-views-select-all", sigc::mem_fun(this, &Document::select_all));
	views_actions->add_action("document-views-select-none", sigc::mem_fun(this, &Document::deselect_all));
	views_actions->add_action("document-views-select-even", sigc::mem_fun(this, &Document::select_even));
	views_actions->add_action("document-views-select-odd", sigc::mem_fun(this, &Document::select_odd));
	views_actions->add_action("document-views-invert-selection", sigc::mem_fun(this, &Document::invert_selection));
	views_actions->add_action("document-views-remove", sigc::mem_fun(this, &Document::delete_selection));
#	ifdef CRN_USING_HARU
	views_actions->add_action("document-views-export-pdf", sigc::mem_fun(this, &Document::export_pdf));
#	endif
	// views_actions->set_sensitive(false); // TODO
#else
	views_actions->add(Gtk::Action::create("document-views-menu", _("_Views"), _("Views")));
	views_actions->add(Gtk::Action::create("document-views-add", Gtk::StockID("gtk-crn-add-view"), _("_Add Views"), _("Add Views")), sigc::mem_fun(this, &Document::append_views_dialog));
	views_actions->add(Gtk::Action::create("document-views-refresh", Gtk::Stock::REFRESH, _("_Refresh Views"), _("Refresh Views")), sigc::mem_fun(this, &Document::refresh_views));
	views_actions->add(Gtk::Action::create("document-views-select-first", Gtk::Stock::GOTO_FIRST, _("Select _First View"), _("Select First View")), sigc::mem_fun(this, &Document::select_first));
	views_actions->add(Gtk::Action::create("document-views-select-previous", Gtk::Stock::GO_BACK, _("Select _Previous View"), _("Select Previous View")), sigc::mem_fun(this, &Document::select_previous));
	views_actions->add(Gtk::Action::create("document-views-select-next", Gtk::Stock::GO_FORWARD, _("Select _Next View"), _("Select Next View")), sigc::mem_fun(this, &Document::select_next));
	views_actions->add(Gtk::Action::create("document-views-select-last", Gtk::Stock::GOTO_LAST, _("Select _Last View"), _("Select Last View")), sigc::mem_fun(this, &Document::select_last));
	views_actions->add(Gtk::Action::create("document-views-select-all", Gtk::Stock::SELECT_ALL, _("Select _All Views"), _("Select All Views")), sigc::mem_fun(this, &Document::select_all));
	views_actions->add(Gtk::Action::create("document-views-select-none", Gtk::Stock::CLEAR, _("_Deselect Views"), _("Deselect Views")), sigc::mem_fun(this, &Document::deselect_all));
	views_actions->add(Gtk::Action::create("document-views-select-even", Gtk::StockID("gtk-crn-even"), _("Select _Even Views"), _("Select _Even Views")), sigc::mem_fun(this, &Document::select_even));
	views_actions->add(Gtk::Action::create("document-views-select-odd", Gtk::StockID("gtk-crn-odd"), _("Select _Odd Views"), _("Select _Odd Views")), sigc::mem_fun(this, &Document::select_odd));
	views_actions->add(Gtk::Action::create("document-views-invert-selection", Gtk::StockID("gtk-crn-invert"), _("_Invert View Selection"), _("Invert View Selection")), sigc::mem_fun(this, &Document::invert_selection));
	views_actions->add(Gtk::Action::create("document-views-remove", Gtk::StockID("gtk-crn-delete-view"), _("_Remove View Selection"), _("Remove View Selection")), sigc::mem_fun(this, &Document::delete_selection));
#	ifdef CRN_USING_HARU
	views_actions->add(Gtk::Action::create("document-views-export-pdf", Gtk::Stock::CONVERT, _("_Export PDF"), _("Export PDF")), sigc::mem_fun(this, &Document::export_pdf));
#	endif
	views_actions->set_sensitive(false);
#endif
	// tree
#ifdef CRN_USING_GTKMM3
	tree_actions->add_action("document-blocks-add", sigc::mem_fun(this, &Document::add_subblock));
	Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-add"))->set_enabled(false);
	tree_actions->add_action("document-blocks-remove", sigc::mem_fun(this, &Document::rem_subblock));
	Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-remove"))->set_enabled(false);
	tree_actions->add_action_bool("document-blocks-show", sigc::mem_fun(this, &Document::show_hide_subblocks_on_image));
	tree_actions->add_action("document-blocks-configure", sigc::mem_fun(this, &Document::configure_subblocks));
#else
	tree_actions->add(Gtk::Action::create("document-blocks-add", Gtk::StockID("gtk-crn-add-block"), _("_Add Subblock"), _("Add Subblock")), sigc::mem_fun(this, &Document::add_subblock));
	tree_actions->get_action("document-blocks-add")->set_sensitive(false);
	tree_actions->add(Gtk::Action::create("document-blocks-remove", Gtk::StockID("gtk-crn-delete-block"), _("_Remove Subblock"), _("Remove Subblock")), sigc::mem_fun(this, &Document::rem_subblock));
	tree_actions->get_action("document-blocks-remove")->set_sensitive(false);
	tree_actions->add(Gtk::ToggleAction::create("document-blocks-show", Gtk::Stock::FIND, _("_Show/Hide Subblocks"), _("Show/Hide Subblocks"), true), sigc::mem_fun(this, &Document::show_hide_subblocks_on_image));
	tree_actions->add(Gtk::Action::create("document-blocks-configure", Gtk::Stock::PROPERTIES, _("Subblocks _Settings"), _("Subblocks Settings")), sigc::mem_fun(this, &Document::configure_subblocks));
#endif

	//////////////////////////////////
	// left side
	//   atop
	//////////////////////////////////
	vpan.show();
	pack1(vpan, Gtk::FILL);
	if (show_views)
		view_frame.show();
	view_frame.set_shadow_type(Gtk::SHADOW_NONE);
	view_frame.add(view_box);
	view_box.show();
	view_box.set_homogeneous(false);
	view_box.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
	droppedin_handle = view_box.signal_droppedin().connect(sigc::mem_fun(this, &Document::droppedin));
	view_box.signal_moved().connect(sigc::mem_fun(this, &Document::moved));
	view_box.signal_selection_changed().connect(sigc::mem_fun(this, &Document::on_view_selection_changed));
	view_box.signal_key_release_event().connect(sigc::mem_fun(this, &Document::boxkeyevents));
	view_box.set_size_request(120, 120);
	vpan.pack1(view_frame, Gtk::FILL);
	//////////////////////////////////
	//   bottom
	//////////////////////////////////
	if (show_tree)
		tree_frame.show();
	tree_frame.set_shadow_type(Gtk::SHADOW_NONE);
	tree_frame.add(tree_box);
	tree_box.show();
	tree_box.set_homogeneous(false);
	// buttons
#ifdef CRN_USING_GTKMM3
	// TODO
#else
	tree_buttons.append(*Gtk::manage(tree_actions->get_action("document-blocks-add")->create_tool_item()));
	tree_buttons.append(*Gtk::manage(tree_actions->get_action("document-blocks-remove")->create_tool_item()));
	tree_buttons.append(*Gtk::manage(tree_actions->get_action("document-blocks-show")->create_tool_item()));
	tree_buttons.append(*Gtk::manage(tree_actions->get_action("document-blocks-configure")->create_tool_item()));
#endif
	tree_buttons.show();
	tree_box.pack_start(tree_buttons, false, true, 0);
	// tree
	block_tree_store = Gtk::TreeStore::create(block_columns);
	block_tree_view.set_model(block_tree_store);
	block_tree_view.show();
	block_tree_view.append_column(_("Block name"), block_columns.name);
	block_tree_view.append_column(_("Coordinates"), block_columns.coords);
	block_tree_view.get_selection()->signal_changed().connect(sigc::mem_fun(this, &Document::subblock_selection_changed));
	tree_sw.add(block_tree_view);
	tree_sw.show();
	tree_box.pack_start(tree_sw, true, true, 0);
	vpan.pack2(tree_frame, Gtk::FILL);

	//////////////////////////////////
	// right side
	//////////////////////////////////
	image_box.show();
	pack2(image_box, Gtk::EXPAND | Gtk::FILL);
	left_box.show();
	image_box.pack_start(left_box, false, true, 0);
	img.show();
	img.signal_overlay_changed().connect(sigc::mem_fun(this, &Document::on_image_overlay_changed));
	img.get_overlay_config(subblock_list_name).moveable = false;
	img.get_overlay_config(subblock_list_name).editable = false;
	img.get_overlay_config(subblock_list_name).color1 = GdkCRN::ColorFromCRNPixel(treecol1);
	img.get_overlay_config(subblock_list_name).color2 = GdkCRN::ColorFromCRNPixel(treecol2);
	img.get_overlay_config(subblock_list_name).text_color = GdkCRN::ColorFromCRNPixel(treetextcol);
	img.get_overlay_config(subblock_list_name).fill = fill_subblocks;
	img.get_selection_config().color1 = GdkCRN::ColorFromCRNPixel(selcol1);
	img.get_selection_config().color2 = GdkCRN::ColorFromCRNPixel(selcol2);
	image_box.pack_start(img, true, true, 0);
	right_box.show();
	image_box.pack_start(right_box, false, true, 0);

	set_sensitive(false);
}

/*! Destructor */
Document::~Document()
{
	if (crndoc)
	{
		crndoc->Save();
	}
}

//////////////////////////////////////////////////////////////////////////////
// General document management
//////////////////////////////////////////////////////////////////////////////

/*! Sets the document to display
 *
 * \param[in]	doc	the document to display
 */
void Document::set_document(const crn::SDocument &doc)
{
	crndoc = doc;
	if (crndoc)
	{
		refresh_views();
		set_sensitive(true);
#ifdef CRN_USING_GTKMM3
		// TODO
		Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(views_actions->lookup_action("document-views-remove"))->set_enabled(false);
#else
		views_actions->set_sensitive(true);
		views_actions->get_action("document-views-remove")->set_sensitive(false);
#endif
	}
	else
	{
		set_sensitive(false);
#ifdef CRN_USING_GTKMM3
		// TODO
#else
		views_actions->set_sensitive(false);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////////
// Views
//////////////////////////////////////////////////////////////////////////////

Document::View::View(const crn::String &id, crn::SDocument doc):
	view_id(id)
{
	try
	{
		thumb.set(doc->GetThumbnailFilename(id).CStr());
	}
	catch (...)
	{
		//thumb.set(Gtk::Stock::MISSING_IMAGE, Gtk::ICON_SIZE_BUTTON);
	}
	thumb.show();
	pack_start(thumb, FALSE, TRUE, 0);

	try
	{
		index.set_text(crn::StringUTF8(doc->GetViewIndex(id)).CStr());
	}
	catch (...)
	{
		index.set_text("?");
	}
	index.show();
	pack_start(index, FALSE, TRUE, 0);

	try
	{
		lab.set_text(doc->GetViewFilename(id).CStr());
	}
	catch (...)
	{
		lab.set_text(_("Inexistent view"));
	}
	lab.show();
	lab.set_ellipsize(Pango::ELLIPSIZE_START);
	pack_start(lab, FALSE, TRUE, 0);
}

/*! Creates the thumbnails
 * \param[in]	pw	a progress bar
 */
void Document::create_view_cache(crn::Progress *pw)
{
	int nb_errors = 0;
	for (size_t tmp = 0; tmp < crndoc->GetNbViews(); ++tmp)
	{
		try
		{
			crndoc->GetThumbnailFilename(tmp);
		}
		catch (crn::ExceptionUninitialized &)
		{
			App::show_message(_("The document was never saved. It is impossible to generate thumbnails."), Gtk::MESSAGE_WARNING);
			nb_errors = 0;
			break;
		}
		catch (...)
		{
			nb_errors += 1;
		}
		pw->Advance();
	}
	if (nb_errors)
	{
		gchar message[1024];
		g_snprintf(message, 1024, _("%i error(s) occurred during the generation of the thumbnails. Some images may be deffective."), nb_errors);
		App::show_message(message, Gtk::MESSAGE_WARNING);
	}
	//throw Glib::Thread::Exit(); // is this necessary?
}

/*! Redraws the view list
 */
void Document::refresh_views()
{
	// store selection
	const std::vector<crn::String> sel(get_selected_views_ids());
	const crn::String lastsel(get_selected_view_id());

	// empty the box
	views.clear();
	view_box.clear();

	if (!crndoc)
		return;
	if (crndoc->GetNbViews() == 0)
		return;

	// make sure all cache images exist (with a progress bar)
	GtkCRN::ProgressWindow pwin(_("Refresh"), (Gtk::Window*)get_ancestor(GTK_TYPE_WINDOW), true);
	crn::Progress *prog = pwin.get_crn_progress(pwin.add_progress_bar(_("Loading view"), (int)crndoc->GetNbViews()));
	prog->SetType(crn::Progress::Type::ABSOLUTE);
	pwin.run(sigc::bind(sigc::mem_fun(this, &Document::create_view_cache), prog));

	bool reselectlast = false;
	// add document views
	for (size_t tmp = 0; tmp < crndoc->GetNbViews(); ++tmp)
	{
		const crn::String vid = crndoc->GetViewId(tmp);
		if (vid == lastsel)
		{
			reselectlast = true;
		}
		views.push_back(std::make_shared<View>(vid, crndoc));
		views.back()->show();
		view_box.pack_start(*views.back(), false, true, 0);
	}
	// recall selection
	for (size_t tmp = 0; tmp < sel.size(); ++tmp)
	{
		try
		{
			const crn::String vid = sel[tmp];
			if (vid != lastsel)
			{
				view_box.set_selected(crndoc->GetViewIndex(sel[tmp]), true, reselectlast || (tmp != (sel.size() - 1)));
			}
		}
		catch (...)
		{ // a view was removed. that's not a problem.
		}
	}
	if (reselectlast)
	{
		view_box.set_selected(crndoc->GetViewIndex(lastsel), true, false);
	}
}

std::vector<crn::String> Document::get_selected_views_ids()
{
	std::vector<Gtk::Widget*> selection(view_box.get_selection());
	std::vector<crn::String> selid;
	for (Gtk::Widget *w : selection)
	{
		View *v = dynamic_cast<View*>(w);
		selid.push_back(v->get_view_id());
	}
	return selid;
}

crn::String Document::get_selected_view_id()
{
	View *v(dynamic_cast<View*>(view_box.get_last_selected()));
	if (v)
	{
		return v->get_view_id();
	}
	return U"";
}

/*! Sets the one selected view
 * \throws	crn::ExceptionNotFound	id not found
 * \throws	Glib::OptionError	view not shown in the list (the crn::Document object is not consistent with the GtkCRN::Document object!)
 */
void Document::set_selected_view(const crn::String &view_id)
{
	size_t index = crndoc->GetViewIndex(view_id);
	view_box.set_selection(index);
}

/*! Inserts views in the document
 * \param[in]	pos	the position of the insertion
 * \param[in]	data	a list of filenames separated by carriage returns
 */
void Document::droppedin(int pos, crn::StringUTF8 data)
{
	std::vector<crn::StringUTF8> files(data.Split("\r\n"));
	for (const crn::StringUTF8 &fname : files)
	{
		if (fname != "")
		{
			pos += 1; // +1 because -1 is the first drop zone.
			crn::Path p(fname);
			p.ToLocal();
			crndoc->InsertView(p, pos); // TODO pass complete URI and make sure libcrn supports it!!!
		}
	}
	refresh_views();
	// TODO what about the selection?
}

/*! Reorders the views
 * \param[in]	to	the new order using old indices
 * \param[in]	from	the previous order using new indices
 */
void Document::moved(std::vector<size_t> to, std::vector<size_t> from)
{
	crndoc->ReorderViewsTo(to);
	for (std::shared_ptr<View> v : views)
	{
		v->set_index(crndoc->GetViewIndex(v->get_view_id()));
	}
}

/*! Refreshes the display
 * \param[in]	last_selected_widget	the widget indicating which view shall be displayed
 * \param[in]	selection	the list of selected views
 */
void Document::on_view_selection_changed(Gtk::Widget *last_selected_widget, const std::vector<Gtk::Widget*> selection)
{
	crn::String lastselid;
	std::vector<crn::String> selid(get_selected_views_ids());
	try
	{
		if (last_selected_widget != nullptr)
		{
			View *v = dynamic_cast<View*>(last_selected_widget);
			if (v)
			{
				lastselid = v->get_view_id();
				current_block = crndoc->GetView(v->get_view_id());
				load_tree(v->get_view_id());
#ifdef CRN_USING_GTKMM3
				Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(views_actions->lookup_action("document-views-remove"))->set_enabled(true);
#else
				views_actions->get_action("document-views-remove")->set_sensitive(true);
#endif
				selection_changed.emit(lastselid, selid);
				return;
			}
		}
	}
	catch (...)
	{ // could not open image (cannot be a wrong id, I think)
	}
	current_block = nullptr;
	clear_tree();
#ifdef CRN_USING_GTKMM3
	Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-add"))->set_enabled(false);
	Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-remove"))->set_enabled(false);
	Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(views_actions->lookup_action("document-views-remove"))->set_enabled(false);
#else
	tree_actions->get_action("document-blocks-add")->set_sensitive(false);
	tree_actions->get_action("document-blocks-remove")->set_sensitive(false);
	views_actions->get_action("document-views-remove")->set_sensitive(false);
#endif
	selection_changed.emit(lastselid, selid);
}

/*! Handler for keystrokes in the view list
 * \param[in]	ev	the Gdk event
 * \return	false
 */
bool Document::boxkeyevents(GdkEventKey *ev)
{
#ifdef CRN_USING_GTKMM3
	if (ev->keyval == GDK_KEY_Delete)
#else /* CRN_USING_GTKMM3 */
	if (ev->keyval == GDK_Delete)
#endif /* CRN_USING_GTKMM3 */
	{
		delete_selection();
	}
	return false;
}

void Document::set_show_thumbnails(bool s)
{
	show_thumbnails = s;
	for (std::shared_ptr<View> v : views)
	{
		v->set_show_thumbnail(s);
	}
}

void Document::set_show_labels(bool s)
{
	show_labels = s;
	for (std::shared_ptr<View> v : views)
	{
		v->set_show_label(s);
	}
}

void Document::set_show_indexes(bool s)
{
	show_indexes = s;
	for (std::shared_ptr<View> v : views)
	{
		v->set_show_index(s);
	}
}

/*! reloads the image (to be used in a thread)
 * \param[in]	data	an internal structure
 * \return	false
 */
gboolean Document::tsreloadimage(DVI *data)
{
	data->doc->reload_image(data->id);
	delete data;
	return FALSE;
}

void Document::delete_selection()
{
	const std::vector<Gtk::Widget*> sel(view_box.get_selection());
	if (sel.size() != 0)
	{
		if (!may_delete_selection.empty())
		{
			std::vector<crn::String> selid(get_selected_views_ids());
			if (!may_delete_selection.emit(selid))
			{
				return;
			}
		}

		for (Gtk::Widget *w : sel)
		{
			View *v = dynamic_cast<View*>(w);
			if (v)
			{
				try
				{
					crndoc->RemoveView(v->get_view_id());
				}
				catch (...)
				{ // invalid id, do nothing
				}
			}
		}
		refresh_views();
	}
}

void Document::append_views(const std::vector<crn::Path> &filenames)
{
	for (crn::Path fname : filenames)
	{
		fname.ToLocal();
		if (fname.IsNotEmpty())
			crndoc->AddView(fname);
	}
	refresh_views();
}

void Document::append_views_dialog()
{
	Gtk::FileChooserDialog dial((Gtk::Window&)(*get_ancestor(GTK_TYPE_WINDOW)), _("Please select views to add"), Gtk::FILE_CHOOSER_ACTION_OPEN);
	dial.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
#ifdef CRN_USING_GTKMM3
	auto ff = Gtk::FileFilter::create();
	ff->add_pixbuf_formats();
#else /* CRN_USING_GTKMM3 */
	Gtk::FileFilter ff;
	ff.add_pixbuf_formats();
#endif /* CRN_USING_GTKMM3 */
	dial.set_filter(ff);
	dial.set_select_multiple(true);
#ifdef CRN_USING_GTKMM3
	dial.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
	dial.add_button(_("_Add"), Gtk::RESPONSE_ACCEPT);
#else
	dial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dial.add_button(Gtk::Stock::ADD, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	dial.set_alternative_button_order_from_array(altbut);
#endif
	dial.set_default_response(Gtk::RESPONSE_ACCEPT);
	dial.show();
	int resp = dial.run();
	dial.hide();
	if (resp == Gtk::RESPONSE_ACCEPT)
	{
#ifdef CRN_USING_GTKMM3
		const auto uris = dial.get_uris();
		auto fnames = std::vector<crn::Path>{};
		for (const auto &uri : uris)
			fnames.push_back(uri.c_str());
#else /* CRN_USING_GTKMM3 */
		std::vector<crn::Path> fnames(dial.get_uris());
#endif /* CRN_USING_GTKMM3 */
		append_views(fnames);
	}
}

void Document::select_even()
{
	view_box.select_even();
}

void Document::select_odd()
{
	view_box.select_odd();
}

void Document::select_all()
{
	view_box.select_all();
}

void Document::deselect_all()
{
	view_box.deselect_all();
}

void Document::invert_selection()
{
	view_box.invert_selection();
}

void Document::select_first()
{
	view_box.select_first();
}

void Document::select_previous()
{
	view_box.select_previous();
}

void Document::select_next()
{
	view_box.select_next();
}

void Document::select_last()
{
	view_box.select_last();
}

//////////////////////////////////////////////////////////////////////////////
// Subblock tree
//////////////////////////////////////////////////////////////////////////////

/*! Clears the subblock tree */
void Document::clear_tree()
{
	block_tree_store->clear();
	img.set_pixbuf(Glib::RefPtr<Gdk::Pixbuf>(nullptr));
}

/*! Populates the subblock tree
 * \param[in]	view_id	the view to display
 */
void Document::load_tree(const crn::String &view_id)
{
	clear_tree();
	try
	{
		crn::SBlock b(crndoc->GetView(view_id));
		if (b)
		{
			Gtk::TreeModel::iterator it(block_tree_store->append());
			it->set_value(block_columns.name, Glib::ustring(b->GetName().CStr()));
			it->set_value(block_columns.block, b);
			it->set_value(block_columns.is_tree, false);
			it->set_value(block_columns.coords, Glib::ustring(b->GetAbsoluteBBox().ToString().CStr()));
			tree_add_children(it, b);
			block_tree_view.expand_row(Gtk::TreePath(it), false);
			block_tree_view.get_selection()->select(it);
		}
	}
	catch (...)
	{ // wrong id should never happen…
	}
}

/*! Adds a node to the subblock tree
 * \param[in]	it	the place to insert the node
 * \param[in]	b	the block to display
 */
void Document::tree_add_children(Gtk::TreeModel::iterator &it, crn::SBlock b)
{
	std::vector<crn::String> trees(b->GetTreeNames());
	for (const crn::String &tname : trees)
	{
		Gtk::TreeModel::iterator bit(block_tree_store->append(it->children()));
		bit->set_value(block_columns.name, Glib::ustring(tname.CStr()));
		bit->set_value(block_columns.block, b);
		bit->set_value(block_columns.is_tree, true);
		for (crn::SObject sbo : b->GetTree(tname))
		{
			crn::SBlock sb(std::static_pointer_cast<crn::Block>(sbo));
			Gtk::TreeModel::iterator sit(block_tree_store->append(bit->children()));
			sit->set_value(block_columns.name, Glib::ustring(sb->GetName().CStr()));
			sit->set_value(block_columns.block, sb);
			sit->set_value(block_columns.is_tree, false);
			sit->set_value(block_columns.coords, Glib::ustring(sb->GetAbsoluteBBox().ToString().CStr()));
			tree_add_children(sit, sb);
		}
	}
}

/*! Updates the UI and image when subblock selection changed */
void Document::subblock_selection_changed()
{
	crn::String viewid(get_selected_view_id());
	if (viewid.IsNotEmpty() && block_tree_view.get_selection()->count_selected_rows())
	{
		try
		{
			std::lock_guard<std::mutex> lock(crn::FileShield::GetMutex("GtkCRNDocument::" + crndoc->GetViewFilename(viewid))); // lock image
			Gtk::TreeIter it(block_tree_view.get_selection()->get_selected());
			// set block
			blocksel = it->get_value(block_columns.block);
			// load image
			Glib::RefPtr<Gdk::Pixbuf> pb = GdkCRN::PixbufFromFile(crndoc->GetViewFilename(viewid));
			// crop
			crn::Rect clip(blocksel->GetAbsoluteBBox());
			Glib::RefPtr<Gdk::Pixbuf> pb2(Gdk::Pixbuf::create_subpixbuf(pb, clip.GetLeft(), clip.GetTop(), clip.GetWidth(), clip.GetHeight()));
			img.set_pixbuf(pb2);
#ifdef CRN_USING_GTKMM3
			// set sensitive the add subblock button if there is a selection on the image
			Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-add"))->set_enabled(img.has_selection());
			// set sensitive the remove subblock button if the selected row is not the first
			Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-remove"))->set_enabled(it != block_tree_store->children()[0]);
#else
			// set sensitive the add subblock button if there is a selection on the image
			tree_actions->get_action("document-blocks-add")->set_sensitive(img.has_selection());
			// set sensitive the remove subblock button if the selected row is not the first
			tree_actions->get_action("document-blocks-remove")->set_sensitive(it != block_tree_store->children()[0]);
#endif
		}
		catch (...)
		{ // invalid block id or cannot open image
			img.set_pixbuf(Glib::RefPtr<Gdk::Pixbuf>(nullptr));
#ifdef CRN_USING_GTKMM3
			Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-add"))->set_enabled(false);
			Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-remove"))->set_enabled(false);
#else
			tree_actions->get_action("document-blocks-add")->set_sensitive(false);
			tree_actions->get_action("document-blocks-remove")->set_sensitive(false);
#endif
		}
	}
	else
	{
		// set unsensitive the remove subblock button
#ifdef CRN_USING_GTKMM3
		Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-remove"))->set_enabled(false);
#else
		tree_actions->get_action("document-blocks-remove")->set_sensitive(false);
#endif
	}
	show_hide_subblocks_on_image();
}

/*! Adds a subblock with dialog */
void Document::add_subblock()
{
	// get data
	crn::Rect bbox(img.get_selection_as_rect());
	crn::SBlock b;
	crn::StringUTF8 autoselect;
	if (block_tree_view.get_selection()->count_selected_rows())
	{
		Gtk::TreeIter it(block_tree_view.get_selection()->get_selected());
		b = it->get_value(block_columns.block);
		if (it->get_value(block_columns.is_tree))
			autoselect = it->get_value(block_columns.name).c_str();
	}
	else
	{
		b = current_block;
	}
	// create dialog
	Gtk::Dialog dial(_("Add a subblock"), (Gtk::Window&)(*get_ancestor(GTK_TYPE_WINDOW)), true);
	dial.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
#ifdef CRN_USING_GTKMM3
	dial.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
	dial.add_button(_("_OK"), Gtk::RESPONSE_ACCEPT);
#else
	dial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dial.add_button(Gtk::Stock::OK, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	dial.set_alternative_button_order_from_array(altbut);
#endif
	dial.set_default_response(Gtk::RESPONSE_ACCEPT);
	Gtk::Label lab1(_("Subblock tree"));
	Gtk::Label lab2(_("New subblock's name"));
#ifdef CRN_USING_GTKMM3
	Gtk::Grid tab;
	tab.attach(lab1, 0, 0, 1, 1);
	tab.attach(lab2, 0, 1, 1, 1);
#else
	Gtk::Table tab(2, 2);
	tab.attach(lab1, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
	tab.attach(lab2, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
#endif
#ifdef CRN_USING_GTKMM3
	Gtk::ComboBoxText combo(true);
#else /* CRN_USING_GTKMM3 */
	Gtk::ComboBoxEntryText combo;
#endif /* CRN_USING_GTKMM3 */
	std::vector<crn::String> trees(b->GetTreeNames());
	for (const crn::String &tname : trees)
	{
#ifdef CRN_USING_GTKMM3
		combo.append(tname.CStr());
#else /* CRN_USING_GTKMM3 */
		combo.append_text(tname.CStr());
#endif /* CRN_USING_GTKMM3 */
	}
	combo.get_entry()->set_text(autoselect.CStr());
	combo.get_entry()->set_activates_default(true);
#ifdef CRN_USING_GTKMM3
	tab.attach(combo, 1, 0, 1, 1);
#else
	tab.attach(combo, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
#endif
	Gtk::Entry entname;
	entname.set_text(_("New subblock"));
	entname.set_activates_default(true);
#ifdef CRN_USING_GTKMM3
	tab.attach(entname, 1, 1, 1, 1);
#else
	tab.attach(entname, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::SHRINK);
#endif
	tab.show_all();
	dial.get_content_area()->pack_start(tab, true, true, 0);

	if (dial.run() == Gtk::RESPONSE_ACCEPT)
	{
		dial.hide();
		// add to block
		crn::StringUTF8 treename(combo.get_entry()->get_text().c_str());
		crn::SBlock nb(nullptr);
		try
		{
			nb = b->AddChildRelative(treename, bbox, entname.get_text().c_str());
		}
		catch (...)
		{
			App::show_message(_("Cannot add such selection to the subblock tree."), Gtk::MESSAGE_WARNING);
			return;
		}
		// find iter
		Gtk::TreeIter it;
		if (block_tree_view.get_selection()->count_selected_rows())
		{
			it = block_tree_view.get_selection()->get_selected();
			if (it->get_value(block_columns.is_tree))
			{ // if a subblock tree is selected, we now do as if it was its parent node
				it = it->parent();
			}
		}
		else
			it = block_tree_store->children()[0];
		bool found = false;
		Gtk::TreeIter newit;
		for (const Gtk::TreeRow &row : it->children())
		{
			if (row.get_value(block_columns.is_tree) && (row.get_value(block_columns.name) == treename.CStr()))
			{
				// add to tree store
				newit = block_tree_store->append(row.children());
				found = true;
				break;
			}
		}
		if (!found)
		{
			// must create 2 entries in the store
			// the new subblock tree
			newit = block_tree_store->append(it->children());
			newit->set_value(block_columns.name, Glib::ustring(treename.CStr()));
			newit->set_value(block_columns.block, b);
			newit->set_value(block_columns.is_tree, true);

			// the new subblock
			newit = block_tree_store->append(newit->children());
		}
		newit->set_value(block_columns.name, entname.get_text());
		newit->set_value(block_columns.block, nb);
		newit->set_value(block_columns.is_tree, false);
		newit->set_value(block_columns.coords, Glib::ustring(bbox.ToString().CStr()));
	}
}

/*! Removes a subblock or a subblock tree */
void Document::rem_subblock()
{
	if (block_tree_view.get_selection()->count_selected_rows())
	{ // a row is selected
		Gtk::TreeIter it(block_tree_view.get_selection()->get_selected());
		if (it->get_value(block_columns.is_tree))
		{
			crn::SBlock b(it->get_value(block_columns.block));
			b->RemoveTree(it->get_value(block_columns.name).c_str());
			block_tree_store->erase(it);
		}
		else
		{
			crn::SBlock b(it->get_value(block_columns.block));
			crn::Block *pb = b->GetParent().lock().get();
			if (pb)
			{
				Gtk::TreeIter pit(it->parent());
				try
				{
					pb->RemoveChild(pit->get_value(block_columns.name).c_str(), b);
				} catch (...) { }
				block_tree_store->erase(it);
				block_tree_view.get_selection()->select(pit);
			}
		}
	}
}

/*! Shows a dialog to personalize the subblock display */
void Document::configure_subblocks()
{
	// create dialog
	Gtk::Dialog dial(_("Configure subblocks display"), (Gtk::Window&)(*get_ancestor(GTK_TYPE_WINDOW)), true);
	dial.set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
#ifdef CRN_USING_GTKMM3
	dial.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
	dial.add_button(_("_OK"), Gtk::RESPONSE_ACCEPT);
#else
	dial.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dial.add_button(Gtk::Stock::OK, Gtk::RESPONSE_ACCEPT);
	std::vector<int> altbut;
	altbut.push_back(Gtk::RESPONSE_ACCEPT);
	altbut.push_back(Gtk::RESPONSE_CANCEL);
	dial.set_alternative_button_order_from_array(altbut);
#endif
	dial.set_default_response(Gtk::RESPONSE_ACCEPT);

	Gtk::Label lab1(_("Frame color"));
#ifdef CRN_USING_GTKMM3
	Gtk::Grid tab;
	tab.attach(lab1, 0, 0, 1, 1);
#else
	Gtk::Table tab(4, 2);
	tab.attach(lab1, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
#endif
#ifndef CRN_USING_GTKMM3 // XXX TODO
	Gdk::Color color;
	color.set_rgb(gushort(treecol1.r * 256), gushort(treecol1.g * 256), gushort(treecol1.b * 255));
	Gtk::ColorButton cb1(color);
	tab.attach(cb1, 1, 2, 0, 1, Gtk::SHRINK, Gtk::SHRINK);
	Gtk::Label lab2(_("Fill color"));
	tab.attach(lab2, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
	color.set_rgb(gushort(treecol2.r * 256), gushort(treecol2.g * 256), gushort(treecol2.b * 255));
	Gtk::ColorButton cb2(color);
	tab.attach(cb2, 1, 2, 1, 2, Gtk::SHRINK, Gtk::SHRINK);
	Gtk::CheckButton show_labels_cb(_("Show subblock _labels"));
	show_labels_cb.set_active(show_subblock_labels);
	tab.attach(show_labels_cb, 0, 2, 2, 3, Gtk::SHRINK, Gtk::SHRINK);
	Gtk::Label lab3(_("Label color"));
	color.set_rgb(gushort(treetextcol.r * 256), gushort(treetextcol.g * 256), gushort(treetextcol.b * 255));
	Gtk::ColorButton cbt(color);
	tab.attach(cbt, 1, 2, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
	tab.attach(lab3, 0, 1, 3, 4, Gtk::SHRINK, Gtk::SHRINK);
#endif
	tab.show_all();
	dial.get_content_area()->pack_start(tab, true, true, 0);

	if (dial.run() == Gtk::RESPONSE_ACCEPT)
	{
#ifndef CRN_USING_GTKMM3 // XXX TODO
		set_show_subblock_labels(show_labels_cb.get_active());
		color = cb1.get_color();
		crn::pixel::RGB8 p1(uint8_t(color.get_red() / 256), uint8_t(color.get_green() / 256), uint8_t(color.get_blue() / 256));
		color = cb2.get_color();
		crn::pixel::RGB8 p2(uint8_t(color.get_red() / 256), uint8_t(color.get_green() / 256), uint8_t(color.get_blue() / 256));
		color = cbt.get_color();
		crn::pixel::RGB8 pt(uint8_t(color.get_red() / 256), uint8_t(color.get_green() / 256), uint8_t(color.get_blue() / 256));
		set_subblocks_colors(p1, p2, pt);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////////
// Displayed image
//////////////////////////////////////////////////////////////////////////////
const crn::String Document::subblock_list_name(U"GtkCRN::Document::Current_subblocks");

void Document::reload_image(const crn::String &view_id)
{
	for (std::shared_ptr<View> v : views)
	{
		if (v->get_view_id() == view_id)
		{
			v->set_thumbnail(crndoc->GetThumbnailFilename(view_id));

			if (get_selected_view_id() == view_id)
			{ // the image has to be reloaded
				try
				{
					crn::Path lastsel = crndoc->GetViewFilename(get_selected_view_id());
					std::lock_guard<std::mutex> lock(crn::FileShield::GetMutex(lastsel)); // lock image
					Glib::RefPtr<Gdk::Pixbuf> pb = GdkCRN::PixbufFromFile(lastsel);
					img.set_pixbuf(pb);
				}
				catch (...)
				{
					img.set_pixbuf(Glib::RefPtr<Gdk::Pixbuf>(nullptr));
				}
			}
		}
	}
}

void Document::thread_safe_reload_image(const crn::String &view_id)
{
	DVI *data(new DVI(this, view_id));
	g_idle_add(GSourceFunc(Document::tsreloadimage), data);
}

/*! Updates the UI when the overlay changed
 * \param[in]	overlay_id	the overlay's name
 * \param[in]	overlay_item_id	the overlay item's name
 * \param[in]	mm	the mouse mode (drag, select, etc.)
 */
void Document::on_image_overlay_changed(crn::String overlay_id, crn::String overlay_item_id, Image::MouseMode mm)
{
	if (overlay_id == Image::selection_overlay())
	{
#ifdef CRN_USING_GTKMM3
		Glib::RefPtr<Gio::SimpleAction>::cast_dynamic(tree_actions->lookup_action("document-blocks-add"))->set_enabled(overlay_item_id.IsNotEmpty());
#else
		tree_actions->get_action("document-blocks-add")->set_sensitive(overlay_item_id.IsNotEmpty());
#endif
	}
}

/*! Shows or hides the subblocks as overlays */
void Document::show_hide_subblocks_on_image()
{
	// clear selections
	img.clear_overlay(subblock_list_name);
#ifdef CRN_USING_GTKMM3
	bool ok = false;
	tree_actions->lookup_action("document-blocks-show")->get_state(ok);
	if (ok)
#else
	Glib::RefPtr<Gtk::ToggleAction> sa(Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(tree_actions->get_action("document-blocks-show")));
	if (sa->get_active())
#endif
	{ // must show subblocks
		if (block_tree_view.get_selection()->count_selected_rows())
		{ // a row is selected
			Gtk::TreeIter it(block_tree_view.get_selection()->get_selected());
			if (it->get_value(block_columns.is_tree))
			{ // the selected row is a tree
				crn::SBlock b(it->get_value(block_columns.block));
				crn::String name(it->get_value(block_columns.name).c_str());
				int cnt = 0;
				for (crn::SObject sbo : b->GetTree(name))
				{
					crn::SBlock sb(std::static_pointer_cast<crn::Block>(sbo));
					img.add_overlay_item(subblock_list_name, cnt++, sb->GetRelativeBBox(), sb->GetName().CStr());
				}
			}
		}
	}
}

void Document::set_selection_colors(const crn::pixel::RGB8 &col1, const crn::pixel::RGB8 &col2)
{
	selcol1 = col1;
	selcol2 = col2;
	img.get_selection_config().color1 = GdkCRN::ColorFromCRNPixel(selcol1);
	img.get_selection_config().color2 = GdkCRN::ColorFromCRNPixel(selcol2);
}

void Document::set_subblocks_colors(const crn::pixel::RGB8 &col1, const crn::pixel::RGB8 &col2, const crn::pixel::RGB8 &textcol)
{
	treecol1 = col1;
	treecol2 = col2;
	treetextcol = textcol;
	img.get_overlay_config(subblock_list_name).color1 = GdkCRN::ColorFromCRNPixel(treecol1);
	img.get_overlay_config(subblock_list_name).color2 = GdkCRN::ColorFromCRNPixel(treecol2);
	img.get_overlay_config(subblock_list_name).text_color = GdkCRN::ColorFromCRNPixel(treetextcol);
}

void Document::set_show_subblock_labels(bool s)
{
	show_subblock_labels = s;
	img.get_overlay_config(subblock_list_name).show_labels = s;
	/*
	show_hide_subblocks_on_image();
	*/
}

#ifdef CRN_USING_HARU
/*! Opens dialog to select a file to export the document as PDF */
void Document::export_pdf()
{
	static std::shared_ptr<Gtk::FileChooserDialog> dial(nullptr);
#ifdef CRN_USING_GTKMM3
	static auto ff = Gtk::FileFilter::create();
#else /* CRN_USING_GTKMM3 */
	static Gtk::FileFilter ff;
#endif /* CRN_USING_GTKMM3 */
	static PDFAttributes attr;
	if (!dial)
	{
		dial = std::make_shared<Gtk::FileChooserDialog>((Gtk::Window&)(*get_ancestor(GTK_TYPE_WINDOW)), _("Export PDF"), Gtk::FILE_CHOOSER_ACTION_SAVE);
		dial->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
#ifdef CRN_USING_GTKMM3
		ff->add_pattern("*.pdf");
		ff->add_pattern("*.Pdf");
		ff->add_pattern("*.PDF");
#else /* CRN_USING_GTKMM3 */
		ff.add_pattern("*.pdf");
		ff.add_pattern("*.Pdf");
		ff.add_pattern("*.PDF");
#endif /* CRN_USING_GTKMM3 */
		dial->set_filter(ff);
		dial->set_select_multiple(false);
#ifdef CRN_USING_GTKMM3
		dial->add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
		dial->add_button(_("_Save"), Gtk::RESPONSE_ACCEPT);
#else
		dial->add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
		dial->add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_ACCEPT);
		std::vector<int> altbut;
		altbut.push_back(Gtk::RESPONSE_ACCEPT);
		altbut.push_back(Gtk::RESPONSE_CANCEL);
		dial->set_alternative_button_order_from_array(altbut);
#endif
		dial->set_default_response(Gtk::RESPONSE_ACCEPT);
		attr.show();
		dial->get_content_area()->pack_start(attr, false, true, 0);
	}
	dial->show();
	int resp = dial->run();
	dial->hide();
	if (resp == Gtk::RESPONSE_ACCEPT)
	{
		crn::Path fname(dial->get_uri());
		ProgressWindow pwin(_("Export PDF"), (Gtk::Window*)get_ancestor(GTK_TYPE_WINDOW), false);
		crn::Progress *prog = pwin.get_crn_progress(pwin.add_progress_bar(_("Writing image")));
		prog->SetType(crn::Progress::Type::ABSOLUTE);
		pwin.set_terminate_on_exception(false);
		pwin.run(sigc::hide_return(sigc::bind(sigc::mem_fun(crndoc.get(), &crn::Document::ExportPDF), fname, attr.get_attributes(), prog)));
	}
}
#endif

