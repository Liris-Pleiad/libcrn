/* Copyright 2011-2016 CoReNum, INSA-Lyon, Université Paris Descartes, ENS-Lyon
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
 * file: CRNConfigElement.h
 * \author Yann LEYDIER
 */

#ifndef CRNConfigElement_HEADER
#define CRNConfigElement_HEADER

#include <CRNData/CRNVector.h>
#include <CRNData/CRNData.h>

namespace crn
{
	/*! \brief A element of configuration
	 *
	 * Helper class to store configuration values with their description
	 *
	 * \author	Yann LEYDIER
	 * \date	Oct 2011
	 * \version 0.1
	 * \ingroup	utils
	 */
	class ConfigElement
	{
		public:
			/*! \brief Default constructor for serialization. */
			ConfigElement();
			/*! \brief Creates a configuration element for an int value */
			ConfigElement(const String& nam, int val, const String& desc = U"");
			/*! \brief Creates a configuration element for a double value */
			ConfigElement(const String& nam, double val, const String& desc = U"");
			/*! \brief Creates a configuration element for a boolean value */
			ConfigElement(const String& nam, bool val, const String& desc = U"");
			/*! \brief Creates a configuration element for a Prop3 value */
			ConfigElement(const String& nam, const Prop3& val, const String& desc = U"");
			/*! \brief Creates a configuration element for a String */
			ConfigElement(const String& nam, const String& val, const String& desc = U"");
			/*! \brief Creates a configuration element for a StringUTF8 */
			ConfigElement(const String& nam, const StringUTF8& val, const String& desc = U"");
			/*! \brief Creates a configuration element for a Path */
			ConfigElement(const String& nam, const Path& val, const String& desc = U"");
			ConfigElement(const ConfigElement&) = delete;
			ConfigElement(ConfigElement&&) = default;
			ConfigElement& operator=(const ConfigElement&) = delete;
			ConfigElement& operator=(ConfigElement&&) = default;
			~ConfigElement() = default;

			/*! \brief Gets the inner type of data in the element */
			String GetType() const;
			/*! \brief Gets the translated name of the element */
			const String& GetName() const noexcept { return name; }
			/*! \brief Gets the description of the element */
			const String& GetDescription() const noexcept { return description; }
			/*! \brief Gets the inner representation of the element's value */
			const SObject& GetValue() noexcept { return value; }
			/*! \brief Gets the inner representation of the element's value */
			SCObject GetValue() const { return value; }
			/*! \brief Gets the element's value
			 * \throws	ExceptionInvalidArgument	impossible to convert
			 * \return	the converted element's value
			 */
			template<typename T> T GetValue() const
			{
				return Data::Convert<T>(*value);
			}
			/*! \brief Sets the element's value
			 * \throws	ExceptionUninitialized	the element was not initialized
			 * \param[in]	val	the value to assign
			 */
			template<typename T> void SetValue(T val)
			{
				setValue(val, value);
			}
			/*! \brief Tells if the element has min value */
			bool HasMinValue() const noexcept { return minValue.get() != nullptr; }
			/*! \brief Gets the inner representation of the element's min value */
			const SObject& GetMinValue() noexcept { return minValue; }
			/*! \brief Gets the inner representation of the element's min value */
			SCObject GetMinValue() const { return minValue; }
			/*! \brief Gets the element's minimal value
			 * \throws	ExceptionUninitialized	no min value
			 * \throws	ExceptionInvalidArgument	impossible to convert
			 */
			template<typename T> T GetMinValue() const
			{
				if (!minValue)
					throw ExceptionUninitialized("The element has no minimal value.");
				return Data::Convert<T>(*minValue);
			}
			/*! \brief Sets the element's min value
			 * \throws	ExceptionUninitialized	the element was not initialized
			 * \param[in]	val	the value to assign
			 */
			template<typename T> void SetMinValue(T val)
			{
				const String t(GetType());
				if (!minValue)
					minValue = Clone(*value);
				setValue(val, minValue);
			}
			/*! \brief Tells if the element has max value */
			bool HasMaxValue() const noexcept { return maxValue.get() != nullptr; }
			/*! \brief Gets the inner representation of the element's max value */
			const SObject& GetMaxValue() noexcept { return maxValue; }
			/*! \brief Gets the inner representation of the element's max value */
			SCObject GetMaxValue() const { return maxValue; }
			/*! \brief Gets the element's minimal value
			 * \throws	ExceptionUninitialized	no max value
			 * \throws	ExceptionInvalidArgument	impossible to convert
			 */
			template<typename T> T GetMaxValue() const
			{
				if (!maxValue)
					throw ExceptionUninitialized("The element has no maximal value.");
				return Data::Convert<T>(*maxValue);
			}
			/*! \brief Sets the element's max value
			 * \throws	ExceptionUninitialized	the element was not initialized
			 * \param[in]	val	the value to assign
			 */
			template<typename T> void SetMaxValue(T val)
			{
				const String t(GetType());
				if (!maxValue)
					maxValue = Clone(*value);
				setValue(val, maxValue);
			}

			/*! \brief Returns the list of allowed values */
			const crn::Vector& GetAllowedValues() const { return allowedValues; }
			/*! \brief Returns the list of allowed values
			 * \throws	ExceptionInvalidArgument	impossible to convert
			 * \return	the converted list of allowed values
			 */
			template<typename T> const std::vector<T> GetAllowedValues() const
			{
				std::vector<T> lst;
				for (SCObject obj : allowedValues)
				{
					lst.push_back(Data::Convert<T>(*obj));
				}
				return lst;
			}
			/*! \brief Sets the list of allowed values
			 * \param[in]	b	the beginning iterator
			 * \param[in]	e	the end iterator
			 */
			template<typename Iter> void SetAllowedValues(Iter b, Iter e)
			{
				allowedValues.Clear();
				for (Iter tmp = b; tmp != e; ++tmp)
					AddAllowedValue(*tmp);
			}
			/*! \brief Adds a value to the list of allowed values
			 * \param[in]	val	the value to add
			 */
			template<typename T> void AddAllowedValue(T val)
			{
				allowedValues.PushBack(Clone(*value));
				setValue(val, allowedValues.Back());
			}

		private:
			/*! \brief Converts and sets a value
			 * \param[in]	val	the value to convert
			 * \param[in]	value	the placeholder
			 */
			template<typename T> static void setValue(T val, const SObject &value)
			{
				const String valstring(val);
				auto i = std::static_pointer_cast<Int>(value);
				if (i)
				{
					*i = valstring.ToInt();
					return;
				}
				auto r = std::static_pointer_cast<Real>(value);
				if (r)
				{
					*r = valstring.ToDouble();
					return;
				}
				auto p = std::static_pointer_cast<Prop3>(value);
				if (p)
				{
					*p = valstring.ToProp3();
					return;
				}
				auto s = std::static_pointer_cast<String>(value);
				if (s)
				{
					*s = valstring;
					return;
				}
				auto su = std::static_pointer_cast<StringUTF8>(value);
				if (su)
				{
					*su = valstring.CStr();
					return;
				}
				auto pa = std::static_pointer_cast<Path>(value);
				if (pa)
				{
					*pa = valstring.CStr();
					return;
				}
			}
			
			SObject value; /*!< Current value */
			SObject minValue; /*!< Minimal value */
			SObject maxValue; /*!< Maximal value */
			String name; /*!< The name of the value */
			String description; /*!< A description of the value */
			Vector allowedValues; /*!< A list of allowed values */
	};

	CRN_ALIAS_SMART_PTR(ConfigElement)
}

#endif


