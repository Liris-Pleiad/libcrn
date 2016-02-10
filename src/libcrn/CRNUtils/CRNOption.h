/* Copyright 2011-2015 CoReNum, INSA-Lyon, Universite Paris Descartes
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
 * file: CRNOption.h
 * \author Yann LEYDIER
 */

#ifndef CRNOption_HEADER
#define CRNOption_HEADER

namespace crn
{
	/*! \brief A class to store an optional value
	 *
	 * A class to store an optional value
	 *
	 * \ingroup utils
	 */
	template<typename T> class Option
	{
		public:
			/*! \brief Default constructor */
			constexpr Option() noexcept : var(nullptr) {}
			/*! \brief Constructor from value */
			inline Option(const T &value):var(new T(value)) {}
			/*! \brief Copy constructor */
			inline Option(const Option &o)
			{
				if (o.var) var = new T(*(o.var));
				else var = nullptr;
			}
			inline Option(Option &&o) noexcept : var(o.var) { o.var = nullptr; }
			/*! \brief Copy operator */
			inline Option& operator=(const Option &o)
			{
				if (&o == this) return *this;
				if (o.var)
				{
					if (var) *var = *(o.var);
					else var = new T(*(o.var));
				}
				else if (var) { delete var; var = nullptr; }
				return *this;
			}
			inline Option& operator=(Option &&o) noexcept { if (&o == this) return *this; delete var; var = o.var; o.var = nullptr; return *this; }
			/*! \brief Destructor */
			inline ~Option() { delete var; }
			/*! \brief Is the option set? */
			inline operator bool() const noexcept { return var != nullptr; }
			/*! \brief Value access (undefined behaviour if the value is not set) */
			inline T& Get() noexcept { return *var; }
			/*! \brief Value access (undefined behaviour if the value is not set) */
			inline const T& Get() const noexcept { return *var; }
			/*! \brief Value access (undefined behaviour if the value is not set) */
			inline T& operator*() noexcept { return *var; }
			/*! \brief Value access (undefined behaviour if the value is not set) */
			inline const T& operator*() const noexcept { return *var; }
			/*! \brief Method access (undefined behaviour if the value is not set) */
			inline T* operator->() noexcept { return var; }
			/*! \brief Method access (undefined behaviour if the value is not set) */
			inline const T* operator->() const noexcept { return var; }
		private:
			T *var; /*!< Internal representation of the value */
	};
}

#endif


