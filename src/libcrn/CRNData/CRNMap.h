/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, Université Paris Descartes, ENS-Lyon
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
 * file: CRNMap.h
 * \author Yann LEYDIER
 */

#ifndef CRNMAP_H
#define CRNMAP_H

#include <CRNComplexObject.h>
#include <map>
#include <CRNData/CRNMapPtr.h>
#include <CRNData/CRNForeach.h>
#include <set>

namespace crn
{
	/****************************************************************************/
	/*! \brief Data map class
	 *
	 * \author 	Yann LEYDIER
	 * \date		April 2007
	 * \version 0.3
	 * \ingroup containers
	 */
	class Map: public Object
	{
		public:
			/*! \brief Default constructor */
			Map();
			/*! \brief Destructor */
			virtual ~Map() override;

			Map(const Map &other);
			Map& operator=(const Map &other);
			Map(Map &&) = default;
			Map& operator=(Map &&) = default;

			/*! \brief Returns the number of data objects in the map */
			size_t Size() const noexcept { return data.size(); }
			/*! \brief Tests if the map is empty */
			bool IsEmpty() const noexcept { return data.empty(); }

			/*! \brief Returns an object from index. No constraint check is performed if the reference is used as a lvalue.
			 *
			 * \warning  No constraint check is performed if the reference is used as a lvalue. */
			SObject& operator[](const String &s) { return data[s]; }
			/*! \brief Returns an object from index or nullptr if inexistent */
			SObject Get(const String &s);
			/*! \brief Returns an object from index or nullptr if inexistent */
			SCObject Get(const String &s) const;
			/*! \brief Sets a value for a key with constraints check */
			void Set(const String &key, SObject value);

			/*! \brief iterator on the contents of the container */
			using iterator = std::map<String, SObject>::iterator;

			/*! \brief Removes an element (safe) */
			void Remove(const String &key);
			/*! \brief Removes an element (safe) */
			void Remove(const SObject &obj);
			/*! \brief Removes an element (safe) */
			void Remove(iterator it);
			/*! \brief Removes an element (safe) */
			void Remove(iterator first, iterator end_);
			/*! \brief Empties the map */
			void Clear() noexcept { data.clear(); }

			/*! \brief Returns an iterator to the first element */
			iterator begin() { return data.begin(); }
			/*! \brief Returns an iterator after the last element */
			iterator end() { return data.end(); }
			/*! \brief Returns an iterator to a specific key */
			iterator Find(const String &key) { return data.find(key); }

			/*! \brief const_iterator on the contents of the container */
			using const_iterator = std::map<String, SObject>::const_iterator;
			/*! \brief Returns a const iterator to the first element */
			const_iterator begin() const { return data.begin(); }
			/*! \brief Returns a const iterator after the last element */
			const_iterator end() const { return data.end(); }
			/*! \brief Returns a const iterator to the first element */
			const_iterator cbegin() const { return data.cbegin(); }
			/*! \brief Returns a const iterator after the last element */
			const_iterator cend() const { return data.cend(); }
			/*! \brief Returns a const_iterator to a specific key */
			const_iterator Find(const String &key) const { return data.find(key); }

			/*! \brief reverse_iterator on the contents of the container */
			using reverse_iterator = std::map<String, SObject>::reverse_iterator;
			/*! \brief Returns a reverse iterator to the last element */
			reverse_iterator rbegin() { return data.rbegin(); }
			/*! \brief Returns a reverse iterator before the first element */
			reverse_iterator rend() { return data.rend(); }

			/*! \brief const_reverse_iterator on the contents of the container */
			using const_reverse_iterator = std::map<String, SObject>::const_reverse_iterator;
			/*! \brief Returns a const reverse iterator to the last element */
			const_reverse_iterator rbegin() const { return data.rbegin(); }
			/*! \brief Returns a const reverse iterator before the first element */
			const_reverse_iterator rend() const { return data.rend(); }

			/*! \brief A (key, value) pair */
			using pair = std::pair<const crn::String, SObject>;

			/*! \brief Returns all keys */
			std::set<String> GetKeys() const;
			/*! \brief Returns the first (lowest) key */
			String FirstKey() const;
			/*! \brief Returns the last (greatest) key */
			String LastKey() const;

			/*! \brief Swaps contents with another map */
			void Swap(Map &other) noexcept;

			std::map<String, SObject> Std() && { return std::move(data); }

			/*! \brief Reads from an XML node if applicable */
			void Deserialize(xml::Element &el);
			/*! \brief Dumps to an XML node if applicable */
			xml::Element Serialize(xml::Element &parent) const;

			void Load(const Path &fname);
			void Save(const Path &fname) const;

		private:
			std::map<String, SObject> data; /*!< internal data storage */

		CRN_DECLARE_CLASS_CONSTRUCTOR(Map)
		public: Map(xml::Element &el) { Deserialize(el); }
	};
	template<> struct IsSerializable<Map> : public std::true_type {};
	template<> struct IsClonable<Map> : public std::true_type {};

	inline void Swap(Map &m1, Map &m2) noexcept { m1.Swap(m2); }
}
CRN_ADD_RANGED_FOR_TO_POINTERS(crn::Map)
CRN_ADD_RANGED_FOR_TO_CONST_POINTERS(crn::Map)

namespace std
{
	inline void swap(crn::Map &m1, crn::Map &m2) noexcept { m1.Swap(m2); }
}
#endif
