/* Copyright 2012-2014 INSA-Lyon
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
 * file: CRNFeatureExtractorOverlappingAngleHistogram.h
 * \author Yann LEYDIER
 */

#ifndef CRNFeatureExtractorOverlappingAngleHistogram_HEADER
#define CRNFeatureExtractorOverlappingAngleHistogram_HEADER

#include <CRNFeature/CRNFeatureExtractor.h>
#include <CRNMath/CRNMath.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief OverlappingAngleHistogram features extractor class.
	 *
	 * Computes 9 overlapping angle histograms. Each angle is weighted by the gaussian distance to the reference point of each histogram. The central histogram is placed on the center of mass, the others on a square at a distance of distance_factor×std deviation.
	 * 
	 * \author 	Yann LEYDIER
	 * \date		December 2012
	 * \version 0.1
	 * \ingroup featex
	 */
	class FeatureExtractorOverlappingAngleHistogram: public FeatureExtractor
	{
		public:
			/*! \brief Constructor.
			 * 
			 * Constructor
			 * 
			 * \param[in]	nb_angles	the number of angles per histogram
			 * \param[in]	distance_factor	a factor to compute the distance between the reference points
			 */
			FeatureExtractorOverlappingAngleHistogram(unsigned int nb_angles, double distance_factor = 1.0):nbangles(nb_angles),distance(distance_factor) { initTables(); }

			FeatureExtractorOverlappingAngleHistogram(const FeatureExtractorOverlappingAngleHistogram&) = delete;
			FeatureExtractorOverlappingAngleHistogram(FeatureExtractorOverlappingAngleHistogram&&) = default;
			FeatureExtractorOverlappingAngleHistogram& operator=(const FeatureExtractorOverlappingAngleHistogram&) = delete;
			FeatureExtractorOverlappingAngleHistogram& operator=(FeatureExtractorOverlappingAngleHistogram&&) = default;
			virtual ~FeatureExtractorOverlappingAngleHistogram() override {}

			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"FeatureExtractorOverlappingAngleHistogram"); return cn; }

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
		
			unsigned int nbangles; /*!< the number of angles per histogram */
			double distance; /*!< factor to compute the distance between the reference points */

			std::vector<Angle<ByteAngle> > angletab; /*!< angle table */
		CRN_DECLARE_CLASS_CONSTRUCTOR(FeatureExtractorOverlappingAngleHistogram)
		CRN_SERIALIZATION_CONSTRUCTOR(FeatureExtractorOverlappingAngleHistogram)
	};

	CRN_ALIAS_SMART_PTR(FeatureExtractorOverlappingAngleHistogram)
}

#endif

