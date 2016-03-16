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
 * file: CRNFeatureExtractorAngleZoning.h
 * \author Yann LEYDIER
 */

#ifndef CRNFEATUREEXTRACTORANGLEZONING_HEADER
#define CRNFEATUREEXTRACTORANGLEZONING_HEADER

#include <CRNFeature/CRNFeatureExtractor.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief AngleZoning features extractor class.
	 *
	 * AngleZoning features extractor class.
	 * 
	 * \author 	Yann LEYDIER
	 * \date		October 2007
	 * \version 0.2
	 * \ingroup featex
	 */
	class FeatureExtractorAngleZoning: public FeatureExtractor
	{
		public:
			/*! \brief Constructor.
			 * 
			 * Constructor
			 * 
			 * \param[in]	x_div	the number of zones on a line
			 * \param[in]	y_div	the number of zones on a column
			 * \param[in]	max_val	the maximal value in the histogram
			 * \param[in]	zone_size the size of the histogram for each zone
			 */
			FeatureExtractorAngleZoning(int x_div, int y_div, int max_val, int zone_size = 16):xdiv(x_div),ydiv(y_div),maxval(max_val),zonesize(zone_size) { }

			FeatureExtractorAngleZoning(const FeatureExtractorAngleZoning&) = delete;
			FeatureExtractorAngleZoning(FeatureExtractorAngleZoning&&) = default;
			FeatureExtractorAngleZoning& operator=(const FeatureExtractorAngleZoning&) = delete;
			FeatureExtractorAngleZoning& operator=(FeatureExtractorAngleZoning&&) = default;
			virtual ~FeatureExtractorAngleZoning() override {}

			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"FeatureExtractorAngleZoning"); return cn; }

			/*! \brief Returns a CRNPROTOCOL_FEATURE object computed from a block */
			virtual SObject Extract(Block &b) override;
			virtual SObject ExtractWithMask(Block &b, ImageIntGray &mask) override;

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;
		
			int xdiv; /*!< the number of zones on a line */
			int ydiv; /*!< the number of zones on a column */
			int maxval; /*!< the value of the sum of the histogram's bins */
			int zonesize; /*!< the size of the histogram for each zone */

		CRN_DECLARE_CLASS_CONSTRUCTOR(FeatureExtractorAngleZoning)
		CRN_SERIALIZATION_CONSTRUCTOR(FeatureExtractorAngleZoning)
	};
	namespace protocol
	{
		template<> struct IsSerializable<FeatureExtractorAngleZoning> : public std::true_type {};
	}

	CRN_ALIAS_SMART_PTR(FeatureExtractorAngleZoning)
}

#endif

