/* Copyright 2008-2015 INSA Lyon, CoReNum
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
 * file: CRNClassifResult.h
 * \author Yann LEYDIER
 */

#ifndef CRNCLASSIFRESULT_HEADER
#define CRNCLASSIFRESULT_HEADER

#include <CRNString.h>
#include <math.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief A generic classification result
	 *
	 * A generic classification result.
	 *
	 * \author 	Yann LEYDIER
	 * \date		October 2008
	 * \version 0.1
	 * \ingroup classify
	 */
	class ClassifResult
	{
		public:
			/*! \brief Default constructor */
			ClassifResult():class_id(-1),class_label(U""),distance(HUGE_VAL),prototype(nullptr) {}
			/*! \brief Constructor from prototype
			 * 
			 * Constructor from prototype
			 * 
			 * \param[in]	cid	the class index
			 * \param[in]	dist	the distance form the prototype to the object
			 * \param[in]	proto	the protype */
			ClassifResult(int cid, double dist, const SCObject &proto):class_id(cid),class_label(cid),distance(dist),prototype(proto) {}
			/*! \brief Constructor from prototype and label
			 * 
			 * Constructor from prototype and label
			 * 
			 * \param[in]	cid	the class index
			 * \param[in]	label	the label of the class
			 * \param[in]	dist	the distance form the prototype to the object
			 * \param[in]	proto	the protype */
			ClassifResult(int cid, const String &label, double dist, const SCObject &proto):class_id(cid),class_label(label),distance(dist),prototype(proto) {}

			ClassifResult(const ClassifResult&) = default;
			ClassifResult(ClassifResult&&) = default;
			ClassifResult& operator=(const ClassifResult&) = default;
			ClassifResult& operator=(ClassifResult&&) = default;

			int class_id; /*!< index of the class */
			String class_label; /*!< label of the class */
			double distance; /*!< distance from the prototype to the object */
			const SCObject prototype; /*!< prototype */

			/*! \brief Comparison operator
			 * 
			 * Comparison operator
			 * 
			 * \param[in]	cr	the result to compare with
			 * \return	true if distance < cr.distance, false else */
			inline bool operator<(const ClassifResult &cr) const noexcept { return distance < cr.distance; }

			/*! \brief Functor to select results in a container 
			 * 
			 * Functor to select results in a container 
			 */
			class SelectId: public std::unary_function<const ClassifResult&, bool>
			{
				public:
					SelectId() = default;
					/*! \brief Initialization of the functor
					 * 
					 * Initialization of the functor
					 * 
					 * \param[in]	id	the class index to select */
					inline SelectId(int id) noexcept:class_id(id) {}
					/*! \brief Functor operator
					 * 
					 * Functor operator
					 * 
					 * \param[in]	cr	a result
					 * \return	true in cr.class_id = selected class index, false else
					 */
					inline bool operator()(const ClassifResult &cr) const noexcept { return class_id == cr.class_id; }

				private:
					int class_id = 0; /*!< The selected class index*/
			};
	};

	CRN_ALIAS_SMART_PTR(ClassifResult)
}
#endif
