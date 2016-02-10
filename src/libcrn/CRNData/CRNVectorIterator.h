/* Copyright 2009-2014 INSA Lyon, CoReNum
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
 * file: CRNVectorIterator.h
 * \author Yann LEYDIER
 */

#ifndef CRNVectorIterator_HEADER
#define CRNVectorIterator_HEADER

class const_iterator;

class iterator: public std::iterator<std::random_access_iterator_tag, SObject>
{
	public:
		friend class Vector::const_iterator;
		friend class Vector;
		/* constructors */
		iterator():it() {}
		iterator(const iterator &) = default;
		iterator(iterator &&) = default;
		iterator(const std::vector<SObject>::iterator &current):it(current) {}
		virtual ~iterator() {}
		iterator& operator=(const iterator&) = default;
		iterator& operator=(iterator&&) = default;
		/* forward iterator */
		iterator& operator++() { ++it; return *this; }
		iterator operator++(int) { iterator bis(it); ++it; return bis; }
		bool operator==(iterator const &other) const { return it == other.it; }
		bool operator!=(iterator const &other) const { return it != other.it; }
		reference operator*() const { return *it; }
		reference operator->() const { return *it; }
		/* bidirectional iterator */
		iterator& operator--() { --it; return *this; }
		iterator operator--(int) { iterator bis(it); --it; return bis; }
		/* random access iterator */
		ptrdiff_t operator-(iterator const &rhs) const { return it - rhs.it; }
		bool operator<(iterator const &other) const { return it < other.it; }
		bool operator>(iterator const &other) const { return it > other.it; }
		bool operator<=(iterator const &other) const { return it <= other.it; }
		bool operator>=(iterator const &other) const { return it >= other.it; }
		iterator operator+(int step) const { return iterator(it + step); }
		iterator operator-(int step) const { return iterator(it - step); }
		iterator& operator+=(int step) { it += step; return *this; }
		iterator& operator-=(int step) { it -= step; return *this; }
		reference operator[](int index) { return it[index]; }
	protected:
		std::vector<SObject>::iterator it;
};

class const_iterator: public std::iterator<std::random_access_iterator_tag, SCObject>
{
	public:
		friend class Vector;
		/* constructors */
		const_iterator():it() {}
		const_iterator(const const_iterator &) = default;
		const_iterator(const_iterator &&) = default;
		const_iterator(const Vector::iterator &other):it(other.it) {}
		const_iterator(const std::vector<SObject>::const_iterator &current):it(current) {}
		virtual ~const_iterator() {}
		const_iterator& operator=(const const_iterator&) = default;
		const_iterator& operator=(const_iterator&&) = default;
		/* forward const_iterator */
		const_iterator& operator++() { ++it; return *this; }
		const_iterator operator++(int) { const_iterator bis(it); ++it; return bis; }
		bool operator==(const_iterator const &other) const { return it == other.it; }
		bool operator!=(const_iterator const &other) const { return it != other.it; }
		const value_type operator*() const { return *it; }
		const value_type operator->() const { return *it; }
		/* bidirectional const_iterator */
		const_iterator& operator--() { --it; return *this; }
		const_iterator operator--(int) { const_iterator bis(it); --it; return bis; }
		/* random access const_iterator */
		ptrdiff_t operator-(const_iterator const &rhs) const { return it - rhs.it; }
		bool operator<(const_iterator const &other) const { return it < other.it; }
		bool operator>(const_iterator const &other) const { return it > other.it; }
		bool operator<=(const_iterator const &other) const { return it <= other.it; }
		bool operator>=(const_iterator const &other) const { return it >= other.it; }
		const_iterator operator+(int step) const { return const_iterator(it + step); }
		const_iterator operator-(int step) const { return const_iterator(it - step); }
		const_iterator& operator+=(int step) { it += step; return *this; }
		const_iterator& operator-=(int step) { it -= step; return *this; }
		const value_type operator[](int index) const { return it[index]; }
	protected:
		std::vector<SObject>::const_iterator it;
};



#endif


