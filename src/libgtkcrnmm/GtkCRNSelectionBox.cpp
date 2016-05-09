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
 * file: GtkCRNSelectionBox.cpp
 * \author Yann LEYDIER
 */

#include <libgtkcrnmm_config.h>
#include <CRNi18n.h>
#include <GtkCRNSelectionBox.h>
#include <CRN.h>
#include <CRNIO/CRNIO.h>
#include <CRNMath/CRNMath.h>
#include <CRNData/CRNForeach.h>

using namespace GtkCRN;

const crn::String SelectionBox::reorderKey(U"GtkCRN::SelectionBox::Reorder");
const int SelectionBox::reorderId(9);
const int SelectionBox::dropId(4);

/*! Constructor
 *
 * \throws	Glib::OptionError	invalid orientation
 *
 * \param[in]	ori	the orientation of the box
 */
SelectionBox::SelectionBox(crn::Orientation ori):
	orientation(ori),
	can_reorder(true),
	first_drop_zone(sigc::mem_fun(this, &SelectionBox::drop), this, nullptr),
	last_drop_zone(sigc::mem_fun(this, &SelectionBox::drop), this, (Element*)(-1)),
	shift_key(false),
	control_key(false),
	selecting(false),
	vscrolldiv(0),
	hscrolldiv(0)
{
	signal_key_press_event().connect(sigc::mem_fun(this, &SelectionBox::keyevents), true);
	signal_key_release_event().connect(sigc::mem_fun(this, &SelectionBox::keyevents));
	add_events(Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK);
	signal_enter_notify_event().connect(sigc::mem_fun(this, &SelectionBox::cursor_cross));
	signal_leave_notify_event().connect(sigc::mem_fun(this, &SelectionBox::cursor_cross));

	sw.show();
	add(sw);

	if (ori == crn::Orientation::INVALID)
		throw Glib::OptionError(Glib::OptionError::BAD_VALUE, _("GtkCRN::SelectionBox: Invalid orientation."));
	if (ori == crn::Orientation::VERTICAL)
		box = Gtk::manage(new Gtk::VBox());
	else
		box = Gtk::manage(new Gtk::HBox());
	box->show();

	sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	sw.add(*box);

	box->pack_start(first_drop_zone, false, true, 0);
	box->pack_end(last_drop_zone, true, true, 0);

	std::vector<Gtk::TargetEntry> tl;
	tl.push_back(Gtk::TargetEntry("text/plain"));
	tl.push_back(Gtk::TargetEntry("text/uri-list"));
	drag_dest_set(tl, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_COPY|Gdk::ACTION_MOVE);
	signal_drag_motion().connect(sigc::mem_fun(this, &SelectionBox::drag_motion));

	set_size_request(50, 50);
	Glib::signal_timeout().connect(sigc::mem_fun(this, &SelectionBox::autoscroll), 100);

	set_can_focus(true);
}

/*! Destructor */
SelectionBox::~SelectionBox()
{
}

/*! Left/top side insert a widget to a box. 
 *
 * [Widget.Widget.Widget.NewWidget.......Widget.Widget.Widget]
 *
 * \param[in]	child	the widget to add
 * \param[in]	options	the packing options
 * \param[in]	padding	the space to add around the widget
 */
void SelectionBox::pack_start(Gtk::Widget& child, Gtk::PackOptions options, guint padding)
{
	std::shared_ptr<Element> vel(std::make_shared<Element>(child, this));
	content.push_back(vel);
	box->pack_start(*vel, options, padding);
}

/*! Left/top side insert a widget to a box. 
 *
 * [Widget.Widget.Widget.NewWidget.......Widget.Widget.Widget]
 *
 * \param[in]	child	the widget to add
 * \param[in]	expand	shall the widget expand to all free space
 * \param[in]	fill	shall the widget fill its allocated space
 * \param[in]	padding	the space to add around the widget
 */
void SelectionBox::pack_start(Gtk::Widget& child, bool expand, bool fill, guint padding)
{
	std::shared_ptr<Element> vel(std::make_shared<Element>(child, this));
	content.push_back(vel);
	box->pack_start(*vel, expand, fill, padding);
}

/*! Right/bottom side insert a widget to a box. 
 *
 * [Widget.Widget.Widget.......NewWidget.Widget.Widget.Widget]
 *
 * \param[in]	child	the widget to add
 * \param[in]	options	the packing options
 * \param[in]	padding	the space to add around the widget
 */
void SelectionBox::pack_end(Gtk::Widget& child, Gtk::PackOptions options, guint padding)
{
	std::shared_ptr<Element> vel(std::make_shared<Element>(child, this));
	content.push_back(vel);
	box->pack_end(*vel, options, padding);
}

/*! Right/bottom side insert a widget to a box. 
 *
 * [Widget.Widget.Widget.......NewWidget.Widget.Widget.Widget]
 *
 * \param[in]	child	the widget to add
 * \param[in]	expand	shall the widget expand to all free space
 * \param[in]	fill	shall the widget fill its allocated space
 * \param[in]	padding	the space to add around the widget
 */
void SelectionBox::pack_end(Gtk::Widget& child, bool expand, bool fill, guint padding)
{
	std::shared_ptr<Element> vel(std::make_shared<Element>(child, this));
	content.push_back(vel);
	box->pack_end(*vel, expand, fill, padding);
}

/*! Erase all elements */
void SelectionBox::clear()
{
	box->foreach(sigc::mem_fun(*box, &Gtk::Box::remove));
	box->pack_start(first_drop_zone, false, true, 0);
	box->pack_end(last_drop_zone, true, true, 0);
	content.clear();
	selection.clear();
	last_selected = std::weak_ptr<Element>();
	selection_changed.emit(nullptr, get_selection());
}

/*! Returns the list of selected widgets
 *
 * \return	a vector of pointers to widgets
 */
std::vector<Gtk::Widget*> SelectionBox::get_selection() const
{
	std::vector<Gtk::Widget*> sel;
	for (std::shared_ptr<Element> el : content)
	{
		if (selection.find(el) != selection.end())
		{
			sel.push_back(&el->GetWidget());
		}
	}
	return sel;
}

/*! Returns the list of widgets inside the box
 *
 * \return	a vector of pointers to widgets
 */
std::vector<Gtk::Widget*> SelectionBox::get_content() const
{
	std::vector<Gtk::Widget*> sel;
	for (std::shared_ptr<Element> el : content)
	{
		sel.push_back(&el->GetWidget());
	}
	return sel;
}

/*! Returns the last widget that was selected
 *
 * \return	a pointer to the last selected widget
 */
Gtk::Widget* SelectionBox::get_last_selected() const
{
	if (last_selected.expired())
		return nullptr;
	/*std::shared_ptr<Element> el(last_selected);
	if (!el)
		return nullptr;
	return &el->GetWidget();*/
	return const_cast<Gtk::Widget*>(&last_selected.lock()->GetWidget());
}

/*! a key was pressed or released
 *
 * \param[in]	ev	the key event
 * \return	false
 */
bool SelectionBox::keyevents(GdkEventKey *ev)
{
#ifdef CRN_USING_GTKMM3
	if ((ev->keyval == GDK_KEY_Shift_L) || (ev->keyval == GDK_KEY_Shift_R))
#else /* CRN_USING_GTKMM3 */
	if ((ev->keyval == GDK_Shift_L) || (ev->keyval == GDK_Shift_R))
#endif /* CRN_USING_GTKMM3 */
	{
		if (ev->type == GDK_KEY_PRESS)
		{
			shift_key = true;
		}
		else
		{
			shift_key = false;
		}
	}
#ifdef CRN_USING_GTKMM3
	if ((ev->keyval == GDK_KEY_Control_L) || (ev->keyval == GDK_KEY_Control_R))
#else /* CRN_USING_GTKMM3 */
	if ((ev->keyval == GDK_Control_L) || (ev->keyval == GDK_Control_R))
#endif /* CRN_USING_GTKMM3 */
	{
		if (ev->type == GDK_KEY_PRESS)
		{
			control_key = true;
		}
		else
		{
			control_key = false;
		}
	}
	return false;
}

#ifdef CRN_USING_GTKMM3
/*! An object has been dropped on a target, we send the data
 *
 * \param[in]	dropon	the element on which the drop was done
 * \param[in]	selection_data	the content of the drop in raw text
 * \param[in]	info	reorderId (current selection will be moved below dropon) or dropId (selection data contains a list of URIs)
 */
void SelectionBox::dodrop(Element *dropon, const Gtk::SelectionData *selection_data, int info)
#else /* CRN_USING_GTKMM3 */
/*! An object has been dropped on a target, we send the data
 *
 * \param[in]	dropon	the element on which the drop was done
 * \param[in]	selection_data	the content of the drop in raw text
 * \param[in]	info	reorderId (current selection will be moved below dropon) or dropId (selection data contains a list of URIs)
 */
void SelectionBox::dodrop(Element *dropon, const GtkSelectionData *selection_data, int info)
#endif /* CRN_USING_GTKMM3 */
{
	vscrolldiv = 0;
	hscrolldiv = 0;

	// pos is the position in the "content" list. However as there is a drop zone at the beginning of the box, the position in the box is pos+1.
	int pos;
	if (dropon == nullptr)
	{
		pos = -1;
	}
	else if (dropon == (Element*)(-1))
	{
		pos = int(content.size()) - 1;
	}
	else
		pos	= int(std::find_if(content.begin(), content.end(), 
					[dropon](const std::shared_ptr<Element>&el) { return el.get() == dropon; } ) - content.begin());

	if (info == reorderId)
	{
		if (!can_reorder)
			return;

		std::vector<size_t> move_to(content.size());
		std::vector<size_t> moved_from(content.size());
		int to = 0;
		// copy non selected elements up to the position
		for (int tmp = 0; tmp <= pos; ++tmp)
		{
			if (selection.find(content[tmp]) == selection.end())
			{
				move_to[tmp] = to;
				moved_from[to] = tmp;
				to += 1;
			}
		}
		// copy selected elements
		std::set<size_t> selpos;
		for (const std::shared_ptr<Element> &el : selection)
		{
			size_t elpos = std::find(content.begin(), content.end(), el) - content.begin();
			selpos.insert(elpos);
		}
		for (size_t elpos : selpos)
		{
			move_to[elpos] = to;
			moved_from[to] = elpos;
			to += 1;
		}
		// copy the remaining non selected elements
		for (int tmp = pos + 1; tmp < int(content.size()); ++tmp)
		{
			if (selection.find(content[tmp]) == selection.end())
			{
				move_to[tmp] = to;
				moved_from[to] = tmp;
				to += 1;
			}
		}

		// now, we reorder the box
		// to do that, we move all the element to the top from last to first in the new order
		box->remove(last_drop_zone); // remove the last drop zone because it messes up with the reordering algo
		std::vector<size_t> move(moved_from);
		for (int tmp = int(move.size()) - 1; tmp >= 0; --tmp)
		{
#ifdef CRN_USING_GTKMM3
			box->reorder_child(*box->get_children()[move[tmp]], 1);
#else
			Gtk::Box_Helpers::BoxList::iterator it(box->children().begin()), bit(box->children().begin());
			std::advance(it, move[tmp] + 1); // +1 because the first element is a DropZone
			std::advance(bit, 1); // idem
			box->children().reorder(it, bit);
#endif /* CRN_USING_GTKMM3 */
			for (size_t &p : move)
			{ // as an element was moved, all elements previously before it are shifted
				if (p < move[tmp])
					p += 1;
			}
		}
		box->pack_end(last_drop_zone, true, true, 0); // put back the last drop zone
		// reorder the content vector
		std::vector<std::shared_ptr<Element> > nc;
		for (size_t tmp = 0; tmp < moved_from.size(); ++tmp)
			nc.push_back(content[moved_from[tmp]]);
		content.swap(nc);
		
		// emit the signal only if the order was really changed
		// (the order may not be changed if the drop zone is inside the selection)
		for (size_t tmp = 0; tmp < move_to.size(); ++tmp)
		{
			if (move_to[tmp] != tmp)
			{
				moved.emit(move_to, moved_from);
				break;
			}
		}
	}
	else if (info == dropId)
	{ // dropped from outside
#ifdef CRN_USING_GTKMM3
		crn::StringUTF8 dat(selection_data->get_text().c_str());
#else /* CRN_USING_GTKMM3 */
		crn::StringUTF8 dat((char*)selection_data->data);
#endif /* CRN_USING_GTKMM3 */
		//CRNVerbose(dat);
		/*
		std::vector<Glib::ustring>  file_list;
		file_list = selection_data.get_uris();
		crn::StringUTF8 li;
		CRNVerbose(file_list.size());
		for (int tmp = 0; tmp < file_list.size(); ++tmp)
		{
			CRNVerbose(file_list[tmp].c_str());
			li += file_list[tmp].c_str();
			li += "\n";
		}
		droppedin.emit(pos, li);
		*/
		droppedin.emit(pos, dat);
	}
}

void SelectionBox::drop(Element *dropon, const Glib::RefPtr<Gdk::DragContext>& context, int, int, const Gtk::SelectionData& selection_data, guint info, guint time)
{
#ifdef CRN_USING_GTKMM3
	dodrop(dropon, &selection_data, info);
#else /* CRN_USING_GTKMM3 */
	dodrop(dropon, selection_data.gobj(), info);
#endif /* CRN_USING_GTKMM3 */
}

bool SelectionBox::drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
	if (y < 10)
	{
		vscrolldiv = -5;
	}
	else if (y < 20)
	{
		vscrolldiv = -10;
	}
	else if (y > get_height() - 10)
	{
		vscrolldiv = 5;
	}
	else if (y > get_height() - 20)
	{
		vscrolldiv = 10;
	}
	else
	{
		vscrolldiv = 0;
	}

	if (x < 10)
	{
		hscrolldiv = -5;
	}
	else if (x < 20)
	{
		hscrolldiv = -10;
	}
	else if (x > get_width() - 10)
	{
		hscrolldiv = 5;
	}
	else if (x > get_width() - 20)
	{
		hscrolldiv = 10;
	}
	else
	{
		hscrolldiv = 0;
	}
	return true;
}

bool SelectionBox::autoscroll()
{
#ifdef CRN_USING_GTKMM3
	auto vad = sw.get_vadjustment();
#else /* CRN_USING_GTKMM3 */
	Gtk::Adjustment *vad = sw.get_vadjustment();
#endif /* CRN_USING_GTKMM3 */
	if (vscrolldiv < 0)
		vad->set_value(crn::Max(0.0, vad->get_value() - vad->get_page_increment() / -vscrolldiv));
	else if (vscrolldiv > 0)
		vad->set_value(crn::Min(vad->get_upper() - vad->get_page_size(), vad->get_value() + vad->get_page_increment() / vscrolldiv));
	
#ifdef CRN_USING_GTKMM3
	auto had = sw.get_hadjustment();
#else /* CRN_USING_GTKMM3 */
	Gtk::Adjustment *had = sw.get_hadjustment();
#endif /* CRN_USING_GTKMM3 */
	if (hscrolldiv < 0)
		had->set_value(crn::Max(0.0, had->get_value() - had->get_page_increment() / -hscrolldiv));
	else if (hscrolldiv > 0)
		had->set_value(crn::Min(had->get_upper() - had->get_page_size(), had->get_value() + had->get_page_increment() / hscrolldiv));
	return true;
}

SelectionBox::DropZone::DropZone(const sigc::slot7<void, Element*, const Glib::RefPtr<Gdk::DragContext>&, int, int, const Gtk::SelectionData&, guint, guint> &dropfun, SelectionBox *sb, Element *el):in(false)
{
	show();

	std::vector<Gtk::TargetEntry> tl;
	tl.push_back(Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP, reorderId)); // reorder
	tl.push_back(Gtk::TargetEntry("text/uri-list", Gtk::TargetFlags(0), dropId)); // add from outside
	drag_dest_set(tl, Gtk::DEST_DEFAULT_MOTION|Gtk::DEST_DEFAULT_DROP, Gdk::ACTION_COPY|Gdk::ACTION_MOVE);
	//drag_dest_add_uri_targets();
	//drag_dest_add_text_targets();
	signal_drag_data_received().connect(sigc::bind<0>(dropfun, el));
	signal_drag_motion().connect(sigc::mem_fun(this, &DropZone::drag_motion));
	signal_drag_leave().connect(sigc::mem_fun(this, &DropZone::drag_leave));
#ifdef CRN_USING_GTKMM3
	signal_draw().connect(sigc::mem_fun(this, &DropZone::expose));
#else /* CRN_USING_GTKMM3 */
	signal_expose_event().connect(sigc::mem_fun(this, &DropZone::expose));
#endif /* CRN_USING_GTKMM3 */
	
	set_size_request(10, 10);
}

bool SelectionBox::DropZone::drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
	if (!in)
	{
		in = true;
#ifdef CRN_USING_GTKMM3
		get_style_property("theme-fg-color", bg);
#else /* CRN_USING_GTKMM3 */
		get_window()->set_background(get_style()->get_fg(get_state()));
#endif /* CRN_USING_GTKMM3 */
		queue_draw();
	}
	return true;
}

void SelectionBox::DropZone::drag_leave(const Glib::RefPtr<Gdk::DragContext>& context, guint time)
{
	in = false;
#ifdef CRN_USING_GTKMM3
	get_style_property("theme-bg-color", bg);
#else /* CRN_USING_GTKMM3 */
	get_window()->set_background(get_style()->get_bg(get_state()));
#endif /* CRN_USING_GTKMM3 */
	queue_draw();
}

#ifdef CRN_USING_GTKMM3
bool SelectionBox::DropZone::expose(const Cairo::RefPtr<Cairo::Context>&)
#else /* CRN_USING_GTKMM3 */
bool SelectionBox::DropZone::expose(GdkEventExpose *ev)
#endif /* CRN_USING_GTKMM3 */
{
#ifdef CRN_USING_GTKMM3
	auto cr = get_window()->create_cairo_context();
	cr->set_source_rgb(bg.get_red(), bg.get_green(), bg.get_blue());
	cr->paint();
#else /* CRN_USING_GTKMM3 */
	get_window()->clear();
#endif /* CRN_USING_GTKMM3 */
	return true;
}

SelectionBox::Element::Element(Gtk::Widget &w, SelectionBox *const sb):
	widget(w),
	dz(sigc::mem_fun(sb, &SelectionBox::drop), sb, this)
{
	unset_label();
	set_shadow_type(Gtk::SHADOW_NONE);

	if (sb->get_orientation() == crn::Orientation::HORIZONTAL)
		box = std::make_shared<Gtk::HBox>();
	else
		box = std::make_shared<Gtk::VBox>();

	w.show();
	
	// make the color of the button more visible when selected
#ifdef CRN_USING_GTKMM3
	Gdk::RGBA bgcol;
	get_style_property("theme-bg-color", bgcol);
	// TODO
	//tb.override_background_color(bgcol, Gtk::STATE_FLAG_ACTIVE);
	//tb.override_background_color(bgcol, Gtk::STATE_FLAG_PRELIGHT);
#else
	crn::String colors(Gtk::Settings::get_default()->property_gtk_color_scheme().get_value().c_str());
	if (colors.IsNotEmpty())
	{
		crn::String bgsel(U"selected_bg_color: ");
		size_t pos = colors.Find(bgsel);
		pos += bgsel.Size();
		size_t endpos = colors.Find(U"\n", pos);
		colors.Crop(pos, endpos - pos);
		tb.modify_bg(Gtk::STATE_ACTIVE, Gdk::Color(Glib::ustring(colors.CStr())));
		tb.modify_bg(Gtk::STATE_PRELIGHT, Gdk::Color(Glib::ustring(colors.CStr())));
	}
#endif /* CRN_USING_GTKMM3 */

	tb.show();
	tb.signal_toggled().connect(sigc::bind(sigc::mem_fun(*this, &Element::toggled), sb));
	tb.add(w);

	std::vector<Gtk::TargetEntry> tl;
	tl.push_back(Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP));
	tb.drag_source_set(tl, Gdk::MODIFIER_MASK, Gdk::ACTION_MOVE);
	tb.signal_drag_begin().connect(sigc::mem_fun(*this, &Element::drag_begin));
	tb.signal_drag_data_get().connect(sigc::mem_fun(*this, &Element::drag_data_get));

	box->pack_start(tb, false, true, 0);
	
	dz.show();
	box->pack_start(dz, false, true, 0);
	
	box->show();
	add(*box);

	show();
}

void SelectionBox::Element::toggled(SelectionBox *const sb)
{
	std::shared_ptr<Element> shthis(*std::find_if(sb->content.begin(), sb->content.end(), 
				[this](const std::shared_ptr<Element>&el) { return el.get() == this; } ));
	if (tb.get_active())
	{ // an element was selected
		if (sb->shift_key && !sb->last_selected.expired() && !sb->selecting)
		{ // must select all elements between last click and this element
			sb->selecting = true;
			std::vector<std::shared_ptr<Element> >::iterator last, cur;
			last = std::find(sb->content.begin(), sb->content.end(), sb->last_selected.lock());
			cur = std::find_if(sb->content.begin(), sb->content.end(), 
					[this](const std::shared_ptr<Element>& el){ return el.get() == this; });
			if (last >= cur)
			{
				for (std::vector<std::shared_ptr<Element> >::iterator it = cur + 1; it != last; ++it)
				{
					(*it)->Select();
				}
			}
			else
			{
				for (std::vector<std::shared_ptr<Element> >::iterator it = last + 1; it != cur; ++it)
				{
					(*it)->Select();
				}
			}
			sb->selecting = false;
		}
		if (!sb->shift_key && !sb->control_key && !sb->selecting)
		{ // nor shift nor control was hold -> single selection: we must deselect all
			sb->selecting = true;
			for (std::shared_ptr<Element> el : sb->content)
			{
				if (el != shthis)
					el->Deselect();
			}
			sb->selecting = false;
		}
		// select this and update last selected
		//sb->selection.insert(this);
		sb->selection.insert(shthis);
		sb->last_selected = shthis;
	}
	else
	{ // an element was deselected
		if (sb->shift_key && !sb->last_selected.expired() && !sb->selecting)
		{ // must deselect all elements between last click and this element
			sb->selecting = true;
			std::vector<std::shared_ptr<Element> >::iterator last, cur;
			last = std::find(sb->content.begin(), sb->content.end(), sb->last_selected.lock());
			cur = std::find(sb->content.begin(), sb->content.end(), shthis);
			if (last >= cur)
			{
				for (std::vector<std::shared_ptr<Element> >::iterator it = cur + 1; it != last + 1; ++it)
				{
					(*it)->Deselect();
				}
			}
			else
			{
				for (std::vector<std::shared_ptr<Element> >::iterator it = last; it != cur; ++it)
				{
					(*it)->Deselect();
				}
			}
			sb->selecting = false;
		}
		if (!sb->control_key && !sb->shift_key && !sb->selecting)
		{ // nor control nor shift key is hold -> deselect all and reselect this
			sb->selecting = true;
			for (std::shared_ptr<Element> el : sb->content)
			{
				el->Deselect();
			}
			sb->selecting = false;
			this->Select();
		}
		else
		{ // deselect this and update last_selected
			sb->selection.erase(shthis);
			if (sb->last_selected.lock() == shthis)
			{
				if (sb->selection.size() == 0)
					sb->last_selected = std::weak_ptr<Element>();
				else
					sb->last_selected = *sb->selection.rbegin();
			}
		}
	}

	if (!sb->selecting)
		sb->selection_changed.emit(sb->last_selected.expired() ? nullptr : &sb->last_selected.lock()->GetWidget(), sb->get_selection());
}

bool SelectionBox::cursor_cross(GdkEventCrossing *ev)
{
	if (ev->type == GDK_ENTER_NOTIFY)
	{
		control_key = ev->state & GDK_CONTROL_MASK;
		shift_key = ev->state & GDK_SHIFT_MASK;
	}
	grab_focus();
	return false;
}

/*! Sets the selection to one element
 *
 * \throws	Glib::OptionError	index out of bounds
 *
 * \param[in]	index	the index of the element to select
 */
void SelectionBox::set_selection(size_t index)
{
	deselect_all();
	if (index >= content.size())
		throw Glib::OptionError(Glib::OptionError::BAD_VALUE, Glib::ustring("void SelectionBox::set_selection(size_t index): ") + _("index out of bounds."));
	std::shared_ptr<Element> el(content[index]);
	el->Select();
}

/*! Adds or remove an element from the selection
 *
 * \throws	Glib::OptionError	index out of bounds
 *
 * \param[in]	index	the index of the element add or remove from selection
 * \param[in]	selected	shall the element be selected or deselected?
 * \param[in]	silent	if true, does not signal selection change
 */
void SelectionBox::set_selected(size_t index, bool selected, bool silent)
{
	if (index >= content.size())
		throw Glib::OptionError(Glib::OptionError::BAD_VALUE, Glib::ustring("void SelectionBox::set_selected(size_t index, bool selected, bool silent): ") + _("index out of bounds."));
	bool oldctrl = control_key;
	control_key = true;
	bool oldshift = shift_key;
	std::shared_ptr<Element> el(content[index]);
	bool oldselecting = selecting;
	if (silent)
		selecting = true;
	if (selected)
	{
		if (selection.find(el) == selection.end())
		{
			el->Select();
		}
	}
	else
	{
		if (selection.find(el) != selection.end())
		{
			el->Deselect();
		}
	}
	if (silent)
		selecting = oldselecting;
	control_key = oldctrl;
	shift_key = oldshift;
}

/*! Is an element selected?
 *
 * \throws	Glib::OptionError	index out of bounds
 *
 * \param[in]	index	the index of the element to check
 * \return	true if the element is selected, false else
 */
bool SelectionBox::is_selected(size_t index)
{
	if (index >= content.size())
		throw Glib::OptionError(Glib::OptionError::BAD_VALUE, Glib::ustring("bool SelectionBox::is_selected(size_t index): ") + _("index out of bounds."));
	std::shared_ptr<Element> el(content[index]);
	return selection.find(el) != selection.end();
}


void SelectionBox::select_all()
{
	if (content.empty())
		return;
	if (content.size() > 1)
	{
		// inert select the first element
		selecting = true;
		content.front()->Select();
		selecting = false;
	}
	// shift select the last element
	bool old_shift = shift_key;
	shift_key = true;
	content.back()->Select();
	shift_key = old_shift;
}

void SelectionBox::deselect_all()
{
	if (content.empty())
		return;
	if (content.size() > 1)
	{
		// inert deselect the first element
		selecting = true;
		content.front()->Deselect();
		selecting = false;
	}
	// shift deselect the last element
	bool old_shift = shift_key;
	shift_key = true;
	content.back()->Deselect();
	shift_key = old_shift;
}

void SelectionBox::select_even()
{
	if (content.empty())
		return;
	selecting = true;
	for (size_t tmp = 0; tmp < content.size(); ++tmp)
	{
		if (tmp & 0x01) // this is an odd number, however, as the first index is 0, this is an even element
		{
			content[tmp]->Select();
			last_selected = content[tmp];
		}
		else
			content[tmp]->Deselect();
	}
	selecting = false;
	selection_changed.emit(last_selected.expired() ? nullptr : &last_selected.lock()->GetWidget(), get_selection());
}

void SelectionBox::select_odd()
{
	if (content.empty())
		return;
	selecting = true;
	for (size_t tmp = 0; tmp < content.size(); ++tmp)
	{
		if (tmp & 0x01) // this is an odd number, however, as the first index is 0, this is an even element
			content[tmp]->Deselect();
		else
		{
			content[tmp]->Select();
			last_selected = content[tmp];
		}
	}
	selecting = false;
	selection_changed.emit(last_selected.expired() ? nullptr : &last_selected.lock()->GetWidget(), get_selection());
}

void SelectionBox::invert_selection()
{
	if (content.empty())
		return;
	selecting = true;
	for (size_t tmp = 0; tmp < content.size() - 1; ++tmp)
	{
		set_selected(tmp, !is_selected(tmp));
	}
	selecting = false;
	set_selected(content.size() - 1, !is_selected(content.size() - 1));
}


/*! If selection size is <= 1, then select the first item, else move last_selected to the first selected element */
void SelectionBox::select_first()
{
	if (content.size() == 0)
		return;
	switch (selection.size())
	{
		case 1:
			{ // deselect the selected element and goto 0
				std::shared_ptr<Element> el(*selection.begin());
				el->Deselect();
			}
		case 0:
			// select the first element
			content.front()->Select();
			break;
		default:
			{ // look for the first selected element
				std::vector<std::shared_ptr<Element> >::reverse_iterator lsel(std::find(content.rbegin(), content.rend(), last_selected.lock()));
				std::vector<std::shared_ptr<Element> >::reverse_iterator nsel(content.rend());
				for (std::vector<std::shared_ptr<Element> >::reverse_iterator it(lsel); it != content.rend(); ++it)
				{
					if (selection.find(*it) != selection.end())
						nsel = it;
				}
				if ((nsel != content.rend()) && (nsel != lsel))
				{
					last_selected = *nsel;
					selection_changed.emit(&last_selected.lock()->GetWidget(), get_selection());
				}
			}
	}
}

/*! If selection size is 0, the select the first element, if selection size is 1, then select the previous item, else move last_selected to the previous selected element */
void SelectionBox::select_previous()
{
	if (content.size() == 0)
		return;
	switch (selection.size())
	{
		case 0:
			// select the first element
			content.front()->Select();
			break;
		case 1:
			{ // if there is an element before the selected element, select it
				std::vector<std::shared_ptr<Element> >::reverse_iterator lsel(std::find(content.rbegin(), content.rend(), last_selected.lock()));
				std::vector<std::shared_ptr<Element> >::reverse_iterator nsel(lsel);
				++nsel;
				if (nsel != content.rend())
				{
					std::shared_ptr<Element>(*lsel)->Deselect();
					std::shared_ptr<Element>(*nsel)->Select();
				}
			}
			break;
		default:
			{ // look for the select element before last_selected
				std::vector<std::shared_ptr<Element> >::reverse_iterator lsel(std::find(content.rbegin(), content.rend(), last_selected.lock()));
				std::vector<std::shared_ptr<Element> >::reverse_iterator nsel(content.rend());
				for (std::vector<std::shared_ptr<Element> >::reverse_iterator it(lsel + 1); it != content.rend(); ++it)
				{
					if (selection.find(*it) != selection.end())
					{
						nsel = it;
						break;
					}
				}
				if (nsel != content.rend())
				{
					last_selected = *nsel;
					selection_changed.emit(&last_selected.lock()->GetWidget(), get_selection());
				}
			}
	}
}

/*! If selection size is 0, the select the first element, if selection size is 1, then select the next item, else move last_selected to the next selected element */
void SelectionBox::select_next()
{
	if (content.size() == 0)
		return;
	switch (selection.size())
	{
		case 0:
			// select the first element
			content.front()->Select();
			break;
		case 1:
			{ // if there is an element after the selected element, select it
				std::vector<std::shared_ptr<Element> >::iterator lsel(std::find(content.begin(), content.end(), last_selected.lock()));
				std::vector<std::shared_ptr<Element> >::iterator nsel(lsel);
				++nsel;
				if (nsel != content.end())
				{
					std::shared_ptr<Element>(*lsel)->Deselect();
					std::shared_ptr<Element>(*nsel)->Select();
				}
			}
			break;
		default:
			{ // look for the select element after last_selected
				std::vector<std::shared_ptr<Element> >::iterator lsel(std::find(content.begin(), content.end(), last_selected.lock()));
				std::vector<std::shared_ptr<Element> >::iterator nsel(content.end());
				for (std::vector<std::shared_ptr<Element> >::iterator it(lsel + 1); it != content.end(); ++it)
				{
					if (selection.find(*it) != selection.end())
					{
						nsel = it;
						break;
					}
				}
				if (nsel != content.end())
				{
					last_selected = *nsel;
					selection_changed.emit(&last_selected.lock()->GetWidget(), get_selection());
				}
			}
	}
}

/*! If selection size is <= 1, then select the last item, else move last_selected to the last selected element */
void SelectionBox::select_last()
{
	if (content.size() == 0)
		return;
	switch (selection.size())
	{
		case 1:
			{ // deselect the selected element and goto 0
				std::shared_ptr<Element> el(*selection.begin());
				el->Deselect();
			}
		case 0:
			// select the first element
			content.back()->Select();
			break;
		default:
			{ // look for the first selected element
				std::vector<std::shared_ptr<Element> >::iterator lsel(std::find(content.begin(), content.end(), last_selected.lock()));
				std::vector<std::shared_ptr<Element> >::iterator nsel(content.end());
				for (std::vector<std::shared_ptr<Element> >::iterator it(lsel); it != content.end(); ++it)
				{
					if (selection.find(*it) != selection.end())
						nsel = it;
				}
				if ((nsel != content.end()) && (nsel != lsel))
				{
					last_selected = *nsel;
					selection_changed.emit(&last_selected.lock()->GetWidget(), get_selection());
				}
			}
	}
}


