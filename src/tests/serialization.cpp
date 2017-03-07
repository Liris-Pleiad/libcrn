/* Copyright 2016 ENS-Lyon
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
 * file: serialization.cpp
 * \author Yann LEYDIER
 */

#include "catch.hpp"
#include <CRNObject.h>
#include <CRNProtocols.h>
#include <CRNXml/CRNXml.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNString.h>

struct ser: public crn::Object
{
	ser() = default;
	virtual ~ser() = default;
	crn::xml::Element Serialize(crn::xml::Element &parent) const
	{
		auto el = parent.PushBackElement("ser");
		el.SetAttribute("test", 42);
		return el;
	}
	void Deserialize(crn::xml::Element &el)
	{
		if (el.GetName() != "ser")
			throw crn::ExceptionDomain("not a ser object");
		if (el.GetAttribute<int>("test", false) != 42) // may throw
			throw crn::ExceptionInvalidArgument("wrong magic number");
	}
	CRN_SERIALIZATION_CONSTRUCTOR(ser)
	CRN_DECLARE_CLASS_CONSTRUCTOR(ser)
};
namespace crn
{
	template<> struct IsSerializable<ser> : public std::true_type {};
}
CRN_BEGIN_CLASS_CONSTRUCTOR(ser)
	CRN_DATA_FACTORY_REGISTER(U"ser", ser);
	crn::Serializer::Register<ser>();
CRN_END_CLASS_CONSTRUCTOR(ser)

TEST_CASE("Register, save and load a custom class", "[serialization]")
{
	auto s = ser{};
	auto doc = crn::xml::Document{};
	auto root = doc.PushBackElement("test");
	auto el = crn::Serialize(s, root);
	REQUIRE(el);
	REQUIRE_NOTHROW(crn::Deserialize(s, el));
}

#include <CRNData/CRNInt.h>
TEST_CASE("Register, save and load Int", "[serialization]")
{
	auto i = crn::Int{42};
	auto doc = crn::xml::Document{};
	auto root = doc.PushBackElement("test");
	auto el = crn::Serialize(i, root);
	REQUIRE(el);
	auto i2 = crn::Int{0};
	REQUIRE_NOTHROW(crn::Deserialize(i2, el));
	CHECK(i2 == 42);
}

#include <CRNData/CRNReal.h>
TEST_CASE("Register, save and load Real", "[serialization]")
{
	auto r = crn::Real{42.42};
	auto doc = crn::xml::Document{};
	auto root = doc.PushBackElement("test");
	auto el = crn::Serialize(r, root);
	REQUIRE(el);
	auto r2 = crn::Real{0};
	REQUIRE_NOTHROW(crn::Deserialize(r2, el));
	CHECK(r2 == Approx(42.42));
}

#include <CRNData/CRNVector.h>
TEST_CASE("Register, save and load Vector", "[serialization]")
{
	auto v = crn::Vector{};
	v.PushBack(std::make_shared<crn::Real>(42.42));
	v.PushBack(std::make_shared<crn::Int>(42));
	auto doc = crn::xml::Document{};
	auto root = doc.PushBackElement("test");

	SECTION("With library types")
	{
		auto el = crn::Serialize(v, root);
		REQUIRE(el);
		auto v2 = crn::Vector{};
		REQUIRE_NOTHROW(crn::Deserialize(v2, el));
		REQUIRE(v2.Size() == 2);
		const auto r = std::dynamic_pointer_cast<const crn::Real>(v2[0]);
		REQUIRE(r);
		CHECK(*r == Approx(42.42));
		const auto i = std::dynamic_pointer_cast<const crn::Int>(v2[1]);
		REQUIRE(i);
		CHECK(*i == 42);
	}

	SECTION("With custom type")
	{
		v.PushBack(std::make_shared<ser>());

		auto el = crn::Serialize(v, root);
		REQUIRE(el);
		auto v2 = crn::Vector{};
		REQUIRE_NOTHROW(crn::Deserialize(v2, el));
		REQUIRE(v2.Size() == 3);
		const auto r = std::dynamic_pointer_cast<const crn::Real>(v2[0]);
		REQUIRE(r);
		CHECK(*r == Approx(42.42));
		const auto i = std::dynamic_pointer_cast<const crn::Int>(v2[1]);
		REQUIRE(i);
		CHECK(*i == 42);
		const auto s = std::dynamic_pointer_cast<const ser>(v2[2]);
		REQUIRE(s);
	}
}

#ifndef CRN_PF_ANDROID // MT : PB with m[U"r"] and m[U"i"] -> Invalid address 0x........ passed to free: value not allocated

#include <CRNData/CRNMap.h>
TEST_CASE("Register, save and load Map", "[serialization]")
{
	auto m = crn::Map{};
	m[U"r"] = std::make_shared<crn::Real>(42.42);
	m[U"i"] = std::make_shared<crn::Int>(42);
	auto doc = crn::xml::Document{};
	auto root = doc.PushBackElement("test");

	SECTION("With library types")
	{
		auto el = crn::Serialize(m, root);
		REQUIRE(el);
		auto m2 = crn::Map{};
		REQUIRE_NOTHROW(crn::Deserialize(m2, el));
		REQUIRE(m2.Size() == 2);
		const auto r = std::dynamic_pointer_cast<const crn::Real>(m2[U"r"]);
		REQUIRE(r);
		CHECK(*r == Approx(42.42));
		const auto i = std::dynamic_pointer_cast<const crn::Int>(m2[U"i"]);
		REQUIRE(i);
		CHECK(*i == 42);
	}

	SECTION("With custom type")
	{
		m[U"s"] = std::make_shared<ser>();

		auto el = crn::Serialize(m, root);
		REQUIRE(el);
		auto m2 = crn::Map{};
		REQUIRE_NOTHROW(crn::Deserialize(m2, el));
		REQUIRE(m2.Size() == 3);
		const auto r = std::dynamic_pointer_cast<const crn::Real>(m2[U"r"]);
		REQUIRE(r);
		CHECK(*r == Approx(42.42));
		const auto i = std::dynamic_pointer_cast<const crn::Int>(m2[U"i"]);
		REQUIRE(i);
		CHECK(*i == 42);
		const auto s = std::dynamic_pointer_cast<const ser>(m2[U"s"]);
		REQUIRE(s);
	}
}

#endif
