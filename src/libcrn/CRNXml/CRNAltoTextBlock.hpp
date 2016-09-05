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
 * file: CRNAltoTextBlock.h
 * \author Yann LEYDIER
 */

/*! \cond */
#ifdef CRNAlto_HEADER
/*! \endcond */

/*! \brief Alto text block */
class TextBlock: public Block
{
	public:
		TextBlock(const TextBlock&) = delete;
		TextBlock(TextBlock&&) = default;
		virtual ~TextBlock() override {}
		TextBlock& operator=(const TextBlock&) = delete;
		TextBlock& operator=(TextBlock&&) = default;

		/*! \brief Returns the language of the text inside the block */
		Option<StringUTF8> GetLanguage() const;

		/*! \brief Alto text line */
		class TextLine: public Element
		{
			public:
				TextLine(const TextLine&) = delete;
				TextLine(TextLine&&) = default;
				virtual ~TextLine() override {}
				TextLine& operator=(const TextLine&) = delete;
				TextLine& operator=(TextLine&&) = default;

				/*! \brief Returns the id of the element */
				const Id& GetId() const { return id; }

				/*! \brief Returns the list of style references */
				std::vector<Id> GetStyles() const;
				/*! \brief Adds a reference to a style */
				void AddStyle(const Id &styleid);
				/*! \brief Removes a reference to a style */
				void RemoveStyle(const Id &styleid);

				/*! \brief Returns the width of the line */
				double GetWidth() const;
				/*! \brief Sets the width of the line */
				void SetWidth(double d);
				/*! \brief Returns the height of the line */
				double GetHeight() const;
				/*! \brief Sets the height of the line */
				void SetHeight(double d);
				/*! \brief Returns the abscissa of the line */
				double GetHPos() const;
				/*! \brief Sets the abscissa of the line */
				void SetHPos(double d);
				/*! \brief Returns the ordinate of the line */
				double GetVPos() const;
				/*! \brief Sets the ordinate of the line */
				void SetVPos(double d);
				/*! \brief Returns the ordinate of the baseline */
				Option<double> GetBaseline() const;
				/*! \brief Sets the baseline ordinate */
				void SetBaseline(double d, bool check_bounds = true);
				/*! \brief Unsets the baseline ordinate */
				void UnsetBaseline();
				/*! \brief Returns whether the line was manually corrected or not */
				Option<bool> GetManuallyCorrected() const;
				/*! \brief Sets whether the line was manually corrected or not */
				void SetManuallyCorrected(bool c);
				/*! \brief Unsets whether the line was manually corrected or not */
				void UnsetManuallyCorrected();

				/*! \brief Base class for elements in a text line */
				class LineElement: public Element
				{
					public:
						LineElement(const LineElement&) = delete;
						LineElement(LineElement&&) = default;
						virtual ~LineElement() override {}
						LineElement& operator=(const LineElement&) = delete;
						LineElement& operator=(LineElement&&) = default;

						/*! \brief Returns the width of the element */
						Option<double> GetWidth() const;
						/*! \brief Sets the width of the element */
						void SetWidth(double d);
						/*! \brief Returns the abscissa of the element */
						Option<double> GetHPos() const;
						/*! \brief Sets the abscissa of the element */
						void SetHPos(double d);
						/*! \brief Returns the ordinate of the element */
						Option<double> GetVPos() const;
						/*! \brief Sets the ordinate of the element */
						void SetVPos(double d);

					protected:
						/*! \brief Constructor */
						LineElement(const Element &el);
						/*! \brief Constructor */
						LineElement(const Element &el, const Option<double> &x, const Option<double> &y, const Option<double> &w);
				};
				typedef std::weak_ptr<LineElement> LineElementPtr;

				/*! \brief Returns the list of elements in the line */
				std::vector<LineElementPtr> GetLineElements() const;
				/*! \brief Returns the number of elements in the line */
				size_t GetNbLineElements() const { return lineElements.size(); }

				/*! \brief Alto word ("String" element) */
				class Word: public LineElement
				{
					public:
						Word(const Word&) = delete;
						Word(Word&&) = default;
						virtual ~Word() override {}
						Word& operator=(const Word&) = delete;
						Word& operator=(Word&&) = default;

						/*! \brief Returns the id of the element */
						const Option<Id>& GetId() const { return id; }

						/*! \brief Returns the list of style references */
						std::vector<Id> GetStyles() const;
						/*! \brief Adds a reference to a style */
						void AddStyle(const Id &styleid);
						/*! \brief Removes a reference to a style */
						void RemoveStyle(const Id &styleid);

						/*! \brief Returns the transcription of the word */
						StringUTF8 GetContent() const;
						/*! \brief Sets the transcription of the word */
						void SetContent(const StringUTF8 &s);
						/*! \brief Returns the height of the word */
						Option<double> GetHeight() const;
						/*! \brief Returns the height of the word */
						void SetHeight(double d);
						/*! \brief Returns the font style of the word */
						Option<Alto::Styles::Text::FontStyle> GetFontStyle() const;
						/*! \brief Sets the font style of the word */
						void SetFontStyle(Alto::Styles::Text::FontStyle fs);
						/*! \brief Unsets the font style of the word */
						void UnsetFontStyle();
						enum class SubstitutionType { Undef = 0, HypPart1, HypPart2, Abbreviation };
						/*! \brief Returns the substitution type of the word */
						Option<SubstitutionType> GetSubstitutionType() const;
						/*! \brief Returns the substitution of the word */
						Option<StringUTF8> GetSubstitutionContent() const;
						/*! \brief Sets the substitution of the word */
						void SetSubstitution(SubstitutionType stype, const StringUTF8 &scontent);
						/*! \brief Returns the OCR confidence of the word [0, 1] */
						Option<double> GetWC() const;
						/*! \brief Sets the OCR confidence of the word [0, 1] */
						void SetWC(double conf);
						/*! \brief Returns the OCR confidence of the characters */
						Option<StringUTF8> GetCC() const;
						/*! \brief Unsets the OCR confidence of the word */
						void UnsetWC();
						// TODO alternatives

					private:
						/*! \brief Constructor from file */
						Word(const Element &el);
						/*! \brief Constructor */
						Word(const Element &el, const Id &id_, const StringUTF8 &text, const Option<double> &x, const Option<double> &y, const Option<double> &w, const Option<double> &h);

						Option<Id> id;
					friend class TextLine;
				};
				typedef std::weak_ptr<Word> WordPtr;

				/*! \brief Returns the list of words in the line */
				const std::vector<WordPtr>& GetWords() const;
				/*! \brief Returns the number of words in the line */
				size_t GetNbWords() const { return words.size(); }
				/*! \brief Returns a word contained in the line */
				Word& GetWord(const Id &id_);
				/*! \brief Adds a word in the line */
				Word& AddWord(const Id &id_, const StringUTF8 &text, const Option<double> &x = Option<double>(), const Option<double> &y = Option<double>(), const Option<double> &w = Option<double>(), const Option<double> &h = Option<double>());
				/*! \brief Adds a word in the line */
				Word& AddWordAfter(const Id &pred, const Id &id_, const StringUTF8 &text, const Option<double> &x = Option<double>(), const Option<double> &y = Option<double>(), const Option<double> &w = Option<double>(), const Option<double> &h = Option<double>());
				/*! \brief Adds a word in the line */
				Word& AddWordBefore(const Id &next, const Id &id_, const StringUTF8 &text, const Option<double> &x = Option<double>(), const Option<double> &y = Option<double>(), const Option<double> &w = Option<double>(), const Option<double> &h = Option<double>());

				/*! \brief Removes a word */
				void RemoveWord(const Id &wid);

				/*! \brief Alto white space ("SP" element) */
				class WhiteSpace: public LineElement
				{
					public:
						WhiteSpace(const WhiteSpace&) = delete;
						WhiteSpace(WhiteSpace&&) = default;
						virtual ~WhiteSpace() override {}
						WhiteSpace& operator=(const WhiteSpace&) = delete;
						WhiteSpace& operator=(WhiteSpace&&) = default;

						/*! \brief Returns the id of the element */
						const Option<Id>& GetId() const { return id; }

					private:
						/*! \brief Constructor from file */
						WhiteSpace(const Element &el);

						Option<Id> id;

					friend class TextLine;
				};
				typedef std::weak_ptr<WhiteSpace> WhiteSpacePtr;

				/*! \brief Alto hyphen ("HYP" element) */
				class Hyphen: public LineElement
				{
					public:
						Hyphen(const Hyphen&) = delete;
						Hyphen(Hyphen&&) = default;
						virtual ~Hyphen() override {}
						Hyphen& operator=(const Hyphen&) = delete;
						Hyphen& operator=(Hyphen&&) = default;

						/*! \brief Returns hyphenation marker */
						StringUTF8 GetContent() const;
						/*! \brief Sets the hyphenation marker */
						void SetContent(const StringUTF8 &s);

					private:
						/*! \brief Constructor */
						Hyphen(const Element &el);

					friend class TextLine;
				};
				typedef std::weak_ptr<Hyphen> HyphenPtr;

			private:
				/*! \brief Constructor from file */
				TextLine(const Element &el);
				/*! \brief Constructor */
				TextLine(const Element &el, const Id &id_, double x, double y, double w, double h);
				/*! \brief Reads the sub-elements */
				void update_subelements();

				Id id;
				mutable std::vector<std::shared_ptr<LineElement> > lineElements;
				mutable std::vector<WordPtr> words;
				mutable std::map<Id, WordPtr> id_words;

			friend class TextBlock;
		};
		typedef std::weak_ptr<TextLine> TextLinePtr;

		/*! \brief Returns the list of text lines in the block */
		std::vector<TextLinePtr> GetTextLines() const;
		/*! \brief Returns the number of text lines in the block */
		size_t GetNbTextLines() const { return lines.size(); }
		/*! \brief Returns a text line contained in the block */
		TextLine& GetTextLine(const Id &id_);
		/*! \brief Adds a text line in the block */
		TextLine& AddTextLine(const Id &id_, double x, double y, double w, double h);
		/*! \brief Adds a text line in the block */
		TextLine& AddTextLineAfter(const Id &pred, const Id &id_, double x, double y, double w, double h);
		/*! \brief Adds a text line in the block */
		TextLine& AddTextLineBefore(const Id &next, const Id &id_, double x, double y, double w, double h);

		/*! \brief Removes a text line */
		void RemoveTextLine(const Id &tid);

	private:
		/*! \brief Constructor from file */
		TextBlock(const Element &el);
		/*! \brief Constructor */
		TextBlock(const Element &el, const Id &id_, int x, int y, int w, int h);
		/*! \brief Updates the textline cache */
		void update_subelements();

		std::vector<std::shared_ptr<TextLine> > lines;
		std::map<Id, TextLinePtr> id_lines;
	
	friend class Space;
};
#else
#error	you cannot include this file directly
#endif

