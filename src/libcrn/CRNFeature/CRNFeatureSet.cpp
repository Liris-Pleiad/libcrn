/* Copyright 2007-2014 Yann LEYDIER, INSA-Lyon
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
 * file: CRNFeatureVector.cpp
 * \author Yann LEYDIER
 */

#include <CRNi18n.h>
#include <CRNFeature/CRNFeatureSet.h>
#include <CRNFeature/CRNFeatureExtractor.h>
#include <CRNException.h>
#include <CRNBlock.h>
#include <CRNData/CRNDataFactory.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Extracts all the features of the set from a block.
 *
 * \param[in]	b	the block to process
 * \return the newly create feature vector, or nullptr if failed
 */
SVector FeatureSet::Extract(Block &b)
{
	SVector fv = std::make_shared<Vector>(Protocol::Feature);
	for (size_t tmp = 0; tmp < Size(); tmp++)
		fv->PushBack(std::static_pointer_cast<FeatureExtractor>(At(tmp))->Extract(b));
	return fv;
}

/*****************************************************************************/
/*!
 * Extracts all the features of the set from a block.
 *
 * \param[in]	b	the block to process
 * \param[in]	mask	the block mask image
 * \return the newly create feature vector, or nullptr if failed
 */
SVector FeatureSet::ExtractWithMask(Block &b, ImageIntGray &mask)
{
	SVector fv = std::make_shared<Vector>(Protocol::Feature);
	for (size_t tmp = 0; tmp < Size(); tmp++)
		fv->PushBack(std::static_pointer_cast<FeatureExtractor>(At(tmp))->ExtractWithMask(b, mask));
	return fv;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(FeatureSet)
	CRN_DATA_FACTORY_REGISTER(U"FeatureSet", FeatureSet)
CRN_END_CLASS_CONSTRUCTOR(FeatureSet)

