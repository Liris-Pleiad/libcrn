/* Copyright 2012-2016 CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNFeatureExtractorAngleRadialZoning.h
 * \author Yann LEYDIER
 */

#ifndef CRNFEATUREEXTRACTORANGLERADIALZONING_HEADER
#define CRNFEATUREEXTRACTORANGLERADIALZONING_HEADER

#include <CRNFeature/CRNFeatureExtractor.h>
#include <CRNMath/CRNMath.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief AngleRadialZoning features extractor class.
	 *
	 * AngleRadialZoning features extractor class.
	 * 
	 * \author 	Yann LEYDIER
	 * \date		October 2007
	 * \version 0.2
	 * \ingroup featex
	 */
	class FeatureExtractorAngleRadialZoning: public FeatureExtractor
	{
		public:
			/*! \brief Constructor.
			 * 
			 * Constructor
			 * 
			 * \param[in]	nb_zones	the number of radial zones
			 * \param[in]	max_val	the maximal value in the histogram
			 * \param[in]	zone_size the size of the histogram for each zone
			 * \param[in]	auto_center	shall we use the center of gravity instead of the image's center?
			 */
			FeatureExtractorAngleRadialZoning(unsigned int nb_zones, unsigned int max_val, unsigned int zone_size = 16, bool auto_center = true):nbzones(nb_zones),maxval(max_val),zonesize(zone_size),autocenter(auto_center) { initTables(); }

			FeatureExtractorAngleRadialZoning(const FeatureExtractorAngleRadialZoning&) = delete;
			FeatureExtractorAngleRadialZoning(FeatureExtractorAngleRadialZoning&&) = default;
			FeatureExtractorAngleRadialZoning& operator=(const FeatureExtractorAngleRadialZoning&) = delete;
			FeatureExtractorAngleRadialZoning& operator=(FeatureExtractorAngleRadialZoning&&) = default;
			virtual ~FeatureExtractorAngleRadialZoning() override {}

			/*! \brief Returns the id of the class */
			virtual StringUTF8 GetClassName() const override { return "FeatureExtractorAngleRadialZoning"; }

			/*! \brief Returns a CRNPROTOCOL_FEATURE object computed from a block */
			virtual SObject Extract(Block &b) override;
			virtual SObject ExtractWithMask(Block &b, ImageIntGray &mask) override;

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;

			/*! \brief creates tables to speed up the process */
			void initTables();
		
			unsigned int nbzones; /*!< the number of radial zones */
			unsigned int maxval; /*!< the value of the sum of the histogram's bins */
			unsigned int zonesize; /*!< the size of the histogram for each zone */
			bool autocenter; /*!< use the center of gravity? */

			std::vector<Angle<ByteAngle> > zonetab, feattab; /*!< angle tables */
		CRN_DECLARE_CLASS_CONSTRUCTOR(FeatureExtractorAngleRadialZoning)
		CRN_SERIALIZATION_CONSTRUCTOR(FeatureExtractorAngleRadialZoning)
	};
	template<> struct IsSerializable<FeatureExtractorAngleRadialZoning> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(FeatureExtractorAngleRadialZoning)
}

#endif

