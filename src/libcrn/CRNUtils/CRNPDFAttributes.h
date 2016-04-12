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
 * file: CRNPDFAttributes.h
 * \author Yann LEYDIER
 */

#ifndef CRNPDFAttributes_HEADER
#define CRNPDFAttributes_HEADER

#include <CRNStringUTF8.h>
#include <CRNString.h>

namespace crn
{
	namespace PDF
	{
		/*! \brief Attributes to create a PDF export of a document.
		 *
		 * Attributes to create a PDF export of a document.
		 * \author	Yann LEYDIER
		 * \version	0.2
		 * \date	Feb. 2011
		 * \ingroup	PDF
		 */
		class Attributes: public Object
		{
			public:
				enum class Layout : int { ONE_PAGE = 1, CONTINUOUS = 2, OPEN_LEFT = 3, OPEN_RIGHT  = 4 };
				/*! \brief Default constructor */
				Attributes():lossy_compression(true),jpeg_qual(80),layout(Layout::ONE_PAGE),creator("libcrn"),printable(true),copyable(true) {}
				Attributes(const Attributes&) = default;
				Attributes(Attributes&&) = default;
				Attributes& operator=(const Attributes&) = default;
				Attributes& operator=(Attributes&&) = default;
				virtual ~Attributes() override {}

				bool lossy_compression; /*!< true = JPEG images, false = PNG images */
				int jpeg_qual; /*!< 0: high compression / bad quality, 100: low compression / better quality */
				Layout layout; /*!< Page display layout */
				StringUTF8 author, creator, title, subject, keywords;
				StringUTF8 owner_password, user_password;
				bool printable; /*!< Can the document be printed? (needs owner_password to be set) */
				bool copyable; /*!< Can the document be copy-pasted? (needs owner_password to be set) */

				void Deserialize(xml::Element &el);
				xml::Element Serialize(xml::Element &parent) const;
			
			private:
			CRN_DECLARE_CLASS_CONSTRUCTOR(Attributes)
			CRN_SERIALIZATION_CONSTRUCTOR(Attributes)
		};
		CRN_ALIAS_SMART_PTR(Attributes)
	}
	template<> struct IsSerializable<PDF::Attributes> : public std::true_type {};
	template<> struct IsClonable<PDF::Attributes> : public std::true_type {};

}

#endif


