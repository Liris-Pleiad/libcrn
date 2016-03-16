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
 * file: CRNFeatureExtractorProjection.h
 * \author Yann LEYDIER
 */

#ifndef CRNFEATUREEXTRACTORPROJECTION_HEADER
#define CRNFEATUREEXTRACTORPROJECTION_HEADER

#include <CRNFeature/CRNFeatureExtractor.h>
#include <CRNMath/CRNMath.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Projection features extractor class.
	 *
	 * Projection features extractor class.
	 * 
	 * \author 	Yann LEYDIER
	 * \date		October 2007
	 * \version 0.2
	 * \ingroup featex
	 */
	class FeatureExtractorProjection: public FeatureExtractor
	{
		public:
			/*! \brief Constructor. */
			FeatureExtractorProjection(Orientation orientations, int un_size, int max_val);

			FeatureExtractorProjection(const FeatureExtractorProjection&) = delete;
			FeatureExtractorProjection(FeatureExtractorProjection&&) = default;
			FeatureExtractorProjection& operator=(const FeatureExtractorProjection&) = delete;
			FeatureExtractorProjection& operator=(FeatureExtractorProjection&&) = default;
			virtual ~FeatureExtractorProjection() override {}

			/*! \brief Returns the id of the class */
			virtual const String& GetClassName() const override { static const String cn(U"FeatureExtractorProjection"); return cn; }

			/*! \brief Returns a CRNPROTOCOL_FEATURE object computed from a block */
			virtual SObject Extract(Block &b) override;

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;
		
			Orientation dirs; /*!< the orientations */
			int size; /*!< the size of each profile */
			int maxval; /*!< the maximal height of the profiles */

		CRN_DECLARE_CLASS_CONSTRUCTOR(FeatureExtractorProjection)
		CRN_SERIALIZATION_CONSTRUCTOR(FeatureExtractorProjection)
	};
	namespace protocol
	{
		template<> struct IsSerializable<FeatureExtractorProjection> : public std::true_type {};
	}

	CRN_ALIAS_SMART_PTR(FeatureExtractorProjection)
}

#endif

