/* Copyright 2009-2016 INSA Lyon, Université Paris-Descartes, ENS-Lyon
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
 * file: CRNData.h
 * \author Yann LEYDIER
 */

#ifndef CRNData_HEADER
#define CRNData_HEADER

#include <vector>
#include <CRNData/CRNInt.h>
#include <CRNData/CRNReal.h>
#include <CRNString.h>
#include <CRNStringUTF8.h>
#include <CRNIO/CRNPath.h>
#include <memory>
#include <CRNException.h>
#include <CRNMath/CRNProp3.h>
#include <cstring>

// TODO
namespace crn
{
	/*! \defgroup data Numeric data */
	/*@{*/
	namespace Data
	{
		/*! \brief Converts an int to CRN data */
		UInt ToCRN(int i);
		/*! \brief Converts a double to CRN data */
		UReal ToCRN(double d);
		/*! \brief Converts a String to CRN data */
		UString ToCRN(const String &str);
		/*! \brief Converts a StringUTF8 to CRN data */
		UStringUTF8 ToCRN(const StringUTF8 &str);
		/*! \brief Converts a Path to CRN data */
		UPath ToCRN(const Path &str);
		/*! \brief Converts a Prop3 to CRN data */
		UProp3 ToCRN(const Prop3 &val);
		/*! \brief Converts a shared pointer to CRN data */
		template<typename T> inline SObject ToCRN(std::shared_ptr<T> ptr)
		{
			SObject obj(std::dynamic_pointer_cast<Object>(ptr));
			if (!obj)
				throw ExceptionInvalidArgument("ToCRN(std::shared_ptr<T>): not a crn::Object.");
			return obj;
		}

		/*! \brief Converts a pointer to CRN data (fails in not a Clonable object) */
		template<typename T> inline UObject ToCRN(const T* ptr)
		{
			return ptr->Clone();
		}

		/*! \brief Converts an object to CRN data (fails in not a Clonable object) */
		template<typename T> inline UObject ToCRN(const T &obj)
		{
			return obj.Clone();
		}
		/*! \brief Converts from a CRN object to another type */
		template<typename T> inline T Convert(const Object &obj)
		{
			return Convert<T>(&obj);
		}
		/*! \brief Converts from a CRN object to int
		 * \throws	ExceptionInvalidArgument	impossible to convert
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline int Convert(const Object &obj)
		{
			const Int *i = dynamic_cast<const Int*>(&obj);
			if (i)
				return int(*i);
			const String *s = dynamic_cast<const String*>(&obj);
			if (s)
				return s->ToInt();
			const StringUTF8 *su = dynamic_cast<const StringUTF8*>(&obj);
			if (su) // works for StringUTF8 and Path
				return su->ToInt();
			throw ExceptionInvalidArgument("Cannot convert between these types.");
		}
		/*! \brief Converts from a CRN object to unsigned int
		 * \throws	ExceptionInvalidArgument	impossible to convert
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline unsigned int Convert(const Object &obj)
		{
			const String *s = dynamic_cast<const String*>(&obj);
			if (s)
				return s->ToUInt();
			const StringUTF8 *su = dynamic_cast<const StringUTF8*>(&obj);
			if (su) // works for StringUTF8 and Path
				return su->ToUInt();
			throw ExceptionInvalidArgument("Cannot convert between these types.");
		}
		/*! \brief Converts from a CRN object to double
		 * \throws	ExceptionInvalidArgument	impossible to convert
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline double Convert(const Object &obj)
		{
			const Int *i = dynamic_cast<const Int*>(&obj);
			if (i)
				return int(*i);
			const Real *r = dynamic_cast<const Real*>(&obj);
			if (r)
				return *r;
			const String *s = dynamic_cast<const String*>(&obj);
			if (s)
				return s->ToDouble();
			const StringUTF8 *su = dynamic_cast<const StringUTF8*>(&obj);
			if (su) // works for StringUTF8 and Path
				return su->ToDouble();
			throw ExceptionInvalidArgument("Cannot convert between these types.");
		}
		/*! \brief Converts from a CRN object to String
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline String Convert(const Object &obj)
		{
			return obj.ToString();
		}
		/*! \brief Converts from a CRN object to StringUTF8
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline StringUTF8 Convert(const Object &obj)
		{
			return obj.ToString().CStr();
		}
		/*! \brief Converts from a CRN object to Path
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline Path Convert(const Object &obj)
		{
			return obj.ToString().CStr();
		}
		/*! \brief Converts from a CRN object to Prop3
		 * \throws	ExceptionInvalidArgument	impossible to convert
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline Prop3 Convert(const Object &obj)
		{
			const Prop3 *p = dynamic_cast<const Prop3*>(&obj);
			if (p)
				return *p;
			throw ExceptionInvalidArgument("Cannot convert between these types.");
		}
		/*! \brief Converts from a CRN object to bool
		 * \throws	ExceptionInvalidArgument	impossible to convert
		 * \param[in]	obj	an object to convert
		 * \return	the converted value
		 */
		template<> inline bool Convert(const Object &obj)
		{
			const Int *i = dynamic_cast<const Int*>(&obj);
			if (i)
				return *i != 0;
			const Prop3 *p = dynamic_cast<const Prop3*>(&obj);
			if (p)
				return p->IsTrue();
			throw ExceptionInvalidArgument("Cannot convert between these types.");
		}

		/*! \brief Encodes any data into a printable string */
		crn::StringUTF8 ASCII85Encode(const uint8_t * const data, size_t len);
		/*! \brief Decodes an ASCII85 string to a vector of bytes */
		std::vector<uint8_t> ASCII85Decode(const crn::StringUTF8 &s);
		/*! \brief Decodes an ASCII85 string to a vector
		 * \throws	ExceptionInvalidArgument	invalid ASCII85 string
		 * \throws	ExceptionDimension	invalid size
		 * \param[in]	s	an ASCII85 string
		 * \return	a vector of data
		 */
		template<typename T> std::vector<T> ASCII85Decode(const crn::StringUTF8 &s)
		{
			const auto deco = ASCII85Decode(s);
			const auto nelem = deco.size() / sizeof(T);
			if (nelem * sizeof(T) != deco.size())
				throw ExceptionDimension{StringUTF8("ASCII85Decode<") + typeid(T).name() + StringUTF8(">(): invalid size.")};
			auto res = std::vector<T>(nelem);
			std::copy(deco.begin(), deco.end(), reinterpret_cast<uint8_t*>(res.data()));
			return res;
		}

	}
	/*@}*/
}

#endif


