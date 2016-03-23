/* Copyright 2009-2016 CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNBlockTreeExtractorTextLinesFromProjection.h
 * \author Jean DUONG, Yann LEYDIER
 */


#ifndef CRNBlockTreeExtractorTextLinesFromProjection_HEADER
#define CRNBlockTreeExtractorTextLinesFromProjection_HEADER

#include <CRNFeature/CRNBlockTreeExtractor.h>

namespace crn
{

	/*! \brief A class to extract text lines from a text block
	 *
	 * A class to extract text lines from a text block using horizontal projection 
	 *
	 * \author 	Jean DUONG
	 * \date		17/08/09
	 * \version 0.1
	 * \ingroup btex
	 */
	class BlockTreeExtractorTextLinesFromProjection: public BlockTreeExtractor
	{
		public:
			/*! \brief Constructor
			 *
			 *	Constructor
			 * \param[in] lineName	name of the tree that will contain the text lines.
			 * \param[in] ccTreeName	name of the tree containing the connected components. If it doesn't exist, components will be extracted using a default algorithm and placed in this tree
			 */
			BlockTreeExtractorTextLinesFromProjection(const String &lineName = U"CRN::TextLines", const String &ccTreeName = U"CRN::CCs"):
				lineTreeName(lineName),connectedComponentTreeName(ccTreeName) {}

			BlockTreeExtractorTextLinesFromProjection(const BlockTreeExtractorTextLinesFromProjection&) = delete;
			BlockTreeExtractorTextLinesFromProjection(BlockTreeExtractorTextLinesFromProjection&&) = default;
			BlockTreeExtractorTextLinesFromProjection& operator=(const BlockTreeExtractorTextLinesFromProjection&) = delete;
			BlockTreeExtractorTextLinesFromProjection& operator=(BlockTreeExtractorTextLinesFromProjection&&) = default;
			virtual ~BlockTreeExtractorTextLinesFromProjection() override {}

			/*! \brief Returns the id of the class */
			virtual StringUTF8 GetClassName() const override { return "BlockTreeExtractorTextLinesFromProjection"; }

			/*! \brief Returns the name of the tree that is extracted */
			virtual String GetTreeName() override { return lineTreeName; }
			/*! \brief Computes a child-tree on a block */
			virtual void Extract(Block &b) override;

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;

			String lineTreeName; /*!< name of the tree that will contain the lines */
			String connectedComponentTreeName; /*!< name of the tree containing connected components*/
		
		CRN_DECLARE_CLASS_CONSTRUCTOR(BlockTreeExtractorTextLinesFromProjection)
		CRN_SERIALIZATION_CONSTRUCTOR(BlockTreeExtractorTextLinesFromProjection)
	};
	template<> struct IsSerializable<BlockTreeExtractorTextLinesFromProjection> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(BlockTreeExtractorTextLinesFromProjection)
}

#endif

