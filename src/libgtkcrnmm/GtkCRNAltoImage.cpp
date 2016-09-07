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
 * file: GdkCRNAltoImage.cpp
 * \author Yann LEYDIER
 */

#ifndef CRN_USING_GTKMM3
#include <GtkCRNAltoImage.h>
#include <GdkCRNPixbuf.h>
#include <CRNi18n.h>

using namespace GtkCRN;

const crn::String AltoImage::pageList(U"nimrod::4pages");
const crn::String AltoImage::spaceList(U"nimrod::3spaces");
const crn::String AltoImage::blockList(U"nimrod::2blocks");
const crn::String AltoImage::lineList(U"nimrod::1lines");
const crn::String AltoImage::wordList(U"nimrod::0words");

AltoImage::AltoImage(Image *image):
	actions(Gtk::ActionGroup::create("alto image")),
	img(nullptr),
	can_edit_words(true), 
	can_edit_lines(true), 
	can_edit_blocks(true), 
	can_edit_spaces(true), 
	can_edit_pages(true)
{
	actions->add(Gtk::ToggleAction::create("alto-pages", Gtk::StockID("gtk-crn-two-pages"), _("Show/hide _pages"), _("Show/hide pages")), sigc::mem_fun(this, &AltoImage::show_hide_pages));
	actions->add(Gtk::ToggleAction::create("alto-spaces", Gtk::StockID("gtk-crn-block"), _("Show/hide _spaces"), _("Show/hide spaces")), sigc::mem_fun(this, &AltoImage::show_hide_spaces));
	actions->add(Gtk::ToggleAction::create("alto-textblocks", Gtk::StockID("gtk-crn-paragraph"), _("Show/hide _text blocks"), _("Show/hide text blocks")), sigc::mem_fun(this, &AltoImage::show_hide_blocks));
	actions->add(Gtk::ToggleAction::create("alto-textlines", Gtk::Stock::JUSTIFY_LEFT, _("Show/hide text _lines"), _("Show/hide text lines")), sigc::mem_fun(this, &AltoImage::show_hide_lines));
	actions->add(Gtk::ToggleAction::create("alto-words", Gtk::Stock::ITALIC, _("Show/hide _words"), _("Show/hide words")), sigc::mem_fun(this, &AltoImage::show_hide_words));
	actions->add(Gtk::ToggleAction::create("alto-edit", Gtk::Stock::EDIT, _("_Edit alto"), _("Edit alto")), sigc::mem_fun(this, &AltoImage::toggle_edit));
	Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-edit"))->set_active(false);
	
	set_image(image);
}

AltoImage::~AltoImage()
{
	set_image(nullptr);
}

/*! Sets the Image widget to wrap around. The previously wrapped image is unconfigured
 * \param[in]	image	the width to wrap or nullptr
 */
void AltoImage::set_image(Image *image)
{
	if (img)
	{ // remove configuration
		img->clear_overlay(pageList);
		img->clear_overlay(spaceList);
		img->clear_overlay(blockList);
		img->clear_overlay(lineList);
		img->clear_overlay(wordList);
		
		rmb_connect.disconnect();
		overlay_connect.disconnect();
	}
	img = image;
	if (img)
	{ // configure Image
		// pages in blue
		crn::pixel::RGB8 pcol1(0, 0, 255);
		crn::pixel::RGB8 pcol2(0, 0, 127);
		img->get_overlay_config(pageList).color1 = GdkCRN::ColorFromCRNPixel(pcol1);
		img->get_overlay_config(pageList).color2 = GdkCRN::ColorFromCRNPixel(pcol2);
		img->get_overlay_config(pageList).text_color = GdkCRN::ColorFromCRNPixel(pcol2);
		img->get_overlay_config(pageList).fill = false;
		img->get_overlay_config(pageList).show_labels = false;
		// spaces in cyan
		crn::pixel::RGB8 scol1(0, 255, 255);
		crn::pixel::RGB8 scol2(0, 127, 127);
		img->get_overlay_config(spaceList).color1 = GdkCRN::ColorFromCRNPixel(scol1);
		img->get_overlay_config(spaceList).color2 = GdkCRN::ColorFromCRNPixel(scol2);
		img->get_overlay_config(spaceList).text_color = GdkCRN::ColorFromCRNPixel(scol2);
		img->get_overlay_config(spaceList).fill = false;
		img->get_overlay_config(spaceList).show_labels = false;
		// blocks in red
		crn::pixel::RGB8 bcol1(255, 0, 0);
		crn::pixel::RGB8 bcol2(127, 0, 0);
		img->get_overlay_config(blockList).color1 = GdkCRN::ColorFromCRNPixel(bcol1);
		img->get_overlay_config(blockList).color2 = GdkCRN::ColorFromCRNPixel(bcol2);
		img->get_overlay_config(blockList).text_color = GdkCRN::ColorFromCRNPixel(bcol2);
		img->get_overlay_config(blockList).fill = false;
		img->get_overlay_config(blockList).show_labels = false;
		// lines in yellow
		crn::pixel::RGB8 lcol1(255, 255, 0);
		crn::pixel::RGB8 lcol2(127, 127, 0);
		img->get_overlay_config(lineList).color1 = GdkCRN::ColorFromCRNPixel(lcol1);
		img->get_overlay_config(lineList).color2 = GdkCRN::ColorFromCRNPixel(lcol2);
		img->get_overlay_config(lineList).text_color = GdkCRN::ColorFromCRNPixel(lcol2);
		img->get_overlay_config(lineList).fill = false;
		img->get_overlay_config(lineList).show_labels = false;
		// words in green
		crn::pixel::RGB8 wcol1(0, 255, 0);
		crn::pixel::RGB8 wcol2(0, 127, 0);
		img->get_overlay_config(wordList).color1 = GdkCRN::ColorFromCRNPixel(wcol1);
		img->get_overlay_config(wordList).color2 = GdkCRN::ColorFromCRNPixel(wcol2);
		img->get_overlay_config(wordList).text_color = GdkCRN::ColorFromCRNPixel(wcol2);
		img->get_overlay_config(wordList).fill = false;
		img->get_overlay_config(wordList).show_labels = true;
		
		// setup overlays
		show_hide_pages();
		show_hide_spaces();
		show_hide_blocks();
		show_hide_lines();
		show_hide_words();
		toggle_edit();

		// signals
		rmb_connect = img->signal_rmb_clicked().connect(sigc::mem_fun(this, &AltoImage::on_rmb_clicked));
		overlay_connect = img->signal_overlay_changed().connect(sigc::mem_fun(this, &AltoImage::on_overlay_changed));
	}
}

/*! Sets the alto view
 * \param[in]	v	the view
 * \param[in]	display_image	shall the AltoImage set the wrapped Image or was it done before (e.g. by GtkCRN::Document)
 */
void AltoImage::set_view(const crn::xml::AltoWrapper::View &v, bool display_image)
{
	if (display_image && img)
	{
		img->set_pixbuf(GdkCRN::PixbufFromCRNImage(*const_cast<crn::xml::AltoWrapper::View&>(v).GetBlock()->GetRGB()));
	}
	view.reset(new crn::xml::AltoWrapper::View(v));
	set_overlays();
}

/*! Unsets the alto view
 * \param[in]	clear_image	shall the AltoImage set the wrapped Image or was it done before (e.g. by GtkCRN::Document)
 */
void AltoImage::unset_view(bool clear_image)
{
	if (clear_image && img)
	{
		img->set_pixbuf(Glib::RefPtr<Gdk::Pixbuf>(nullptr));
	}
	view.reset();
	set_overlays();
}

void AltoImage::show_hide_pages()
{
	showpages = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-pages"))->get_active();
	if (img)
		img->set_overlay_visible(pageList, showpages);
}

void AltoImage::show_hide_spaces()
{
	showspaces = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-spaces"))->get_active();
	if (img)
		img->set_overlay_visible(spaceList, showspaces);
}

void AltoImage::show_hide_blocks()
{
	showtextblocks = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-textblocks"))->get_active();
	if (img)
		img->set_overlay_visible(blockList, showtextblocks);
}

void AltoImage::show_hide_lines()
{
	showtextlines = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-textlines"))->get_active();
	if (img)
		img->set_overlay_visible(lineList, showtextlines);
}

void AltoImage::show_hide_words()
{
	showwords = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-words"))->get_active();
	if (img)
		img->set_overlay_visible(wordList, showwords);
}

void AltoImage::toggle_edit()
{
	edit_mode = Glib::RefPtr<Gtk::ToggleAction>::cast_dynamic(actions->get_action("alto-edit"))->get_active();
	if (img)
	{
		img->set_selection_type(edit_mode ? GtkCRN::Image::Overlay::Rectangle : GtkCRN::Image::Overlay::None);
		img->get_overlay_config(spaceList).editable = edit_mode && can_edit_spaces;
		img->get_overlay_config(blockList).editable = edit_mode && can_edit_blocks;
		img->get_overlay_config(lineList).editable = edit_mode && can_edit_lines;
		img->get_overlay_config(wordList).editable = edit_mode && can_edit_words;
	}
}

void AltoImage::on_rmb_clicked(guint mouse_button, guint32 time, std::vector<std::pair<crn::String, crn::String> > overlay_items_under_mouse, int x, int y)
{
	if (!edit_mode)
		return;
	popup.reset(new Gtk::Menu);
	bool ok = false;
	for (size_t tmp = 0; tmp < overlay_items_under_mouse.size(); ++tmp)
	{
		if (overlay_items_under_mouse[tmp].first == GtkCRN::Image::selection_overlay())
		{
			// TODO add lines, blokcs and spaces (and pages?)
			if (can_edit_words)
			{
				Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Add word")));
				mit->signal_activate().connect(sigc::mem_fun(this, &AltoImage::add_word));
				popup->append(*mit);
				ok = true;
			}
		}
		// TODO check for lines, blocks and spaces (and pages?)
		else if ((overlay_items_under_mouse[tmp].first == wordList) && can_edit_words)
		{
			Gtk::MenuItem *mit = Gtk::manage(new Gtk::MenuItem(_("Delete word")));
			mit->signal_activate().connect(sigc::bind(sigc::mem_fun(this, &AltoImage::delete_word), overlay_items_under_mouse[tmp].second));
			popup->append(*mit);
			ok = true;
		}
	}
	if (ok)
	{
		popup->show_all();
		popup->popup(mouse_button, time);
	}
}

void AltoImage::on_overlay_changed(crn::String overlay_id, crn::String overlay_item_id, GtkCRN::Image::MouseMode mm)
{
	if (!view)
		return;
	if (overlay_item_id.IsEmpty())
		return; // send by clear()

	if (overlay_id == spaceList)
	{
		try
		{
			crn::xml::SpacePath path(overlay_item_id);
			crn::xml::AltoWrapper::Page page(view->GetPage(path.page_id));
			crn::xml::AltoWrapper::Space space(page.GetSpace(path.space_id));
			Image::Rectangle &rect(static_cast<Image::Rectangle&>(img->get_overlay_item(overlay_id, overlay_item_id)));
			view->ResizeSpace(path, rect.rect, false);
			// update the space's box (in case of a crop)
			img->add_overlay_item(spaceList, path.ToString(), space.GetBBox());
			// refresh all parent boxes
			img->add_overlay_item(pageList, crn::xml::PagePath(path).ToString(), page.GetBBox());
			// refresh content
			const std::vector<crn::xml::Id> blocks(space.GetTextBlocks());
			for (const crn::xml::Id &bid : blocks)
			{
				crn::xml::AltoWrapper::TextBlock tb(space.GetTextBlock(bid));
				img->add_overlay_item(blockList, tb.GetPath().ToString(), tb.GetBBox());
				const std::vector<crn::xml::Id> lines(tb.GetTextLines());
				for (const crn::xml::Id &lid : lines)
				{
					crn::xml::AltoWrapper::TextLine tl(tb.GetTextLine(lid));
					img->add_overlay_item(lineList, tl.GetPath().ToString(), tl.GetBBox());
					const std::vector<crn::xml::Id> words(tl.GetWords());
					for (const crn::xml::Id &wid : words)
					{
						crn::xml::AltoWrapper::Word w(tl.GetWord(wid));
						img->add_overlay_item(wordList, w.GetPath().ToString(), w.GetBBox(), w.GetContent());
						word_resized.emit(w.GetPath());
					}
				}
			}
		}
		catch (crn::ExceptionNotFound&)
		{ }
		catch (crn::ExceptionDomain&)
		{
			// update the space's box (no changes were made)
			crn::xml::SpacePath path(overlay_item_id);
			crn::xml::AltoWrapper::Space space(view->GetPage(path.page_id).GetSpace(path.space_id)); // should not throw
			img->add_overlay_item(spaceList, overlay_item_id, space.GetBBox());
		}
	}
	else if (overlay_id == blockList)
	{
		try
		{
			crn::xml::BlockPath path(overlay_item_id);
			crn::xml::AltoWrapper::Page page(view->GetPage(path.page_id));
			crn::xml::AltoWrapper::Space space(page.GetSpace(path.space_id));
			crn::xml::AltoWrapper::TextBlock tb(space.GetTextBlock(path.block_id));
			Image::Rectangle &rect(static_cast<Image::Rectangle&>(img->get_overlay_item(overlay_id, overlay_item_id)));
			view->ResizeTextBlock(path, rect.rect, false);
			// update the block's box (in case of a crop)
			img->add_overlay_item(blockList, path.ToString(), tb.GetBBox());
			// refresh all parent boxes
			img->add_overlay_item(pageList, crn::xml::PagePath(path).ToString(), page.GetBBox());
			img->add_overlay_item(spaceList, crn::xml::SpacePath(path).ToString(), space.GetBBox());
			// refresh content
			const std::vector<crn::xml::Id> lines(tb.GetTextLines());
			for (const crn::xml::Id &lid : lines)
			{
				crn::xml::AltoWrapper::TextLine tl(tb.GetTextLine(lid));
				img->add_overlay_item(lineList, tl.GetPath().ToString(), tl.GetBBox());
				const std::vector<crn::xml::Id> words(tl.GetWords());
				for (const crn::xml::Id &wid : words)
				{
					crn::xml::AltoWrapper::Word w(tl.GetWord(wid));
					img->add_overlay_item(wordList, w.GetPath().ToString(), w.GetBBox(), w.GetContent());
					word_resized.emit(w.GetPath());
				}
			}
		}
		catch (crn::ExceptionNotFound&)
		{ }
		catch (crn::ExceptionDomain&)
		{
			// update the block's box (no changes were made)
			crn::xml::BlockPath path(overlay_item_id);
			crn::xml::AltoWrapper::TextBlock tb(view->GetPage(path.page_id).GetSpace(path.space_id).GetTextBlock(path.block_id)); // should not throw
			img->add_overlay_item(blockList, overlay_item_id, tb.GetBBox());
		}
	}
	else if (overlay_id == lineList)
	{
		try
		{
			crn::xml::TextLinePath path(overlay_item_id);
			crn::xml::AltoWrapper::Page page(view->GetPage(path.page_id));
			crn::xml::AltoWrapper::Space space(page.GetSpace(path.space_id));
			crn::xml::AltoWrapper::TextBlock tb(space.GetTextBlock(path.block_id));
			crn::xml::AltoWrapper::TextLine tl(tb.GetTextLine(path.textline_id));
			Image::Rectangle &rect(static_cast<Image::Rectangle&>(img->get_overlay_item(overlay_id, overlay_item_id)));
			view->ResizeTextLine(path, rect.rect, false);
			// update the line's box (in case of a crop)
			img->add_overlay_item(lineList, path.ToString(), tl.GetBBox());
			// refresh all parent boxes
			img->add_overlay_item(pageList, crn::xml::PagePath(path).ToString(), page.GetBBox());
			img->add_overlay_item(spaceList, crn::xml::SpacePath(path).ToString(), space.GetBBox());
			img->add_overlay_item(blockList, crn::xml::BlockPath(path).ToString(), tb.GetBBox());
			// refresh content
			const std::vector<crn::xml::Id> words(tl.GetWords());
			for (const crn::xml::Id &wid : words)
			{
				crn::xml::AltoWrapper::Word w(tl.GetWord(wid));
				img->add_overlay_item(wordList, w.GetPath().ToString(), w.GetBBox(), w.GetContent());
				word_resized.emit(w.GetPath());
			}
		}
		catch (crn::ExceptionNotFound&)
		{ }
		catch (crn::ExceptionDomain&)
		{
			// update the line's box (no changes were made)
			crn::xml::TextLinePath path(overlay_item_id);
			crn::xml::AltoWrapper::TextLine tl(view->GetPage(path.page_id).GetSpace(path.space_id).GetTextBlock(path.block_id).GetTextLine(path.textline_id)); // should not throw
			img->add_overlay_item(lineList, overlay_item_id, tl.GetBBox());
		}
	}
	else if (overlay_id == wordList)
	{
		try
		{
			crn::xml::WordPath path(overlay_item_id);
			crn::xml::AltoWrapper::Page page(view->GetPage(path.page_id));
			crn::xml::AltoWrapper::Space space(page.GetSpace(path.space_id));
			crn::xml::AltoWrapper::TextBlock tb(space.GetTextBlock(path.block_id));
			crn::xml::AltoWrapper::TextLine tl(tb.GetTextLine(path.textline_id));
			crn::xml::AltoWrapper::Word w(tl.GetWord(path.word_id));
			Image::Rectangle &rect(static_cast<Image::Rectangle&>(img->get_overlay_item(overlay_id, overlay_item_id)));
			view->ResizeWord(path, rect.rect);
			// update the word's box (in case of a crop)
			img->add_overlay_item(wordList, path.ToString(), w.GetBBox(), w.GetContent());
			// refresh all parent boxes
			img->add_overlay_item(pageList, crn::xml::PagePath(path).ToString(), page.GetBBox());
			img->add_overlay_item(spaceList, crn::xml::SpacePath(path).ToString(), space.GetBBox());
			img->add_overlay_item(blockList, crn::xml::BlockPath(path).ToString(), tb.GetBBox());
			img->add_overlay_item(lineList, crn::xml::TextLinePath(path).ToString(), tl.GetBBox());

			word_resized.emit(path);
		}
		catch (crn::ExceptionNotFound&)
		{ }
	}
}

void AltoImage::set_overlays()
{
	if (!img)
		return;
	img->clear_overlay(pageList);
	img->clear_overlay(spaceList);
	img->clear_overlay(blockList);
	img->clear_overlay(lineList);
	img->clear_overlay(wordList);
	if (view)
	{
		const std::vector<crn::xml::Id> pages(view->GetPages());
		for (const crn::xml::Id &pid : pages)
		{ // add pages
			crn::xml::AltoWrapper::Page apage(view->GetPage(pid));
			img->add_overlay_item(pageList, apage.GetPath().ToString(), apage.GetBBox());
			const std::vector<crn::xml::Id> spaces(apage.GetSpaces());
			for (const crn::xml::Id &sid : spaces)
			{ // add spaces
				crn::xml::AltoWrapper::Space aspace(apage.GetSpace(sid));
				img->add_overlay_item(spaceList, aspace.GetPath().ToString(), aspace.GetBBox());
				const std::vector<crn::xml::Id> blocks(aspace.GetTextBlocks());
				for (const crn::xml::Id &bid : blocks)
				{ // add text blocks
					crn::xml::AltoWrapper::TextBlock ablock(aspace.GetTextBlock(bid));
					img->add_overlay_item(blockList, ablock.GetPath().ToString(), ablock.GetBBox());
					const std::vector<crn::xml::Id> lines(ablock.GetTextLines());
					for (const crn::xml::Id &lid : lines)
					{ // add lines
						crn::xml::AltoWrapper::TextLine aline(ablock.GetTextLine(lid));
						img->add_overlay_item(lineList, aline.GetPath().ToString(), aline.GetBBox());
						const std::vector<crn::xml::Id> words(aline.GetWords());
						for (const crn::xml::Id &wid : words)
						{ // add words
							crn::xml::AltoWrapper::Word aword(aline.GetWord(wid));
							img->add_overlay_item(wordList, aword.GetPath().ToString(), aword.GetBBox(), aword.GetContent());
						} // add words
					} // add lines
				} // add text blocks
			} // add spaces
		} // add pages
	} // a view is displayed
}

void AltoImage::delete_word(crn::String spath)
{
	try
	{
		crn::xml::WordPath path(spath);
		crn::xml::AltoWrapper::Page page(view->GetPage(path.page_id));
		crn::xml::AltoWrapper::Space space(page.GetSpace(path.space_id));
		crn::xml::AltoWrapper::TextBlock tb(space.GetTextBlock(path.block_id));
		crn::xml::AltoWrapper::TextLine tl(tb.GetTextLine(path.textline_id));
		tl.RemoveWord(path.word_id);
		img->remove_overlay_item(wordList, spath);
		word_deleted.emit(path);
	}
	catch (...)
	{ }
}

void AltoImage::add_word()
{
	if (view)
	{
		crn::Rect bbox(img->get_selection_as_rect());
		// find where to add it...
		const std::vector<crn::xml::Id> pages(view->GetPages());
		for (const crn::xml::Id &pid : pages)
		{ // pages
			crn::xml::AltoWrapper::Page apage(view->GetPage(pid));
			if (!(apage.GetBBox() & bbox).IsValid())
				continue;
			const std::vector<crn::xml::Id> spaces(apage.GetSpaces());
			for (const crn::xml::Id &sid : spaces)
			{ // spaces
				crn::xml::AltoWrapper::Space aspace(apage.GetSpace(sid));
				if (!(aspace.GetBBox() & bbox).IsValid())
					continue;
				const std::vector<crn::xml::Id> blocks(aspace.GetTextBlocks());
				for (const crn::xml::Id &bid : blocks)
				{ // text blocks
					crn::xml::AltoWrapper::TextBlock ablock(aspace.GetTextBlock(bid));
					if (!(ablock.GetBBox() & bbox).IsValid())
						continue;
					const std::vector<crn::xml::Id> lines(ablock.GetTextLines());
					for (const crn::xml::Id &lid : lines)
					{ // lines
						crn::xml::AltoWrapper::TextLine aline(ablock.GetTextLine(lid));
						if (!(aline.GetBBox() & bbox).IsValid())
							continue;

						// this is the line!
						const bool horiz = aline.GetBBox().GetWidth() > aline.GetBBox().GetHeight();
						const std::vector<crn::xml::Id> words(aline.GetWords());
						bool found = false;
						crn::xml::Id nextword;
						for (const crn::xml::Id &wid : words)
						{ // words
							crn::xml::AltoWrapper::Word aword(aline.GetWord(wid));
							if (horiz)
							{
								if (aword.GetBBox().GetLeft() > bbox.GetLeft())
								{
									nextword = aword.GetId();
									break;
								}
							}
							else
							{
								if (aword.GetBBox().GetTop() > bbox.GetTop())
								{
									nextword = aword.GetId();
									break;
								}
							}
						} // words
						// resize line if needed
						crn::Rect linebox(aline.GetBBox() | bbox);
						if (linebox != aline.GetBBox())
						{ // strech the line
							GtkCRN::Image::Rectangle &item(static_cast<Image::Rectangle&>(img->get_overlay_item(lineList, aline.GetPath().ToString())));
							item.rect = linebox;
							img->force_redraw();
							view->ResizeTextLine(aline.GetPath(), linebox, false);
							// update display
							on_overlay_changed(lineList, aline.GetPath().ToString(), GtkCRN::Image::MouseMode::STRETCH_BOTTOM_RIGHT);
						}
						// add word
						crn::xml::WordPath path;
						if (nextword.IsNotEmpty())
						{
							crn::xml::AltoWrapper::Word newword(aline.AddWordBefore(nextword, "", bbox));
							img->add_overlay_item(wordList, newword.GetPath().ToString(), newword.GetBBox(), newword.GetContent());
							path = newword.GetPath();
						}
						else
						{
							crn::xml::AltoWrapper::Word newword(aline.AddWord("", bbox));
							img->add_overlay_item(wordList, newword.GetPath().ToString(), newword.GetBBox(), newword.GetContent());
							path = newword.GetPath();
						}
						word_added.emit(path);
						img->clear_selection();
						return;
					} // lines
				} // text blocks
			} // spaces
		} // pages
	} // a view is displayed
}

#endif

