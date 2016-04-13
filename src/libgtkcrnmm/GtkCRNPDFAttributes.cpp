/* Copyright 2010-2016 CoReNum, ENS-Lyon
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
 * file: GtkCRNPDFAttributes.cpp
 * \author Yann LEYDIER
 */

#include <GtkCRNPDFAttributes.h>
#include <CRNException.h>
#include <CRNi18n.h>

using namespace GtkCRN;

const Glib::ustring PDFAttributes::onePage(N_("Single pages"));
const Glib::ustring PDFAttributes::continuous(N_("Continuous pages"));
const Glib::ustring PDFAttributes::openLeft(N_("Two pages, first on the left"));
const Glib::ustring PDFAttributes::openRight(N_("Tow pages, first on the right"));

/*! Constructor */
PDFAttributes::PDFAttributes():
	//Gtk::Table(6, 4, false),
	lossy(_("Lossy image compression")),
	copyable(_("Allow users to copy the content")),
	printable(_("Allow users to print the document"))
{
#ifdef CRN_USING_GTKMM3
	attach(*Gtk::manage(new Gtk::Label(_("Author"))), 0, 1, 0, 1);
#else
	attach(*Gtk::manage(new Gtk::Label(_("Author"))), 0, 1, 0, 1, Gtk::FILL, Gtk::FILL);
#endif
	author.set_text(attr.author.CStr());
	author.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_author_changed));
#ifdef CRN_USING_GTKMM3
	attach(author, 1, 2, 0, 1);
	attach(*Gtk::manage(new Gtk::Label(_("Title"))), 2, 3, 0, 1);
#else
	attach(author, 1, 2, 0, 1, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
	attach(*Gtk::manage(new Gtk::Label(_("Title"))), 2, 3, 0, 1, Gtk::FILL, Gtk::FILL);
#endif
	title.set_text(attr.title.CStr());
	title.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_title_changed));
	//attach(title, 3, 4, 0, 1, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	//attach(*Gtk::manage(new Gtk::Label(_("Subject"))), 0, 1, 1, 2, Gtk::FILL, Gtk::FILL);
	subject.set_text(attr.subject.CStr());
	subject.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_subject_changed));
	//attach(subject, 1, 2, 1, 2, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
	//attach(*Gtk::manage(new Gtk::Label(_("Keywords"))), 2, 3, 1, 2, Gtk::FILL, Gtk::FILL);
	keywords.set_text(attr.keywords.CStr());
	keywords.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_keywords_changed));
	//attach(keywords, 3, 4, 1, 2, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	//attach(*Gtk::manage(new Gtk::Label(_("Page layout"))), 0, 1, 2, 3, Gtk::FILL, Gtk::FILL);
#ifdef CRN_USING_GTKMM3
	layout.append(_(onePage.c_str()));
	layout.append(_(continuous.c_str()));
	layout.append(_(openLeft.c_str()));
	layout.append(_(openRight.c_str()));
#else /* CRN_USING_GTKMM3 */
	layout.append_text(_(onePage.c_str()));
	layout.append_text(_(continuous.c_str()));
	layout.append_text(_(openLeft.c_str()));
	layout.append_text(_(openRight.c_str()));
#endif /* CRN_USING_GTKMM3 */
	layout.set_active_text(layout2str(attr.layout));
	layout.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_layout_changed));
	//attach(layout, 1, 4, 2, 3, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	lossy.set_active(attr.lossy_compression);
	lossy.signal_toggled().connect(sigc::mem_fun(this, &PDFAttributes::on_lossy_changed));
	//attach(lossy, 0, 2, 3, 4, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
	//attach(*Gtk::manage(new Gtk::Label(_("Quality"))), 2, 3, 3, 4, Gtk::FILL, Gtk::FILL);
	qual.set_digits(0);
	qual.set_range(0, 100);
	qual.set_increments(10, 15);
	qual.set_value(attr.jpeg_qual);
	qual.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_qual_changed));
	//attach(qual, 3, 4, 3, 4, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	//attach(*Gtk::manage(new Gtk::Label(_("Editor's password"))), 0, 1, 4, 5, Gtk::FILL, Gtk::FILL);
	owner_pass.set_visibility(false);
	owner_pass.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_owner_pass_changed));
	//attach(owner_pass, 1, 2, 4, 5, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
	//attach(*Gtk::manage(new Gtk::Label(_("User's password"))), 2, 3, 4, 5, Gtk::FILL, Gtk::FILL);
	user_pass.set_visibility(false);
	user_pass.signal_changed().connect(sigc::mem_fun(this, &PDFAttributes::on_user_pass_changed));
	//attach(user_pass, 3, 4, 4, 5, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	copyable.set_active(attr.copyable);
	copyable.signal_toggled().connect(sigc::mem_fun(this, &PDFAttributes::on_copyable_changed));
	//attach(copyable, 0, 2, 5, 6, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);
	printable.set_active(attr.printable);
	printable.signal_toggled().connect(sigc::mem_fun(this, &PDFAttributes::on_printable_changed));
	//attach(printable, 2, 4, 5, 6, Gtk::FILL|Gtk::EXPAND, Gtk::FILL);

	if (!attr.lossy_compression)
	{
		qual.set_sensitive(false);
	}
	if (attr.owner_password.IsEmpty())
	{
		user_pass.set_sensitive(false);
		printable.set_sensitive(false);
		copyable.set_sensitive(false);
	}
	show_all();
	hide();
}

/*! Sets all attributes at once 
 * \param[in]	nattr	the attributes to copy
 */
void PDFAttributes::set_attributes(const crn::PDF::Attributes &nattr)
{
	set_author(nattr.author);
	set_creator(nattr.creator);
	set_title(nattr.title);
	set_subject(nattr.subject);
	set_keywords(nattr.keywords);
	set_layout(nattr.layout);
	set_lossy(nattr.lossy_compression);
	set_jpeg_qality(nattr.jpeg_qual);
	set_owner_password(nattr.owner_password);
	set_user_password(nattr.user_password);
	set_copyable(nattr.copyable);
	set_printable(nattr.printable);
}

Glib::ustring PDFAttributes::layout2str(crn::PDF::Attributes::Layout lay)
{
	switch (lay)
	{
		case crn::PDF::Attributes::Layout::ONE_PAGE:
			return _(onePage.c_str());
		case crn::PDF::Attributes::Layout::CONTINUOUS:
			return _(continuous.c_str());
		case crn::PDF::Attributes::Layout::OPEN_LEFT:
			return _(openLeft.c_str());
		case crn::PDF::Attributes::Layout::OPEN_RIGHT:
			return _(openRight.c_str());
		default:
			return _(onePage.c_str());
	}
}

crn::PDF::Attributes::Layout PDFAttributes::str2layout(const Glib::ustring &lay)
{
	if (lay == _(onePage.c_str()))
		return crn::PDF::Attributes::Layout::ONE_PAGE;
	if (lay == _(continuous.c_str()))
		return crn::PDF::Attributes::Layout::CONTINUOUS;
	if (lay == _(openLeft.c_str()))
		return crn::PDF::Attributes::Layout::OPEN_LEFT;
	if (lay == _(openRight.c_str()))
		return crn::PDF::Attributes::Layout::OPEN_RIGHT;
	return crn::PDF::Attributes::Layout::ONE_PAGE;
}

/*! Sets the author name
 * \param[in]	s	the new author name
 */
void PDFAttributes::set_author(const crn::StringUTF8 &s)
{
	author.set_text(s.CStr());
}

/*! Sets the creator name
 * \param[in]	s	the new creator name
 */
void PDFAttributes::set_creator(const crn::StringUTF8 &s)
{
	attr.creator = s;
}

/*! Sets the document title
 * \param[in]	s	the new document title
 */
void PDFAttributes::set_title(const crn::StringUTF8 &s)
{
	title.set_text(s.CStr());
}

/*! Sets the document's subject
 * \param[in]	s	the new document's subject
 */
void PDFAttributes::set_subject(const crn::StringUTF8 &s)
{
	subject.set_text(s.CStr());
}

/*! Sets the document's keywords
 * \param[in]	s	the new document's keywords
 */
void PDFAttributes::set_keywords(const crn::StringUTF8 &s)
{
	keywords.set_text(s.CStr());
}

/*! Sets the document's layout
 * \param[in]	lay	the new document's layout
 */
void PDFAttributes::set_layout(crn::PDF::Attributes::Layout lay)
{
	layout.set_active_text(layout2str(lay));
}

/*! Sets if the images are compressed in JPEG
 * \param[in]	l	true to compress images in JPEG, false to compress images in PNG
 */
void PDFAttributes::set_lossy(bool l)
{
	lossy.set_active(l);
}

/*! Sets JPEG quality
 * \throws	crn::ExceptionDomain	baq quality value
 * \param[in]	q	the new quality
 */
void PDFAttributes::set_jpeg_qality(int q)
{
	if ((q < 0) || (q > 100))
		throw crn::ExceptionDomain(_("JPEG quality must be between 0 and 100."));
	qual.set_value(q);
}

/*! Sets the document's administrative password
 * \param[in]	s	the new document's administrative password
 */
void PDFAttributes::set_owner_password(const crn::StringUTF8 &s)
{
	owner_pass.set_text(s.CStr());
}

/*! Sets the document's reader password
 * \param[in]	s	the new document's reader password
 */
void PDFAttributes::set_user_password(const crn::StringUTF8 &s)
{
	user_pass.set_text(s.CStr());
}

/*! Sets if the document's content can be copied to another document
 * \param	c	true to enable copy/paste
 */
void PDFAttributes::set_copyable(bool c)
{
	copyable.set_active(c);
}

/*! Sets if the document can be printed
 * \param	p	true to enable printing
 */
void PDFAttributes::set_printable(bool p)
{
	printable.set_active(p);
}

void PDFAttributes::on_author_changed()
{
	attr.author = author.get_text().c_str();
}

void PDFAttributes::on_title_changed()
{
	attr.title = title.get_text().c_str();
}

void PDFAttributes::on_subject_changed()
{
	attr.subject = subject.get_text().c_str();
}

void PDFAttributes::on_keywords_changed()
{
	attr.keywords = keywords.get_text().c_str();
}

void PDFAttributes::on_layout_changed()
{
	attr.layout = str2layout(layout.get_active_text());
}

void PDFAttributes::on_lossy_changed()
{
	attr.lossy_compression = lossy.get_active();
	qual.set_sensitive(attr.lossy_compression);
}

void PDFAttributes::on_qual_changed()
{
	attr.jpeg_qual = qual.get_value_as_int();
}

void PDFAttributes::on_owner_pass_changed()
{
	attr.owner_password = owner_pass.get_text().c_str();
	if (attr.owner_password.IsEmpty())
	{
		attr.user_password = "";
		attr.copyable = true;
		attr.printable = true;
		user_pass.set_sensitive(false);
		copyable.set_sensitive(false);
		printable.set_sensitive(false);
	}
	else
	{
		attr.user_password = user_pass.get_text().c_str();
		attr.copyable = copyable.get_active();
		attr.printable = printable.get_active();
		user_pass.set_sensitive(true);
		copyable.set_sensitive(true);
		printable.set_sensitive(true);
	}
}

void PDFAttributes::on_user_pass_changed()
{
	attr.user_password = user_pass.get_text().c_str();
}

void PDFAttributes::on_copyable_changed()
{
	attr.copyable = copyable.get_active();
}

void PDFAttributes::on_printable_changed()
{
	attr.printable = printable.get_active();
}

