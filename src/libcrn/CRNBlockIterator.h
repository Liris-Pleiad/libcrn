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
 * file: CRNBlockIterator.h
 * \author Yann LEYDIER
 */

#ifndef CRNBlockIterator_HEADER
#define CRNBlockIterator_HEADER

class const_block_iterator;

/*! \brief Iterator on the blocks of a child tree */
class block_iterator: public std::iterator<std::random_access_iterator_tag, SObject>
{
	public:
		friend class Block::const_block_iterator;
		/* constructors */
		block_iterator():it() {}
		block_iterator(const block_iterator&) = default;
		block_iterator(block_iterator&&) = default;
		block_iterator(const Vector::iterator &current):it(current) {}
		virtual ~block_iterator() {}

		block_iterator& operator=(const block_iterator&) = default;
		block_iterator& operator=(block_iterator&&) = default;
		/* forward iterator */
		const block_iterator& operator++() { ++it; return *this; }
		block_iterator operator++(int) { block_iterator bis(it); ++it; return bis; }
		bool operator==(block_iterator const &other) const { return it == other.it; }
		bool operator!=(block_iterator const &other) const { return it != other.it; }
		reference operator*() const { return *it; }
		SBlock AsBlock() const { return std::static_pointer_cast<Block>(*it); }
		SBlock operator->() const { return std::static_pointer_cast<Block>(*it); }
		/* bidirectional iterator */
		block_iterator& operator--() { --it; return *this; }
		block_iterator operator--(int) { block_iterator bis(it); --it; return bis; }
		/* random access iterator */
		ptrdiff_t operator-(block_iterator const &rhs) const { return it - rhs.it; }
		bool operator<(block_iterator const &other) const { return it < other.it; }
		bool operator>(block_iterator const &other) const { return it > other.it; }
		bool operator<=(block_iterator const &other) const { return it <= other.it; }
		bool operator>=(block_iterator const &other) const { return it >= other.it; }
		block_iterator operator+(int step) const { return block_iterator(it + step); }
		block_iterator operator-(int step) const { return block_iterator(it - step); }
		const block_iterator& operator+=(int step) { it += step; return *this; }
		const block_iterator& operator-=(int step) { it -= step; return *this; }
		reference operator[](int index) { return it[index]; }
	private:
		Vector::iterator it;
};

/*! \brief const Iterator on the blocks of a child tree */
class const_block_iterator: public std::iterator<std::random_access_iterator_tag, SCObject>
{
	public:
		/* constructors */
		const_block_iterator():it() {}
		const_block_iterator(const block_iterator &other):it(other.it) {}
		const_block_iterator(const const_block_iterator&) = default;
		const_block_iterator(const_block_iterator&&) = default;
		const_block_iterator(const Vector::const_iterator &current):it(current) {}
		virtual ~const_block_iterator() {}

		const_block_iterator& operator=(const const_block_iterator&) = default;
		const_block_iterator& operator=(const_block_iterator&&) = default;
		/* forward iterator */
		const const_block_iterator& operator++() { ++it; return *this; }
		const_block_iterator operator++(int) { const_block_iterator bis(it); ++it; return bis; }
		bool operator==(const_block_iterator const &other) const { return it == other.it; }
		bool operator!=(const_block_iterator const &other) const { return it != other.it; }
		value_type operator*() const { return *it; }
		SCBlock AsBlock() const { return std::static_pointer_cast<const Block>(*it); }
		SCBlock operator->() const { return std::static_pointer_cast<const Block>(*it); }
		/* bidirectional iterator */
		const_block_iterator& operator--() { --it; return *this; }
		const_block_iterator operator--(int) { const_block_iterator bis(it); --it; return bis; }
		/* random access iterator */
		ptrdiff_t operator-(const_block_iterator const &rhs) const { return it - rhs.it; }
		bool operator<(const_block_iterator const &other) const { return it < other.it; }
		bool operator>(const_block_iterator const &other) const { return it > other.it; }
		bool operator<=(const_block_iterator const &other) const { return it <= other.it; }
		bool operator>=(const_block_iterator const &other) const { return it >= other.it; }
		const_block_iterator operator+(int step) const { return const_block_iterator(it + step); }
		const_block_iterator operator-(int step) const { return const_block_iterator(it - step); }
		const const_block_iterator& operator+=(int step) { it += step; return *this; }
		const const_block_iterator& operator-=(int step) { it -= step; return *this; }
		value_type operator[](int index) const { return it[index]; }
	private:
		Vector::const_iterator it;
};



#endif


