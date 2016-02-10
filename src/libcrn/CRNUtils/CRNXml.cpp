/* Copyright 2012-2014 CoReNum
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
 * file: CRNXml.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNCharsetConverter.h>
#include <CRNUtils/CRNXml.h>
#include <3rdParty/tixml2/tinyxml2.h>
#include <CRNException.h>
#include <CRNi18n.h>

using namespace crn;
using namespace xml;

static void xmlerror(int err, const char *str1, const char *str2)
{
	StringUTF8 msg;
	if (str1)
	{
		msg += "\n";
		msg += str1;
	}
	if (str2)
	{
		msg += "\n";
		msg += str2;
	}
	switch (err)
	{
		//case tinyxml2::XML_NO_ERROR:
		case tinyxml2::XML_SUCCESS:
			return;
		case tinyxml2::XML_NO_ATTRIBUTE:
			throw ExceptionNotFound(_("Cannot find attribute.") + msg);
		case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE:
			throw ExceptionDomain(_("Wrong attribute type.") + msg);
		case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
			throw ExceptionNotFound(_("File not found.") + msg);
		case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
			throw ExceptionIO(_("File could not be opened.") + msg);
		case tinyxml2::XML_ERROR_FILE_READ_ERROR:
			throw ExceptionRuntime(_("File read error.") + msg);
		case tinyxml2::XML_ERROR_ELEMENT_MISMATCH:
		case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:
			throw ExceptionRuntime(_("Element mismatch.") + msg);
		case tinyxml2::XML_ERROR_PARSING_ELEMENT:
			throw ExceptionRuntime(_("Could not parse element.") + msg);
		case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:
			throw ExceptionRuntime(_("Could not parse attribute.") + msg);
		case tinyxml2::XML_ERROR_IDENTIFYING_TAG:
			throw ExceptionRuntime(_("Could not identify tag.") + msg);
		case tinyxml2::XML_ERROR_PARSING_TEXT:
			throw ExceptionRuntime(_("Could not parse text.") + msg);
		case tinyxml2::XML_ERROR_PARSING_CDATA:
			throw ExceptionRuntime(_("Could not parse CDATA.") + msg);
		case tinyxml2::XML_ERROR_PARSING_COMMENT:
			throw ExceptionRuntime(_("Could not parse comment.") + msg);
		case tinyxml2::XML_ERROR_PARSING_DECLARATION:
			throw ExceptionRuntime(_("Could not parse declaration.") + msg);
		case tinyxml2::XML_ERROR_PARSING_UNKNOWN:
			throw ExceptionRuntime(_("Could not parse unknown item.") + msg);
		case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:
			throw ExceptionRuntime(_("Empty document.") + msg);
		case tinyxml2::XML_ERROR_PARSING:
			throw ExceptionRuntime(_("Parse error.") + msg);
		case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:
			throw ExceptionRuntime(_("Cannot convert text.") + msg);
		case tinyxml2::XML_NO_TEXT_NODE:
			throw ExceptionNotFound(_("No text node.") + msg);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Node
///////////////////////////////////////////////////////////////////////////////

/*! Constructor 
 * \param[in]	n	the inner node pointer
 * \param[in]	c	a character set converter
 */
Node::Node(tinyxml2::XMLNode *n, const SCharsetConverter &c):
	conv(c),
	node(n)
{ }

/*! \returns	true if the node is an element */
bool Node::IsElement()
{
	if (!node)
		return false;
	return node->ToElement() != nullptr;
}

/*! \returns	true if the node is a comment */
bool Node::IsComment()
{
	if (!node)
		return false;
	return node->ToComment() != nullptr;
}

/*! \returns	true if the node is a text */
bool Node::IsText()
{
	if (!node)
		return false;
	return node->ToText() != nullptr;
}

/*! Converts to element
 * \throws	ExceptionDomain	not an element
 * \return	An element
 */
Element Node::AsElement()
{
	if (!node)
		throw ExceptionDomain(_("Not an element."));
	tinyxml2::XMLElement *el = node->ToElement();
	if (!el)
		throw ExceptionDomain(_("Not an element."));
	return Element(el, conv);
}

/*! Converts to comment
 * \throws	ExceptionDomain	not a comment
 * \return	A comment
 */
Comment Node::AsComment()
{
	if (!node)
		throw ExceptionDomain(_("Not a comment."));
	tinyxml2::XMLComment *c = node->ToComment();
	if (!c)
		throw ExceptionDomain(_("Not a comment."));
	return Comment(c, conv);
}

/*! Converts to text
 * \throws	ExceptionDomain	not a text
 * \return	A text
 */
Text Node::AsText()
{
	if (!node)
		throw ExceptionDomain(_("Not a text."));
	tinyxml2::XMLText *t = node->ToText();
	if (!t)
		throw ExceptionDomain(_("Not a text."));
	return Text(t, conv);
}

/*! Gets the content of the node
 * \return	the content of the node
 */
StringUTF8 Node::GetValue() const
{
	return conv->ToUTF8(node->Value());
}

/*! Sets the content of the node
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	s	the new content of the node
 */
void Node::SetValue(const StringUTF8 &s)
{
	node->SetValue(conv->FromUTF8(s).c_str());
}

/*! Gets the parent node if any
 * \return	the parent node of null node
 */
Node Node::GetParent()
{
	tinyxml2::XMLNode *par = const_cast<tinyxml2::XMLNode*>(node->Parent());
	return Node(par, conv);
}

/*! Gets the previous sibling node
 * \return	the previous sibling node or null node
 */
Node Node::GetPreviousSibling()
{
	return Node(node->PreviousSibling(), conv);
}

/*! Gets the next sibling node
 * \return	the next sibling node or null node
 */
Node Node::GetNextSibling()
{
	return Node(node->NextSibling(), conv);
}

/*! Moves to the next sibling node
 * \return	the next sibling node or null node
 */
Node& Node::operator++()
{
	*this = GetNextSibling();
	return *this;
}

/*! Moves to the next sibling node
 * \return	the current sibling node
 */
Node Node::operator++(int)
{
	Node bak(*this);
	*this = GetNextSibling();
	return bak;
}

/*! Gets the previous sibling element
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the element or empty string for any element
 * \return	the previous sibling element or null element
 */
Element Node::GetPreviousSiblingElement(const StringUTF8 &name)
{
	return Element(node->PreviousSiblingElement(name.IsEmpty() ? nullptr : conv->FromUTF8(name).c_str()), conv);
}

/*! Gets the next sibling element
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the element or empty string for any element
 * \return	the next sibling element or null element
 */
Element Node::GetNextSiblingElement(const StringUTF8 &name)
{
	return Element(node->NextSiblingElement(name.IsEmpty() ? nullptr : conv->FromUTF8(name).c_str()), conv);
}

///////////////////////////////////////////////////////////////////////////////
// Element
///////////////////////////////////////////////////////////////////////////////

/*! Constructor 
 * \param[in]	el	the inner element pointer
 * \param[in]	c	a character set converter
 */
Element::Element(tinyxml2::XMLElement *el, const SCharsetConverter &c):
	Node(el, c),
	element(el)
{
	count_subnondes();
}

/*! \brief Updated the count cache */
void Element::count_subnondes()
{
	nb_subnodes = 0;
	nb_subelems = 0;
	if (element)
	{
		for (tinyxml2::XMLNode *sn = element->FirstChild(); sn != NULL; sn = sn->NextSibling())
			nb_subnodes += 1;
		for (tinyxml2::XMLElement *se = element->FirstChildElement(); se != NULL; se = se->NextSiblingElement())
			nb_subelems += 1;
	}
}

/*! Moves to the next sibling element 
 * \return	the next element or null element
 */
Element& Element::operator++()
{
	*this = GetNextSiblingElement();
	return *this;
}

/*! Moves to the next sibling element
 * \return	the current element
 */
Element Element::operator++(int)
{
	Element bak(*this);
	++(*this);
	return bak;
}

/*! Gets the first child node
 * \return	the first child node or null node
 */
Node Element::GetFirstChild()
{
	return Node(element->FirstChild(), conv);
}

/*! Gets the last child node
 * \return	the last child node or null node
 */
Node Element::GetLastChild()
{
	return Node(element->LastChild(), conv);
}

/*! Gets the first child element
 * \param[in]	name	the name of the element or empty string for any element
 * \return	the first child element or null element
 */
Element Element::GetFirstChildElement(const StringUTF8 &name)
{
	return Element(element->FirstChildElement(name.IsEmpty() ? nullptr : name.CStr()), conv);
}

/*! Gets the last child element
 * \param[in]	name	the name of the element or empty string for any element
 * \return	the last child element or null element
 */
Element Element::GetLastChildElement(const StringUTF8 &name)
{
	return Element(element->LastChildElement(name.IsEmpty() ? nullptr : name.CStr()), conv);
}

/*! Gets the first child as text 
 * \throws	ExceptionNotFound	no child
 * \throws	ExceptionDomain first child is not Text
 * \return	the text of the first child node
 */
StringUTF8 Element::GetFirstChildText()
{
	Node n(GetFirstChild());
	if (!n)
		throw ExceptionNotFound(StringUTF8("const StringUTF8 Element::GetFirstChildText(): ") + _("No child node."));
	Text t(n.AsText()); // may throw
	return t.GetValue();
}

/*! Adds an element at the end of the children list 
 * \throws	ExceptionInvalidArgument	empty element name
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the element
 * \return	a handle on the newly created element
 */
Element Element::PushBackElement(const StringUTF8 &name)
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty element name."));
	tinyxml2::XMLElement *el = element->GetDocument()->NewElement(conv->FromUTF8(name).c_str());
	element->InsertEndChild(el);
	nb_subnodes += 1;
	nb_subelems += 1;
	return Element(el, conv);
}

/*! Adds an element at the front of the children list 
 * \throws	ExceptionInvalidArgument	empty element name
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the element
 * \return	a handle on the newly created element
 */
Element Element::PushFrontElement(const StringUTF8 &name)
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty element name."));
	tinyxml2::XMLElement *el = element->GetDocument()->NewElement(conv->FromUTF8(name).c_str());
	element->InsertFirstChild(el);
	nb_subnodes += 1;
	nb_subelems += 1;
	return Element(el, conv);
}

/*! Inserts an element after a node
 * \throws	ExceptionInvalidArgument	null node or empty element name
 * \throws	ExceptionNotFound	node not found
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	n	the node after which the new element will be inserted
 * \param[in]	name	the name of the element
 * \return	a handle on the newly created element
 */
Element Element::InsertElement(Node &n, const StringUTF8 &name)
{
	if (!n)
		throw ExceptionInvalidArgument(_("Null node."));
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty element name."));
	tinyxml2::XMLElement *el = element->GetDocument()->NewElement(conv->FromUTF8(name).c_str());
	if (!element->InsertAfterChild(n.node, el))
	{
		element->GetDocument()->DeleteNode(el);
		throw ExceptionNotFound(_("Node not found."));
	}
	nb_subnodes += 1;
	nb_subelems += 1;
	return Element(el, conv);
}

/*! Adds a comment at the end of the children list 
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in] text	the content of the comment
 * \return	a handle on the newly created comment
 */
Comment Element::PushBackComment(const StringUTF8 &text)
{
	tinyxml2::XMLComment *c = element->GetDocument()->NewComment(conv->FromUTF8(text).c_str());
	element->InsertEndChild(c);
	nb_subnodes += 1;
	return Comment(c, conv);
}

/*! Adds a comment at the front of the children list 
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	text	the content of the comment
 * \return	a handle on the newly created comment
 */
Comment Element::PushFrontComment(const StringUTF8 &text)
{
	tinyxml2::XMLComment *c = element->GetDocument()->NewComment(conv->FromUTF8(text).c_str());
	element->InsertFirstChild(c);
	nb_subnodes += 1;
	return Comment(c, conv);
}

/*! Inserts a comment after a node
 * \throws	ExceptionNotFound	node not found
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	n	the node after which the new comment will be inserted
 * \param[in]	text	the content of the comment
 * \return	a handle on the newly created comment
 */
Comment Element::InsertComment(Node &n, const StringUTF8 &text)
{
	if (!n)
		throw ExceptionInvalidArgument(_("Null node."));
	tinyxml2::XMLComment *c = element->GetDocument()->NewComment(conv->FromUTF8(text).c_str());
	if (!element->InsertAfterChild(n.node, c))
	{
		element->GetDocument()->DeleteNode(c);
		throw ExceptionNotFound(_("Node not found."));
	}
	nb_subnodes += 1;
	return Comment(c, conv);
}

/*! Adds a text at the end of the children list 
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	text	the content of the text
 * \param[in]	cdata	is the text stored as a CData?
 * \return	a handle on the newly created text
 */
Text Element::PushBackText(const StringUTF8 &text, bool cdata)
{
	tinyxml2::XMLText *t = element->GetDocument()->NewText(cdata ? text.CStr() : conv->FromUTF8(text).c_str());
	t->SetCData(cdata);
	element->InsertEndChild(t);
	nb_subnodes += 1;
	return Text(t, conv);
}

/*! Adds a text at the front of the children list 
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	text	the content of the text
 * \param[in]	cdata	is the text stored as a CData?
 * \return	a handle on the newly created text
 */
Text Element::PushFrontText(const StringUTF8 &text, bool cdata)
{
	tinyxml2::XMLText *t = element->GetDocument()->NewText(cdata ? text.CStr() : conv->FromUTF8(text).c_str());
	t->SetCData(cdata);
	element->InsertFirstChild(t);
	nb_subnodes += 1;
	return Text(t, conv);
}

/*! Inserts a text after a node
 * \throws	ExceptionInvalidArgument	null node
 * \throws	ExceptionNotFound	node not found
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	n	the node after which the new text will be inserted
 * \param[in]	text	the content of the text
 * \param[in]	cdata	is the text stored as a CData?
 * \return	a handle on the newly created text
 */
Text Element::InsertText(Node &n, const StringUTF8 &text, bool cdata)
{
	if (!n)
		throw ExceptionInvalidArgument(_("Null node."));
	tinyxml2::XMLText *t = element->GetDocument()->NewText(cdata ? text.CStr() : conv->FromUTF8(text).c_str());
	t->SetCData(cdata);
	if (!element->InsertAfterChild(n.node, t))
	{
		element->GetDocument()->DeleteNode(t);
		throw ExceptionNotFound(_("Node not found."));
	}
	nb_subnodes += 1;
	return Text(t, conv);
}

/*! Adds a copy of a node at the end of the children list
 * \throws	ExceptionInvalidArgument	null or invalid node
 * \param[in]	n	the node to copy
 * \param[in]	recursive	shall the subelements be copied too? (only if n is an Element)
 * \return	a handle on the newly created node
 */
Node Element::PushBackClone(Node &n, bool recursive)
{
	if (!n)
		throw ExceptionInvalidArgument(_("Null node."));
	// element?
	try
	{
		Element el(n.AsElement());
		Element newel(PushBackElement(el.GetName()));
		for (Element::Attribute a = el.BeginAttribute(); a != el.EndAttribute(); ++a)
			newel.SetAttribute(a.GetName(), a.GetValue<StringUTF8>());
		if (recursive)
		{
			for (Node sn = el.BeginNode(); sn != el.EndNode(); ++sn)
				newel.PushBackClone(sn, recursive);
		}
		return newel;
	}
	catch (ExceptionDomain &) { }
	// comment?
	try
	{
		Comment c(n.AsComment());
		return PushBackComment(c.GetValue());
	}
	catch (ExceptionDomain &) { }
	// text?
	try
	{
		Text t(n.AsText());
		return PushBackText(t.GetValue(), t.IsCData());
	}
	catch (ExceptionDomain &) { }
	// other
	// - declaration: not possible in an element
	// - unknown
	tinyxml2::XMLUnknown *un = n.node->ToUnknown();
	if (un)
	{
		tinyxml2::XMLUnknown *nun = element->GetDocument()->NewUnknown(un->Value());
		element->InsertEndChild(nun);
		nb_subnodes += 1;
		return Node(nun, conv);
	}
	else
		throw ExceptionInvalidArgument(_("Invalid node."));
}

/*! Removes all children */
void Element::Clear()
{
	element->DeleteChildren();
	count_subnondes();
}

/*! Removes a child node 
 * \param[in]	n	the node to remove
 */
void Element::RemoveChild(Node &n)
{
	element->DeleteChild(n.node);
	n.node = nullptr;
	count_subnondes();
}

/*! Sets the value of an attribute
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 * \param[in]	value	the new value of the attribute
 */
void Element::SetAttribute(const StringUTF8 &name, const StringUTF8 &value)
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	element->SetAttribute(conv->FromUTF8(name).c_str(), conv->FromUTF8(value).c_str());
}

/*! Removes an attribute 
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 */
void Element::RemoveAttribute(const StringUTF8 &name)
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	element->DeleteAttribute(conv->FromUTF8(name).c_str());
}

/*! Gets the first attribute
 * \return	the first attribute
 */
Element::Attribute Element::BeginAttribute()
{
	return Attribute(const_cast<tinyxml2::XMLAttribute*>(element->FirstAttribute()), conv);
}

/*! Gets the null attribute
 * \return	the null attribute
 */
Element::Attribute Element::EndAttribute()
{
	return Attribute(nullptr, conv);
}

/*! Gets a text attribute
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	ExceptionNotFound	attribute not found
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 * \param[in]	value	the variable where the value will be stored
 */
void Element::queryAttribute(const StringUTF8 &name, StringUTF8 &value) const
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	const char *val = element->Attribute(conv->FromUTF8(name).c_str());
	if (!val)
		throw ExceptionNotFound(_("Cannot find attribute: ") + name);
	value = conv->ToUTF8(val);
}

/*! Gets an int attribute
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	cannot convert attribute
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 * \param[in]	value	the variable where the value will be stored
 */
void Element::queryAttribute(const StringUTF8 &name, int &value) const
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	int res = element->QueryIntAttribute(conv->FromUTF8(name).c_str(), &value);
	xmlerror(res, element->GetDocument()->GetErrorStr1(), element->GetDocument()->GetErrorStr2());
}

/*! Gets an unsigned int attribute
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	cannot convert attribute
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 * \param[in]	value	the variable where the value will be stored
 */
void Element::queryAttribute(const StringUTF8 &name, unsigned int &value) const
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	int res = element->QueryUnsignedAttribute(conv->FromUTF8(name).c_str(), &value);
	xmlerror(res, element->GetDocument()->GetErrorStr1(), element->GetDocument()->GetErrorStr2());
}

/*! Gets a boolean attribute
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	cannot convert attribute
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 * \param[in]	value	the variable where the value will be stored
 */
void Element::queryAttribute(const StringUTF8 &name, bool &value) const
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	int res = element->QueryBoolAttribute(conv->FromUTF8(name).c_str(), &value);
	xmlerror(res, element->GetDocument()->GetErrorStr1(), element->GetDocument()->GetErrorStr2());
}

/*! Gets a double attribute
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	cannot convert attribute
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 * \param[in]	value	the variable where the value will be stored
 */
void Element::queryAttribute(const StringUTF8 &name, double &value) const
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	int res = element->QueryDoubleAttribute(conv->FromUTF8(name).c_str(), &value);
	xmlerror(res, element->GetDocument()->GetErrorStr1(), element->GetDocument()->GetErrorStr2());
}

/*! Gets a float attribute
 * \throws	ExceptionInvalidArgument	empty attribute name
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	cannot convert attribute
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the attribute
 * \param[in]	value	the variable where the value will be stored
 */
void Element::queryAttribute(const StringUTF8 &name, float &value) const
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty attribute name."));
	int res = element->QueryFloatAttribute(conv->FromUTF8(name).c_str(), &value);
	xmlerror(res, element->GetDocument()->GetErrorStr1(), element->GetDocument()->GetErrorStr2());
}

///////////////////////////////////////////////////////////////////////////////
// Attribute
///////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \param[in]	a	the inner attribute handle
 * \param[in]	c	a character set converter
 */
Element::Attribute::Attribute(tinyxml2::XMLAttribute *a, const SCharsetConverter &c):
	attr(a),
	conv(c)
{ }

/*! Move to next attribute 
 * \return	the next attribute
 */
Element::Attribute& Element::Attribute::operator++()
{
	attr = const_cast<tinyxml2::XMLAttribute*>(attr->Next());
	return *this;
}

/*! Move to next attribute 
 * \return	the current attribute
 */
Element::Attribute Element::Attribute::operator++(int)
{
	Attribute bak(*this);
	attr = const_cast<tinyxml2::XMLAttribute*>(attr->Next());
	return bak;
}

/*! Gets next attribute
 * \return	the next attribute
 */
Element::Attribute Element::Attribute::Next()
{
	return Attribute(const_cast<tinyxml2::XMLAttribute*>(attr->Next()), conv);
}

/*! Gets the name of the attribute
 * \return the name of the attribute
 */
StringUTF8 Element::Attribute::GetName() const
{
	return conv->ToUTF8(attr->Name());
}

/*! Sets the value of the attribute
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	value	the new value
 */
void Element::Attribute::SetValue(const StringUTF8 &value)
{
	attr->SetAttribute(conv->FromUTF8(value).c_str());
}

/*! Gets text value
 * \param[in]	value	the variable that will hold the value
 */
void Element::Attribute::queryValue(StringUTF8 &value) const
{
	const char *val = attr->Value();
	value = conv->ToUTF8(val);
}

/*! Gets int value
 * \throws	ExceptionDomain	wrong attribute type
 * \param[in]	value	the variable that will hold the value
 */
void Element::Attribute::queryValue(int &value) const
{
	int res = attr->QueryIntValue(&value);
	xmlerror(res, NULL, NULL);
}

/*! Gets unsigned int value
 * \throws	ExceptionDomain	wrong attribute type
 * \param[in]	value	the variable that will hold the value
 */
void Element::Attribute::queryValue(unsigned int &value) const
{
	int res = attr->QueryUnsignedValue(&value);
	xmlerror(res, NULL, NULL);
}

/*! Gets boolean value
 * \throws	ExceptionDomain	wrong attribute type
 * \param[in]	value	the variable that will hold the value
 */
void Element::Attribute::queryValue(bool &value) const
{
	int res = attr->QueryBoolValue(&value);
	xmlerror(res, NULL, NULL);
}

/*! Gets double value
 * \throws	ExceptionDomain	wrong attribute type
 * \param[in]	value	the variable that will hold the value
 */
void Element::Attribute::queryValue(double &value) const
{
	int res = attr->QueryDoubleValue(&value);
	xmlerror(res, NULL, NULL);
}

/*! Gets float value
 * \throws	ExceptionDomain	wrong attribute type
 * \param[in]	value	the variable that will hold the value
 */
void Element::Attribute::queryValue(float &value) const
{
	int res = attr->QueryFloatValue(&value);
	xmlerror(res, NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Comment
///////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \param[in]	c	the inner node handle
 * \param[in]	co	a character set converter
 */
Comment::Comment(tinyxml2::XMLComment *c, const SCharsetConverter &co):
	Node(c, co),
	comment(c)
{ }

///////////////////////////////////////////////////////////////////////////////
// Text
///////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \param[in]	t	the inner node handle
 * \param[in]	c	a character set converter
 */
Text::Text(tinyxml2::XMLText *t, const SCharsetConverter &c):
	Node(t, c),
	text(t)
{ }

/*! Is the text a CData? 
 * \return	true is the text is stored as a CData
 */
bool Text::IsCData() const
{
	return text->CData();
}

/*! Gets the content of the node
 * \return	the content of the node
 */
StringUTF8 Text::GetValue() const
{
	if (text->CData())
		return text->Value();
	else
		return conv->ToUTF8(text->Value());
}

///////////////////////////////////////////////////////////////////////////////
// Document
///////////////////////////////////////////////////////////////////////////////

/*! Constructor
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	encoding	the character encoding (default: UTF-8)
 * \param[in]	version	the XML standard version (default: 1.0)
 * \param[in]	char_conversion_throws	shall exceptions be thrown when a character conversion error occurs? (default: true)
 */
Document::Document(const StringUTF8 &encoding, const StringUTF8 &version, bool char_conversion_throws):
	doc(std::make_unique<tinyxml2::XMLDocument>()),
	enc(encoding),
	ver(version),
	conv(std::make_shared<CharsetConverter>(encoding.Std(), true, char_conversion_throws))
{
	StringUTF8 decl = "xml version=\"" + version + "\" encoding=\"" + encoding + "\"";
	doc->InsertEndChild(doc->NewDeclaration(conv->FromUTF8(decl).c_str()));
}

/*! Constructor
 * \throws	ExceptionNotFound	file not found
 * \throws	ExceptionIO	cannot open file
 * \throws	ExceptionRuntime	error parsing the file
 * \param[in]	fname	the xml file name
 * \param[in]	char_conversion_throws	shall exceptions be thrown when a character conversion error occurs? (default: true)
 */
Document::Document(const Path &fname, bool char_conversion_throws):
	doc(std::make_unique<tinyxml2::XMLDocument>()),
	filename(fname)
{
	int res = doc->LoadFile(fname.CStr());
	if (res)
	{
		crn::StringUTF8 s1, s2;
		if (doc->GetErrorStr1())
			s1 = doc->GetErrorStr1();
		if (doc->GetErrorStr2())
			s2 = doc->GetErrorStr2();
		doc.reset();
		xmlerror(res, s1.IsEmpty() ? NULL : s1.CStr(), s2.IsEmpty() ? NULL : s2.CStr());
	}
	tinyxml2::XMLNode *node = doc->FirstChild();
	while (node)
	{
		tinyxml2::XMLDeclaration *decl = node->ToDeclaration();
		if (decl)
		{
			StringUTF8 val = decl->Value();
			StringUTF8 ekey = "encoding=";
			size_t beg = val.Find(ekey);
			if (beg == StringUTF8::NPos())
				enc = "UTF-8";
			else
			{
				beg += ekey.Size();
				char sep = val[beg];
				beg += 1;
				size_t en = val.Find(sep, beg);
				if (en == StringUTF8::NPos())
					enc = "UTF-8";
				else
					enc = val.SubString(beg, en - beg);
			}
			StringUTF8 vkey = "version=";
			beg = val.Find(vkey);
			if (beg == StringUTF8::NPos())
				ver = "1.0";
			else
			{
				beg += vkey.Size();
				char sep = val[beg];
				beg += 1;
				size_t en = val.Find(sep, beg);
				if (en == StringUTF8::NPos())
					ver = "1.0";
				else
					ver = val.SubString(beg, en - beg);
			}
		}
		node = node->NextSibling();
	}
	conv = std::make_shared<CharsetConverter>(enc.Std(), true, char_conversion_throws);
}

/*! Constructor from buffer
 * \param[in]	content	a C string to an XML text
 * \param[in]	char_conversion_throws	shall exceptions be thrown when a character conversion error occurs? (default: true)
 * \throws	ExceptionRuntime	the XML content is malformed
 */
Document::Document(const char *content, bool char_conversion_throws):
	doc(std::make_unique<tinyxml2::XMLDocument>()),
	enc("UTF-8"),
	ver("1.0"),
	conv(std::make_shared<CharsetConverter>("UTF-8", true, char_conversion_throws))
{
	int res = doc->Parse(content);
	xmlerror(res, doc->GetErrorStr1(), doc->GetErrorStr2());
}

/*! Destructor */
Document::~Document() = default;

Document::Document(Document&&) = default;
Document& Document::operator=(Document&&) = default;

/*! Saves the XML to a file
 * \throws	ExceptionNotFound	file not found
 * \throws	ExceptionIO	cannot open file
 * \throws	ExceptionRuntime	error creating the file
 * \param[in]	fname	the xml file name
 */
void Document::Save(const Path &fname)
{
	int res = doc->SaveFile(fname.CStr());
	xmlerror(res, doc->GetErrorStr1(), doc->GetErrorStr2());
	filename = fname;
}

/*! Saves the XML to a file
 * \throws	ExceptionUninitialized	emtpy filename
 * \throws	ExceptionNotFound	file not found
 * \throws	ExceptionIO	cannot open file
 * \throws	ExceptionRuntime	error creating the file
 */
void Document::Save()
{
	if (filename.IsEmpty())
		throw ExceptionUninitialized(_("Empty filename."));
	int res = doc->SaveFile(filename.CStr());
	xmlerror(res, doc->GetErrorStr1(), doc->GetErrorStr2());
}

/*! Gets the first element
 * \throws	ExceptionNotFound	no element in the xml
 * \return	a handle on the first element in the document
 */
Element Document::GetRoot()
{
	Element el(BeginElement());
	if (el == EndElement())
		throw ExceptionNotFound(_("No element in the XML."));
	return el;
}

/*! Gets the first node
 * \return	the first node or null node
 */
Node Document::GetFirstNode()
{
	return Node(doc->FirstChild(), conv);
}

/*! Gets the last node
 * \return	the last child node or null node
 */
Node Document::GetLastNode()
{
	return Node(doc->LastChild(), conv);
}

/*! Gets the first element
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the element or empty string for any element
 * \return	the first element or null element
 */
Element Document::GetFirstElement(const StringUTF8 &name)
{
	return Element(doc->FirstChildElement(name.IsEmpty() ? nullptr : conv->FromUTF8(name).c_str()), conv);
}

/*! Gets the last element
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the element or empty string for any element
 * \return	the last element or null element
 */
Element Document::GetLastElement(const StringUTF8 &name)
{
	return Element(doc->LastChildElement(name.IsEmpty() ? nullptr : conv->FromUTF8(name).c_str()), conv);
}

/*! Adds an element at the end of the document
 * \throws	ExceptionInvalidArgument	empty element name
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	name	the name of the element
 * \return	a handle on the newly created element
 */
Element Document::PushBackElement(const StringUTF8 &name)
{
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty element name."));
	tinyxml2::XMLElement *el = doc->NewElement(conv->FromUTF8(name).c_str());
	doc->InsertEndChild(el);
	return Element(el, conv);
}

/*! Inserts an element after a node
 * \throws	ExceptionInvalidArgument	null node or empty element name
 * \throws	ExceptionNotFound	node not found
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	n	the node after which the new element will be inserted
 * \param[in]	name	the name of the element
 * \return	a handle on the newly created element
 */
Element Document::InsertElement(Node &n, const StringUTF8 &name)
{
	if (!n)
		throw ExceptionInvalidArgument(_("Null node."));
	if (name.IsEmpty())
		throw ExceptionInvalidArgument(_("Empty element name."));
	tinyxml2::XMLElement *el = doc->NewElement(conv->FromUTF8(name).c_str());
	if (!doc->InsertAfterChild(n.node, el))
	{
		doc->GetDocument()->DeleteNode(el);
		throw ExceptionNotFound(_("Node not found."));
	}
	return Element(el, conv);
}

/*! Adds a comment at the end of the document
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in] text	the content of the comment
 * \return	a handle on the newly created comment
 */
Comment Document::PushBackComment(const StringUTF8 &text)
{
	tinyxml2::XMLComment *c = doc->NewComment(conv->FromUTF8(text).c_str());
	doc->InsertEndChild(c);
	return Comment(c, conv);
}

/*! Inserts a comment after a node
 * \throws	ExceptionNotFound	node not found
 * \throws	CharsetConverter::ExceptionInvalidCharacter	invalid character
 * \throws	CharsetConverter::ExceptionIncompleteCode	incomplete multibyte character
 * \param[in]	n	the node after which the new comment will be inserted
 * \param[in]	text	the content of the comment
 * \return	a handle on the newly created comment
 */
Comment Document::InsertComment(Node &n, const StringUTF8 &text)
{
	if (!n)
		throw ExceptionInvalidArgument(_("Null node."));
	tinyxml2::XMLComment *c = doc->NewComment(conv->FromUTF8(text).c_str());
	if (!doc->InsertAfterChild(n.node, c))
	{
		doc->GetDocument()->DeleteNode(c);
		throw ExceptionNotFound(_("Node not found."));
	}
	return Comment(c, conv);
}

/*! Adds a copy of a node at the end of the children list
 * \throws	ExceptionInvalidArgument	null or invalid node
 * \param[in]	n	the node to copy
 * \param[in]	recursive	shall the subelements be copied too? (only if n is an Element)
 * \return	a handle on the newly created node
 */
Node Document::PushBackClone(Node &n, bool recursive)
{
	if (!n)
		throw ExceptionInvalidArgument(_("Null node."));
	// element?
	try
	{
		Element el(n.AsElement());
		Element newel(PushBackElement(el.GetName()));
		for (Element::Attribute a = el.BeginAttribute(); a != el.EndAttribute(); ++a)
			newel.SetAttribute(a.GetName(), a.GetValue<StringUTF8>());
		if (recursive)
		{
			for (Node sn = el.BeginNode(); sn != el.EndNode(); ++sn)
				newel.PushBackClone(sn, recursive);
		}
		return newel;
	}
	catch (ExceptionDomain &) { }
	// comment?
	try
	{
		Comment c(n.AsComment());
		return PushBackComment(c.GetValue());
	}
	catch (ExceptionDomain &) { }
	// text?
	try
	{
		Text t(n.AsText());
		tinyxml2::XMLText *tx = doc->NewText(t.IsCData() ? t.GetValue().CStr() : conv->FromUTF8(t.GetValue()).c_str());
		tx->SetCData(t.IsCData());
		doc->InsertEndChild(tx);
		return Text(tx, conv);
	}
	catch (ExceptionDomain &) { }
	// declaration?
	tinyxml2::XMLDeclaration *de = n.node->ToDeclaration();
	if (de)
	{
		tinyxml2::XMLDeclaration *nde = doc->NewDeclaration(de->Value());
		doc->InsertEndChild(nde);
		return Node(nde, conv);
	}
	// unknown?
	tinyxml2::XMLUnknown *un = n.node->ToUnknown();
	if (un)
	{
		tinyxml2::XMLUnknown *nun = doc->NewUnknown(un->Value());
		doc->InsertEndChild(nun);
		return Node(nun, conv);
	}
	throw ExceptionInvalidArgument(_("Invalid node."));
}

/*! Exports the document to a string 
 * \return	a string containing the document
 */
StringUTF8 Document::AsString()
{
	tinyxml2::XMLPrinter pr;
	doc->Print(&pr);
	return pr.CStr();
}

