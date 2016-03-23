/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, ENS-Lyon
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
 * file: CRNProtocols.h
 * \author Yann LEYDIER, Jean DUONG
 */

#ifndef CRNPROTOCOLS
#define CRNPROTOCOLS

#include <CRNType.h>
#include <CRNObject.h>
#include <CRNMath/CRNMath.h>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <CRNUtils/CRNXml.h>

/*! \addtogroup base */
/*@{*/
namespace crn
{
	/*! \brief Distance between two objects */
	template<
		typename T,
						 typename std::enable_if<std::is_class<T>::value, int>::type = 0
							 > 
							 double Distance(const T &o1, const T &o2)
							 {
								 return o1.Distance(o2);
							 }
	/*! \brief Distance between two numbers */
	template<
		typename T,
						 typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0
							 >
							 double Distance(T o1, T o2)
							 {
								 return double(Abs(o1 - o2));
							 }

	class Serializer
	{
		public:
			static void Deserialize(Object &obj, xml::Element &el)
			{
				const auto it = getInstance().serializers.find(typeid(obj));
				if (it == getInstance().serializers.end())
					throw ExceptionProtocol{};
				it->second->Deserialize(obj, el);
			}
			static xml::Element Serialize(const Object &obj, xml::Element &el)
			{
				const auto it = getInstance().serializers.find(typeid(obj));
				if (it == getInstance().serializers.end())
					throw ExceptionProtocol{};
				return it->second->Serialize(obj, el);
			}
			template<typename T> static void Register()
			{
				getInstance().serializers.emplace(typeid(T), std::make_unique<serializerImpl<T>>());
			}
		private:
			Serializer() {}
			static inline Serializer& getInstance() { static Serializer s; return s; }
			struct serializer
			{
				virtual void Deserialize(Object &obj, xml::Element &el) = 0;
				virtual xml::Element Serialize(const Object &obj, xml::Element &parent) = 0;
			};
			template<typename T> struct serializerImpl: public serializer
		{
			virtual void Deserialize(Object &obj, xml::Element &el) override
			{
				static_cast<T&>(obj).Deserialize(el);
			}
			virtual xml::Element Serialize(const Object &obj, xml::Element &parent) override
			{
				return static_cast<const T&>(obj).Serialize(parent);
			}
		};
			std::unordered_map<std::type_index, std::unique_ptr<serializer>> serializers;
	};

	class Cloner
	{
		public:
			static UObject Clone(const Object &obj)
			{
				const auto it = getInstance().cloners.find(typeid(obj));
				if (it == getInstance().cloners.end())
					throw ExceptionProtocol{};
				return it->second->Clone(obj);
			}
			template<typename T> static void Register()
			{
				getInstance().cloners.emplace(typeid(T), std::make_unique<clonerImpl<T>>());
			}
		private:
			Cloner() {}
			static inline Cloner& getInstance() { static Cloner c; return c; }
			struct cloner
			{
				virtual UObject Clone(const Object &o) const = 0;
			};
			template<typename T> struct clonerImpl: public cloner
		{
			virtual UObject Clone(const Object &o) const override { return std::make_unique<T>(static_cast<const T&>(o)); }
		};
			std::unordered_map<std::type_index, std::unique_ptr<cloner>> cloners;
	};

}
/*@}*/
#endif

