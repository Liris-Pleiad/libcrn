/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, CoReNum
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
 * file: CRNVector.cpp
 * \author Yann LEYDIER
 */

#include <CRNData/CRNVector.h>
#include <CRNException.h>
#include <CRNIO/CRNIO.h>
#include <algorithm>
#include <CRNData/CRNMap.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*!
 * Default constructor
 * \param[in]	protos	the mandatory protocols for the contents
 */
Vector::Vector(Protocol protos):
	protocols(protos)
{
}

/*!
 * Destructor
 */
Vector::~Vector()
{
}

/*!
 * Returns the protocols supported by the container depending on its contents.
 * \return the protocols supported by the container.
 */
Protocol Vector::GetClassProtocols() const noexcept
{
	return protocols & (crn::Protocol::Object|crn::Protocol::Clonable|crn::Protocol::Serializable|crn::Protocol::Metric);
}

/*!
 * Dumps the contents to a string
 * \return	a string representing the contents of the container
 */
String Vector::ToString() const
{
	String s = GetClassName();
	s += " { ";
	for (const_iterator it = begin(); it != end(); ++it)
	{
		s += (*it)->ToString();
		const_iterator nit(it);
		++nit;
		if (nit != end())
			s += ", ";
	}
	s += " }";
	return s;
}

/*!
 * Adds an object at the end of the list
 *
 * \throws	ExceptionProtocol	object has wrong protocols
 *
 * \param[in]	d	the object to add
 */
void Vector::PushBack(SObject d)
{
	if (checkProtocols(*d))
	{
		data.push_back(d);
	}
	else
	{
		throw ExceptionProtocol(StringUTF8(_("Object not added: protocols differ obj=")) + int(d->GetClassProtocols()) + crn::StringUTF8("  // vec=") + int(protocols));
	}
}

/*!
 * Checks of the object is in the vector
 *
 * \param[in]	o	the object to check
 * \return	true if o is inside the container, false else
 */
bool Vector::Contains(const SCObject &o) const
{
	for (const_iterator it = begin(); it != end(); ++it)
		if (*it == o)
			return true;
	return false;
}

/*! 
 * Finds an object in the container
 *
 * \param[in]	o	the object to find
 * \return a const_iterator on the object or on end() if not found
 */
Vector::const_iterator Vector::Find(const SCObject &o) const
{
	for (const_iterator it = begin(); it != end(); ++it)
		if (*it == o)
			return it;
	return end();
}

/*! 
 * Finds an object in the container
 *
 * \param[in]	o	the object to find
 * \return an iterator on the object or on end() if not found
 */
Vector::iterator Vector::Find(const SCObject &o)
{
	for (iterator it = begin(); it != end(); ++it)
		if (*it == o)
			return it;
	return end();
}

/*! 
 * Inserts an object at a given position
 *
 * \throws	ExceptionProtocol	object has wrong protocols
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	d	the object to add
 * \param[in]	pos	the position
 */
void Vector::Insert(SObject d, size_t pos)
{
	if (checkProtocols(*d))
	{
		if (pos < data.size())
		{
			data.insert(data.begin() + pos, d);
		}
		else if ((pos == data.size()) || (pos == 0))
		{
			data.push_back(d);
		}
		else
			throw ExceptionDomain(_("Index out of bounds."));
	}
	else
		throw ExceptionProtocol(StringUTF8(_("Object not added: protocols differ obj=")) + int(d->GetClassProtocols()) + crn::StringUTF8("  // vec=") + int(protocols));
}

/*! Reorders the elements
 *
 * \throws	ExceptionDimension	changeset is of wrong size
 * \throws	ExceptionLogic	changeset contains duplicates
 * \throws	ExceptionDomain	changeset contains values out of bounds
 *
 * \param[in]  from  a vector containing the previous index of each element
 */
void Vector::ReorderFrom(const std::vector<size_t> &from)
{
	if (from.size() != data.size())
	{
		throw ExceptionDimension(StringUTF8("void Vector::ReorderFrom(const std::vector<size_t> &from): ") + 
				_("bad changeset size."));
	}
	std::set<size_t> cont;
	for (size_t i : from)
	{
		cont.insert(i);
	}
	if (cont.size() != data.size())
	{
		throw ExceptionLogic(StringUTF8("void Vector::ReorderFrom(const std::vector<size_t> &from): ") + 
				_("changeset contains duplicates."));
	}
	if ((*cont.begin() != 0) || (*cont.rbegin() != data.size() - 1))
	{
		throw ExceptionDomain(StringUTF8("void Vector::ReorderFrom(const std::vector<size_t> &from): ") + 
				_("changeset contains values out of bounds."));
	}
	std::vector<SObject> newdata;
	for (size_t i : from)
	{
		newdata.push_back(data[i]);
	}
	data.swap(newdata);
}

/*! Reorders the elements
 *
 * \throws	ExceptionDimension	changeset is of wrong size
 * \throws	ExceptionLogic	changeset contains duplicates
 * \throws	ExceptionDomain	changeset contains values out of bounds
 *
 * \param[in]  to  a vector containing the new index of each element
 */
void Vector::ReorderTo(const std::vector<size_t> &to)
{
	if (to.size() != data.size())
	{
		throw ExceptionDimension(StringUTF8("void Vector::ReorderTo(const std::vector<size_t> &to): ") + 
				_("bad changeset size."));
	}
	std::set<size_t> cont;
	for (size_t i : to)
	{
		cont.insert(i);
	}
	if (cont.size() != data.size())
	{
		throw ExceptionLogic(StringUTF8("void Vector::ReorderTo(const std::vector<size_t> &to): ") + 
				_("changeset contains duplicates."));
	}
	if ((*cont.begin() != 0) || (*cont.rbegin() != data.size() - 1))
	{
		throw ExceptionDomain(StringUTF8("void Vector::ReorderTo(const std::vector<size_t> &to): ") + 
				_("changeset contains values out of bounds."));
	}
	std::vector<SObject> newdata(data.size());
	for (size_t tmp = 0; tmp < to.size(); ++tmp)
	{
		newdata[to[tmp]] = data[tmp];
	}
	data.swap(newdata);
}

/*! Removes an element by index
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	index	the index of the element to remove
 */
void Vector::Remove(size_t index)
{
	if (index >= data.size())
		throw ExceptionDomain(_("Index out of bounds."));
	data.erase(data.begin() + index);
}

/*! Removes an element by pointer
 *
 * \throws	ExceptionNotFound	object not found.
 *
 * \param[in]	obj	the objet to remove
 */
void Vector::Remove(const SCObject &obj)
{
	for (auto it = data.begin(); it != data.end(); ++it)
		if (*it == obj)
		{
			data.erase(it);
			return;
		}
	throw ExceptionNotFound(_("Object not found."));
}

/*! Removes element
 *
 * \param[in]	it	an iterator on the object to remove
 */
void Vector::Remove(iterator it)
{
	data.erase(it.it);
}

/*! Removes elements
 *
 * \param[in]	begin	an iterator on the first object to remove
 * \param[in]	end	an iterator after the last object to remove
 */
void Vector::Remove(iterator begin, iterator end)
{
	data.erase(begin.it, end.it);
}

/*!
 * Clones the container and its contents if applicable.
 *
 * \throws	ExceptionProtocol	the content of the vector is not clonable
 *
 * \return	the cloned container
 */
UObject Vector::Clone() const
{
	if (!(protocols & crn::Protocol::Clonable))
		throw ExceptionProtocol(_("This vector does not contain clonable objects."));

	UVector v = std::make_unique<Vector>(protocols);
	v->SetName(GetName());
	for (const_iterator it = begin(); it != end(); ++it)
		v->PushBack((*it)->Clone());
	return std::forward<UVector>(v);
}

/*!
 * Sorts the list using the POSET protocol if applicable.
 * \throws	ExceptionProtocol	the content of the vector is not POSET
 */
void Vector::Sort()
{
	if (!(protocols & crn::Protocol::POSet))
		throw ExceptionProtocol(_("This vector does not contain poset objects."));

	std::sort(begin(), end(), [](const SCObject &o1, const SCObject &o2)
				{
					return (o1->LT(*o2)).IsTrue();
				}
			);
}

/*****************************************************************************/
/*!
 * Unsafe load
 *
 * \throws	ExceptionProtocol	the content of the vector is not serializable
 * \throws	ExceptionInvalidArgument	not a Vector
 * \throws	ExceptionRuntime	impossible to read an XML element
 * \throws	ExceptionNotFound	unknown type
 *
 * \param[in]	el	the element to load
 */
void Vector::deserialize(xml::Element &el)
{
	if (!(protocols & crn::Protocol::Serializable))
		throw ExceptionProtocol(_("This vector does not contain serializable objects."));
	if (el.GetValue() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Vector::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	protocols = Protocol(el.GetAttribute<int>("protocols"));
	std::multimap<int, SObject> xmllist;
	for (xml::Element te = el.BeginElement(); te != el.EndElement(); ++te)
	{
		SObject d(nullptr);
		try
		{
			d = DataFactory::CreateData(te);
		}
		catch (crn::Exception &)
		{	// XXX throw?
			CRNWarning(String(U"bool Vector::deserialize"
						U"(xml::Element &el): ") + String(_("Unknown XML element: ")) +
					te.GetValue());
		}
		if (!d)
			continue;
		int index;
		try { index = te.GetAttribute<int>("vector_index", false); } catch (...) { index = int(xmllist.size()); }
		xmllist.insert(std::make_pair(index, d));
	}
	Clear();
	for (auto & elem : xmllist)
		data.push_back(elem.second);
	ShrinkToFit();
}

/*****************************************************************************/
/*!
 * Unsafe save
 * \throws	ExceptionProtocol	the content of the vector is not serializable
 *
 * \param[in]	parent	the parent element to which we will add the new element
 * \return The newly created element, nullptr if failed.
 */
xml::Element Vector::serialize(xml::Element &parent) const
{
	if (!(protocols & crn::Protocol::Serializable))
		throw ExceptionProtocol(_("This vector does not contain serializable objects."));
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("protocols", int(protocols));

	for (size_t tmp = 0; tmp < data.size(); tmp++)
	{
		xml::Element item = data[tmp]->Serialize(el);
		item.SetAttribute("vector_index", int(tmp));
	}

	return el;
}

/*!
 * Computes the distance to another vector using the Metric protocal if applicable.
 *
 * \throws	ExceptionProtocol	the content of the vector is not metric
 * \throws	ExceptionDimension	the vector do not have the same size
 * \param[in]	obj	the other vector
 * \return	the distance
 */
double Vector::distance(const Object &obj) const
{
	if (!(protocols & crn::Protocol::Metric))
		throw ExceptionProtocol(_("This vector does not contain metric objects."));
	const Vector &other = static_cast<const Vector&>(obj);
	if (other.Size() != Size())
		throw ExceptionDimension(StringUTF8("Double Vector::distance(const Object &obj) const: ") + 
				_("The vector do not have the same size."));
	double d = 0;
	for (size_t tmp = 0; tmp < Size(); tmp++)
		d += data[tmp]->Distance(*other.At(tmp));
	return d;
}

/*! 
 * Swaps contents with another vector
 *
 * \param[in]	other	the vector to swap with
 */
void Vector::Swap(Vector &other) noexcept
{
	data.swap(other.data);
	std::swap(protocols, other.protocols);
}

/*! 
 * Checks if the protocols are compatible with the object's constraints
 *
 * \param[in]  obj  the object to check
 * \return true if the protocols are compatible with the constraints, false else
 */
bool Vector::checkProtocols(const Object &obj)
{
	if ((obj.GetClassProtocols() & protocols) == protocols)
	{
		return true;
	}
	try
	{
		const Vector &v(dynamic_cast<const Vector&>(obj));
		if ((v.protocols & protocols) == protocols)
		{
			return true;
		}
	}
	catch (...) { }
	try
	{
		const Map &m(dynamic_cast<const Map&>(obj));
		if ((m.GetContentProtocols() & protocols) == protocols)
		{
			return true;
		}
	}
	catch (...) { }
	return false;
}

/*! Optimizes the memory usage */
void Vector::ShrinkToFit()
{
	std::vector<SObject> (data.begin(), data.end()).swap(data);
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Vector)
	CRN_DATA_FACTORY_REGISTER(U"Vector", Vector)
CRN_END_CLASS_CONSTRUCTOR(Vector)

