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
 * file: CRNXml.h
 * \author Yann LEYDIER
 */

#ifndef CRNXml_HEADER
#define CRNXml_HEADER

#include <CRNIO/CRNPath.h>
#include <CRNUtils/CRNCharsetConverter.h>

namespace tinyxml2
{
	class XMLDocument;
	class XMLNode;
	class XMLComment;
	class XMLElement;
	class XMLText;
	class XMLAttribute;
}
namespace crn
{
	/*! \brief Wrapper on TinyXml2
	 *
	 * Wrapper on TinyXml2
	 *
	 * \author	Yann LEYDIER
	 * \date	August 2012
	 * \ingroup	utils
	 * \defgroup	xml	XML file handling
	 */
	namespace xml
	{
		class Element;
		class Comment;
		class Text;
		/*! \brief XML node
		 *
		 * An XML node
		 * \version 1.0
		 * \author Yann LEYDIER
		 * \date	August 2012
		 * \ingroup	xml
		 */
		class Node
		{
			public:
				/*! \brief Destructor */
				virtual ~Node() {}
				Node(const Node&) = default;
				Node(Node&&) = default;
				Node& operator=(const Node&) = default;
				Node& operator=(Node&&) = default;

				/*! \brief Checks if the node is an element */
				bool IsElement();
				/*! \brief Checks if the node is a comment */
				bool IsComment();
				/*! \brief Checks if the node is a text */
				bool IsText();

				/*! \brief Converts to element */
				Element AsElement();
				/*! \brief Converts to comment */
				Comment AsComment();
				/*! \brief Converts to text */
				Text AsText();

				/*! \brief Checks if the node is not null */
				operator bool() const noexcept { return node != nullptr; }
				/*! \brief Checks if the node is null */
				bool operator!() const noexcept { return node == nullptr; }
				/*! \brief Comparison operator */
				bool operator==(const Node &other) const noexcept { return node == other.node; }
				/*! \brief Comparison operator */
				bool operator!=(const Node &other) const noexcept { return !(*this == other); }

				/*! \brief Gets the content of the node */
				virtual StringUTF8 GetValue() const;
				/*! \brief Sets the content of the node */
				void SetValue(const StringUTF8 &s);

				/*! \brief Gets the parent node if any */
				Node GetParent();

				/*! \brief Gets the previous sibling node */
				Node GetPreviousSibling();
				/*! \brief Gets the next sibling node */
				Node GetNextSibling();
				/*! \brief Moves to the next sibling node */
				Node& operator++();
				/*! \brief Moves to the next sibling node */
				Node operator++(int);

				/*! \brief Gets the next sibling element */
				Element GetNextSiblingElement(const StringUTF8 &name = "");
				/*! \brief Gets the previous sibling element */
				Element GetPreviousSiblingElement(const StringUTF8 &name = "");

			protected:
				/*! \brief Constructor */
				Node(tinyxml2::XMLNode *n, const SCharsetConverter &c);
				SCharsetConverter conv;

			private:
				tinyxml2::XMLNode *node;

			friend class Document;
			friend class Element;
		};

		/*! \brief XML element
		 *
		 * An XML element
		 * \version 1.0
		 * \author Yann LEYDIER
		 * \date	August 2012
		 * \ingroup	xml
		 */
		class Element: public Node
		{
			public:
				/*! \brief Destructor */
				virtual ~Element() override {}
				Element(const Element&) = default;
				Element(Element&&) = default;
				Element& operator=(const Element&) = default;
				Element& operator=(Element&&) = default;

				/*! \brief Gets the label of the element */
				StringUTF8 GetName() const { return GetValue(); }
				/*! \brief Sets the label of the element */
				void SetName(const StringUTF8 &s) { SetValue(s); }

				/*! \brief Moves to the next sibling element */
				Element& operator++();
				/*! \brief Moves to the next sibling element */
				Element operator++(int);

				// Children
				/*! \brief Returns the number of sub-nodes */
				size_t GetNbSubnodes() const { return nb_subnodes; }
				/*! \brief Returns the number of sub-elements */
				size_t GetNbSubelements() const { return nb_subelems; }
				/*! \brief Gets the first child node */
				Node GetFirstChild();
				/*! \brief Gets the last child node */
				Node GetLastChild();
				/*! \brief Gets the first child node */
				Node BeginNode() { return GetFirstChild(); }
				/*! \brief Gets a null node */
				Node EndNode() { return Node(nullptr, conv); }

				/*! \brief Gets the first child element */
				Element GetFirstChildElement(const StringUTF8 &name = "");
				/*! \brief Gets the last child element */
				Element GetLastChildElement(const StringUTF8 &name = "");
				/*! \brief Gets the first child element */
				Element BeginElement() { return GetFirstChildElement(); }
				/*! \brief Gets a null node */
				Element EndElement() { return Element(nullptr, conv); }

				/*! \brief Gets the first child as text */
				StringUTF8 GetFirstChildText();

				/*! \brief Adds an element at the end of the children list */
				Element PushBackElement(const StringUTF8 &name);
				/*! \brief Adds an element at the front of the children list */
				Element PushFrontElement(const StringUTF8 &name);
				/*! \brief Inserts an element after a node */
				Element InsertElement(Node &n, const StringUTF8 &name);

				/*! \brief Adds a comment at the end of the children list */
				Comment PushBackComment(const StringUTF8 &text);
				/*! \brief Adds a comment at the front of the children list */
				Comment PushFrontComment(const StringUTF8 &text);
				/*! \brief Inserts a comment after a node */
				Comment InsertComment(Node &n, const StringUTF8 &text);

				/*! \brief Adds a text at the end of the children list */
				Text PushBackText(const StringUTF8 &text, bool cdata = false);
				/*! \brief Adds a text at the front of the children list */
				Text PushFrontText(const StringUTF8 &text, bool cdata = false);
				/*! \brief Inserts a text after a node */
				Text InsertText(Node &n, const StringUTF8 &text, bool cdata = false);

				/*! \brief Adds a copy of a node at the end of the children list */
				Node PushBackClone(Node &n, bool recursive = false);

				/*! \brief Removes all children */
				void Clear();
				/*! \brief Removes a child node */
				void RemoveChild(Node &n);

				// Attributes
				/*! \brief Gets an attribute
				 * \throws	ExceptionInvalidArgument	empty attribute name
				 * \throws	ExceptionNotFound	attribute not found
				 * \throws	ExceptionDomain	cannot convert attribute
				 * \param[in]	name	the name of the attribute
				 * \param[in]	silent	shall exceptions be dropped?
				 * \return	the attribute's value (or 0 if silent and not found or cannot convert)
				 */
				template<typename T> T GetAttribute(const StringUTF8 &name, bool silent = true) const
				{
					T attr;
					if (silent)
					{
						try { queryAttribute(name, attr); }
						catch (...) { return T(0); }
					}
					else
						queryAttribute(name, attr);
					return attr;
				}
				/*! \brief Sets the value of an attribute */
				void SetAttribute(const StringUTF8 &name, const StringUTF8 &value);
				/*! \brief Removes an attribute */
				void RemoveAttribute(const StringUTF8 &name);

				class Attribute
				{
					public:
						Attribute(const Attribute&) = default;
						Attribute(Attribute&&) = default;
						Attribute& operator=(const Attribute&) = default;
						Attribute& operator=(Attribute&&) = default;
						/*! \brief Destructor */
						~Attribute() {}

						/*! \brief Move to next attribute */
						Attribute& operator++();
						/*! \brief Move to next attribute */
						Attribute operator++(int);
						/*! \brief Gets next attribute */
						Attribute Next();

						/*! \brief Comparison operator */
						bool operator==(const Attribute &other) const noexcept { return attr == other.attr; }
						/*! \brief Comparison operator */
						bool operator!=(const Attribute &other) const noexcept { return !(*this == other); }

						/*! \brief Gets the name of the attribute */
						StringUTF8 GetName() const;
						/*! \brief Gets the value of the attribute
						 * \throws	ExceptionInvalidArgument	empty attribute name
						 * \throws	ExceptionNotFound	attribute not found
						 * \throws	ExceptionDomain	cannot convert attribute
						 * \param[in]	silent	shall exceptions be dropped?
						 * \return	the attribute's value (or 0 if silent and not found or cannot convert)
						 */
						template<typename T> T GetValue(bool silent = true) const
						{
							T val;
							if (silent)
							{
								try { queryValue(val); }
								catch (...) { return T(0); }
							}
							else
								queryValue(val);
							return val;
						}
						/*! \brief Sets the value of the attribute */
						void SetValue(const StringUTF8 &value);

					private:
						/*! \brief Constructor */
						Attribute(tinyxml2::XMLAttribute *a, const SCharsetConverter &c);

						/*! \brief Gets text value */
						void queryValue(StringUTF8 &value) const;
						/*! \brief Gets int value */
						void queryValue(int &value) const;
						/*! \brief Gets unsigned int value */
						void queryValue(unsigned int &value) const;
						/*! \brief Gets boolean value */
						void queryValue(bool &value) const;
						/*! \brief Gets double value */
						void queryValue(double &value) const;
						/*! \brief Gets float value */
						void queryValue(float &value) const;

						tinyxml2::XMLAttribute *attr;
						SCharsetConverter conv;

					friend class Element;
				};
				/*! \brief Gets the first attribute */
				Attribute BeginAttribute();
				/*! \brief Gets the null attribute */
				Attribute EndAttribute();

			protected:
				/*! \brief Constructor */
				Element(tinyxml2::XMLElement *el, const SCharsetConverter &c);

			private:
				/*! \brief Gets a text attribute */
				void queryAttribute(const StringUTF8 &name, StringUTF8 &value) const;
				/*! \brief Gets an int attribute */
				void queryAttribute(const StringUTF8 &name, int &value) const;
				/*! \brief Gets an unsigned int attribute */
				void queryAttribute(const StringUTF8 &name, unsigned int &value) const;
				/*! \brief Gets a boolean attribute */
				void queryAttribute(const StringUTF8 &name, bool &value) const;
				/*! \brief Gets a double attribute */
				void queryAttribute(const StringUTF8 &name, double &value) const;
				/*! \brief Gets a float attribute */
				void queryAttribute(const StringUTF8 &name, float &value) const;
				/*! \brief Updated the count cache */
				void count_subnondes();

				tinyxml2::XMLElement *element;
				size_t nb_subnodes, nb_subelems;

			friend class Node;
			friend class Document;
		};
		/*! \brief Gets an attribute
		 * \throws	ExceptionInvalidArgument	empty attribute name
		 * \throws	ExceptionNotFound	attribute not found
		 * \throws	ExceptionDomain	cannot convert attribute
		 * \param[in]	name	the name of the attribute
		 * \param[in]	silent	shall exceptions be dropped?
		 * \return	the attribute's value (or 0 if silent and not found or cannot convert)
		 */
		template<> inline StringUTF8 Element::GetAttribute<StringUTF8>(const StringUTF8 &name, bool silent) const
		{
			StringUTF8 attr;
			if (silent)
			{
				try { queryAttribute(name, attr); }
				catch (...) { return StringUTF8(); }
			}
			else
				queryAttribute(name, attr);
			return attr;
		}

		/*! \brief XML comment
		 *
		 * An XML comment
		 * \version 1.0
		 * \author Yann LEYDIER
		 * \date	August 2012
		 * \ingroup	xml
		 */
		class Comment: public Node
		{
			public:
				Comment(const Comment&) = default;
				Comment(Comment&&) = default;
				Comment& operator=(const Comment&) = default;
				Comment& operator=(Comment&&) = default;
				/*! \brief Destructor */
				virtual ~Comment() override {}

			protected:
				/*! \brief Constructor */
				Comment(tinyxml2::XMLComment *c, const SCharsetConverter &co);

			private:
				tinyxml2::XMLComment *comment;

			friend class Element;
			friend class Node;
			friend class Document;
		};

		/*! \brief XML text
		 *
		 * An XML text
		 * \version 1.0
		 * \author Yann LEYDIER
		 * \date	August 2012
		 * \ingroup	xml
		 */
		class Text: public Node
		{
			public:
				Text(const Text&) = default;
				Text(Text&&) = default;
				Text& operator=(const Text&) = default;
				Text& operator=(Text&&) = default;
				/*! \brief Destructor */
				virtual ~Text() override {}

				/*! \brief Is the text a CData? */
				bool IsCData() const;
				
				/*! \brief Gets the content of the node */
				virtual StringUTF8 GetValue() const override;
			protected:
				/*! \brief Constructor */
				Text(tinyxml2::XMLText *t, const SCharsetConverter &c);

			private:
				tinyxml2::XMLText *text;

			friend class Element;
			friend class Node;
			friend class Document;
		};

		/*! \brief XML document
		 *
		 * An XML document
		 * \version 1.0
		 * \author Yann LEYDIER
		 * \date August 2012
		 * \ingroup	xml
		 */
		class Document
		{
			public:
				/*! \brief Constructor */
				Document(const StringUTF8 &encoding = "UTF-8", const StringUTF8 &version = "1.0", bool char_conversion_throws = true);
				/*! \brief Constructor from file */
				Document(const Path &fname, bool char_conversion_throws = true);
				/*! \brief Constructor from buffer */
				Document(const char *content, bool char_conversion_throws = true);
				Document(const Document&) = delete;
				Document(Document&&);
				Document& operator=(const Document&) = delete;
				Document& operator=(Document&&);
				/*! \brief Destructor */
				virtual ~Document();

				/*! \brief Checks if the document is open */
				operator bool() const noexcept { return doc != nullptr; }
				/*! \brief Checks if the document is not open */
				bool operator!() const noexcept { return doc == nullptr; }

				/*! \brief Saves to file */
				void Save(const Path &fname);
				/*! \brief Saves to file */
				void Save();
				/*! \brief Gets the filename if the document exists on the disk */
				const Path& GetFilename() const noexcept { return filename; }

				/*! \brief Gets the character encoding of the file */
				const StringUTF8& GetEncoding() const noexcept { return enc; }
				/*! \brief Gets the XML version of the file */
				const StringUTF8& GetVersion() const noexcept { return ver; }

				/*! \brief Gets the first element */
				Element GetRoot();

				/*! \brief Gets the first child node */
				Node GetFirstNode();
				/*! \brief Gets the last child node */
				Node GetLastNode();
				/*! \brief Gets the first child node */
				Node BeginNode() { return GetFirstNode(); }
				/*! \brief Gets a null node */
				Node EndNode() { return Node(nullptr, conv); }

				/*! \brief Gets the first child element */
				Element GetFirstElement(const StringUTF8 &name = "");
				/*! \brief Gets the last child element */
				Element GetLastElement(const StringUTF8 &name = "");
				/*! \brief Gets the first child element */
				Element BeginElement() { return GetFirstElement(); }
				/*! \brief Gets a null node */
				Element EndElement() { return Element(nullptr, conv); }

				/*! \brief Adds an element at the end of the children list */
				Element PushBackElement(const StringUTF8 &name);
				/*! \brief Inserts an element after a node */
				Element InsertElement(Node &n, const StringUTF8 &name);

				/*! \brief Adds a comment at the end of the children list */
				Comment PushBackComment(const StringUTF8 &text);
				/*! \brief Inserts a comment after a node */
				Comment InsertComment(Node &n, const StringUTF8 &text);

				/*! \brief Adds a copy of a node at the end of the children list */
				Node PushBackClone(Node &n, bool recursive = false);

				/*! \brief Exports the document to a string */
				StringUTF8 AsString();

			private:
				std::unique_ptr<tinyxml2::XMLDocument> doc;
				StringUTF8 enc, ver;
				SCharsetConverter conv;
				Path filename;
		};

	}
}


#endif


