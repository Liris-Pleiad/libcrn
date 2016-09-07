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
 * file: CRNAlto.h
 * \author Yann LEYDIER
 */

#ifndef CRNAlto_HEADER
#define CRNAlto_HEADER

#include <CRNXml/CRNAltoUtils.h>
#include <CRNUtils/CRNOption.h>
#include <set>
#include <map>

namespace crn
{
	namespace xml
	{
		/*! \brief XML Alto file wrapper
		 * 
		 * A class to load, modify and save an XML Alto file
		 * \ingroup xml
		 * \author Yann LEYDIER
		 */
		class Alto: public Document
		{
			public:
				/*! \brief Constructor from a file */
				Alto(const Path &fname, bool char_conversion_throws = true);
				/*! \brief Constructor from image */
				Alto(const Path &imagename, const StringUTF8 &ns, const StringUTF8 &encoding = "UTF-8", const StringUTF8 &version = "1.0", bool char_conversion_throws = true);
				Alto(const Alto&) = delete;
				Alto(Alto&&) = default;
				virtual ~Alto() override {}
				Alto& operator=(const Alto&) = delete;
				Alto& operator=(Alto&&) = default;

				class Description;
				class Styles;
				class Layout;
			private:
				/*! Root element of the XML Alto */
				class Root: public Element
				{
					public:
						Root(const Root&) = delete;
						Root(Root&&) = default;
						virtual ~Root() override {}
						Root& operator=(const Root&) = delete;
						Root& operator=(Root&&) = default;

						Description& GetDescription() { return *description; }
						const Description& GetDescription() const { return *description; }
						Styles& GetStyles() { return *styles; }
						const Styles& GetStyles() const { return *styles; }
						Layout& GetLayout() { return *layout; }
						const Layout& GetLayout() const { return *layout; }

					private:
						/*! \brief Constructor from file */
						Root(const Element &el);
						/*! \brief Constructor from file */
						Root(const Element &el, const Path &imagename, const StringUTF8 &ns);
						/*! \brief Creates the inner elements if needed */
						void init(const StringUTF8 &imgname);

						std::unique_ptr<Description> description;
						std::unique_ptr<Styles> styles;
						std::unique_ptr<Layout> layout;

					friend class Alto;
				};
			public:
#include <CRNXml/CRNAltoDescription.hpp>
				/*! \brief Gets the global description part of the Alto (may be null) */
				Description& GetDescription() { return root->GetDescription(); }
				const Description& GetDescription() const { return root->GetDescription(); }

#include <CRNXml/CRNAltoStyles.hpp>
				/*! \brief Gets the styles description part of the Alto (may be null) */
				Styles& GetStyles() { return root->GetStyles(); }
				const Styles& GetStyles() const { return root->GetStyles(); }

#include <CRNXml/CRNAltoLayout.hpp>
				/*! \brief Gets the layout description part of the Alto */
				Layout& GetLayout() { return root->GetLayout(); }
				const Layout& GetLayout() const { return root->GetLayout(); }

				Layout::Page& GetPage(const Id &id);
				const Layout::Page& GetPage(const Id &id) const;
				Layout::Page::Space& GetSpace(const Id &id);
				const Layout::Page::Space& GetSpace(const Id &id) const;
				Layout::Page::Space::Block& GetBlock(const Id &id);
				const Layout::Page::Space::Block& GetBlock(const Id &id) const;
				Layout::Page::Space::TextBlock& GetTextBlock(const Id &id);
				const Layout::Page::Space::TextBlock& GetTextBlock(const Id &id) const;
				Layout::Page::Space::TextBlock::TextLine& GetTextLine(const Id &id);
				const Layout::Page::Space::TextBlock::TextLine& GetTextLine(const Id &id) const;
				Layout::Page::Space::TextBlock::TextLine::Word& GetWord(const Id &id);
				const Layout::Page::Space::TextBlock::TextLine::Word& GetWord(const Id &id) const;
				Element& GetElement(const Id &id);
				const Element& GetElement(const Id &id) const;

				/*! \brief Creates a new id for the document */
				Id CreateId();
				/*! \brief Checks if an id already exists in the document */
				bool CheckId(const Id &id) const;

				/*! \brief Adds an id to an element */
				Id AddId(Element &el);

			private:
				/*! \brief Adds an element to the global map */
				void register_ids();
				
				//std::vector<std::shared_ptr<Node> > nodes;
				std::unique_ptr<Root> root;
				std::set<Id> ids;
		};

		using AltoDescription = Alto::Description;
		using AltoStyles = Alto::Styles;
		using AltoLayout = Alto::Layout;
		using AltoPage = Alto::Layout::Page;
		using AltoPagePtr = Alto::Layout::PagePtr;
		using AltoSpace = Alto::Layout::Page::Space;
		using AltoSpacePtr = Alto::Layout::Page::SpacePtr;
		using AltoBlock = Alto::Layout::Page::Space::Block;
		using AltoBlockPtr = Alto::Layout::Page::Space::BlockPtr;
		using AltoTextBlock = Alto::Layout::Page::Space::TextBlock;
		using AltoTextBlockPtr = Alto::Layout::Page::Space::TextBlockPtr;
		using AltoIllustration = Alto::Layout::Page::Space::Illustration;
		using AltoIllustrationPtr = Alto::Layout::Page::Space::IllustrationPtr;
		using AltoGraphicalElement = Alto::Layout::Page::Space::GraphicalElement;
		using AltoGraphicalElementPtr = Alto::Layout::Page::Space::GraphicalElementPtr;
		using AltoComposedBlock = Alto::Layout::Page::Space::ComposedBlock;
		using AltoComposedBlockPtr = Alto::Layout::Page::Space::ComposedBlockPtr;
		using AltoTextLine = Alto::Layout::Page::Space::TextBlock::TextLine;
		using AltoTextLinePtr = Alto::Layout::Page::Space::TextBlock::TextLinePtr;
		using AltoTextLineElement = Alto::Layout::Page::Space::TextBlock::TextLine::LineElement;
		using AltoTextLineElementPtr = Alto::Layout::Page::Space::TextBlock::TextLine::LineElementPtr;
		using AltoWord = Alto::Layout::Page::Space::TextBlock::TextLine::Word;
		using AltoWordPtr = Alto::Layout::Page::Space::TextBlock::TextLine::WordPtr;
		using AltoWhiteSpace = Alto::Layout::Page::Space::TextBlock::TextLine::WhiteSpace;
		using AltoWhiteSpacePtr = Alto::Layout::Page::Space::TextBlock::TextLine::WhiteSpacePtr;
		using AltoHyphen = Alto::Layout::Page::Space::TextBlock::TextLine::Hyphen;
		using AltoHyphenPtr = Alto::Layout::Page::Space::TextBlock::TextLine::HyphenPtr;

		CRN_ALIAS_SMART_PTR(Alto)
	}
}

CRN_DECLARE_ENUM_OPERATORS(crn::xml::AltoStyles::Text::FontStyle)

#endif

