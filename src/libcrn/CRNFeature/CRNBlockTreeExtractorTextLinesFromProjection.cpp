/* Copyright 2009-2014 CoReNum, INSA-Lyon
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
 * file: CRNBlockTreeExtractorTextLinesFromProjection.cpp
 * \author Jean DUONG
 */

#include <CRNFeature/CRNBlockTreeExtractorTextLinesFromProjection.h>
#include <CRNImage/CRNImageBW.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNData/CRNInt.h>
#include <CRNBlock.h>
#include <functional>
#include <limits>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*! Computes a child-tree on a block 
 * \throws	ExceptionInvalidArgument	null block
 * \param[in] b text block from which text lines will be extracted
 */
void BlockTreeExtractorTextLinesFromProjection::Extract(Block &b)
{
	if (!b.HasTree(connectedComponentTreeName))
	{
		// Check if block contains a tree of connected components
		// If not, perform a component extraction
		b.ExtractCC(connectedComponentTreeName);
			
		// Drop connected components having width AND height smaller than 2
		b.FilterMinAnd (connectedComponentTreeName, 2, 2);
	}

	size_t nbCCs = b.GetNbChildren(connectedComponentTreeName);

	if (nbCCs == 0)
	{
		// No connected component in this block => no text line !!!
		// Nothing to be done but no error. Just return.
		return;
	}

	/****** General case ******/

	/*** Key idea : histogram average smoothing and mode detection ***/	

	// Step 1 : compute some thresholds
	
	// Average value for connected component height

	SVector ccList = b.GetTree(connectedComponentTreeName);
	double avgCCHeight = 0.0;
	
	for (SObject ccBlock : ccList)
	{
		avgCCHeight += std::static_pointer_cast<Block>(ccBlock)->GetAbsoluteBBox().GetHeight();
	}

	// Size of the sliding window for projection smoothing
	// Also threshold for acceptable mode proximity
	avgCCHeight /= double(ccList->Size());
	
	// Step 2 : perform horizontal projection
	
	auto hProj = HorizontalProjection(*b.GetBW());

	// Step 3 : smoothing histogram until modes get mutually far enough
	
	bool repeat = true;
	int smooting_parameter = (int) avgCCHeight / 2;
	
	while(repeat)
	{
		repeat = false;
		hProj.AverageSmoothing(smooting_parameter);
		std::vector<size_t> modes = hProj.Modes();
		size_t nbModes = modes.size();
		size_t i = 0;
		
		while ((i < nbModes - 1) && (!repeat))
		{
			size_t j = i + 1;
			
			while ((j < nbModes) && (!repeat))
			{
				repeat = Abs((double)modes[i] - double(modes[j])) < avgCCHeight;
				j++;
			}
			
			i++;
		}
		
	}
	
	// [optional] Make image to display histogram
	// hProj.MakeImageBW(hProj.Size())->SavePNG(U"histo.png");
	
	// Step 4 : mode detection
	
	std::vector<size_t> modes = hProj.Modes();		
	
	// Step 5 : gather connected components and extend 
	//          rectangles to get text line bounding boxes

	size_t nbModes = modes.size();
	std::vector<Rect> growing_rectangles;
	
	// Seeding text line bounding boxes
	for (size_t k = 0; k < nbModes; k++)
	{
		growing_rectangles.push_back(Rect());
	}
	
	// Explore all connected components and 
	// "stretch" text line bounding boxes
	for (SObject ccobj : ccList)
	{
		SBlock ccBlock(std::static_pointer_cast<Block>(ccobj));
		crn::Rect ccRect = ccBlock->GetAbsoluteBBox();
		int ccBlockCenterY = ccRect.GetCenterY();
		int dist = std::numeric_limits<int>::max();
		size_t argmin = 0;
		int nb_crossing = 0;
		
		// For current connected component, 
		// find the closest text line bounding box
		for (size_t k = 0; k < nbModes; k++)
		{
			size_t modeCenterY = modes[k];
			int d = Abs((int)modeCenterY - ccBlockCenterY);
			
			if (d < dist)
			{
				dist = d;
				argmin = k;
			}
			
			if ((ccRect.GetTop() <= int(modeCenterY)) && (ccRect.GetBottom() >= int(modeCenterY)))
			{
				nb_crossing++;
			}
		}
		
		// XXX Heuristic !!!
		if (nb_crossing < 2)
		{
			if (growing_rectangles[argmin].GetArea() == 0)
			{
				// Rectangle not processed yet !
				growing_rectangles[argmin] = ccBlock->GetAbsoluteBBox();
			}
			else
			{
				// Stretch the bounding box corresponding to the text line
				growing_rectangles[argmin] |= ccBlock->GetAbsoluteBBox();
			}
		}	
	}
	// remove invalid rectangles
	growing_rectangles.erase(std::remove_if(
				growing_rectangles.begin(), growing_rectangles.end(),
				std::not1(std::mem_fun_ref(&Rect::IsValid))), 
			growing_rectangles.end());
	
	// Step 6 : estimate baseline for each text line
	
	// Retrieve unsmoothed projection histogram
	
	hProj = HorizontalProjection(*b.GetBW());
	
	std::vector< int > baselines;
	std::vector< int > x_heights;
	
	for (Rect &r : growing_rectangles)
	{
		int r_top = r.GetTop();
		int r_height = r.GetHeight();
		int baseline = r_top;
		int x_height = 0;
		int hProj_index = r_top;
		
		// Histogram for image portion corresponding to current text line
		SHistogram r_hProj = std::make_shared<Histogram>(r_height);
		
		for (int k = 0; k < r_height; k++)
		{
			r_hProj->SetBin(k, hProj.GetBin(hProj_index));
			hProj_index++;
		}
		
		// Get modes and extract the two greatest ones
				
		const std::vector<size_t> modes = r_hProj->Modes();
		
		unsigned int max_main = 0;
		unsigned int max_secundary = 0;
		size_t argmax_main = 0;
		size_t argmax_secundary = 0;
		
		for (size_t k = 0; k < modes.size(); k++)
		{
			unsigned int value = r_hProj->GetBin(modes[k]);
						
			if (value > max_secundary)
			{
				if (value > max_main)
				{
					max_secundary = max_main;
					argmax_secundary = argmax_main;
					max_main = value;
					argmax_main = modes[k];
				}
				else
				{
					max_secundary = value;
					argmax_secundary = modes[k];
				}
			}
		}
		
		x_height = Abs(int(argmax_main) - int(argmax_secundary));
		
		if (argmax_main > argmax_secundary)
		{
			baseline += int(argmax_main);
		}
		else
		{
			baseline += int(argmax_secundary);
		}
		
		baselines.push_back(baseline);
		x_heights.push_back(x_height);
	}
	
	// Step 7 : store text line bounding boxes
	
	if (b.HasTree(GetTreeName()))
	{
		// If the tree containing text lines already exists, purge it !
		b.RemoveTree(GetTreeName());
	}
	
	for (size_t k = 0; k < growing_rectangles.size(); k++)
	{
		SBlock rct = b.AddChildRelative(GetTreeName(), growing_rectangles[k]);
		rct->SetUserData(U"baseline", std::make_shared<Int>(baselines[k]));
		rct->SetUserData(U"x-height", std::make_shared<Int>(x_heights[k]));
	}
	
	b.SetUserData(U"nb_textlines", std::make_shared<Int>(int(growing_rectangles.size())));
	
}

/*****************************************************************************/

/*! 
 * Initializes the object from an XML element. Unsafe. 
 * \throws	ExceptionInvalidArgument	not a FeatureExtractorProjection
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 */
void BlockTreeExtractorTextLinesFromProjection::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool BlockTreeExtractorTextLinesFromProjection::deserialize("
					"xml::Element &el): ") + _("Wrong XML element."));
	}
	StringUTF8 ltn = el.GetAttribute<StringUTF8>("lineTreeName", false); // may throw
	StringUTF8 cctn = el.GetAttribute<StringUTF8>("ccTreeName", false); // may throw
	lineTreeName = ltn;
	connectedComponentTreeName = cctn;
}

/*****************************************************************************/
/*! 
 * Dumps the object to an XML element. Unsafe. 
 */
xml::Element BlockTreeExtractorTextLinesFromProjection::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("lineTreeName", lineTreeName.CStr());
	el.SetAttribute("ccTreeName", connectedComponentTreeName.CStr());
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(BlockTreeExtractorTextLinesFromProjection)
	CRN_DATA_FACTORY_REGISTER(U"BlockTreeExtractorTextLinesFromProjection", BlockTreeExtractorTextLinesFromProjection)
CRN_END_CLASS_CONSTRUCTOR(BlockTreeExtractorTextLinesFromProjection)


