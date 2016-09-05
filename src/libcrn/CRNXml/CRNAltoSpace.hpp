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
 * file: CRNAltoSpace.h
 * \author Yann LEYDIER
 */

/*! \cond */
#ifdef CRNAlto_HEADER
/*! \endcond */

/*! \brief A print space on a page */
class Space: public Element
{
	public:
		Space(const Space&) = delete;
		Space(Space&&) = default;
		virtual ~Space() override {}
		Space& operator=(const Space&) = delete;
		Space& operator=(Space&&) = default;

		/*! \brief Returns the id of the element */
		Option<Id> GetId() const;

		/*! \brief Returns the list of style references */
		std::vector<Id> GetStyles() const;
		/*! \brief Adds a reference to a style */
		void AddStyle(const Id &styleid);
		/*! \brief Adds a reference to a style */
		void RemoveStyle(const Id &styleid);

		/*! \brief Returns the height of the element */
		double GetHeight() const;
		/*! \brief Sets the height of the element */
		void SetHeight(double d);
		/*! \brief Returns the width of the element */
		double GetWidth() const;
		/*! \brief Sets the width of the element */
		void SetWidth(double d);
		/*! \brief Returns the abscissa of the element */
		double GetHPos() const;
		/*! \brief Sets the abscissa of the element */
		void SetHPos(double d);
		/*! \brief Returns the ordinate of the element */
		double GetVPos() const;
		/*! \brief Sets the ordinate of the element */
		void SetVPos(double d);

		/*! \brief Base class for blocks in spaces  */
		class Block: public Element
		{
			public:
				Block(const Block&) = delete;
				Block(Block&&) = default;
				virtual ~Block() override {}
				Block& operator=(const Block&) = delete;
				Block& operator=(Block&&) = default;

				/*! \brief Returns the id of the element */
				const Id& GetId() const { return id; }

				/*! \brief Returns the list of style references */
				std::vector<Id> GetStyles() const;
				/*! \brief Adds a reference to a style */
				void AddStyle(const Id &styleid);
				/*! \brief Removes a reference to a style */
				void RemoveStyle(const Id &styleid);

				/*! \brief Returns the height of the element */
				int GetHeight() const;
				/*! \brief Sets the height of the element */
				void SetHeight(int i);
				/*! \brief Returns the width of the element */
				int GetWidth() const;
				/*! \brief Sets the width of the element */
				void SetWidth(int i);
				/*! \brief Returns the abscissa of the element */
				int GetHPos() const;
				/*! \brief Sets the abscissa of the element */
				void SetHPos(int i);
				/*! \brief Returns the ordinate of the element */
				int GetVPos() const;
				/*! \brief Sets the ordinate of the element */
				void SetVPos(int i);
				/*! \brief Returns the rotation of the object, in degree, counterclockwise */
				Option<double> GetRotation() const;
				/*! \brief Returns the id of the next block */
				Option<Id> GetNextId() const;

			protected:
				/*! \brief Constructor */
				Block(const Element &el);
				/*! \brief Constructor */
				Block(const Element &el, const Id &id_, int x, int y, int w, int h);

				Id id;
				// std::vector<crn::WeakPointer<Shape> > shapes; // TODO
		};
		typedef std::weak_ptr<Block> BlockPtr;
		/*! \brief Returns a text block contained in the print space */
		Block& GetBlock(const Id &bid);
		/*! \brief Returns the list of blocks in the print space */
		std::vector<BlockPtr> GetBlocks() const;

#include <CRNXml/CRNAltoTextBlock.hpp>
		typedef std::weak_ptr<TextBlock> TextBlockPtr;

		/*! \brief Returns the list of text blocks in the print space */
		const std::vector<std::weak_ptr<TextBlock>>& GetTextBlocks() const { return textBlocks; }
		/*! \brief Returns a text block contained in the print space */
		TextBlock& GetTextBlock(const Id &id);
		/*! \brief Adds a text block in the print space */
		TextBlock& AddTextBlock(const Id &id_, int x, int y, int w, int h);
		/*! \brief Adds a text block in the print space */
		TextBlock& AddTextBlockAfter(const Id &pred, const Id &id_, int x, int y, int w, int h);
		/*! \brief Adds a text block in the print space */
		TextBlock& AddTextBlockBefore(const Id &next, const Id &id_, int x, int y, int w, int h);

		/*! \brief Illustration in a space */
		class Illustration: public Block
		{
			public:
				Illustration(const Illustration&) = default;
				Illustration(Illustration&&) = default;
				virtual ~Illustration() override {}
				Illustration& operator=(const Illustration&) = default;
				Illustration& operator=(Illustration&&) = default;

				/*! \brief Returns the type of illustration (photo, map, drawing, chart…) */
				Option<StringUTF8> GetType() const;
				/*! \brief Returns a link to an image containing only this illustration */
				Option<StringUTF8> GetFileId() const;

			private:
				/*! \brief Constructor from file */
				Illustration(const Element &el);
				/*! \brief Constructor */
				Illustration(const Element &el, const Id &id_, int x, int y, int w, int h);

			friend class Space;
		};
		typedef std::weak_ptr<Illustration> IllustrationPtr;

		/*! \brief Returns the list of illustrations in the print space */
		const std::vector<IllustrationPtr>& GetIllustrations() const;
		/*! \brief Adds an illustration in the print space */
		Illustration& AddIllustration(const Id &id_, int x, int y, int w, int h);
		/*! \brief Adds an illustration in the print space */
		Illustration& AddIllustrationAfter(const Id &pred, const Id &id_, int x, int y, int w, int h);
		/*! \brief Adds an illustration in the print space */
		Illustration& AddIllustrationBefore(const Id &next, const Id &id_, int x, int y, int w, int h);

		/*! \brief Graphical element in a space */
		class GraphicalElement: public Block
		{
			public:
				GraphicalElement(const GraphicalElement&) = default;
				GraphicalElement(GraphicalElement&&) = default;
				virtual ~GraphicalElement() override {}
				GraphicalElement& operator=(const GraphicalElement&) = default;
				GraphicalElement& operator=(GraphicalElement&&) = default;

			private:
				/*! \brief Constructor from file */
				GraphicalElement(const Element &el);
				/*! \brief Constructor */
				GraphicalElement(const Element &el, const Id &id_, int x, int y, int w, int h);

			friend class Space;
		};
		typedef std::weak_ptr<GraphicalElement> GraphicalElementPtr;

		/*! \brief Returns the list of graphical elements in the print space */
		const std::vector<GraphicalElementPtr>& GetGraphicalElements() const;
		/*! \brief Adds an graphical element in the print space */
		GraphicalElement& AddGraphicalElement(const Id &id_, int x, int y, int w, int h);
		/*! \brief Adds an graphical element in the print space */
		GraphicalElement& AddGraphicalElementAfter(const Id &pred, const Id &id_, int x, int y, int w, int h);
		/*! \brief Adds an graphical element in the print space */
		GraphicalElement& AddGraphicalElementBefore(const Id &next, const Id &id_, int x, int y, int w, int h);

		/*! \brief A composed block in a space */
		class ComposedBlock: public Block
		{
			public:
				ComposedBlock(const ComposedBlock&) = default;
				ComposedBlock(ComposedBlock&&) = default;
				virtual ~ComposedBlock() override {}
				ComposedBlock& operator=(const ComposedBlock&) = default;
				ComposedBlock& operator=(ComposedBlock&&) = default;

				/*! \brief Returns the type of block (table, advertisement…) */
				Option<StringUTF8> GetType() const;
				/*! \brief Returns a link to an image containing only this block */
				Option<StringUTF8> GetFileId() const;
				/*! \brief Returns the list of subblocks in the block */
				std::vector<BlockPtr> GetBlocks() const { return std::vector<BlockPtr>(blocks.begin(), blocks.end()); }
				/*! \brief Returns the list of text blocks in the block */
				const std::vector<std::weak_ptr<TextBlock> >& GetTextBlocks() const { return textBlocks; }
				/*! \brief Returns the list of illustrations in the block */
				const std::vector<std::weak_ptr<Illustration> >& GetIllustrations() const { return illustrations; }
				/*! \brief Returns the list of graphical elements in the block */
				const std::vector<std::weak_ptr<GraphicalElement> >& GetGraphicalElements() const { return graphicalElements; }
				/*! \brief Returns the list of composed blocks in the block */
				const std::vector<std::weak_ptr<ComposedBlock> >& GetComposedBlocks() const { return composedBlocks; }

			private:
				/*! \brief Constructor from file */
				ComposedBlock(const Element &el);

				std::vector<std::shared_ptr<Block> > blocks;
				std::vector<TextBlockPtr> textBlocks;
				std::vector<IllustrationPtr> illustrations;
				std::vector<GraphicalElementPtr> graphicalElements;
				std::vector<std::weak_ptr<ComposedBlock> > composedBlocks;

			friend class Space;
		};
		typedef std::weak_ptr<ComposedBlock> ComposedBlockPtr;

		/*! \brief Returns the list of composed blocks in the print space */
		const std::vector<ComposedBlockPtr>& GetComposedBlocks() const;

		/*! \brief Removes a block */
		void RemoveBlock(const Id &bid);

	private:
		/*! \brief Constructor from file */
		Space(const Element &el);
		/*! \brief Constructor */
		Space(const Element &el, const Id &id_, double x, double y, double w, double h);
		/*! \brief Updates the block cache */
		void update_subelements();

		std::vector<std::shared_ptr<Block> > blocks;
		std::map<Id, std::weak_ptr<Block> > id_blocks;
		std::vector<TextBlockPtr> textBlocks;
		std::map<Id, TextBlockPtr> id_textBlocks;
		std::vector<IllustrationPtr> illustrations;
		std::vector<GraphicalElementPtr> graphicalElements;
		std::vector<ComposedBlockPtr> composedBlocks;

	friend class Page;
};
#else
#error	you cannot include this file directly
#endif

