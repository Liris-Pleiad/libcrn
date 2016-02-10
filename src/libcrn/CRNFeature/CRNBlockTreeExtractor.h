/* Copyright 2007-2015 Yann LEYDIER, INSA-Lyon, CoReNum
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
 * file: CRNBlockTreeExtractor.h
 * \author Yann LEYDIER
 */

#ifndef CRNBLOCKTREEEXTRACTOR_HEADER
#define CRNBLOCKTREEEXTRACTOR_HEADER

#include <CRNUtils/CRNDefaultAction.h>
#include <CRNImage/CRNImageFormats.h>

namespace crn
{
	class Block;

	/*! \defgroup btex Block tree extractors
	 * \ingroup action */

	/****************************************************************************/
	/*! \brief BlockTree extractor base class.
	 *
	 * BlockTree extractor base class.
	 *
	 * Classes that implement BlockTreeExtractor must also implement crn::protocol::Serializable.
	 *
	 * \author 	Yann LEYDIER
	 * \date		April 2009
	 * \version 0.1
	 * \ingroup btex
	 */
	class BlockTreeExtractor: public Action
	{
		public:
			virtual ~BlockTreeExtractor() override {}
			/*! \brief Returns the name of the tree that is extracted */
			virtual String GetTreeName() = 0;
			/*! \brief Computes a child-tree on a block */
			virtual void Extract(Block &b) = 0;
			/*! \brief Computes a child-tree on a block with mask */
			virtual void ExtractWithMask(Block &b, ImageIntGray &mask) { Extract(b); }

	};
	CRN_ALIAS_SMART_PTR(BlockTreeExtractor)
}
#endif
