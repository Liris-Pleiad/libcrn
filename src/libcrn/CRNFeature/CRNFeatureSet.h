/* Copyright 2007-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNFeatureSet.h
 * \author Yann LEYDIER
 */

#ifndef CRNFEATURESET
#define CRNFEATURESET

#include <CRNData/CRNVector.h>
#include <CRNImage/CRNImageFormats.h>

namespace crn
{
	class Block;
	/****************************************************************************/
	/*! \brief Feature vector class
	 *
	 * \author 	Yann LEYDIER
	 * \date		October 2007
	 * \version 0.2
	 * \ingroup featex
	 */
	class FeatureSet: public Vector
	{
		public:
			/*! \brief Default constructor */
			FeatureSet() = default;

			FeatureSet(const FeatureSet&) = delete;
			FeatureSet(FeatureSet&&) = default;
			FeatureSet& operator=(const FeatureSet&) = delete;
			FeatureSet& operator=(FeatureSet&&) = default;
			virtual ~FeatureSet() override = default;

			/*! \brief Extracts all the features of the set from a block */
			SVector Extract(Block &b);
			/*! \brief Extracts all the features of the set from a block */
			SVector ExtractWithMask(Block &b, ImageIntGray &mask);
	
			FeatureSet(xml::Element &el):Vector(el) { }
			CRN_DECLARE_CLASS_CONSTRUCTOR(FeatureSet)
		private:
			virtual std::string getClassName() const override { return "FeatureSet"; }				
	};
	template<> struct IsSerializable<FeatureSet> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(FeatureSet)
}

#endif

