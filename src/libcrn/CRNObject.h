/* Copyright 2006-2014 Yann LEYDIER, CoReNum, INSA-Lyon
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
 * file: CRNObject.h
 * \author Yann LEYDIER
 */

#ifndef CRNOBJECT_HEADER
#define CRNOBJECT_HEADER

#include <CRN.h>
#include <vector>
#include <CRNProtocols.h>

namespace crn
{
	class Object;
	CRN_ALIAS_SMART_PTR(Object)
}
namespace crn
{
	class String;
	class Prop3;
	namespace xml
	{
		class Element;
	}

	/*! \defgroup base Base classes */
	
	/****************************************************************************/
	/*! \brief base abstract class
	 *
	 * The base abstract class from which all classes must inherit.
	 * 
	 * All derived classes must define:
	 * 
	 * 	- virtual const crn::String& GetClassName() const
	 * 	- virtual unsigned int GetClassProtocols() const
	 *
	 * This class offers several protocols. If a protocol's
	 * method is called without having been implemented, an exception will be
	 * thrown. The overloadable methods are the following (in parenthesis, the
	 * corresponding flag in CRNProtocols.h):
	 * 
	 * 	- Clonable (crn::protocol::Clonable)
	 * 		- SObject Clone() const
	 * 	- Serializable (crn::protocol::Serializable)
	 * 		- void deserialize(xml::Element &el)
	 * 		- xml::Element serialize(xml::Element &parent) const
	 * 	- Metric (crn::protocol::Metric)
	 * 		- double distance(const Object &obj) const
	 * 	- Magma (crn::protocol::Magma)
	 * 		- void add(const Object &v)
	 * 		- bool equals(const Object &v) const
	 * 	- Group (crn::protocol::Group)
	 * 		- void sub(const Object &v)
	 * 	- Vector over IR (crn::protocol::VectorOverR)
	 * 		- void Mult(double m)
	 * 		- SObject sum(const vector&lt;pair&lt;const Object*, double&gt; &gt; &amp;plist) const
	 * 		- SObject mean(const vector&lt;pair&lt;const Object*, double&gt; &gt; &amp;plist) const
	 * 	- Ring (crn::protocol::Ring)
	 * 		- void mult(const Object &obj)
	 * 		- If the object is also a vector over IR, add "using Object::Mult;" before overloading Mult.
	 * 	- Field (crn::protocol::Field)
	 * 		- void div(const Object &obj)
	 * 	- POSet (crn::protocol::POSet)
	 * 		- Prop3 ge(const Object &l)
	 * 		- Prop3 le(const Object &l)
	 *
	 * \author 	Yann LEYDIER
	 * \date		February 2007
	 * \version 0.4
	 * \ingroup base
	 */
	class Object
	{
		/**************************************************************************/
		/* Object interface                                                       */
		/**************************************************************************/
		public:
			virtual ~Object() {}

			/*! \brief Returns the name of the class */
			virtual const String& GetClassName() const = 0;
			/*! \brief Returns the id of the class */
			virtual Protocol GetClassProtocols() const noexcept = 0;
			/*! \brief Checks if a protocol is implemented */
			bool Implements(Protocol classid) const noexcept { if ((GetClassProtocols() & classid) == classid) return true; else return false; }

			/*! \brief Converts object to string */
			virtual String ToString() const;

		/**************************************************************************/
		/* Clonable protocol                                                      */
		/**************************************************************************/
		public:
			/*!\brief Creates a new object, copied from this */
			virtual UObject Clone() const;
			template<typename T> inline std::unique_ptr<T> CloneAs() const { return std::unique_ptr<T>(dynamic_cast<T*>(Clone().release())); }

		/**************************************************************************/
		/* Serializable protocol                                                  */
		/**************************************************************************/
		public:
			/*! \brief Initializes the object from an XML element. Safe. */
			void Deserialize(xml::Element &el);
			/*! \brief Dumps the object to an XML element. Safe. */
			xml::Element Serialize(xml::Element &parent) const;
		private:
			/*! \brief Initializes the object from an XML element. Unsafe. */
			virtual void deserialize(xml::Element &el);
			/*! \brief Dumps the object to an XML element. Unsafe. */
			virtual xml::Element serialize(xml::Element &parent) const;
			/*! \brief Initializes some internal data from an XML element. */
			virtual void deserialize_internal_data(xml::Element &el);
			/*! \brief Dumps some internal data to an XML element. */
			virtual void serialize_internal_data(xml::Element &el) const;

		/**************************************************************************/
		/* Metric protocol                                                        */
		/**************************************************************************/
		public:
			/*! \brief Distance between two metric objects. Secure method. */
			double Distance(const Object &obj) const;
		private:
			/*! \brief Distance between two metric objects. Insecure method to be provided. */
			virtual double distance(const Object &obj) const;
			
		/**************************************************************************/
		/* Magma protocol                                                         */
		/**************************************************************************/
		public:
			/*! \brief Safe method to add an object */
			void Add(const Object &v);
			/*! \brief Safe method to test the equality of two vectors. */
			bool Equals(const Object &v) const;
		private:
			/*! \brief Unsafe method to add an object. */
			virtual void add(const Object &v);
			/*! \brief Unsafe method to test the equality of two vectors. */
			virtual bool equals(const Object &v) const;
				
		/**************************************************************************/
		/* Group protocol                                                         */
		/**************************************************************************/
		public:
			/*! \brief Safe method to subtract an object */
			void Sub(const Object &v);
		private:
			/*! \brief Unsafe method to subtract an object. */
			virtual void sub(const Object &v);
				
		/**************************************************************************/
		/* Vector over IR protocol                                                */
		/**************************************************************************/
		public:	
			/*! \brief Multiply by a real */
			virtual void Mult(double m);
			/*! \brief Safe method to compute a sum of vectors. */
			UObject Sum(const std::vector<std::pair<const Object*, double> > &plist) const;
			/*! \brief Sage method to compute a mean of vectors. */
			UObject Mean(const std::vector<std::pair<const Object*, double> > &plist) const;
		private:
			/*! \brief Unsafe method to compute a sum of vectors. */
			virtual UObject sum(const std::vector<std::pair<const Object*, double> > &plist) const;
			/*! \brief Unsafe Method to compute a mean of vectors. */
			virtual UObject mean(const std::vector<std::pair<const Object*, double> > &plist) const;

		/**************************************************************************/
		/* Ring protocol                                                          */
		/**************************************************************************/
		public:
			/*! \brief Internal product. SAFE */
			void Mult(const Object &obj);
		private:
			/*! \brief Internal product. UNSAFE */
			virtual void mult(const Object &obj);

		/**************************************************************************/
		/* Field protocol                                                         */
		/**************************************************************************/
		public:
			/*! \brief Internal division. SAFE */
			void Div(const Object &obj);
		private:
			/*! \brief Internal division. UNSAFE */
			virtual void div(const Object &obj);

		/**************************************************************************/
		/* POSet protocol                                                         */
		/**************************************************************************/
		public:
			/*! \brief SAFE Greater or Equal */
			Prop3 GE(const Object &l) const;
			/*! \brief SAFE Lower or Equal */
			Prop3 LE(const Object &l) const;
			/*! \brief Greater Than */
			Prop3 GT(const Object &l) const;
			/*! \brief Lower Than */
			Prop3 LT(const Object &l) const;
		private:
			/*! \brief UNSAFE Greater or Equal */
			virtual Prop3 ge(const Object &l) const;
			/*! \brief UNSAFE Lower or Equal */
			virtual Prop3 le(const Object &l) const;
			
	};

}

/*! \brief	Defines a default constructor from xml element 
 * \param[in]	classname	the class in which the constructor is added
 * \ingroup	base
 */
#define CRN_SERIALIZATION_CONSTRUCTOR(classname) public: classname(crn::xml::Element &el) { Deserialize(el); }

/*! \brief Declares a class constructor
 *
 * Declares a class constructor. Add this to a class declaration.
 * \param[in]	classname	the name of the current class
 * \ingroup	base
 */
#define CRN_DECLARE_CLASS_CONSTRUCTOR(classname) public:\
		static void Initialize();\
	private:\
struct Init { Init() { classname::Initialize(); } };\
		static Init init;

/*! \brief Defines a class constructor
 *
 * Defines a class constructor. Add this to a class definition file.
 * \param[in]	classname	the name of the current class
 * \ingroup	base
 */
#define CRN_BEGIN_CLASS_CONSTRUCTOR(classname) void classname::Initialize()\
	{\
		static bool init_done = false;\
		if (!init_done)\
		{\
			init_done = true;

/*! \brief Defines a class constructor
 *
 * Defines a class constructor. Add this to a class definition file.
 * \param[in]	classname	the name of the current class
 * \ingroup	base
 */
#define CRN_END_CLASS_CONSTRUCTOR(classname) }\
	}\
	classname::Init classname::init;

#endif
