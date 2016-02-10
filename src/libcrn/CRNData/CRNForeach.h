/* Copyright 2009-2015 INSA Lyon, CoReNum
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

/*! \brief Registers a container's "begin" method
 * 
 * Registers a container's "begin" method. Must be call out all of namespaces.
 * 
 * \param[in]	TYPE	the container's type with full namespace scope
 * \param[in]	METHOD	the name of the "begin" method, without parenthesis.
 */
#define CRN_ADD_RANGED_FOR_BEGIN(TYPE, METHOD) \
	namespace std { \
		inline auto begin(TYPE& v) -> decltype (v.METHOD()) { return v.METHOD(); } \
		inline auto begin(const std::unique_ptr<TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		inline auto begin(const std::shared_ptr<TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		}

/*! \brief Registers a container's "end" method
 * 
 * Registers a container's "end" method. Must be call out all of namespaces.
 * 
 * \param[in]	TYPE	the container's type with full namespace scope
 * \param[in]	METHOD	the name of the "end" method, without parenthesis.
 */
#define CRN_ADD_RANGED_FOR_END(TYPE, METHOD) \
	namespace std { \
		inline auto end(TYPE& v) -> decltype (v.METHOD()) { return v.METHOD(); } \
		inline auto end(const std::unique_ptr<TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		inline auto end(const std::shared_ptr<TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		}

/*! \brief Registers a container's const "begin" method
 * 
 * Registers a container's const "begin" method. Must be call out all of namespaces.
 * 
 * \param[in]	TYPE	the container's type with full namespace scope
 * \param[in]	METHOD	the name of the "begin" method, without parenthesis.
 */
#define CRN_ADD_RANGED_FOR_CONST_BEGIN(TYPE, METHOD) \
	namespace std { \
		inline auto begin(const TYPE& v) -> decltype (v.METHOD()) { return v.METHOD(); } \
		inline auto begin(const std::unique_ptr<const TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		inline auto begin(const std::shared_ptr<const TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		}

/*! \brief Registers a container's const "end" method
 * 
 * Registers a container's const "end" method. Must be call out all of namespaces.
 * 
 * \param[in]	TYPE	the container's type with full namespace scope
 * \param[in]	METHOD	the name of the "end" method, without parenthesis.
 */
#define CRN_ADD_RANGED_FOR_CONST_END(TYPE, METHOD) \
	namespace std { \
		inline auto end(const TYPE& v) -> decltype (v.METHOD()) { return v.METHOD(); } \
		inline auto end(const std::unique_ptr<const TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		inline auto end(const std::shared_ptr<const TYPE> &v) -> decltype (v->METHOD()) { return v->METHOD(); } \
		}

/*@}*/

#endif


