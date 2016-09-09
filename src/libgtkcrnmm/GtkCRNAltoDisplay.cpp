/* Copyright 2011-2016 CoReNum, INSA-Lyon
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
 * file: GdkCRNAltoDisplay.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNAltoDisplay.h>
#include <CRNImage/CRNPixel.h>
#include <GdkCRNPixbuf.h>
#include <GtkCRNApp.h>
#include <GtkCRNAltoWordEditor.h>
#include <GtkCRNAltoTextLineEditor.h>
#include <CRNi18n.h>

#ifndef CRN_USING_GTKMM3
using namespace GtkCRN;

const crn::String Alto::pageList(U"nimrod::4pages");
const crn::String Alto::spaceList(U"nimrod::3spaces");
const crn::String Alto::blockList(U"nimrod::2blocks");
const crn::String Alto::lineList(U"nimrod::1lines");
const crn::String Alto::wordList(U"nimrod::0words");

Alto::Alto():
	actions(Gtk::ActionGroup::create("alto display")),
	alto(nullptr),
	doc(true, false),
	edit_mode(false)
{
	actions->add(Gtk::ToggleAction::create("alto-pages", Gtk::StockID("gtk-crn-view"), _("Show/hide _pages"), _("Show/hide pages")), sigc::mem_fun(this, &Alto::show_hide_pages));
	actions->add(Gtk::ToggleAction::create("alto-spaces", Gtk::StockID("gtk-crn-block"), _("Show/hide _spaces"), _("Show/hide spaces")), sigc::mem_fun(this, &Alto::show_hide_spaces));
	actions->add(Gtk::ToggleAction::create("alto-textblocks", Gtk::StockID("gtk-crn-block"), _("Show/hide _text blocks"), _("Show/hide text blocks")), sigc::mem_fun(this, &Alto::show_hide_blocks));
	actions->add(Gtk::ToggleAction::create("alto-textlines", Gtk::StockID("gtk-crn-block"), _("Show/hide text _lines"), _("Show/hide text lines")), sigc::mem_fun(this, &Alto::show_hide_lines));
	actions->add(Gtk::ToggleAction::create("alto-words", Gtk::StockID("gtk-crn-block"), _("Show/hide _words"), _("Show/hide words")), sigc::mem_fun(this, &Alto::show_hide_words));
	actions->add(Gtk::ToggleAction::create("alto-edit", Gtk::Stock::EDIT, _("_Edit alto"), _("Edit alto")), sigc::mem_fun(this, &Alto::toggle_edit));
	Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-edit"))->set_active(false);
	show_hide_pages();
	show_hide_spaces();
	show_hide_blocks();
	show_hide_lines();
	show_hide_words();

	// pages in blue
	crn::pixel::RGB8 pcol1(0, 0, 255);
	crn::pixel::RGB8 pcol2(0, 0, 127);
	doc.get_image().get_overlay_config(pageList).color1 = GdkCRN::ColorFromCRNPixel(pcol1);
	doc.get_image().get_overlay_config(pageList).color2 = GdkCRN::ColorFromCRNPixel(pcol2);
	doc.get_image().get_overlay_config(pageList).text_color = GdkCRN::ColorFromCRNPixel(pcol2);
	doc.get_image().get_overlay_config(pageList).fill = false;
	doc.get_image().get_overlay_config(pageList).show_labels = false;
	// spaces in cyan
	crn::pixel::RGB8 scol1(0, 255, 255);
	crn::pixel::RGB8 scol2(0, 127, 127);
	doc.get_image().get_overlay_config(spaceList).color1 = GdkCRN::ColorFromCRNPixel(scol1);
	doc.get_image().get_overlay_config(spaceList).color2 = GdkCRN::ColorFromCRNPixel(scol2);
	doc.get_image().get_overlay_config(spaceList).text_color = GdkCRN::ColorFromCRNPixel(scol2);
	doc.get_image().get_overlay_config(spaceList).fill = false;
	doc.get_image().get_overlay_config(spaceList).show_labels = false;
	// blocks in red
	crn::pixel::RGB8 bcol1(255, 0, 0);
	crn::pixel::RGB8 bcol2(127, 0, 0);
	doc.get_image().get_overlay_config(blockList).color1 = GdkCRN::ColorFromCRNPixel(bcol1);
	doc.get_image().get_overlay_config(blockList).color2 = GdkCRN::ColorFromCRNPixel(bcol2);
	doc.get_image().get_overlay_config(blockList).text_color = GdkCRN::ColorFromCRNPixel(bcol2);
	doc.get_image().get_overlay_config(blockList).fill = false;
	doc.get_image().get_overlay_config(blockList).show_labels = false;
	// lines in yellow
	crn::pixel::RGB8 lcol1(255, 255, 0);
	crn::pixel::RGB8 lcol2(127, 127, 0);
	doc.get_image().get_overlay_config(lineList).color1 = GdkCRN::ColorFromCRNPixel(lcol1);
	doc.get_image().get_overlay_config(lineList).color2 = GdkCRN::ColorFromCRNPixel(lcol2);
	doc.get_image().get_overlay_config(lineList).text_color = GdkCRN::ColorFromCRNPixel(lcol2);
	doc.get_image().get_overlay_config(lineList).fill = false;
	doc.get_image().get_overlay_config(lineList).show_labels = false;
	// words in green
	crn::pixel::RGB8 wcol1(0, 255, 0);
	crn::pixel::RGB8 wcol2(0, 127, 0);
	doc.get_image().get_overlay_config(wordList).color1 = GdkCRN::ColorFromCRNPixel(wcol1);
	doc.get_image().get_overlay_config(wordList).color2 = GdkCRN::ColorFromCRNPixel(wcol2);
	doc.get_image().get_overlay_config(wordList).text_color = GdkCRN::ColorFromCRNPixel(wcol2);
	doc.get_image().get_overlay_config(wordList).fill = false;
	doc.get_image().get_overlay_config(wordList).show_labels = true;

	doc.signal_view_selection_changed().connect(sigc::hide(sigc::hide(sigc::mem_fun(this, &Alto::on_view_changed))));
	doc.get_image().signal_rmb_clicked().connect(sigc::mem_fun(this, &Alto::on_rmb_clicked));
	doc.get_image().signal_overlay_changed().connect(sigc::mem_fun(this, &Alto::on_overlay_changed));

	pack_start(doc, true, true, 0);
	doc.show();
}

void Alto::set_wrapper(const crn::xml::SAltoWrapper &w)
{
	alto = w;
	if (alto)
		doc.set_document(alto->GetDocument());
	else
		doc.set_document(nullptr);
	update_boxes();
}

void Alto::on_view_changed()
{
	pagegroup.clear();
	spacegroup.clear();
	textblockgroup.clear();
	textlinegroup.clear();
	wordgroup.clear();
	update_boxes();
}

void Alto::update_boxes()
{
	doc.get_image().clear_overlay(pageList);
	doc.get_image().clear_overlay(spaceList);
	doc.get_image().clear_overlay(blockList);
	doc.get_image().clear_overlay(lineList);
	doc.get_image().clear_overlay(wordList);
	current_alto = nullptr;
	if (doc.get_document() && doc.get_selected_view_id().IsNotEmpty())
	{
		try
		{
			current_alto = alto->GetView(doc.get_selected_view_id()).GetAlto();
		}
		catch (...)
		{ }
		crn::String vid(doc.get_selected_view_id());
		if (vid.IsNotEmpty())
		{
			crn::xml::AltoWrapper::View aview(alto->GetView(vid));
			const std::vector<crn::xml::Id> pages(aview.GetPages());
			for (const crn::xml::Id &pid : pages)
			{ // add pages
				crn::xml::AltoWrapper::Page apage(aview.GetPage(pid));
				doc.get_image().add_overlay_item(pageList, apage.GetId(), apage.GetBBox());
				const std::vector<crn::xml::Id> spaces(apage.GetSpaces());
				for (const crn::xml::Id &sid : spaces)
				{ // add spaces
					crn::xml::AltoWrapper::Space aspace(apage.GetSpace(sid));
					doc.get_image().add_overlay_item(spaceList, aspace.GetId(), aspace.GetBBox());
					const std::vector<crn::xml::Id> blocks(aspace.GetTextBlocks());
					for (const crn::xml::Id &bid : blocks)
					{ // add text blocks
						crn::xml::AltoWrapper::TextBlock ablock(aspace.GetTextBlock(bid));
						doc.get_image().add_overlay_item(blockList, ablock.GetId(), ablock.GetBBox());
						const std::vector<crn::xml::Id> lines(ablock.GetTextLines());
						for (const crn::xml::Id &lid : lines)
						{ // add lines
							crn::xml::AltoWrapper::TextLine aline(ablock.GetTextLine(lid));
							doc.get_image().add_overlay_item(lineList, aline.GetId(), aline.GetBBox());
							const std::vector<crn::xml::Id> words(aline.GetWords());
							for (const crn::xml::Id &wid : words)
							{ // add words
								crn::xml::AltoWrapper::Word aword(aline.GetWord(wid));
								doc.get_image().add_overlay_item(wordList, aword.GetId(), aword.GetBBox(), aword.GetContent());
							} // add words
						} // add lines
					} // add text blocks
				} // add spaces
			} // add pages
		} // a view is selected
	} // a document is loaded
}

void Alto::show_hide_pages()
{
	showpages = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-pages"))->get_active();
	doc.get_image().set_overlay_visible(pageList, showpages);
}

void Alto::show_hide_spaces()
{
	showspaces = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-spaces"))->get_active();
	doc.get_image().set_overlay_visible(spaceList, showspaces);
}

void Alto::show_hide_blocks()
{
	showtextblocks = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-textblocks"))->get_active();
	doc.get_image().set_overlay_visible(blockList, showtextblocks);
}

void Alto::show_hide_lines()
{
	showtextlines = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-textlines"))->get_active();
	doc.get_image().set_overlay_visible(lineList, showtextlines);
}

void Alto::show_hide_words()
{
	showwords = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-words"))->get_active();
	doc.get_image().set_overlay_visible(wordList, showwords);
}

void Alto::toggle_edit()
{
	edit_mode = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-edit"))->get_active();
	doc.get_image().set_selection_type(edit_mode ? GtkCRN::Image::Overlay::Rectangle : GtkCRN::Image::Overlay::None);
	//doc.get_image().get_overlay_config(pageList).editable = edit_mode;
	//doc.get_image().get_overlay_config(spaceList).editable = edit_mode;
	//doc.get_image().get_overlay_config(blockList).editable = edit_mode;
	//doc.get_image().get_overlay_config(lineList).editable = edit_mode;
	doc.get_image().get_overlay_config(wordList).editable = edit_mode;
}

void Alto::on_rmb_clicked(guint mouse_button, guint32 time, std::vector<std::pair<crn::String, crn::String> > overlay_items_under_mouse, int x, int y)
{
	if (edit_mode && current_alto->GetLayout().GetPages().empty())
	{
		popup.reset(new Gtk::Menu);
		Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Create page")));
		mit->signal_activate().connect(sigc::mem_fun(this, &Alto::add_page));
		popup->append(*mit);
		popup->show_all();
		popup->popup(mouse_button, time);
		return;
	}
	if (edit_mode && !overlay_items_under_mouse.empty())
	{
		// count number of submenus to display
		int nmenus = 0;
		for (std::vector<std::pair<crn::String, crn::String> >::iterator it = overlay_items_under_mouse.begin(); it != overlay_items_under_mouse.end(); ++it)
		{
			if ((it->first == pageList) && showpages)
				nmenus += 1;
			else if ((it->first == spaceList) && showspaces)
				nmenus += 1;
			else if ((it->first == blockList) && showtextblocks)
				nmenus += 1;
			else if ((it->first == lineList) && showtextlines)
				nmenus += 1;
			else if ((it->first == wordList) && showwords)
				nmenus += 1;
		}
		if (nmenus == 0)
			return;
		// create new menu
		popup.reset(new Gtk::Menu);
		Gtk::Menu *addto = popup.get();
		for (std::vector<std::pair<crn::String, crn::String> >::iterator it = overlay_items_under_mouse.begin(); it != overlay_items_under_mouse.end(); ++it)
		{ // for each zone under the mouse
			if ((it->first == pageList) && showpages)
			{ // page
				Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Page")));
				popup->append(*mit);
				if (nmenus == 1)
					mit->set_sensitive(false);
				else
				{
					addto = Gtk::manage(new Gtk::Menu);
					mit->set_submenu(*addto);
				}
				mit = Gtk::manage(new Gtk::MenuItem(_("Delete")));
				mit->signal_activate().connect(sigc::bind(sigc::mem_fun(this, &Alto::delete_page), crn::xml::Id(it->second)));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut vertically here")));
				mit->signal_activate().connect(sigc::bind(sigc::mem_fun(this, &Alto::split_page), crn::xml::Id(it->second), x));
				addto->append(*mit);
				crn::StringUTF8 label;
				if (pagegroup.empty())
				{
					label = _("Create group");
				}
				else
				{
					label = _("Add to group");
					label += " (";
					label += pagegroup.size();
					label += ")";
				}
				mit = Gtk::manage(new Gtk::MenuItem(label.CStr()));
				mit->signal_activate().connect(sigc::bind(sigc::ptr_fun(&Alto::add_to_group), &pagegroup, crn::xml::Id(it->second)));
				if (pagegroup.count(crn::xml::Id(it->second)))
					mit->set_sensitive(false);
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Merge group")));
				mit->signal_activate().connect(sigc::mem_fun(this, &Alto::merge_pages));
				mit->set_sensitive(pagegroup.size() > 1);
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Clear group")));
				mit->signal_activate().connect(sigc::bind(sigc::ptr_fun(&Alto::clear_group), &pagegroup));
				mit->set_sensitive(!pagegroup.empty());
				addto->append(*mit);
			}
			else if ((it->first == spaceList) && showspaces)
			{ // print space
				Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Print space")));
				popup->append(*mit);
				if (nmenus == 1)
					mit->set_sensitive(false);
				else
				{
					addto = Gtk::manage(new Gtk::Menu);
					mit->set_submenu(*addto);
				}
				mit = Gtk::manage(new Gtk::MenuItem(_("Delete")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut vertically here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut horizontally here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Add to group")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Merge group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Clear group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
			}
			else if ((it->first == blockList) && showtextblocks)
			{ // text block
				Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Text block")));
				popup->append(*mit);
				if (nmenus == 1)
					mit->set_sensitive(false);
				else
				{
					addto = Gtk::manage(new Gtk::Menu);
					mit->set_submenu(*addto);
				}
				mit = Gtk::manage(new Gtk::MenuItem(_("Delete")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut vertically here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut horizontally here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Add to group")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Merge group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Clear group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
			}
			else if ((it->first == lineList) && showtextlines)
			{ // text line
				Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Text line")));
				popup->append(*mit);
				if (nmenus == 1)
					mit->set_sensitive(false);
				else
				{
					addto = Gtk::manage(new Gtk::Menu);
					mit->set_submenu(*addto);
				}
				mit = Gtk::manage(new Gtk::MenuItem(_("Edit")));
				const crn::xml::AltoTextLine &line(current_alto->GetTextLine(it->second.CStr()));
				mit->signal_activate().connect(sigc::bind(sigc::mem_fun(this, &Alto::edit_line), line.GetId()));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Delete")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut vertically here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut horizontally here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Add to group")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Merge group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Clear group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
			}
			else if ((it->first == wordList) && showwords)
			{ // word/string
				crn::StringUTF8 label(_("Word"));
				label += " (";
				const crn::xml::AltoWord &word(current_alto->GetWord(it->second.CStr()));
				crn::StringUTF8 trans(word.GetContent());
				label += trans.IsEmpty() ? _("No transcription") : trans;
				label += ")";
				Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(label.CStr()));
				popup->append(*mit);
				if (nmenus == 1)
					mit->set_sensitive(false);
				else
				{
					addto = Gtk::manage(new Gtk::Menu);
					mit->set_submenu(*addto);
				}
				mit = Gtk::manage(new Gtk::MenuItem(_("Edit")));
				mit->signal_activate().connect(sigc::bind(sigc::mem_fun(this, &Alto::edit_word), word.GetId().Get()));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Delete")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut vertically here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Cut horizontally here")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Add to group")));
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Merge group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
				mit = Gtk::manage(new Gtk::MenuItem(_("Clear group")));
				mit->set_sensitive(false); // TODO
				addto->append(*mit);
			}
			else if (it->first == GtkCRN::Image::selection_overlay())
			{
				addto->prepend(*Gtk::manage(new Gtk::SeparatorMenuItem));
				if (showwords)
				{
					Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Add word")));
					addto->prepend(*mit);
				}
				if (showtextlines)
				{
					Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Add text line")));
					addto->prepend(*mit);
				}
				if (showtextblocks)
				{
					Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Add text block")));
					addto->prepend(*mit);
				}
				if (showspaces)
				{
					Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Add print space")));
					addto->prepend(*mit);
				}
			}
		}
		popup->show_all();
		popup->popup(mouse_button, time);
	}
}

void Alto::add_page()
{
	try
	{
		crn::String vid(doc.get_selected_view_id());
		crn::SBlock bv(doc.get_document()->GetView(vid));
		alto->GetView(vid).AddPage((int)doc.get_document()->GetViewIndex(vid), bv->GetAbsoluteBBox().GetWidth(), bv->GetAbsoluteBBox().GetHeight(), crn::xml::Alto::Layout::Page::Position::Single);
		update_boxes();
	}
	catch (std::exception &ex)
	{
		GtkCRN::App::show_message(_("Cannot add page."), Gtk::MESSAGE_ERROR);
	}
}

void Alto::delete_page(const crn::xml::Id &id)
{
	try
	{
		crn::String vid(doc.get_selected_view_id());
		alto->GetView(vid).RemovePage(id);
		update_boxes();
	}
	catch (...)
	{
		GtkCRN::App::show_message(_("Cannot remove page."), Gtk::MESSAGE_ERROR);
	}
}

void Alto::split_page(const crn::xml::Id &id, int x)
{
	throw crn::ExceptionTODO();
	// créer deux nouvelles pages
	crn::String vid(doc.get_selected_view_id());
	crn::SBlock bv(doc.get_document()->GetView(vid));
	crn::Rect bbox1(bv->GetAbsoluteBBox()), bbox2(bv->GetAbsoluteBBox());
	bbox1.SetRight(x - 1);
	bbox2.SetLeft(x);
	crn::xml::AltoWrapper::Page p1(alto->GetView(vid).AddPage((int)doc.get_document()->GetViewIndex(vid), bbox1.GetWidth(), bbox1.GetHeight(), crn::xml::Alto::Layout::Page::Position::Left));
	crn::xml::AltoWrapper::Page p2(alto->GetView(vid).AddPage((int)doc.get_document()->GetViewIndex(vid), bbox2.GetWidth(), bbox2.GetHeight(), crn::xml::Alto::Layout::Page::Position::Right));
	// déplacer le contenu de la page dans les deux nouvelles
	// TODO
	throw crn::ExceptionTODO();
	crn::xml::AltoWrapper::Page oldp(alto->GetView(vid).GetPage(id)); // should not throw
	try
	{
		crn::xml::AltoWrapper::Space sp(oldp.GetTopMargin());
		// TODO
	}
	catch (crn::ExceptionNotFound&) { } // no top margin
	// supprimer la vieille page
	update_boxes();
}

void Alto::merge_pages()
{
	throw crn::ExceptionTODO();
	// TODO fusionner les print spaces un à un
	// TODO fusionner les styles des pages et des print spaces
	// TODO mettre à jour la position de la page
	// TODO effacer la seconde
}

void Alto::edit_word(const crn::xml::Id &id)
{
	AltoWordEditor ed(const_cast<crn::xml::Alto&>(*current_alto), const_cast<crn::xml::AltoWord&>(current_alto->GetWord(id)), dynamic_cast<Gtk::Window*>(get_toplevel()));
	ed.run();
}

void Alto::edit_line(const crn::xml::Id &id)
{
	AltoTextLineEditor ed(const_cast<crn::xml::Alto&>(*current_alto), const_cast<crn::xml::AltoTextLine&>(current_alto->GetTextLine(id)), dynamic_cast<Gtk::Window*>(get_toplevel()));
	ed.run();
}

void Alto::on_overlay_changed(crn::String overlay_id, crn::String overlay_item_id, GtkCRN::Image::MouseMode mm)
{
	if (overlay_item_id.IsEmpty())
		return;

	if (overlay_id == pageList)
	{
		// TODO too shitty
	}
	else if (overlay_id == spaceList)
	{
		// TODO too shitty
	}
	else if (overlay_id == blockList)
	{
		// TODO too shitty
	}
	else if (overlay_id == lineList)
	{
		// TODO too shitty
	}
	else if (overlay_id == wordList)
	{
		try
		{
			crn::xml::AltoWord &word = const_cast<crn::xml::AltoWord&>(current_alto->GetWord(overlay_item_id.CStr()));
		}
		catch (...)
		{ }
	}
}

#endif

