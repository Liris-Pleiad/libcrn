/* Copyright 2008-2016 INSA Lyon, ENS-Lyon
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

#include <CRN.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNConfig.h>
#include <CRNIO/CRNIO.h>

int main(int argc, char *argv[])
{
	crn::IO::CurrentMessenger()->VerbosePrefix = U"";
	crn::IO::IsVerbose() = true;
	if (argc < 2)
	{
		CRNVerbose(crn::String(U"Usage: ") +  argv[0] + 
					crn::String(U" image1 [image2 [...]]"));
		getchar();
		return -1;
	}
	for (int tmp = 1; tmp < argc; tmp++)
	{
		try
		{
			auto ig = crn::NewImageGrayFromFile(argv[tmp]);
			CRNVerbose(crn::String(U"*** ") + argv[tmp] + crn::String(U" ***"));
			CRNVerbose(crn::String(U"\tStokes width: ") + StrokesWidth(*ig));
			CRNVerbose(crn::String(U"\tStokes height: ") + StrokesHeight(*ig));
			CRNVerbose(crn::String(U"\tLines height: ") + EstimateLinesXHeight(*ig));
		}
		catch (std::exception &ex)
		{
			CRNVerbose(crn::String(U"*** ") + argv[tmp] + 
					crn::String(U" ignored (not a valid image file). *** ") + ex.what());
		}
	}
}

