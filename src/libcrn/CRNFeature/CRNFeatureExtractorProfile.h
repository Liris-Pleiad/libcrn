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
 * file: CRNFeatureExtractorProfile.h
 * \author Yann LEYDIER
 */

#ifndef CRNFEATUREEXTRACTORPROFILE_HEADER
#define CRNFEATUREEXTRACTORPROFILE_HEADER

#include <CRNFeature/CRNFeatureExtractor.h>
#include <CRNMath/CRNMath.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Profile features extractor class.
	 *
	 * Profile features extractor class.
	 * 
	 * \author 	Yann LEYDIER
	 * \date		October 2007
	 * \version 0.2
	 * \ingroup featex
	 */
	class FeatureExtractorProfile: public FeatureExtractor
	{
		public:
			/*! \brief Constructor. */
			FeatureExtractorProfile(Direction directions, int un_size, int max_val);

			FeatureExtractorProfile(const FeatureExtractorProfile&) = delete;
			FeatureExtractorProfile(FeatureExtractorProfile&&) = default;
			FeatureExtractorProfile& operator=(const FeatureExtractorProfile&) = delete;
			FeatureExtractorProfile& operator=(FeatureExtractorProfile&&) = default;
			virtual ~FeatureExtractorProfile() override {}

			/*! \brief Returns the id of the class */
			virtual StringUTF8 GetClassName() const override { return "FeatureExtractorProfile"; }

			/*! \brief Returns a CRNPROTOCOL_FEATURE object computed from a block */
			virtual SObject Extract(Block &b) override;

		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el) override;
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const override;
		
			Direction dirs; /*!< the directions */
			int size; /*!< the size of each profile */
			int maxval; /*!< the maximal height of the profiles */

		CRN_DECLARE_CLASS_CONSTRUCTOR(FeatureExtractorProfile)
		CRN_SERIALIZATION_CONSTRUCTOR(FeatureExtractorProfile)
	};
	template<> struct IsSerializable<FeatureExtractorProfile> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(FeatureExtractorProfile)
}

#endif

