/* Copyright 2011-2016 CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNPDFAttributes.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNPDFAttributes.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNException.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;
using namespace crn::PDF;

/*!
 * \throws	ExceptionInvalidArgument	not a PDF::Attributes
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 */
void Attributes::Deserialize(xml::Element &el)
{
	if (el.GetName() != "PDFAttributes")
	{
		throw ExceptionInvalidArgument(StringUTF8("void PDFAttributes::Deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	bool lc = el.GetAttribute<bool>("lossy_compression", false); // may throw
	int jq = el.GetAttribute<int>("jpeg_qual", false); // may throw
	int l = el.GetAttribute<int>("layout", false); // may throw
	StringUTF8 a = el.GetAttribute<StringUTF8>("author", false); // may throw
	StringUTF8 c = el.GetAttribute<StringUTF8>("creator", false); // may throw
	StringUTF8 t = el.GetAttribute<StringUTF8>("title", false); // may throw
	StringUTF8 s = el.GetAttribute<StringUTF8>("subject", false); // may throw
	StringUTF8 k = el.GetAttribute<StringUTF8>("keywords", false); // may throw
	StringUTF8 op = el.GetAttribute<StringUTF8>("owner_password", false); // may throw
	StringUTF8 up = el.GetAttribute<StringUTF8>("user_password", false); // may throw
	bool p = el.GetAttribute<bool>("printable", false); // may throw
	bool cop = el.GetAttribute<bool>("copyable", false); // may throw
	lossy_compression = lc;
	jpeg_qual = jq;
	layout = Layout(l);
	author = a;
	creator = c;
	title = t;
	subject = s;
	keywords = k;
	owner_password = op;
	user_password = up;
	printable = p;
	copyable = cop;
}

xml::Element Attributes::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("PDFAttributes"));
	el.SetAttribute("lossy_compression", lossy_compression ? 1 : 0);
	el.SetAttribute("jpeg_qual", jpeg_qual);
	el.SetAttribute("layout", int(layout));
	el.SetAttribute("author", author);
	el.SetAttribute("creator", creator);
	el.SetAttribute("title", title);
	el.SetAttribute("subject", subject);
	el.SetAttribute("keywords", keywords);
	el.SetAttribute("owner_password", owner_password);
	el.SetAttribute("user_password", user_password);
	el.SetAttribute("printable", printable ? 1 : 0);
	el.SetAttribute("copyable", copyable ? 1 : 0);
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Attributes)
	CRN_DATA_FACTORY_REGISTER(U"PDFAttributes", Attributes)
	Cloner::Register<Attributes>();
CRN_END_CLASS_CONSTRUCTOR(Attributes)


