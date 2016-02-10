/* Copyright 2009-2015 INSA Lyon
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
 * file: CRNAtScopeExit.h
 * \author Yann LEYDIER
 */

#ifndef CRNAtScopeExit_HEADER
#define CRNAtScopeExit_HEADER

#include <functional>

namespace crn
{
	/*! \brief An objects that calls a functor when destroyed 
	 * \ingroup utils
	 */
	class Destroyer
	{
		public:
			/*! \brief Constructor 
			 * \param[in]	f	the functor that will be called when the object is destroyed
			 */
			Destroyer(const std::function<void(void)> &f):atdestroy(f) { }
			/*! \brief Constructor 
			 * \param[in]	f	the functor that will be called when the object is destroyed
			 */
			Destroyer(std::function<void(void)> &&f):atdestroy(std::move(f)) { }
			/*! \brief Destructor that calls the functor */
			~Destroyer() { atdestroy(); }
		private:
			std::function<void(void)> atdestroy;
	};
}

/*! \cond */
/*! \brief Internal
 * 
 * Macro to append line number to a label
 */
#define CRNAtScopeExit_UNIFY_VAR(VAR) CRNAtScopeExit_UNIFY_VAR2(VAR,__LINE__)
/*! \brief Internal
 * 
 * Macro to append line number to a label
 */
#define CRNAtScopeExit_UNIFY_VAR2(VAR,CODE) CRNAtScopeExit_UNIFY_VAR3(VAR,CODE)
/*! \brief Internal
 * 
 * Macro to append line number to a label
 */
#define CRNAtScopeExit_UNIFY_VAR3(VAR,CODE) VAR##CODE
/*! \endcond */

/*! \addtogroup utils */
/*@{*/
/*! \brief Executes a functor at the end of the scope
 * \param[in]	functor	a std::function<void(void)>.
 */
#define AtScopeExit(functor) crn::Destroyer CRNAtScopeExit_UNIFY_VAR(at_scope_exit)(functor)
/*@}*/

#endif


