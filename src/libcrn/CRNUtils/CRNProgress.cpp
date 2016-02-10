/* Copyright 2007-2013 Yann LEYDIER, INSA-Lyon
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
 * file: CRNProgress.cpp
 * \author Yann LEYDIER
 */

#include <CRNUtils/CRNProgress.h>
#include <CRNIO/CRNIO.h>
#include <CRNMath/CRNMath.h>
#include <cstdio>

using namespace crn;

/*****************************************************************************/
/*!
 * Progress of one unit
 *
 */
void Progress::Advance()
{
	current += 1;
	double f = (double(current) * 100.0) / double(end);
	char buff[1024];
	switch (type)
	{
		case Type::ABSOLUTE:
			snprintf(buff, 1024, " %li / %li", Min(current + 1, end), end);
			break;
		case Type::PERCENT:
		default:
			snprintf(buff, 1024, " %.2f %%", (double)f);
			break;
	}
	disp = GetName() + buff;
	display(disp);
}

void Progress::Reset()
{
	current = -1;
	Advance();
}

Progress::~Progress()
{
}

void ConsoleProgress::display(const String &msg)
{
	CRNVerbose(msg);
}

