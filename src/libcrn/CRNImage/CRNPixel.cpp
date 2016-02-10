/* Copyright 2015 INSA-Lyon
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
 * file: CRNPixel.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNPixel.h>

using namespace crn;
using namespace crn::pixel;

XYZ::XYZ(const RGB<uint8_t> &p)
{
	static const auto xr = 0.4124;
	static const auto xg = 0.3576;
	static const auto xb = 0.1805;

	static const auto yr = 0.2126;
	static const auto yg = 0.7152;
	static const auto yb = 0.0722;

	static const auto zr = 0.0193;
	static const auto zg = 0.1192;
	static const auto zb = 0.9505;

	auto var_R = p.r / 255.0;
	auto var_G = p.g / 255.0;
	auto var_B = p.b / 255.0;

	if (var_R > 0.04045) 
		var_R = pow((var_R + 0.055) / 1.055, 2.4);
	else
		var_R /= 12.92;

	if (var_G > 0.04045)
		var_G = pow((var_G + 0.055) / 1.055, 2.4);
	else
		var_G /= 12.92;

	if (var_B > 0.04045) 
		var_B = pow((var_B + 0.055) / 1.055, 2.4);
	else
		var_B /= 12.92;

	var_R *= 100;
	var_G *= 100;
	var_B *= 100;

	x = var_R * xr + var_G * xg + var_B * xb;
	y = var_R * yr + var_G * yg + var_B * yb;
	z = var_R * zr + var_G * zg + var_B * zb;
}

YUV::YUV(const RGB<uint8_t> &p)
{
    y = 0.299 * p.r + 0.587 * p.g + 0.114 * p.b;
    u = 0.492 * (p.b - y);
    v = 0.877 * (p.r - y);
}

XYZ::XYZ(const Lab &p)
{
	auto var_Y = (p.l + 16) / 116.0;
	auto var_X = p.a / 500.0 + var_Y;
	auto var_Z = var_Y - p.b / 200.0;

	if (pow(var_Y, 3) > 0.008856)
		var_Y = pow(var_Y, 3);
	else
		var_Y = (var_Y - 16 / 116.0) / 7.787;
	if (pow(var_X, 3) > 0.008856)
		var_X = pow(var_X, 3);
	else
		var_X = (var_X - 16 / 116.0) / 7.787;
	if (pow(var_Z, 3) > 0.008856)
		var_Z = pow(var_Z, 3);
	else
		var_Z = (var_Z - 16 / 116.0) / 7.787;

	x = var_X * 95.047; 
	y = var_Y * 100.000;
	z = var_Z * 108.883;
}

XYZ::XYZ(const Luv &p)
{
	auto var_Y = (p.l + 16) / 116.0;
	if (pow(var_Y, 3) > 0.008856)
		var_Y = pow(var_Y, 3);
	else
		var_Y = (var_Y - 16 / 116.0) / 7.787;

	static const auto ref_X =  95.047;      //Observer= 2°, Illuminant= D65
	static const auto ref_Y = 100.000;
	static const auto ref_Z = 108.883;
	static const auto ref_U = ( 4 * ref_X ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );
	static const auto ref_V = ( 9 * ref_Y ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );

	auto var_U = p.u / (13 * p.l) + ref_U;
	auto var_V = p.v / (13 * p.l) + ref_V;

	y = var_Y * 100;
	x = -( 9 * y * var_U ) / ( ( var_U - 4 ) * var_V  - var_U * var_V );
	z = ( 9 * y - ( 15 * var_V * y ) - ( var_V * x ) ) / ( 3 * var_V );
}

Lab::Lab(const XYZ &p)
{
	auto var_X = p.x / 95.047; 
	auto var_Y = p.y / 100.000;
	auto var_Z = p.z / 108.883;

	if (var_X > 0.008856) 
		var_X = pow(var_X, 1/3.0);
	else
		var_X = (7.787 * var_X) + (16 / 116.0);
	if (var_Y > 0.008856) 
		var_Y = pow(var_Y, 1/3.0);
	else
		var_Y = (7.787 * var_Y) + (16 / 116.0);
	if (var_Z > 0.008856) 
		var_Z = pow(var_Z, 1/3.0);
	else
		var_Z = (7.787 * var_Z) + (16 / 116.0);

	l = (116 * var_Y) - 16;
	a = 500 * (var_X - var_Y);
	b = 200 * (var_Y - var_Z);
}

Luv::Luv(const XYZ &p)
{
	static const auto ref_X =  95.047;        //Observer= 2°, Illuminant= D65
	static const auto ref_Y = 100.000;
	static const auto ref_Z = 108.883;
	static const auto ref_U = ( 4 * ref_X ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );
	static const auto ref_V = ( 9 * ref_Y ) / ( ref_X + ( 15 * ref_Y ) + ( 3 * ref_Z ) );

	auto var_Y = p.y / 100.0;
	if (var_Y > 0.008856 ) 
		var_Y = pow(var_Y, 1.0 / 3.0);
	else                    
		var_Y = ( 7.787 * var_Y ) + 16.0 / 116.0;
	l = 116.0 * var_Y - 16.0;

	auto denom = p.x + 15 * p.y + 3 * p.z;
	if (denom != 0.0)
	{
		auto var_U = (4 * p.x) / denom;
		auto var_V = (9 * p.y) / denom;		
		u= 13 * l * (var_U - ref_U);
		v= 13 * l * (var_V - ref_V);
	}
	else
	{
		u = v = 0;
	}			
}

