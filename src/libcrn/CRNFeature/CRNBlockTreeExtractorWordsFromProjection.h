/* Copyright 2009-2014 CoReNum, INSA-Lyon
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
 * file: CRNBlockTreeExtractorWordsFromProjection.h
 * \author Jean DUONG, Yann LEYDIER
 */


#ifndef CRNBlockTreeExtractorWordsFromProjection_HEADER
#define CRNBlockTreeExtractorWordsFromProjection_HEADER

#include <CRNFeature/CRNBlockTreeExtractor.h>

namespace crn
{

	/*! \brief A class to extract words from a text line
	 *
	 * A class to extract words from a text line using vertical projection 
	 *
	 * \author 	Jean DUONG
	 * \date	20/08/09
	 * \version 0.1
	 * \ingroup btex
	 */
	class BlockTreeExtractorWordsFromProjection: public BlockTreeExtractor
	{
		public:
			/*! \brief Constructor
			 *
			 *	Constructor
			 * \param[in] wordName	name of the tree that will contain the words.
			 * \param[in] ccTreeName	name of the tree containing the connected components. If it doesn't exist, components will be extracted using a default algorithm and placed in this tree
			 */
			BlockTreeExtractorWordsFromProjection(const String &wordName = U"CRN::Words", const String &ccTreeName = U"CRN::CCs"):
					wordTreeName(wordName),connectedComponentTreeName(ccTreeName) {}

			BlockTreeExtractorWordsFromProjection(const BlockTreeExtractorWordsFromProjection&) = delete;
			BlockTreeExtractorWordsFromProjection(BlockTreeExtractorWordsFromProjection&&) = default;
			BlockTreeExtractorWordsFromProjection& operator=(const BlockTreeExtractorWordsFromProjection&) = delete;
			BlockTreeExtractorWordsFromProjection& operator=(BlockTreeExtractorWordsFromProjection&&) = default;
			virtual ~BlockTreeExtractorWordsFromProjection() override {}

			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"BlockTreeExtractorWordsFromProjection"); return cn; }

			/*! \brief Returns the name of the tree that is extracted */
			virtual String GetTreeName() override { return wordTreeName; }
			/*! \brief Computes a child-tree on a block */
			virtual void Extract(Block &b) override;

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;

			String wordTreeName; /*!< name of the tree containing connected components*/
			String connectedComponentTreeName; /*!< name of the tree containing connected components*/
		
			CRN_DECLARE_CLASS_CONSTRUCTOR(BlockTreeExtractorWordsFromProjection)
			CRN_SERIALIZATION_CONSTRUCTOR(BlockTreeExtractorWordsFromProjection)
	};

	CRN_ALIAS_SMART_PTR(BlockTreeExtractorWordsFromProjection)
}

#endif


