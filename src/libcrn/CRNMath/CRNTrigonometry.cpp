/* Copyright 2013 INSA Lyon
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
 * file: CRNTrigonometry.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNMath.h>

using namespace crn;

const Radian::type Radian::MAXVAL(2.0 * M_PI);
const TypeInfo<Radian::type>::SumType Radian::VAL2PI(2.0 * M_PI);

const Degree::type Degree::MAXVAL(360.0);
const TypeInfo<Degree::type>::SumType Degree::VAL2PI(360.0);

const ByteAngle::type ByteAngle::MAXVAL(255);
const TypeInfo<ByteAngle::type>::SumType ByteAngle::VAL2PI(256);


