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
	class Serializer
	{
		public:
			static void Deserialize(Object &obj, xml::Element &el)
			{
				const auto id = std::type_index{typeid(obj)};
				const auto it = getInstance().serializers.find(id);
				if (it == getInstance().serializers.end())
					throw ExceptionProtocol{id.name() + StringUTF8(": not a serializable object.")};
				it->second->deserialize(obj, el);
			}
			static xml::Element Serialize(const Object &obj, xml::Element &el)
			{
				const auto id = std::type_index{typeid(obj)};
				const auto it = getInstance().serializers.find(id);
				if (it == getInstance().serializers.end())
					throw ExceptionProtocol{id.name() + StringUTF8(": not a serializable object.")};
				return it->second->serialize(obj, el);
			}
			template<typename T> static void Register()
			{
				getInstance().serializers.emplace(typeid(T), std::make_unique<serializerImpl<T>>());
			}
		private:
			Serializer() {}
			static Serializer& getInstance();
			struct serializer
			{
				virtual ~serializer() {}
				virtual void deserialize(Object &obj, xml::Element &el) = 0;
				virtual xml::Element serialize(const Object &obj, xml::Element &parent) = 0;
			};
			template<typename T> struct serializerImpl: public serializer
			{
				virtual void deserialize(Object &obj, xml::Element &el) override
				{
					static_cast<T&>(obj).Deserialize(el);
				}
				virtual xml::Element serialize(const Object &obj, xml::Element &parent) override
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
				const auto id = std::type_index{typeid(obj)};
				const auto it = getInstance().cloners.find(id);
				if (it == getInstance().cloners.end())
					throw ExceptionProtocol{id.name() + StringUTF8(": not a clonable object.")};
				return it->second->clone(obj);
			}
			template<typename T> static void Register()
			{
				getInstance().cloners.emplace(typeid(T), std::make_unique<clonerImpl<T>>());
			}

			static std::string GetClasses()
			{
				auto s = std::string{};
				for (const auto &c : getInstance().cloners)
				{
					s += c.first.name();
					s += " ";
				}
				return s;
			}
		private:
			Cloner() {}
			static Cloner& getInstance();
			struct cloner
			{
				virtual ~cloner() {}
				virtual UObject clone(const Object &o) const = 0;
			};
			template<typename T> struct clonerImpl: public cloner
			{
				virtual UObject clone(const Object &o) const override { return std::make_unique<T>(static_cast<const T&>(o)); }
			};
			std::unordered_map<std::type_index, std::unique_ptr<cloner>> cloners;
	};

	class Ruler
	{
		public:
			static double ComputeDistance(const Object &o1, const Object &o2)
			{
				const auto id1 = std::type_index{typeid(o1)};
				const auto id2 = std::type_index{typeid(o2)};
				if (id1 != id2)
					throw ExceptionDomain{"Cannot compute distance between objects of different classes."};
				const auto it = getInstance().rulers.find(id1);
				if (it == getInstance().rulers.end())
					throw ExceptionProtocol{id1.name() + StringUTF8(": not a metric object.")};
				return it->second->distance(o1, o2);
			}
			template<typename T> static void Register()
			{
				getInstance().rulers.emplace(typeid(T), std::make_unique<rulerImpl<T>>());
			}
		private:
			Ruler() {}
			static Ruler& getInstance();
			struct ruler
			{
				virtual ~ruler() {}
				virtual double distance(const Object &o1, const Object &o2) const = 0;
			};
			template<typename T> struct rulerImpl: public ruler
			{
				virtual double distance(const Object &o1, const Object &o2) const override { return Distance(static_cast<const T&>(o1), static_cast<const T&>(o2)); }
			};
			std::unordered_map<std::type_index, std::unique_ptr<ruler>> rulers;
	};

}
/*@}*/
#endif

