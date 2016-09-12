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
 * file: CRNAltoWrapper.h
 * \author Yann LEYDIER
 */

#ifndef CRNAltoWrapper_HEADER
#define CRNAltoWrapper_HEADER

#include <CRNXml/CRNAlto.h>
#include <CRNDocument.h>
#include <CRNUtils/CRNProgress.h>
#include <vector>
#include <utility>

namespace crn
{
	namespace xml
	{
		/*! Path to an alto page
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		struct PagePath
		{
			PagePath() { }
			PagePath(const String &v, const Id &p):view_id(v),page_id(p) { }
			PagePath(const String &p);
			PagePath(const PagePath&) = default;
			PagePath(PagePath&&) = default;
			virtual ~PagePath() {}
			PagePath& operator=(const PagePath&) = default;
			PagePath& operator=(PagePath&&) = default;

			inline bool operator==(const PagePath &other) const { return (view_id == other.view_id) && (page_id == other.page_id); }
			inline bool operator!=(const PagePath &other) const { return !operator==(other); }
			inline bool operator<(const PagePath &other) const
			{ if (view_id < other.view_id) return true;
				else if ((view_id == other.view_id) && (page_id < other.page_id)) return true;
				return false; }
			String view_id;
			Id page_id;
			virtual String ToString() const { return view_id + Separator() + page_id; }
			static const crn::String& Separator();
			static const PagePath& NullPath();
		};
		/*! Path to an alto space
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		struct SpacePath: public PagePath
		{
			SpacePath() { }
			SpacePath(const PagePath &p, const Id &s):PagePath(p),space_id(s) { }
			SpacePath(const String &p);
			SpacePath(const SpacePath&) = default;
			SpacePath(SpacePath&&) = default;
			virtual ~SpacePath() override {}
			SpacePath& operator=(const SpacePath&) = default;
			SpacePath& operator=(SpacePath&&) = default;

			inline bool operator==(const SpacePath &other) const { return PagePath::operator==(other) && (space_id == other.space_id); }
			inline bool operator!=(const SpacePath &other) const { return !operator==(other); }
			inline bool operator<(const SpacePath &other) const
			{ if (PagePath::operator<(other)) return true;
				else if ((page_id == other.page_id) && (space_id < other.space_id)) return true;
				return false; }
			Id space_id;
			virtual String ToString() const override { return PagePath::ToString() + Separator() + space_id; }
			static const SpacePath& NullPath();
		};
		/*! Path to an alto block (text block, illustration, etc.)
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		struct BlockPath: public SpacePath
		{
			BlockPath() { }
			BlockPath(const SpacePath &s, const Id &b):SpacePath(s),block_id(b) { }
			BlockPath(const String &p);
			BlockPath(const BlockPath&) = default;
			BlockPath(BlockPath&&) = default;
			virtual ~BlockPath() override { }
			BlockPath& operator=(const BlockPath&) = default;
			BlockPath& operator=(BlockPath&&) = default;

			inline bool operator==(const BlockPath &other) const { return SpacePath::operator==(other) && (block_id == other.block_id); }
			inline bool operator!=(const BlockPath &other) const { return !operator==(other); }
			inline bool operator<(const BlockPath &other) const
			{ if (SpacePath::operator<(other)) return true;
				else if ((space_id == other.space_id) && (block_id < other.block_id)) return true;
				return false; }
			Id block_id;
			virtual String ToString() const override { return SpacePath::ToString() + Separator() + block_id; }
			static const BlockPath& NullPath();
		};
		/*! Path to an alto text line
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		struct TextLinePath: public BlockPath
		{
			TextLinePath() { }
			TextLinePath(const BlockPath &b, const Id &l):BlockPath(b),textline_id(l) { }
			TextLinePath(const String &p);
			TextLinePath(const TextLinePath&) = default;
			TextLinePath(TextLinePath&&) = default;
			virtual ~TextLinePath() override { }
			TextLinePath& operator=(const TextLinePath&) = default;
			TextLinePath& operator=(TextLinePath&&) = default;

			inline bool operator==(const TextLinePath &other) const { return BlockPath::operator==(other) && (textline_id == other.textline_id); }
			inline bool operator!=(const TextLinePath &other) const { return !operator==(other); }
			inline bool operator<(const TextLinePath &other) const
			{ if (BlockPath::operator<(other)) return true;
				else if ((block_id == other.block_id) && (textline_id < other.textline_id)) return true;
				return false; }
			Id textline_id;
			virtual String ToString() const override { return BlockPath::ToString() + Separator() + textline_id; }
			static const TextLinePath& NullPath();
		};
		/*! Path to an alto string
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		struct WordPath: public TextLinePath
		{
			WordPath() { }
			WordPath(const TextLinePath &l, const Id &w):TextLinePath(l),word_id(w) { }
			WordPath(const String &p);
			WordPath(const WordPath&) = default;
			WordPath(WordPath&&) = default;
			virtual ~WordPath() override { }
			WordPath& operator=(const WordPath&) = default;
			WordPath& operator=(WordPath&&) = default;

			inline bool operator==(const WordPath &other) const { return TextLinePath::operator==(other) && (word_id == other.word_id); }
			inline bool operator!=(const WordPath &other) const { return !operator==(other); }
			inline bool operator<(const WordPath &other) const
			{ if (TextLinePath::operator<(other)) return true;
				else if ((textline_id == other.textline_id) && (word_id < other.word_id)) return true;
				return false; }
			Id word_id;
			virtual String ToString() const override { return TextLinePath::ToString() + Separator() + word_id; }
			static const WordPath& NullPath();
		};
		/*! \brief XML Alto file wrapper to crn::Document
		 * 
		 * A class that associates a crn::Document to XML Alto files
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		class AltoWrapper
		{
			public:
				AltoWrapper(const AltoWrapper&) = delete;
				AltoWrapper(AltoWrapper&&) = default;
				~AltoWrapper() {}
				AltoWrapper& operator=(const AltoWrapper&) = delete;
				AltoWrapper& operator=(AltoWrapper&&) = default;

				/*! \brief Creates a wrapper from a directory containing Altos */
				static std::unique_ptr<AltoWrapper> NewFromDir(const crn::Path &directory, const crn::Path &documentname, const crn::Path &imagedirectory = "", crn::Progress *prog = nullptr, bool throw_exceptions = true);
				/*! \brief Creates a wrapper from two directories containing images and Altos with the same base names */
				static std::unique_ptr<AltoWrapper> NewFromDirs(const crn::Path &image_directory, const crn::Path &xml_directory, const crn::Path &documentname, crn::Progress *prog = nullptr, bool throw_exceptions = true);
				/*! \brief Creates a wrapper from a crn::Document that was created by Nimrod */
				static std::unique_ptr<AltoWrapper> NewFromDocument(const crn::SDocument &document, bool create_altos = false, bool throw_exceptions = true);
				/*! \brief Creates a wrapper from a crn::Document that was created by Nimrod */
				static std::unique_ptr<AltoWrapper> NewFromDocument(const crn::Path &documentname, bool create_altos = false, bool throw_exceptions = true);
				/*! \brief Creates a wrapper from a list of images 
				 * \param[in]	begin_	the first element of the list
				 * \param[in]	end_	the end of the list
				 * \param[in]	documentname	the path of the crn::Document to create
				 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
				 */
				template<typename Iter> static std::unique_ptr<AltoWrapper> NewFromImages(Iter begin_, Iter end_, const crn::Path &documentname, bool throw_exceptions = true)
				{
					auto doc = std::make_shared<crn::Document>();
					for (Iter it = begin_; it != end_; ++it)
						doc->AddView(*it);
					doc->Save(documentname);
					return NewFromDocument(doc, true, throw_exceptions);
				}
				/*! \brief Creates a wrapper from list of images and Alto paths (such as a map or any container<pair>) 
				 * \param[in]	begin_	the first element of the list
				 * \param[in]	end_	the end of the list
				 * \param[in]	documentname	the path of the crn::Document to create
				 * \param[in]	prog	a progress object
				 * \param[in]	throw_exceptions	shall an exception be thrown on character encoding conversion error?
				 */
				template<typename Iter> static std::unique_ptr<AltoWrapper> NewFromList(Iter begin_, Iter end_, const crn::Path &documentname, crn::Progress *prog = nullptr, bool throw_exceptions = true)
				{
					std::vector<std::pair<crn::Path, crn::Path>> imgxml;
					for (Iter it = begin_; it != end_; ++it)
						imgxml.push_back(std::make_pair(it->first, it->second));
					return newFromList(imgxml, documentname, prog, throw_exceptions);
				}

				/*! \brief Creates CRNBlocks and ids where there is none */
				void Synchronize(bool reset = false);

				/*! \brief Adds a view to the document */
				const String AddView(const Path &imagename, const Path &altoname = "");

				SDocument GetDocument() { return doc; }
				SCDocument GetDocument() const { return doc; }

				/*! \brief Internal class used to save modifications at the right time
				 * \internal
				 * \ingroup xml
				 * \author Yann LEYDIER
				 */
				class ViewLock
				{
					public:
						ViewLock(const ViewLock&) = delete;
						ViewLock(ViewLock &&v):block(std::move(v.block)),alto(std::move(v.alto)) {}
						~ViewLock() { if (block) block->Save(); if (alto) alto->Save(); }
						ViewLock& operator=(const ViewLock&) = delete;
						ViewLock& operator=(ViewLock &&v) { block = std::move(v.block); alto = std::move(v.alto); return *this; }

						SBlock GetBlock() { return block; }
						SCBlock GetBlock() const { return block; }
						SAlto GetAlto() { return alto; }
						SCAlto GetAlto() const { return alto; }

					private:
						ViewLock(const SBlock &b, const SAlto &a):block(b),alto(a) { }

						SBlock block;
						SAlto alto;

						friend class AltoWrapper;
				};
				/*! \brief Word proxy
				 * \ingroup xml
				 * \author Yann LEYDIER
				 */
				class Word
				{
					public:
						Word(const SBlock &b, Alto::Layout::Page::Space::TextBlock::TextLine::Word &w, const std::shared_ptr<ViewLock> &l, const TextLinePath &t):block(b),word(&w),lock(l),path(t, w.GetId().Get()) { }
						Word(const Word&) = default;
						Word(Word&&) = default;
						~Word() { }
						Word& operator=(const Word&) = default;
						Word& operator=(Word&&) = default;

						/*! \brief Gets the toplevel alto file */
						SAlto GetAlto() { return lock->GetAlto(); }
						/*! \brief Returns the alto */
						SCAlto GetAlto() const { return lock->GetAlto(); }
						/*! \brief Gets the alto element */
						Alto::Layout::Page::Space::TextBlock::TextLine::Word& GetElement() { return *word; }
						/*! \brief Gets the alto element */
						const Alto::Layout::Page::Space::TextBlock::TextLine::Word& GetElement() const { return *word; }

						/*! \brief Returns the path to the string */
						const WordPath& GetPath() const { return path; }

						/*! \brief Returns the id of the element */
						const Id& GetId() const { return word->GetId().Get(); }
						/*! \brief Returns the list of style references */
						std::vector<Id> GetStyles() const { return word->GetStyles(); }
						/*! \brief Returns the text styles */
						std::vector<Alto::Styles::Text> GetTextStyles();
						/*! \brief Returns the paragraph styles */
						std::vector<Alto::Styles::Paragraph> GetParagraphStyles();
						/*! \brief Adds a reference to a style */
						void AddStyle(const Id &styleid) { word->AddStyle(styleid); }
						/*! \brief Adds a reference to a style */
						void RemoveStyle(const Id &styleid) { word->RemoveStyle(styleid); }
						/*! \brief Gets the coordinates of the word */
						const Rect& GetBBox() const { return block->GetAbsoluteBBox(); }
						/*! \brief Sets the coordinates of the word */
						void SetBBox(const crn::Rect &r);
						/*! \brief Returns the transcription of the word */
						StringUTF8 GetContent() const { return word->GetContent(); }
						/*! \brief Sets the transcription of the word */
						void SetContent(const StringUTF8 &s) { word->SetContent(s); }
						/*! \brief Returns the font style of the word */
						Option<Alto::Styles::Text::FontStyle> GetFontStyle() const { return word->GetFontStyle(); }
						/*! \brief Sets the font style of the word */
						void SetFontStyle(Alto::Styles::Text::FontStyle fs) { word->SetFontStyle(fs); }
						/*! \brief Unsets the font style of the word */
						void UnsetFontStyle() { word->UnsetFontStyle(); }
						/*! \brief Returns the substitution type of the word */
						Option<AltoWord::SubstitutionType> GetSubstitutionType() const { return word->GetSubstitutionType(); }
						/*! \brief Returns the substitution of the word */
						Option<StringUTF8> GetSubstitutionContent() { return word->GetSubstitutionContent(); }
						/*! \brief Sets the substitution of the word */
						void SetSubstitution(AltoWord::SubstitutionType stype, const StringUTF8 &scontent) { word->SetSubstitution(stype, scontent); }
						/*! \brief Returns the image */
						SBlock GetBlock() { return block; }
						/*! \brief Returns the image */
						SCBlock GetBlock() const { return block; }

						/*! \brief Returns the OCR confidence of the word [0, 1] */
						Option<double> GetWC() const { return word->GetWC(); }
						/*! \brief Sets the OCR confidence of the word [0, 1] */
						void SetWC(double conf) { word->SetWC(conf); }

					private:
						SBlock block;
						Alto::Layout::Page::Space::TextBlock::TextLine::Word *word;
						std::shared_ptr<ViewLock> lock;
						WordPath path;
				};
				/*! \brief Gets a Word by path */
				Word GetWord(const WordPath &p);

				/*! \brief Text line proxy
				 * \ingroup xml
				 * \author Yann LEYDIER
				 */
				class TextLine
				{
					public:
						TextLine(const SBlock &b, Alto::Layout::Page::Space::TextBlock::TextLine &t, const std::shared_ptr<ViewLock> &l, const BlockPath &bp):block(b),textline(&t),lock(l),path(bp, t.GetId()) { }
						TextLine(const TextLine&) = default;
						TextLine(TextLine&&) = default;
						~TextLine() { }
						TextLine& operator=(const TextLine&) = default;
						TextLine& operator=(TextLine&&) = default;

						/*! \brief Gets the toplevel alto file */
						SAlto GetAlto() { return lock->GetAlto(); }
						/*! \brief Returns the alto */
						SCAlto GetAlto() const { return lock->GetAlto(); }
						/*! \brief Gets the alto element */
						Alto::Layout::Page::Space::TextBlock::TextLine& GetElement() { return *textline; }
						/*! \brief Gets the alto element */
						const Alto::Layout::Page::Space::TextBlock::TextLine& GetElement() const { return *textline; }

						/*! \brief Returns the path to the text line */
						const TextLinePath& GetPath() const { return path; }

						/*! \brief Returns the id of the element */
						const Id& GetId() const { return textline->GetId(); }
						/*! \brief Returns the list of style references */
						std::vector<Id> GetStyles() const { return textline->GetStyles(); }
						/*! \brief Returns the text styles */
						std::vector<Alto::Styles::Text> GetTextStyles();
						/*! \brief Returns the paragraph styles */
						std::vector<Alto::Styles::Paragraph> GetParagraphStyles();
						/*! \brief Adds a reference to a style */
						void AddStyle(const Id &styleid) { textline->AddStyle(styleid); }
						/*! \brief Adds a reference to a style */
						void RemoveStyle(const Id &styleid) { textline->RemoveStyle(styleid); }
						/*! \brief Gets the coordinates of the text line */
						const Rect& GetBBox() const { return block->GetAbsoluteBBox(); }
						/*! \brief Sets the coordinates of the text line */
						void SetBBox(const crn::Rect &r, bool erase_oob);
						/*! \brief Returns the ordinate of the baseline */
						Option<double> GetBaseline() const { return textline->GetBaseline(); }
						/*! \brief Sets the baseline ordinate */
						void SetBaseline(double d, bool check_bounds = true) { textline->SetBaseline(d, check_bounds); }
						/*! \brief Unsets the baseline ordinate */
						void UnsetBaseline() { textline->UnsetBaseline(); }
						/*! \brief Returns whether the line was manually corrected or not */
						Option<bool> GetManuallyCorrected() { return textline->GetManuallyCorrected(); }// CS element

						/*! \brief Returns the image */
						SBlock GetBlock() { return block; }
						/*! \brief Returns the image */
						SCBlock GetBlock() const { return block; }

						/*! \brief Returns the ids of the words in the line */
						std::vector<Id> GetWords() const;
						/*! \brief Returns the number of words in the line */
						size_t GetNbWords() const { return textline->GetNbWords(); }
						/*! \brief Gets a word proxy */
						Word GetWord(const Id &id);
						/*! \brief Gets a word proxy */
						const Word GetWord(const Id &id) const;
						/*! \brief Adds a word to the line */
						Word AddWord(const StringUTF8 &text, const crn::Rect &bbox);
						/*! \brief Adds a word to the line */
						Word AddWordAfter(const Id &pred, const StringUTF8 &text, const crn::Rect &bbox);
						/*! \brief Adds a word to the line */
						Word AddWordBefore(const Id &next, const StringUTF8 &text, const crn::Rect &bbox);

						/*! \brief Removes a line element */
						void RemoveWord(const Id &wid);

					private:
						SBlock block;
						Alto::Layout::Page::Space::TextBlock::TextLine *textline;
						std::shared_ptr<ViewLock> lock;
						TextLinePath path;
				};
				/*! \brief Gets a TextLine by path */
				TextLine GetTextLine(const TextLinePath &p);

				/*! \brief Text block proxy
				 * \ingroup xml
				 * \author Yann LEYDIER
				 */
				class TextBlock
				{
					public:
						TextBlock(const SBlock &b, Alto::Layout::Page::Space::TextBlock &t, const std::shared_ptr<ViewLock> &l, const SpacePath &s):block(b),textblock(&t),lock(l),path(s, t.GetId()) { }
						TextBlock(const TextBlock&) = default;
						TextBlock(TextBlock&&) = default;
						~TextBlock() { }
						TextBlock& operator=(const TextBlock&) = default;
						TextBlock& operator=(TextBlock&&) = default;

						/*! \brief Gets the toplevel alto file */
						SAlto GetAlto() { return lock->GetAlto(); }
						/*! \brief Returns the alto */
						SCAlto GetAlto() const { return lock->GetAlto(); }
						/*! \brief Gets the alto element */
						Alto::Layout::Page::Space::TextBlock& GetElement() { return *textblock; }
						/*! \brief Gets the alto element */
						const Alto::Layout::Page::Space::TextBlock& GetElement() const { return *textblock; }

						/*! \brief Returns the path to the text block */
						const BlockPath& GetPath() const { return path; }

						/*! \brief Returns the id of the element */
						const Id& GetId() const { return textblock->GetId(); }
						/*! \brief Returns the list of style references */
						std::vector<Id> GetStyles() const { return textblock->GetStyles(); }
						/*! \brief Returns the text styles */
						std::vector<Alto::Styles::Text> GetTextStyles();
						/*! \brief Returns the paragraph styles */
						std::vector<Alto::Styles::Paragraph> GetParagraphStyles();
						/*! \brief Adds a reference to a style */
						void AddStyle(const Id &styleid) { textblock->AddStyle(styleid); }
						/*! \brief Removes a reference to a style */
						void RemoveStyle(const Id &styleid) { textblock->RemoveStyle(styleid); }
						/*! \brief Gets the coordinates of the text block */
						const Rect& GetBBox() const { return block->GetAbsoluteBBox(); }
						/*! \brief Sets the coordinates of the text block */
						void SetBBox(const crn::Rect &r, bool erase_oob);
						/*! \brief Returns the language of the text inside the block */
						Option<StringUTF8> GetLanguage() const { return textblock->GetLanguage(); }

						/*! \brief Returns the image */
						SBlock GetBlock() { return block; }
						/*! \brief Returns the image */
						SCBlock GetBlock() const { return block; }

						/*! \brief Returns the ids of the text lines in the page space */
						std::vector<Id> GetTextLines() const;
						/*! \brief Returns the number of text lines in the page space */
						size_t GetNbTextLines() const { return textblock->GetNbTextLines(); }
						/*! \brief Gets a text line proxy */
						TextLine GetTextLine(const Id &id);
						/*! \brief Adds a text line to the block */
						TextLine AddTextLine(const crn::Rect &bbox);
						/*! \brief Adds a text line to the block */
						TextLine AddTextLineAfter(const Id &pred, const crn::Rect &bbox);
						/*! \brief Adds a text line to the block */
						TextLine AddTextLineBefore(const Id &next, const crn::Rect &bbox);

						/*! \brief Removes a text line */
						void RemoveTextLine(const Id &tid);

					private:
						SBlock block;
						Alto::Layout::Page::Space::TextBlock *textblock;
						std::shared_ptr<ViewLock> lock;
						BlockPath path;
				};
				/*! \brief Gets a TextBlock by path */
				TextBlock GetTextBlock(const BlockPath &p);

				/*! \brief Page space proxy
				 * \ingroup xml
				 * \author Yann LEYDIER
				 */
				class Space
				{
					public:
						Space(const SBlock &b, Alto::Layout::Page::Space &s, const std::shared_ptr<ViewLock> &l, const PagePath &p):block(b),space(&s),lock(l),path(p, s.GetId().Get()) { }
						Space(const Space&) = default;
						Space(Space&&) = default;
						~Space() { }
						Space& operator=(const Space&) = default;
						Space& operator=(Space&&) = default;

						/*! \brief Gets the toplevel alto file */
						SAlto GetAlto() { return lock->GetAlto(); }
						/*! \brief Returns the alto */
						SCAlto GetAlto() const { return lock->GetAlto(); }
						/*! \brief Gets the alto element */
						Alto::Layout::Page::Space& GetElement() { return *space; }
						/*! \brief Gets the alto element */
						const Alto::Layout::Page::Space& GetElement() const { return *space; }

						/*! \brief Returns the path to the space */
						const SpacePath& GetPath() const { return path; }

						/*! \brief Returns the id of the element */
						const Id& GetId() const { return space->GetId().Get(); }
						/*! \brief Returns the list of style references */
						std::vector<Id> GetStyles() const { return space->GetStyles(); }
						/*! \brief Returns the text styles */
						std::vector<Alto::Styles::Text> GetTextStyles();
						/*! \brief Returns the paragraph styles */
						std::vector<Alto::Styles::Paragraph> GetParagraphStyles();
						/*! \brief Adds a reference to a style */
						void AddStyle(const Id &styleid) { space->AddStyle(styleid); }
						/*! \brief Adds a reference to a style */
						void RemoveStyle(const Id &styleid) { space->RemoveStyle(styleid); }
						/*! \brief Gets the coordinates of the space */
						const Rect& GetBBox() const { return block->GetAbsoluteBBox(); }
						/*! \brief Sets the coordinates of the space */
						void SetBBox(const crn::Rect &r, bool erase_oob);

						/*! \brief Returns the image */
						SBlock GetBlock() { return block; }
						/*! \brief Returns the image */
						SCBlock GetBlock() const { return block; }

						/*! \brief Returns the ids of the text blocks in the page space */
						std::vector<Id> GetTextBlocks() const;
						/*! \brief Gets a text block proxy */
						TextBlock GetTextBlock(const Id &id);
						/*! \brief Adds a text block to the space */
						TextBlock AddTextBlock(const crn::Rect &bbox);
						/*! \brief Adds a text block to the space */
						TextBlock AddTextBlockAfter(const Id &pred, const crn::Rect &bbox);
						/*! \brief Adds a text block to the space */
						TextBlock AddTextBlockBefore(const Id &next, const crn::Rect &bbox);

						/*! \brief Removes a block */
						void RemoveBlock(const Id &bid);

					private:
						SBlock block;
						Alto::Layout::Page::Space *space;
						std::shared_ptr<ViewLock> lock;
						SpacePath path;
				};
				/*! \brief Gets a Space by path */
				Space GetSpace(const SpacePath &p);

				/*! \brief Page proxy
				 * \ingroup xml
				 * \author Yann LEYDIER
				 */
				class Page
				{
					public:
						Page(const SBlock &b, Alto::Layout::Page &p, const std::shared_ptr<ViewLock> &l, const String &vid):block(b),page(&p),lock(l),path(vid, p.GetId()) { }
						Page(const Page&) = default;
						Page(Page&&) = default;
						~Page() {}
						Page& operator=(const Page&) = default;
						Page& operator=(Page&&) = default;

						/*! \brief Gets the toplevel alto file */
						SAlto GetAlto() { return lock->GetAlto(); }
						/*! \brief Returns the alto */
						SCAlto GetAlto() const { return lock->GetAlto(); }
						/*! \brief Gets the alto element */
						Alto::Layout::Page& GetElement() { return *page; }
						/*! \brief Gets the alto element */
						const Alto::Layout::Page& GetElement() const { return *page; }

						/*! \brief Returns the path to the page */
						const PagePath& GetPath() const { return path; }

						/*! \brief Returns the id of the element */
						const Id& GetId() const { return page->GetId(); }
						/*! \brief Returns the class of the page (user defined class such as "title") */
						Option<StringUTF8> GetPageClass() const { return page->GetPageClass(); }
						/*! \brief Sets the class of the page (user defined class such as "title") */
						void SetPageClass(const StringUTF8 &s) { page->SetPageClass(s); }
						/*! \brief Returns the list of style references */
						std::vector<Id> GetStyles() const { return page->GetStyles(); }
						/*! \brief Returns the text styles */
						std::vector<Alto::Styles::Text> GetTextStyles();
						/*! \brief Returns the paragraph styles */
						std::vector<Alto::Styles::Paragraph> GetParagraphStyles();
						/*! \brief Adds a reference to a style */
						void AddStyle(const Id &styleid) { page->AddStyle(styleid); }
						/*! \brief Adds a reference to a style */
						void RemoveStyle(const Id &styleid) { page->RemoveStyle(styleid); }
						/*! \brief Gets the coordinates of the page  */
						const Rect& GetBBox() const { return block->GetAbsoluteBBox(); }
						/*! \brief Sets the coordinates of the page  */
						void SetBBox(const crn::Rect &r, bool erase_oob);
						/*! \brief Returns the number of the page within the document */
						int GetPhysicalImageNumber() const { return page->GetPhysicalImageNumber(); }
						/*! \brief Sets the number of the page within the document */
						void SetPhysicalImageNumber(int pnum) { page->SetPhysicalImageNumber(pnum); }
						/*! \brief Returns the page number that is printed on the document */
						Option<StringUTF8> GetPrintedImageNumber() const { return page->GetPrintedImageNumber(); }
						/*! \brief Sets the page number that is printed on the document */
						void SetPrintedImageNumber(const StringUTF8 &s) { page->SetPrintedImageNumber(s); }
						/*! \brief Returns the quality of the original page */
						Option<AltoPage::Quality> GetQuality() const { return page->GetQuality(); }
						/*! \brief Sets the quality of the original page */
						void SetQuality(AltoPage::Quality q) { page->SetQuality(q); }
						/*! \brief Returns details on the quality of the original page */
						Option<StringUTF8> GetQualityDetail() const { return page->GetQualityDetail(); }
						/*! \brief Sets the details on the quality of the original page */
						void SetQualityDetail(const StringUTF8 &s) { page->SetQualityDetail(s); }
						/*! \brief Returns the position of the page */
						Option<AltoPage::Position> GetPosition() const { return page->GetPosition(); }
						/*! \brief Returns the id of the processing applied to the page */
						Option<Id> GetProcessing() const { return page->GetProcessing(); }
						/*! \brief Returns the estimated % of OCR accuracy on the page [0, 100] */
						Option<double> GetAccuracy() const { return page->GetAccuracy(); }
						/*! \brief Sets the estimated % of OCR accuracy on the page [0, 100] */
						void SetAccuracy(double acc) { page->SetAccuracy(acc); }
						/*! \brief Returns the confidence of OCR on the page [0, 1] */
						Option<double> GetPageConfidence() const { return page->GetPageConfidence(); }
						/*! \brief Sets the confidence of OCR on the page [0, 1] */
						void SetPageConfidence(double c) { page->SetPageConfidence(c); }

						/*! \brief Returns the image */
						SBlock GetBlock() { return block; }
						/*! \brief Returns the image */
						SCBlock GetBlock() const { return block; }

						/*! \brief Returns the ids of the spaces in the page */
						std::vector<Id> GetSpaces() const;
						/*! \brief Gets a space proxy */
						Space GetSpace(const Id &spaceid);
						/*! \brief Gets the space proxy on the top margin */
						Space GetTopMargin();
						/*! \brief Adds a top margin */
						Space AddTopMargin(const crn::Rect &bbox);
						/*! \brief Gets the space proxy on the left margin */
						Space GetLeftMargin();
						/*! \brief Adds a left margin */
						Space AddLeftMargin(const crn::Rect &bbox);
						/*! \brief Gets the space proxy on the bottom margin */
						Space GetBottomMargin();
						/*! \brief Adds a bottom margin */
						Space AddBottomMargin(const crn::Rect &bbox);
						/*! \brief Gets the space proxy on the right margin */
						Space GetRightMargin();
						/*! \brief Adds a right margin */
						Space AddRightMargin(const crn::Rect &bbox);
						/*! \brief Gets the space proxy on the print space */
						Space GetPrintSpace();
						/*! \brief Adds the print space */
						Space AddPrintSpace(const crn::Rect &bbox);

						/*! \brief Removes a space */
						void RemoveSpace(const Id &sid);

						/*! \brief Adds a copy of another page's content */
						//void ImportContent(Page &other, const crn::Rect &crop = crn::Rect());

					private:
						SBlock block;
						Alto::Layout::Page *page;
						std::shared_ptr<ViewLock> lock;
						PagePath path;
				};
				/*! \brief Gets a Page by path */
				Page GetPage(const PagePath &p);

				/*! \brief Wrapper on a view
				 * \ingroup xml
				 * \author Yann LEYDIER
				 */
				class View
				{
					public:
						/*! \brief Constructor */
						View(const std::shared_ptr<ViewLock> &l, const String &view_id):lock(l), id(view_id) { }
						View(const View&) = default;
						View(View&&) = default;
						~View() { }
						View& operator=(const View&) = default;
						View& operator=(View&&) = default;

						/*! \brief Returns the id of the view */
						const String& GetId() const { return id; }
						/*! \brief Returns the image */
						SBlock GetBlock() { return lock->GetBlock(); }
						/*! \brief Returns the image */
						SCBlock GetBlock() const { return lock->GetBlock(); }
						/*! \brief Returns the alto */
						SAlto GetAlto() { return lock->GetAlto(); }
						/*! \brief Returns the alto */
						SCAlto GetAlto() const { return lock->GetAlto(); }

						/*! \brief Gets the global description part of the Alto (may be null) */
						Alto::Description& GetDescription() { return lock->GetAlto()->GetDescription(); }

						/*! \brief Gets the styles description part of the Alto (may be null) */
						Alto::Styles& GetStyles() { return lock->GetAlto()->GetStyles(); }

						/*! \brief Gets the list of page ids */
						std::vector<Id> GetPages();
						/*! \brief Gets a page */
						Page GetPage(const Id &pageId);
						/*! \brief Adds a page */
						Page AddPage(int image_number, int w, int h, Option<AltoPage::Position> pos = Option<AltoPage::Position>());
						/*! \brief Adds a page */
						Page AddPageAfter(const Id &pred, int image_number, int w, int h, Option<AltoPage::Position> pos = Option<AltoPage::Position>());
						/*! \brief Adds a page */
						Page AddPageBefore(const Id &next, int image_number, int w, int h, Option<AltoPage::Position> pos = Option<AltoPage::Position>());
						/*! \brief Removes a page */
						void RemovePage(const Id &pageId);

						/*! \brief Changes the size of a word and all its parents if needed */
						void ResizeWord(const WordPath &p, const crn::Rect &r);
						/*! \brief Changes the size of a line and all its parents if needed */
						void ResizeTextLine(const TextLinePath &p, const crn::Rect &r, bool erase_oob);
						/*! \brief Changes the size of a text block and all its parents if needed */
						void ResizeTextBlock(const BlockPath &p, const crn::Rect &r, bool erase_oob);
						/*! \brief Changes the size of a space and all its parents if needed */
						void ResizeSpace(const SpacePath &p, const crn::Rect &r, bool erase_oob);
					private:

						std::shared_ptr<ViewLock> lock;
						String id;
				};
				/*! \brief Gets the list of the view ids of the document */
				std::vector<String> GetViewIds() const { return doc->GetViewIds(); }
				/*! \brief Gets a view by id */
				View GetView(const String &view_id);
				/*! \brief Returns the number of views */
				size_t GetNbViews() const { return doc->GetNbViews(); }
				/*! \brief Gets a view by index */
				View GetView(size_t index);

				/*! \brief Changes the size of a word and all its parents if needed */
				void ResizeWord(const WordPath &p, const crn::Rect &r);
				/*! \brief Changes the size of a line and all its parents if needed */
				void ResizeTextLine(const TextLinePath &p, const crn::Rect &r, bool erase_oob);
				/*! \brief Changes the size of a text block and all its parents if needed */
				void ResizeTextBlock(const BlockPath &p, const crn::Rect &r, bool erase_oob);
				/*! \brief Changes the size of a space and all its parents if needed */
				void ResizeSpace(const SpacePath &p, const crn::Rect &r, bool erase_oob);

				static const String& AltoPathKey(); /*!< the key to access the Alto path in a wrapped crn::Document */
				static const String& PageKey(); /*!< the key to access the pages in a wrapped crn::Block */
				static const String& SpaceKey(); /*!< the key to access the spaces in a wrapped crn::Block */
				static const String& TextBlockKey(); /*!< the key to access the text block in a wrapped crn::Block */
				static const String& IllustrationKey(); /*!< the key to access the illustrations in a wrapped crn::Block */
				static const String& GraphicalElementKey(); /*!< the key to access the graphical elements in a wrapped crn::Block */
				static const String& ComposedBlockKey(); /*!< the key to access the composed blocks in a wrapped crn::Block */
				static const String& TextLineKey(); /*!< the key to access the text lines in a wrapped crn::Block */
				static const String& WordKey(); /*!< the key to access the words in a wrapped crn::Block */

			private:
				/*! \brief Creates a wrapper from list of images and Alto paths */
				static std::unique_ptr<AltoWrapper> newFromList(const std::vector<std::pair<crn::Path, crn::Path> > &filelist, const crn::Path &documentname, crn::Progress *prog, bool throw_exceptions);
				/*! \brief Constructor */
				AltoWrapper(bool throw_exceptions);
				/*! \brief Creates empty altos where needed */
				void createAltos();

				std::shared_ptr<ViewLock> getLock(const String &view_id) const;

				SDocument doc;
				mutable std::map<String, std::weak_ptr<ViewLock> > viewLocks;
				bool throws;
		};
		CRN_ALIAS_SMART_PTR(AltoWrapper)
	}
}


#endif

