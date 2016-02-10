/* Copyright 2008-2015 INSA Lyon, CoReNum
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
 * file: CRNDataFactory.h
 * \author Yann LEYDIER
 */

#ifndef CRNDATAFACTORY_HEADER
#define CRNDATAFACTORY_HEADER

#include <CRNObject.h>
#include <CRNUtils/CRNXml.h>
#include <map>

#ifdef RegisterClass
#	undef RegisterClass
#endif

namespace crn
{
	class String;

	/*! \brief	Base class for factory elements */
	class DataFactoryElementBase
	{
		public:
			virtual ~DataFactoryElementBase() {}
			/*! \brief	Creates an object */
			virtual UObject Create(xml::Element &el) const = 0;
	};
	/*! \brief	Factory element */
	template<class T> class DataFactoryElement: public DataFactoryElementBase
	{
		public:
			virtual ~DataFactoryElement() override {}
			/*! \brief	Creates an object */
			virtual UObject Create(xml::Element &el) const override { return std::make_unique<T>(el); }
	};
	/****************************************************************************/
	/*! \brief Produces CRNData objects from XML
	 *
	 * A factory to produce CRNData objects from an XML element
	 *
	 * \author 	Yann LEYDIER
	 * \date		13 October 2008
	 * \version 0.3
	 * \ingroup	data
	 */
	class DataFactory
	{
		public:
			/*! \brief Creates and initializes a SObject from an XML element */
			static UObject CreateData(xml::Element &el);
			/*! \brief Registers a class in the factory */
			static bool RegisterClass(const String &name, std::unique_ptr<DataFactoryElementBase> &&cstr);
			/*! \brief Returns the list of registered classes */
			static String GetKnownTypes();
			/*! \brief Destructor */
			~DataFactory();

		private:
			/*! \brief Default constructor */
			DataFactory();
			/*! \brief Disable copy constructor */
			DataFactory(const DataFactory &);
			/*! \brief Disable copy operator */
			DataFactory& operator=(const DataFactory &);
			/*! \brief Creates a SObject from class name */
			UObject createObject(const String &name, xml::Element &el);
			/*! \brief Returns singleton's instance */
			static DataFactory& getInstance();

			std::map<String, std::unique_ptr<DataFactoryElementBase> > data; /*!< The list of name/constructor for classes */

	};
}

/*! \brief Registers a class to the data factory.
 *
 * Registers a class to the data factory. Use this in class constructor.
 *
 * \param[in]	elemname	the name/value of the xml element
 * \param[in]	classname	the class to register
 * \ingroup	data
 */
#define CRN_DATA_FACTORY_REGISTER(elemname, classname) crn::DataFactory::RegisterClass(elemname, std::make_unique<crn::DataFactoryElement<classname>>());

#endif
