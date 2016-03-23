/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, CoReNum, Université Paris Descartes, ENS-Lyon
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
 * file: CRNVector.h
 * \author Yann LEYDIER
 */

#ifndef CRNVECTOR_HEADER
#define CRNVECTOR_HEADER

#include <CRNComplexObject.h>
#include <algorithm>
#include <cstddef>
#include <CRNData/CRNVectorPtr.h>
#include <CRNData/CRNForeach.h>
#include <CRNData/CRNData.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Data vector class
	 *
	 * \author 	Yann LEYDIER
	 * \date		April 2007
	 * \version 0.3
	 * \ingroup containers
	 */
	class Vector: public Object
	{
		public:
			/*! \brief Default constructor */
			Vector();
			/*! \brief Constructor from a std::vector containing data that can be casted or converted to SObject 
			 * \param[in]	vec	a vector of objects than can be converted to SObject
			 * \param[in]	protos	the protocols that must be met by the elements of the vector
			 */
			// XXX TODO
			//template<typename T> Vector(std::vector<T> &vec, Protocol protos = Protocol::Object):
				//protocols(protos)
			//{
				//for (typename std::vector<T>::iterator it = vec.begin(); it != vec.end(); ++it)
				//{
					//SObject o(Data::ToCRN(*it));
					//if (!o->Implements(protocols))
						//throw ExceptionProtocol("Vector(std::vector): incompatible protocols");
					//data.push_back(o);
				//}
			//}
			/*! \brief Constructor from iterators pointing to data that can be casted or converted to SObject
			 * \param[in]	begin_it	first iterator on objects than can be converted to SObject
			 * \param[in]	end_it	end iterator
			 * \param[in]	protos	the protocols that must be met by the elements of the vector
			 */
			//template<typename IT> Vector(const IT &begin_it, const IT &end_it, Protocol protos = Protocol::Object):
				//protocols(protos)
			//{
				//for (IT it = begin_it; it != end_it; ++it)
				//{
					//SObject o(Data::ToCRN(*it));
					//if (!o->Implements(protocols))
						//throw ExceptionProtocol("Vector(IT begin, IT end): incompatible protocols");
					//data.push_back(o);
				//}
			//}
			/*! \brief Destructor */
			virtual ~Vector() override;

			Vector(const Vector &) = delete;
			Vector(Vector &&) = default;
			Vector& operator=(const Vector &) = delete;
			Vector& operator=(Vector &&) = default;

			/*! \brief Returns the number of data objects in the vector */
			size_t Size() const noexcept { return data.size(); }
			/*! \brief Tests if the vector is empty */
			bool IsEmpty() const noexcept { return data.empty(); }

			/*! \brief Returns an object from index (unsafe) */
			SObject& operator[](size_t i) { return data[i]; }
			/*! \brief Returns an object from index (unsafe) */
			SCObject operator[](size_t i) const { return data[i]; }
			/*! \brief Returns an object from index (unsafe) */
			SObject& At(size_t i) { return data[i]; }
			/*! \brief Returns an object from index (unsafe) */
			SCObject At(size_t i) const { return data[i]; }

			/*! \brief Returns a reference to the first element */
			SObject& Front() { return data.front(); }
			/*! \brief Returns a reference to the first element */
			SCObject Front() const { return data.front(); }
			/*! \brief Returns a reference to the last element */
			SObject& Back() { return data.back(); }
			/*! \brief Returns a reference to the last element */
			SCObject Back() const { return data.back(); }
			/*! \brief Checks of the object is in the vector */
			bool Contains(const SCObject &o) const;
			/*! \brief Adds an object at the end of the vector */
			void PushBack(const SObject &d);
			/*! \brief Inserts an object at a given position */
			void Insert(const SObject &d, size_t pos);

			/*! \brief Reorders the elements */
			void ReorderFrom(const std::vector<size_t> &from);
			/*! \brief Reorders the elements */
			void ReorderTo(const std::vector<size_t> &to);

			/*! \brief Removes the last element */
			void PopBack() { data.pop_back(); }
			/*! \brief Removes an element (safe) */
			void Remove(size_t index);
			/*! \brief Removes an element (safe) */
			void Remove(const SCObject &obj);
			/*! \brief Empties the vector */
			void Clear() noexcept { data.clear(); }

#include <CRNData/CRNVectorIterator.h>
			/*! \brief Returns an iterator to the first element */
			iterator begin() { return data.begin(); }
			/*! \brief Returns a iterator to the end of the list */
			iterator end() { return data.end(); }
			/*! \brief Returns a const iterator to the first element */
			const_iterator begin() const { return data.begin(); }
			/*! \brief Returns a const iterator to the end of the list */
			const_iterator end() const { return data.end(); }
			/*! \brief Returns a const iterator to the first element */
			const_iterator cbegin() const { return data.begin(); }
			/*! \brief Returns a const iterator to the end of the list */
			const_iterator cend() const { return data.end(); }

			/*! \brief Removes element */
			void Remove(iterator it);
			/*! \brief Removes elements */
			void Remove(iterator begin, iterator end);
			/*! \brief Removes elements if predicate is true */
			template<class Predicate> void RemoveIf(Predicate pred) { Remove(std::remove_if(begin(), end(), pred), end()); }

			/*! \brief Finds an object in the container */
			const_iterator Find(const SCObject &o) const;
			/*! \brief Finds an object in the container */
			iterator Find(const SCObject &o);

			/*! \brief Converts to a std::vector of pointers */
			template<typename T> std::vector<std::shared_ptr<T>> ToStd()
			{
				std::vector<std::shared_ptr<T>> v;
				for (iterator it = begin(); it != end(); ++it)
					v.push_back(std::dynamic_pointer_cast<T>(*it));
				return v;
			}
			/*! \brief Converts to a std::vector of pointers */
			template<typename T> std::vector<std::shared_ptr<const T>> ToStd() const
			{
				std::vector<std::shared_ptr<const T>> v;
				for (const_iterator it = begin(); it != end(); ++it)
					v.push_back(std::dynamic_pointer_cast<const T>(*it));
				return v;
			}
			/*! \brief Gets the inner data */
			std::vector<SObject> Std() && { return std::move(data); }

			/*! \brief Sorts the list using the POSET protocol if applicable */
			void Sort();
			/*! \brief Swaps contents with another vector */
			void Swap(Vector &other) noexcept;

			/*! \brief Optimizes the memory usage */
			void ShrinkToFit();

			/*! \brief Distance to another vector if applicable */
			double Distance(const Vector &obj) const;

			/*! \brief Reads from an XML node if applicable */
			void Deserialize(xml::Element &el);
			/*! \brief Dumps to an XML node if applicable */
			xml::Element Serialize(xml::Element &parent) const;
		private:

			std::vector<SObject> data; /*!< internal data storage */

		CRN_DECLARE_CLASS_CONSTRUCTOR(Vector)
		public: Vector(xml::Element &el) { Deserialize(el); }
	};
	template<> struct IsSerializable<Vector> : public std::true_type {};
	// TODO XXX override Clone()
	template<> struct IsClonable<Vector> : public std::true_type {};
	// TODO XXX override Distance()
	template<> struct IsMetric<Vector> : public std::true_type {};

	/*! \brief Size of a vector */
	inline size_t Size(const Vector &v) noexcept { return v.Size(); }
	/*! \brief Swaps two vectors */
	inline void swap(Vector &v1, Vector &v2) noexcept { v1.Swap(v2); }
}
CRN_ADD_RANGED_FOR_TO_POINTERS(crn::Vector)
CRN_ADD_RANGED_FOR_TO_CONST_POINTERS(crn::Vector)

namespace std
{
	inline void swap(crn::Vector &v1, crn::Vector &v2) noexcept { v1.Swap(v2); }
}
#endif
