/* Copyright 2011-2016 CoReNum
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
 * file: CRNAltoStyles.h
 * \author Yann LEYDIER
 */

/*! \cond */
#ifdef CRNAlto_HEADER
/*! \endcond */

/*! \brief List of styles used in the document
 * \ingroup xml
 * \author Yann LEYDIER
 */
class Styles: public Element
{
	public:
		Styles(const Styles&) = delete;
		Styles(Styles&&) = default;
		virtual ~Styles() override {}
		Styles& operator=(const Styles&) = delete;
		Styles& operator=(Styles&&) = default;

		/*! \brief Returns the ids of the text styles */
		std::vector<Id> GetTextStyles() const;
		/*! \brief Returns the ids of the paragraph styles */
		std::vector<Id> GetParagraphStyles() const;
		/*! \brief A text style
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		class Text:public Element
		{
			public:
				Text(const Text&) = default;
				Text(Text&&) = default;
				virtual ~Text() override {}
				Text& operator=(const Text&) = default;
				Text& operator=(Text&&) = default;

				const Id& GetId() const { return id; }

				Option<StringUTF8> GetFontFamily() const;
				void SetFontFamily(const StringUTF8 &ff);
				void UnsetFontFamily();

				enum class FontType { Undef = 0, Serif, SansSerif };
				Option<FontType> GetFontType() const;
				void SetFontType(FontType ft);
				void UnsetFontType();

				enum class FontWidth { Undef = 0, Proportional, Fixed };
				Option<FontWidth> GetFontWidth() const;
				void SetFontWidth(FontWidth fw);
				void UnsetFontWidth();

				double GetFontSize() const;
				void SetFontSize(double fs);

				Option<uint32_t> GetFontColor() const;
				void SetFontColor(uint32_t fc);
				void UnsetFontColor();

				enum class FontStyle { Undef = 0, Bold = 1, Italics = 2, Subscript = 4, Superscript = 8, SmallCaps = 16, Underline = 32 }; //!< operators !, &, |, &=, |= are allowed
				Option<FontStyle> GetFontStyle() const;
				void SetFontStyle(FontStyle fs);
				void UnsetFontStyle();

			private:
				Text(const Element &el);

				Id id;

				friend class Styles;
		};
		/*! \brief Returns a text style */
		const Text& GetTextStyle(const Id &id_) const;
		/*! \brief Returns a text style */
		Text& GetTextStyle(const Id &id_);
		/*! \brief Adds a text style */
		Text& AddTextStyle(const Id &id_, double size);

		/*! \brief A paragraph style
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		class Paragraph: public Element
		{
			public:
				Paragraph(const Paragraph&) = default;
				Paragraph(Paragraph&&) = default;
				virtual ~Paragraph() override {}
				Paragraph& operator=(const Paragraph&) = default;
				Paragraph& operator=(Paragraph&&) = default;
				const Id& GetId() const { return id; }

				enum class Align { Undef = 0, Left, Right, Center, Block };
				Option<Align> GetAlign() const;
				void SetAlign(Align a);
				void UnsetAlign();

				Option<double> GetLeftIndent() const;
				void SetLeftIndent(double i);
				void UnsetLeftIndent();
				Option<double> GetRightIndent() const;
				void SetRightIndent(double i);
				void UnsetRightIndent();
				Option<double> GetLineSpace() const;
				void SetLineSpace(double i);
				void UnsetLineSpace();
				Option<double> GetFirstLineIndent() const;
				void SetFirstLineIndent(double i);
				void UnsetFirstLineIndent();

			private:
				Paragraph(const Element &el);

				Id id;

				friend class Styles;
		};
		/*! \brief Returns a paragraph style */
		const Paragraph& GetParagraphStyle(const Id &id_) const;
		/*! \brief Returns a paragraph style */
		Paragraph& GetParagraphStyle(const Id &id_);
		/*! \brief Adds a paragraph style */
		Paragraph& AddParagraphStyle(const Id &id_);

	private:
		/*! \brief Constructor from file */
		Styles(const Element &el);

		std::map<Id, std::unique_ptr<Text> > textStyles;
		std::map<Id, std::unique_ptr<Paragraph> > parStyles;

	friend class Root;
};

#else
#error	you cannot include this file directly
#endif
