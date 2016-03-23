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
 * file: estimateStrokes.cpp
 * \author Yann Leydier
 */

#include <typeinfo>
#include <CRNProtocols.h>

#include <CRNAI/CRN2Means.h>
#include <CRNAI/CRNIterativeClustering.h>

#include <iostream>

class A {};
class B {};
//namespace NS
//{
	bool operator<(B b1, B b2) { return false; }
	bool operator>(B b1, B b2) { return false; }
	bool operator<=(B b1, B b2) { return false; }
	bool operator>=(B b1, B b2) { return false; }
	bool operator==(B b1, B b2) { return false; }
	B operator+(B b1, B b2) { return b1; }
	B operator-(B b1, B b2) { return b1; }
	B operator*(B b1, B b2) { return b1; }
	B operator*(B b1, double b2) { return b1; }
	B operator*(double b1, B b2) { return b2; }
	B operator/(B b1, B b2) { return b1; }
//}
struct C
{
	bool operator<(const C& other) const { return false; }
	bool operator>(const C& other) const { return false; }
	bool operator<=(const C& other) const { return false; }
	bool operator>=(const C& other) const { return false; }
	bool operator==(const C& other) const { return false; }
	C operator+(const C& other) const { return other; }
	C operator-(const C& other) const { return other; }
	C operator*(const C& other) const { return other; }
	C operator/(const C& other) const { return other; }
};

using namespace crn;

int main(int argc, char *argv[])
{
	std::cout << "HasLT" << std::endl;
	std::cout << "int " << protocol::HasLT<int>::value << " should be 1" << std::endl;
	std::cout << "A " << protocol::HasLT<A>::value << " should be 0" <<  std::endl;
	std::cout << "B (not using namespace) " << protocol::HasLT<B>::value << " should be 0" <<  std::endl;
	//using NS::operator<;
	std::cout << "B (using namespace) " << protocol::HasLT<B>::value << " should be 1" <<  std::endl;
	std::cout << "C " << protocol::HasLT<C>::value << " should be 1" <<  std::endl;

	std::cout << "IsPOSet" << std::endl;
	std::cout << "int " << IsPOSet<int>::value << " should be 1" << std::endl;
	std::cout << "A " << IsPOSet<A>::value << " should be 0" <<  std::endl;
	std::cout << "B (using namespace) " << IsPOSet<B>::value << " should be 1" <<  std::endl;
	std::cout << "C " << IsPOSet<C>::value << " should be 1" <<  std::endl;

	std::cout << "IsMagma" << std::endl;
	std::cout << "int " << IsMagma<int>::value << " should be 1" << std::endl;
	std::cout << "A " << IsMagma<A>::value << " should be 0" <<  std::endl;
	std::cout << "B (using namespace) " << IsMagma<B>::value << " should be 1" <<  std::endl;
	std::cout << "C " << IsMagma<C>::value << " should be 1" <<  std::endl;

	std::cout << "IsRing" << std::endl;
	std::cout << "int " << IsRing<int>::value << " should be 1" << std::endl;
	std::cout << "A " << IsRing<A>::value << " should be 0" <<  std::endl;
	std::cout << "B (using namespace) " << IsRing<B>::value << " should be 1" <<  std::endl;
	std::cout << "C " << IsRing<C>::value << " should be 1" <<  std::endl;

	std::cout << "IsVectorOverR" << std::endl;
	std::cout << "int " << IsVectorOverR<int>::value << " should be 1" << std::endl;
	std::cout << "A " << IsVectorOverR<A>::value << " should be 0" <<  std::endl;
	std::cout << "B (using namespace) " << IsVectorOverR<B>::value << " should be 1" <<  std::endl;
	std::cout << "C " << IsVectorOverR<C>::value << " should be 0" <<  std::endl;

	std::cout << "IsAlgebra" << std::endl;
	std::cout << "int " << IsAlgebra<int>::value << " should be 1" << std::endl;
	std::cout << "A " << IsAlgebra<A>::value << " should be 0" <<  std::endl;
	std::cout << "B (using namespace) " << IsAlgebra<B>::value << " should be 1" <<  std::endl;
	std::cout << "C " << IsAlgebra<C>::value << " should be 0" <<  std::endl;

	std::cout << "IsField" << std::endl;
	std::cout << "int " << IsField<int>::value << " should be 1" << std::endl;
	std::cout << "A " << IsField<A>::value << " should be 0" <<  std::endl;
	std::cout << "B (using namespace) " << IsField<B>::value << " should be 1" <<  std::endl;
	std::cout << "C " << IsField<C>::value << " should be 0" <<  std::endl;


	auto vec = std::vector<int>{1, 2, 4, 54, 0};
	std::cout << TwoMeans(vec.begin(), vec.end()).second << std::endl;

	auto ic1 = IterativeClustering<int>{};
	auto ic2 = IterativeClustering<B>{};
	//auto ic2 = IterativeClustering<A>{};

#ifdef _MSC_VER
	getchar();
#endif
	return 0;
}

