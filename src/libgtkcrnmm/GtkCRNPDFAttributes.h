/* Copyright 2010-2014 CoReNum
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
 * file: GtkCRNPDFAttributes.h
 * \author Yann LEYDIER
 */

#ifndef GtkCRNPDFAttributes_HEADER
#define GtkCRNPDFAttributes_HEADER

#include <libgtkcrnmm_config.h>
#include <gtkmm.h>
#include <CRNUtils/CRNPDFAttributes.h>

namespace GtkCRN
{
	/*! \brief Configuration widget for crn::PDFAttributes
	 * \ingroup	gtkcrn
	 */
	class PDFAttributes: public Gtk::Table
	{
		public:
			/*! \brief Constructor */
			PDFAttributes();
			virtual ~PDFAttributes() override {}

			/*! \brief Sets the author name */
			void set_author(const crn::StringUTF8 &s);
			/*! \brief Sets the creator name */
			void set_creator(const crn::StringUTF8 &s);
			/*! \brief Sets the document title */
			void set_title(const crn::StringUTF8 &s);
			/*! \brief Sets the document's subject */
			void set_subject(const crn::StringUTF8 &s);
			/*! \brief Sets the document's keywords */
			void set_keywords(const crn::StringUTF8 &s);
			/*! \brief Sets the document's layout */
			void set_layout(crn::PDF::Attributes::Layout lay);
			/*! \brief Sets if the images are compressed in JPEG */
			void set_lossy(bool l);
			/*! \brief Sets JPEG quality */
			void set_jpeg_qality(int q);
			/*! \brief Sets the administrative password */
			void set_owner_password(const crn::StringUTF8 &s);
			/*! \brief Sets the reader password */
			void set_user_password(const crn::StringUTF8 &s);
			/*! \brief Sets if the document's content can be copied to another document */
			void set_copyable(bool c);
			/*! \brief Sets if the document can be printed. */
			void set_printable(bool p);

			/*! \brief Sets all attributes at once */
			void set_attributes(const crn::PDF::Attributes &nattr);
			/*! \brief Gets attributes */
			const crn::PDF::Attributes& get_attributes() const { return attr; }

		private:
			//////////////////////////////////////////////////////////////////////////////
			// Utility methods
			//////////////////////////////////////////////////////////////////////////////
			static Glib::ustring layout2str(crn::PDF::Attributes::Layout lay);
			static crn::PDF::Attributes::Layout str2layout(const Glib::ustring &lay);
			void update();

			//////////////////////////////////////////////////////////////////////////////
			// Callbacks
			//////////////////////////////////////////////////////////////////////////////
			void on_author_changed();
			void on_title_changed();
			void on_subject_changed();
			void on_keywords_changed();
			void on_layout_changed();
			void on_lossy_changed();
			void on_qual_changed();
			void on_owner_pass_changed();
			void on_user_pass_changed();
			void on_copyable_changed();
			void on_printable_changed();

			//////////////////////////////////////////////////////////////////////////////
			// Widgets
			//////////////////////////////////////////////////////////////////////////////
			Gtk::Entry author, title, subject, keywords;
			Gtk::ComboBoxText layout;
			Gtk::CheckButton lossy;
			Gtk::SpinButton qual;
			Gtk::Entry owner_pass, user_pass;
			Gtk::CheckButton copyable, printable;

			//////////////////////////////////////////////////////////////////////////////
			// Attributes
			//////////////////////////////////////////////////////////////////////////////
			crn::PDF::Attributes attr;
			static const Glib::ustring onePage;
			static const Glib::ustring continuous;
			static const Glib::ustring openLeft;
			static const Glib::ustring openRight;
	};
}

#endif


