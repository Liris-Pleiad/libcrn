/* Copyright 2007-2015 Yann LEYDIER, INSA-Lyon
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
 * file: CRNFeatureExtractor.h
 * \author Yann LEYDIER
 */

#ifndef CRNFEATUREEXTRACTOR_HEADER
#define CRNFEATUREEXTRACTOR_HEADER

#include <CRNObject.h>
#include <CRNUtils/CRNDefaultAction.h>
#include <CRNImage/CRNImageFormats.h>

namespace crn
{
	class Block;

	/*! \defgroup featex Feature extractors 
	 * \ingroup action */

	/****************************************************************************/
	/*! \brief Feature extractor base class.
	 *
	 * Feature extractor base class.
	 *
	 * Classes that implement FeatureExtractor must also implement crn::protocol::Serializable.
	 *
	 * \author 	Yann LEYDIER
	 * \date		October 2007
	 * \version 0.2
	 * \ingroup featex
	 */
	class FeatureExtractor: public Action
	{
		public:
			virtual ~FeatureExtractor() override {}
			/*! \brief Returns a crn::protocol::Feature object computed from a block 
			 * \param[in]	b	the block to process
			 * \return	the features
			 */
			virtual SObject Extract(Block &b) = 0;
			/*! \brief Returns a crn::protocol::Feature object computed from a block and its CC mask
			 * \param[in]	b	the block to process
			 * \param[in]	mask	the mask returned by Block::ExtractCC
			 * \return	the features
			 */
			virtual SObject ExtractWithMask(Block &b, ImageIntGray &mask) { return Extract(b); }
	};

	CRN_ALIAS_SMART_PTR(FeatureExtractor)
}

#endif

