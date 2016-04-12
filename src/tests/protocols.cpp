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
 * file: protocols.cpp
 * \author Yann LEYDIER
 */

#include "catch.hpp"

// Empty class
struct Nothing {};

// operator<
struct LtM
{
	bool operator<(const LtM&) const { return false; }
};
struct LtF {};
bool operator<(const LtF&, const LtF&) { return false; }

// all comparison operators
struct CompM
{
	bool operator<(const CompM&) const { return false; }
	bool operator>(const CompM&) const { return false; }
	bool operator<=(const CompM&) const { return false; }
	bool operator>=(const CompM&) const { return false; }
};
struct CompF {};
bool operator<(const CompF&, const CompF&) { return false; }
bool operator>(const CompF&, const CompF&) { return false; }
bool operator<=(const CompF&, const CompF&) { return false; }
bool operator>=(const CompF&, const CompF&) { return false; }

// operator+
struct AddM
{
	AddM operator+(const AddM &) const { return *this; }
};
struct AddF {};
AddF operator+(const AddF &o1, const AddF &o2) { return o1; }

// magma
struct MagmaM
{
	MagmaM operator+(const MagmaM &) const { return *this; }
	bool operator==(const MagmaM &) const { return false; }
};
struct MagmaF {};
MagmaF operator+(const MagmaF &o1, const MagmaF &o2) { return o1; }
bool operator==(const MagmaF &o1, const MagmaF &o2) { return false; }

// operator-
struct MinusM
{
	MinusM operator-(const MinusM &) const { return *this; }
};
struct MinusF {};
MinusF operator-(const MinusF &o1, const MinusF &o2) { return o1; }

// group
struct GroupM
{
	GroupM operator+(const GroupM &) const { return *this; }
	GroupM operator-(const GroupM &) const { return *this; }
	bool operator==(const GroupM &) const { return false; }
};
struct GroupF {};
GroupF operator+(const GroupF &o1, const GroupF &o2) { return o1; }
GroupF operator-(const GroupF &o1, const GroupF &o2) { return o1; }
bool operator==(const GroupF &o1, const GroupF &o2) { return false; }

// operator* (inner)
struct IMultM
{
	IMultM operator*(const IMultM &) const { return *this; }
};
struct IMultF {};
IMultF operator*(const IMultF &o1, const IMultF &o2) { return o1; }

// ring
struct RingM
{
	RingM operator+(const RingM &) const { return *this; }
	RingM operator-(const RingM &) const { return *this; }
	RingM operator*(const RingM &) const { return *this; }
	bool operator==(const RingM &) const { return false; }
};
struct RingF {};
RingF operator+(const RingF &o1, const RingF &o2) { return o1; }
RingF operator-(const RingF &o1, const RingF &o2) { return o1; }
RingF operator*(const RingF &o1, const RingF &o2) { return o1; }
bool operator==(const RingF &o1, const RingF &o2) { return false; }

// operator* (outer)
struct OMultM
{
	OMultM operator*(double) const { return *this; }
};
struct OMultF {};
OMultF operator*(const OMultF &o1, double) { return o1; }
OMultF operator*(double, const OMultF &o1) { return o1; }

// vector over R
struct VectorM
{
	VectorM operator+(const VectorM &) const { return *this; }
	VectorM operator-(const VectorM &) const { return *this; }
	VectorM operator*(double) const { return *this; }
	bool operator==(const VectorM &) const { return false; }
};
struct VectorF {};
VectorF operator+(const VectorF &o1, const VectorF &o2) { return o1; }
VectorF operator-(const VectorF &o1, const VectorF &o2) { return o1; }
VectorF operator*(double, const VectorF &o1) { return o1; }
VectorF operator*(const VectorF &o1, double) { return o1; }
bool operator==(const VectorF &o1, const VectorF &o2) { return false; }

// algebra
struct AlgebraM
{
	AlgebraM operator+(const AlgebraM &) const { return *this; }
	AlgebraM operator-(const AlgebraM &) const { return *this; }
	AlgebraM operator*(const AlgebraM &) const { return *this; }
	AlgebraM operator*(double) const { return *this; }
	bool operator==(const AlgebraM &) const { return false; }
};
struct AlgebraF {};
AlgebraF operator+(const AlgebraF &o1, const AlgebraF &o2) { return o1; }
AlgebraF operator-(const AlgebraF &o1, const AlgebraF &o2) { return o1; }
AlgebraF operator*(const AlgebraF &o1, const AlgebraF &o2) { return o1; }
AlgebraF operator*(double, const AlgebraF &o1) { return o1; }
AlgebraF operator*(const AlgebraF &o1, double) { return o1; }
bool operator==(const AlgebraF &o1, const AlgebraF &o2) { return false; }

// operator/
struct DivM
{
	DivM operator/(const DivM &) const { return *this; }
};
struct DivF {};
DivF operator/(const DivF &o1, const DivF &o2) { return o1; }

// field
struct FieldM
{
	FieldM operator+(const FieldM &) const { return *this; }
	FieldM operator-(const FieldM &) const { return *this; }
	FieldM operator*(const FieldM &) const { return *this; }
	FieldM operator*(double) const { return *this; }
	FieldM operator/(const FieldM &) const { return *this; }
	bool operator==(const FieldM &) const { return false; }
};
struct FieldF {};
FieldF operator+(const FieldF &o1, const FieldF &o2) { return o1; }
FieldF operator-(const FieldF &o1, const FieldF &o2) { return o1; }
FieldF operator*(const FieldF &o1, const FieldF &o2) { return o1; }
FieldF operator*(double, const FieldF &o1) { return o1; }
FieldF operator*(const FieldF &o1, double) { return o1; }
FieldF operator/(const FieldF &o1, const FieldF &o2) { return o1; }
bool operator==(const FieldF &o1, const FieldF &o2) { return false; }

#include <CRNObject.h>
#include <CRNProtocols.h>
using namespace crn;

// clonable
struct C: public Object
{
	C(int i): val(i) {};
	C(const C&) = default;
	virtual ~C() override = default;
	int val;
	CRN_DECLARE_CLASS_CONSTRUCTOR(C)
};
CRN_BEGIN_CLASS_CONSTRUCTOR(C)
	Cloner::Register<C>();
CRN_END_CLASS_CONSTRUCTOR(C)


TEST_CASE("Empty class", "[protocols]")
{
	CHECK(traits::HasLT<Nothing>::value == false);
	CHECK(traits::HasLE<Nothing>::value == false);
	CHECK(traits::HasGT<Nothing>::value == false);
	CHECK(traits::HasGE<Nothing>::value == false);
	CHECK(IsPOSet<Nothing>::value == false);
	CHECK(traits::HasPlus<Nothing>::value == false);
	CHECK(traits::HasEquals<Nothing>::value == false);
	CHECK(IsMetric<Nothing>::value == false);
	CHECK(IsMagma<Nothing>::value == false);
	CHECK(IsGroup<Nothing>::value == false);
	CHECK(IsRing<Nothing>::value == false);
	CHECK(IsVectorOverR<Nothing>::value == false);
	CHECK(IsAlgebra<Nothing>::value == false);
	CHECK(IsField<Nothing>::value == false);
}

TEST_CASE("Class with operator< as method", "[protocols]")
{
	CHECK(traits::HasLT<LtM>::value == true);
	CHECK(traits::HasLE<LtM>::value == false);
	CHECK(traits::HasGT<LtM>::value == false);
	CHECK(traits::HasGE<LtM>::value == false);
	CHECK(IsPOSet<LtM>::value == false);
	CHECK(IsMetric<LtM>::value == false);
	CHECK(IsMagma<LtM>::value == false);
	CHECK(IsGroup<LtM>::value == false);
	CHECK(IsRing<LtM>::value == false);
	CHECK(IsVectorOverR<LtM>::value == false);
	CHECK(IsAlgebra<LtM>::value == false);
	CHECK(IsField<LtM>::value == false);
}

TEST_CASE("Class with operator< as function", "[protocols]")
{
	CHECK(traits::HasLT<LtF>::value == true);
	CHECK(traits::HasLE<LtF>::value == false);
	CHECK(traits::HasGT<LtF>::value == false);
	CHECK(traits::HasGE<LtF>::value == false);
	CHECK(IsPOSet<LtF>::value == false);
	CHECK(IsMetric<LtF>::value == false);
	CHECK(IsMagma<LtF>::value == false);
	CHECK(IsGroup<LtF>::value == false);
	CHECK(IsRing<LtF>::value == false);
	CHECK(IsVectorOverR<LtF>::value == false);
	CHECK(IsAlgebra<LtF>::value == false);
	CHECK(IsField<LtF>::value == false);
}

TEST_CASE("Class with operators <, >, <= and >= as methods", "[protocols]")
{
	CHECK(traits::HasLT<CompM>::value == true);
	CHECK(traits::HasLE<CompM>::value == true);
	CHECK(traits::HasGT<CompM>::value == true);
	CHECK(traits::HasGE<CompM>::value == true);
	CHECK(IsPOSet<CompM>::value == true);
	CHECK(IsMetric<CompM>::value == false);
	CHECK(IsMagma<CompM>::value == false);
	CHECK(IsGroup<CompM>::value == false);
	CHECK(IsRing<CompM>::value == false);
	CHECK(IsVectorOverR<CompM>::value == false);
	CHECK(IsAlgebra<CompM>::value == false);
	CHECK(IsField<CompM>::value == false);
}

TEST_CASE("Class with operators <, >, <= and >= as functions", "[protocols]")
{
	CHECK(traits::HasLT<CompF>::value == true);
	CHECK(traits::HasLE<CompF>::value == true);
	CHECK(traits::HasGT<CompF>::value == true);
	CHECK(traits::HasGE<CompF>::value == true);
	CHECK(IsPOSet<CompF>::value == true);
	CHECK(IsMetric<CompF>::value == false);
	CHECK(IsMagma<CompF>::value == false);
	CHECK(IsGroup<CompF>::value == false);
	CHECK(IsRing<CompF>::value == false);
	CHECK(IsVectorOverR<CompF>::value == false);
	CHECK(IsAlgebra<CompF>::value == false);
	CHECK(IsField<CompF>::value == false);
}

TEST_CASE("Numbers", "[protocols]")
{
	CHECK(IsPOSet<int>::value == true);
	CHECK(IsMetric<int>::value == true);
	CHECK(IsMagma<int>::value == true);
	CHECK(IsGroup<int>::value == true);
	CHECK(IsRing<int>::value == true);
	CHECK(IsVectorOverR<int>::value == true);
	CHECK(IsAlgebra<int>::value == true);
	CHECK(IsField<int>::value == true);

	CHECK(IsPOSet<double>::value == true);
	CHECK(IsMetric<double>::value == true);
	CHECK(IsMagma<double>::value == true);
	CHECK(IsGroup<double>::value == true);
	CHECK(IsRing<double>::value == true);
	CHECK(IsVectorOverR<double>::value == true);
	CHECK(IsAlgebra<double>::value == true);
	CHECK(IsField<double>::value == true);
}

TEST_CASE("Class with operator+ as method", "[protocols]")
{
	CHECK(IsPOSet<AddM>::value == false);
	CHECK(IsMetric<AddM>::value == false);
	CHECK(traits::HasPlus<AddM>::value == true);
	CHECK(traits::HasEquals<AddM>::value == false);
	CHECK(IsMagma<AddM>::value == false);
	CHECK(IsGroup<AddM>::value == false);
	CHECK(IsRing<AddM>::value == false);
	CHECK(IsVectorOverR<AddM>::value == false);
	CHECK(IsAlgebra<AddM>::value == false);
	CHECK(IsField<AddM>::value == false);
}

TEST_CASE("Class with operator+ as function", "[protocols]")
{
	CHECK(IsPOSet<AddF>::value == false);
	CHECK(IsMetric<AddF>::value == false);
	CHECK(traits::HasPlus<AddF>::value == true);
	CHECK(traits::HasEquals<AddF>::value == false);
	CHECK(IsMagma<AddF>::value == false);
	CHECK(IsGroup<AddF>::value == false);
	CHECK(IsRing<AddF>::value == false);
	CHECK(IsVectorOverR<AddF>::value == false);
	CHECK(IsAlgebra<AddF>::value == false);
	CHECK(IsField<AddF>::value == false);
}

TEST_CASE("Class with operators + and == as method", "[protocols]")
{
	CHECK(IsPOSet<MagmaM>::value == false);
	CHECK(IsMetric<MagmaM>::value == false);
	CHECK(traits::HasPlus<MagmaM>::value == true);
	CHECK(traits::HasEquals<MagmaM>::value == true);
	CHECK(IsMagma<MagmaM>::value == true);
	CHECK(IsGroup<MagmaM>::value == false);
	CHECK(IsRing<MagmaM>::value == false);
	CHECK(IsVectorOverR<MagmaM>::value == false);
	CHECK(IsAlgebra<MagmaM>::value == false);
	CHECK(IsField<MagmaM>::value == false);
}

TEST_CASE("Class with operators + and == as function", "[protocols]")
{
	CHECK(IsPOSet<MagmaF>::value == false);
	CHECK(IsMetric<MagmaF>::value == false);
	CHECK(traits::HasPlus<MagmaF>::value == true);
	CHECK(traits::HasEquals<MagmaF>::value == true);
	CHECK(IsMagma<MagmaF>::value == true);
	CHECK(IsGroup<MagmaF>::value == false);
	CHECK(IsRing<MagmaF>::value == false);
	CHECK(IsVectorOverR<MagmaF>::value == false);
	CHECK(IsAlgebra<MagmaF>::value == false);
	CHECK(IsField<MagmaF>::value == false);
}

TEST_CASE("Class with operator- as method", "[protocols]")
{
	CHECK(IsPOSet<MinusM>::value == false);
	CHECK(IsMetric<MinusM>::value == false);
	CHECK(IsMagma<MinusM>::value == false);
	CHECK(traits::HasMinus<MinusM>::value == true);
	CHECK(IsGroup<MinusM>::value == false);
	CHECK(IsRing<MinusM>::value == false);
	CHECK(IsVectorOverR<MinusM>::value == false);
	CHECK(IsAlgebra<MinusM>::value == false);
	CHECK(IsField<MinusM>::value == false);
}

TEST_CASE("Class with operator- as function", "[protocols]")
{
	CHECK(IsPOSet<MinusF>::value == false);
	CHECK(IsMetric<MinusF>::value == false);
	CHECK(IsMagma<MinusF>::value == false);
	CHECK(traits::HasMinus<MinusF>::value == true);
	CHECK(IsGroup<MinusF>::value == false);
	CHECK(IsRing<MinusF>::value == false);
	CHECK(IsVectorOverR<MinusF>::value == false);
	CHECK(IsAlgebra<MinusF>::value == false);
	CHECK(IsField<MinusF>::value == false);
}

TEST_CASE("Class with operators +, - and == as method", "[protocols]")
{
	CHECK(IsPOSet<GroupM>::value == false);
	CHECK(IsMetric<GroupM>::value == false);
	CHECK(traits::HasPlus<GroupM>::value == true);
	CHECK(traits::HasEquals<GroupM>::value == true);
	CHECK(IsGroup<GroupM>::value == true);
	CHECK(traits::HasMinus<GroupM>::value == true);
	CHECK(IsGroup<GroupM>::value == true);
	CHECK(IsRing<GroupM>::value == false);
	CHECK(IsVectorOverR<GroupM>::value == false);
	CHECK(IsAlgebra<GroupM>::value == false);
	CHECK(IsField<GroupM>::value == false);
}

TEST_CASE("Class with operators +, - and == as function", "[protocols]")
{
	CHECK(IsPOSet<GroupF>::value == false);
	CHECK(IsMetric<GroupF>::value == false);
	CHECK(traits::HasPlus<GroupF>::value == true);
	CHECK(traits::HasEquals<GroupF>::value == true);
	CHECK(IsGroup<GroupF>::value == true);
	CHECK(traits::HasMinus<GroupF>::value == true);
	CHECK(IsGroup<GroupF>::value == true);
	CHECK(IsRing<GroupF>::value == false);
	CHECK(IsVectorOverR<GroupF>::value == false);
	CHECK(IsAlgebra<GroupF>::value == false);
	CHECK(IsField<GroupF>::value == false);
}

TEST_CASE("Class with operator* (inner) as method", "[protocols]")
{
	CHECK(IsPOSet<IMultM>::value == false);
	CHECK(IsMetric<IMultM>::value == false);
	CHECK(IsMagma<IMultM>::value == false);
	CHECK(IsGroup<IMultM>::value == false);
	CHECK(traits::HasInnerMult<IMultM>::value == true);
	CHECK(IsRing<IMultM>::value == false);
	CHECK(IsVectorOverR<IMultM>::value == false);
	CHECK(IsAlgebra<IMultM>::value == false);
	CHECK(IsField<IMultM>::value == false);
}

TEST_CASE("Class with operator* (inner) as function", "[protocols]")
{
	CHECK(IsPOSet<IMultF>::value == false);
	CHECK(IsMetric<IMultF>::value == false);
	CHECK(IsMagma<IMultF>::value == false);
	CHECK(IsGroup<IMultF>::value == false);
	CHECK(traits::HasInnerMult<IMultF>::value == true);
	CHECK(IsRing<IMultF>::value == false);
	CHECK(IsVectorOverR<IMultF>::value == false);
	CHECK(IsAlgebra<IMultF>::value == false);
	CHECK(IsField<IMultF>::value == false);
}

TEST_CASE("Class with operators +, -, *(inner) and == as method", "[protocols]")
{
	CHECK(IsPOSet<RingM>::value == false);
	CHECK(IsMetric<RingM>::value == false);
	CHECK(traits::HasPlus<RingM>::value == true);
	CHECK(traits::HasEquals<RingM>::value == true);
	CHECK(IsMagma<RingM>::value == true);
	CHECK(traits::HasMinus<RingM>::value == true);
	CHECK(IsGroup<RingM>::value == true);
	CHECK(traits::HasInnerMult<RingM>::value == true);
	CHECK(IsRing<RingM>::value == true);
	CHECK(IsVectorOverR<RingM>::value == false);
	CHECK(IsAlgebra<RingM>::value == false);
	CHECK(IsField<RingM>::value == false);
}

TEST_CASE("Class with operators +, -, *(inner) and == as function", "[protocols]")
{
	CHECK(IsPOSet<RingF>::value == false);
	CHECK(IsMetric<RingF>::value == false);
	CHECK(traits::HasPlus<RingF>::value == true);
	CHECK(traits::HasEquals<RingF>::value == true);
	CHECK(IsMagma<RingF>::value == true);
	CHECK(traits::HasMinus<RingF>::value == true);
	CHECK(IsGroup<RingF>::value == true);
	CHECK(traits::HasInnerMult<RingF>::value == true);
	CHECK(IsRing<RingF>::value == true);
	CHECK(IsVectorOverR<RingF>::value == false);
	CHECK(IsAlgebra<RingF>::value == false);
	CHECK(IsField<RingF>::value == false);
}

TEST_CASE("Class with operator* (outer) as method", "[protocols]")
{
	CHECK(IsPOSet<OMultM>::value == false);
	CHECK(IsMetric<OMultM>::value == false);
	CHECK(IsMagma<OMultM>::value == false);
	CHECK(IsGroup<OMultM>::value == false);
	CHECK(IsRing<OMultM>::value == false);
	CHECK(traits::HasLeftOuterMult<OMultM>::value == false);
	CHECK(traits::HasRightOuterMult<OMultM>::value == true);
	CHECK(IsVectorOverR<OMultM>::value == false);
	CHECK(IsAlgebra<OMultM>::value == false);
	CHECK(IsField<OMultM>::value == false);
}

TEST_CASE("Class with operator* (outer) as function", "[protocols]")
{
	CHECK(IsPOSet<OMultF>::value == false);
	CHECK(IsMetric<OMultF>::value == false);
	CHECK(IsMagma<OMultF>::value == false);
	CHECK(IsGroup<OMultF>::value == false);
	CHECK(IsRing<OMultF>::value == false);
	CHECK(traits::HasLeftOuterMult<OMultF>::value == true);
	CHECK(traits::HasRightOuterMult<OMultF>::value == true);
	CHECK(IsVectorOverR<OMultF>::value == false);
	CHECK(IsAlgebra<OMultF>::value == false);
	CHECK(IsField<OMultF>::value == false);
}

TEST_CASE("Class with operators +, -, *(outer) and == as method", "[protocols]")
{
	CHECK(IsPOSet<VectorM>::value == false);
	CHECK(IsMetric<VectorM>::value == false);
	CHECK(traits::HasPlus<VectorM>::value == true);
	CHECK(traits::HasEquals<VectorM>::value == true);
	CHECK(IsMagma<VectorM>::value == true);
	CHECK(traits::HasMinus<VectorM>::value == true);
	CHECK(IsGroup<VectorM>::value == true);
	CHECK(traits::HasInnerMult<VectorM>::value == false);
	CHECK(IsRing<VectorM>::value == false);
	CHECK(traits::HasLeftOuterMult<VectorM>::value == false);
	CHECK(traits::HasRightOuterMult<VectorM>::value == true);
	CHECK(IsVectorOverR<VectorM>::value == false);
	CHECK(IsAlgebra<VectorM>::value == false);
	CHECK(IsField<VectorM>::value == false);
}

TEST_CASE("Class with operators +, -, *(outer) and == as function", "[protocols]")
{
	CHECK(IsPOSet<VectorF>::value == false);
	CHECK(IsMetric<VectorF>::value == false);
	CHECK(traits::HasPlus<VectorF>::value == true);
	CHECK(traits::HasEquals<VectorF>::value == true);
	CHECK(IsMagma<VectorF>::value == true);
	CHECK(traits::HasMinus<VectorF>::value == true);
	CHECK(IsGroup<VectorF>::value == true);
	CHECK(traits::HasInnerMult<VectorF>::value == false);
	CHECK(IsRing<VectorF>::value == false);
	CHECK(traits::HasLeftOuterMult<VectorF>::value == true);
	CHECK(traits::HasRightOuterMult<VectorF>::value == true);
	CHECK(IsVectorOverR<VectorF>::value == true);
	CHECK(IsAlgebra<VectorF>::value == false);
	CHECK(IsField<VectorF>::value == false);
}

TEST_CASE("Class with operators +, -, *(both) and == as method", "[protocols]")
{
	CHECK(IsPOSet<AlgebraM>::value == false);
	CHECK(IsMetric<AlgebraM>::value == false);
	CHECK(traits::HasPlus<AlgebraM>::value == true);
	CHECK(traits::HasEquals<AlgebraM>::value == true);
	CHECK(IsMagma<AlgebraM>::value == true);
	CHECK(traits::HasMinus<AlgebraM>::value == true);
	CHECK(IsGroup<AlgebraM>::value == true);
	CHECK(traits::HasInnerMult<AlgebraM>::value == true);
	CHECK(IsRing<AlgebraM>::value == true);
	CHECK(traits::HasLeftOuterMult<AlgebraM>::value == false);
	CHECK(traits::HasRightOuterMult<AlgebraM>::value == true);
	CHECK(IsVectorOverR<AlgebraM>::value == false);
	CHECK(IsAlgebra<AlgebraM>::value == false);
	CHECK(IsField<AlgebraM>::value == false);
}

TEST_CASE("Class with operators +, -, *(both) and == as function", "[protocols]")
{
	CHECK(IsPOSet<AlgebraF>::value == false);
	CHECK(IsMetric<AlgebraF>::value == false);
	CHECK(traits::HasPlus<AlgebraF>::value == true);
	CHECK(traits::HasEquals<AlgebraF>::value == true);
	CHECK(IsMagma<AlgebraF>::value == true);
	CHECK(traits::HasMinus<AlgebraF>::value == true);
	CHECK(IsGroup<AlgebraF>::value == true);
	CHECK(traits::HasInnerMult<AlgebraF>::value == true);
	CHECK(IsRing<AlgebraF>::value == true);
	CHECK(traits::HasLeftOuterMult<AlgebraF>::value == true);
	CHECK(traits::HasRightOuterMult<AlgebraF>::value == true);
	CHECK(IsVectorOverR<AlgebraF>::value == true);
	CHECK(IsAlgebra<AlgebraF>::value == true);
	CHECK(IsField<AlgebraF>::value == false);
}

TEST_CASE("Class with operator/ as method", "[protocols]")
{
	CHECK(IsPOSet<DivM>::value == false);
	CHECK(IsMetric<DivM>::value == false);
	CHECK(IsMagma<DivM>::value == false);
	CHECK(IsGroup<DivM>::value == false);
	CHECK(IsRing<DivM>::value == false);
	CHECK(IsVectorOverR<DivM>::value == false);
	CHECK(IsAlgebra<DivM>::value == false);
	CHECK(traits::HasDivide<DivM>::value == true);
	CHECK(IsField<DivM>::value == false);
}

TEST_CASE("Class with operator/ as function", "[protocols]")
{
	CHECK(IsPOSet<DivF>::value == false);
	CHECK(IsMetric<DivF>::value == false);
	CHECK(IsMagma<DivF>::value == false);
	CHECK(IsGroup<DivF>::value == false);
	CHECK(IsRing<DivF>::value == false);
	CHECK(IsVectorOverR<DivF>::value == false);
	CHECK(IsAlgebra<DivF>::value == false);
	CHECK(traits::HasDivide<DivF>::value == true);
	CHECK(IsField<DivF>::value == false);
}

TEST_CASE("Class with operators +, -, *(both), / and == as method", "[protocols]")
{
	CHECK(IsPOSet<FieldM>::value == false);
	CHECK(IsMetric<FieldM>::value == false);
	CHECK(traits::HasPlus<FieldM>::value == true);
	CHECK(traits::HasEquals<FieldM>::value == true);
	CHECK(IsMagma<FieldM>::value == true);
	CHECK(traits::HasMinus<FieldM>::value == true);
	CHECK(IsGroup<FieldM>::value == true);
	CHECK(traits::HasInnerMult<FieldM>::value == true);
	CHECK(IsRing<FieldM>::value == true);
	CHECK(traits::HasLeftOuterMult<FieldM>::value == false);
	CHECK(traits::HasRightOuterMult<FieldM>::value == true);
	CHECK(IsVectorOverR<FieldM>::value == false);
	CHECK(IsAlgebra<FieldM>::value == false);
	CHECK(traits::HasDivide<FieldM>::value == true);
	CHECK(IsField<FieldM>::value == false);
}

TEST_CASE("Class with operators +, -, *(both), / and == as function", "[protocols]")
{
	CHECK(IsPOSet<FieldF>::value == false);
	CHECK(IsMetric<FieldF>::value == false);
	CHECK(traits::HasPlus<FieldF>::value == true);
	CHECK(traits::HasEquals<FieldF>::value == true);
	CHECK(IsMagma<FieldF>::value == true);
	CHECK(traits::HasMinus<FieldF>::value == true);
	CHECK(IsGroup<FieldF>::value == true);
	CHECK(traits::HasInnerMult<FieldF>::value == true);
	CHECK(IsRing<FieldF>::value == true);
	CHECK(traits::HasLeftOuterMult<FieldF>::value == true);
	CHECK(traits::HasRightOuterMult<FieldF>::value == true);
	CHECK(IsVectorOverR<FieldF>::value == true);
	CHECK(IsAlgebra<FieldF>::value == true);
	CHECK(traits::HasDivide<FieldF>::value == true);
	CHECK(IsField<FieldF>::value == true);
}

TEST_CASE("Clonable class", "[protocols]")
{
	auto c = C{42};
	auto cc = CloneAs<C>(c);
	REQUIRE(cc != nullptr);
	REQUIRE(c.val == cc->val);
}

