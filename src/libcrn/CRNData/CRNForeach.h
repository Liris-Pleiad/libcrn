/* Copyright 2009-2016 INSA Lyon, CoReNum
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
 * file: CRNForeach.h
 * \author Yann LEYDIER
 */

#ifndef CRNForeach_HEADER
#define CRNForeach_HEADER

#include <memory>

/*! \defgroup containers Containers 
 * \ingroup data */
/*@{*/

/*! \brief	Enables ranged for for smart pointers on a type
 * 
 * Enables ranged for for smart pointers on a type. Must be call out all of namespaces.
 * 
 * \param[in]	TYPE	the container's type with full namespace scope
 */
#define CRN_ADD_RANGED_FOR_TO_POINTERS(TYPE) \
	namespace std { \
		inline auto begin(const std::unique_ptr<TYPE> &v) -> decltype (v->begin()) { return v->begin(); } \
		inline auto begin(const std::shared_ptr<TYPE> &v) -> decltype (v->begin()) { return v->begin(); } \
		inline auto end(const std::unique_ptr<TYPE> &v) -> decltype (v->end()) { return v->end(); } \
		inline auto end(const std::shared_ptr<TYPE> &v) -> decltype (v->end()) { return v->end(); } \
		}

/*! \brief	Enables ranged for for smart pointers on a type
 * 
 * Enables ranged for for smart pointers on a type. Must be call out all of namespaces.
 * 
 * \param[in]	TYPE	the container's type with full namespace scope
 */
#define CRN_ADD_RANGED_FOR_TO_CONST_POINTERS(TYPE) \
	namespace std { \
		inline auto begin(const std::unique_ptr<const TYPE> &v) -> decltype (v->cbegin()) { return v->cbegin(); } \
		inline auto begin(const std::shared_ptr<const TYPE> &v) -> decltype (v->cbegin()) { return v->cbegin(); } \
		inline auto end(const std::unique_ptr<const TYPE> &v) -> decltype (v->cend()) { return v->cend(); } \
		inline auto end(const std::shared_ptr<const TYPE> &v) -> decltype (v->cend()) { return v->cend(); } \
		}

/*@}*/

#endif


