/* Copyright 2009-2016 INSA Lyon, CoReNum
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
 * file: ocr4dummies.cpp
 * 
 * A quick OCR engine to recognize capital roman letters.
 * To keep the example short, it is designed for old Latin documents that 
 * have no spacing between words and have well separated lines of text.
 * 
 * Compile with:
g++ `pkg-config --cflags --libs libcrn` ocr4dummies.cpp -o ocr4dummies
 * \author Yann LEYDIER
 */

#include <CRN.h>
#include <fstream>
#include <cstdlib>
#include <CRNConfig.h>
#include <CRNBlock.h>
#include <CRNException.h>
#include <CRNImage/CRNImageRGB.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNImage/CRNImageBW.h>
#include <CRNFeature/CRNFeatureSet.h>
#include <CRNFeature/CRNFeatureExtractorProfile.h>
#include <CRNFeature/CRNFeatureExtractorProjection.h>
#include <CRNMath/CRNMatrixInt.h>
#include <CRNAI/CRNBasicClassify.h>
#include <CRNUtils/CRNTimer.h>
#include <CRNIO/CRNIO.h>

using namespace crn::literals;

int main(int argc, char *argv[])
{
	// Check argument.
	if (argc < 2)
	{
		printf("Usage: %s <document_image_name>\n", argv[0]);
		return -1;
	}
	crn::IO::IsVerbose() = true;
	crn::IO::IsQuiet() = false;

	crn::Timer::Start(U"OCR4dummies");

	/**************************************************************************/
	/* 1. Database                                                            */
	/**************************************************************************/
	// Create a feature extraction engine.
	auto featureExtractor = std::make_unique<crn::FeatureSet>();
	// It will extract the four profiles, reduced each to 10 values in [0..100].
	featureExtractor->PushBack(std::make_shared<crn::FeatureExtractorProfile>(
				crn::Direction::LEFT | crn::Direction::RIGHT | 
				crn::Direction::TOP | crn::Direction::BOTTOM, 10, 100));
	// It will also extract the two projections under the same conditions.
	featureExtractor->PushBack(std::make_shared<crn::FeatureExtractorProjection>(
				crn::Orientation::HORIZONTAL | crn::Orientation::VERTICAL, 10, 100));
	// Create the database.
	auto database = std::make_unique<crn::Vector>();
	// For each capital roman character...
	for (auto c = 'A'; c <= 'Z'; ++c)
	{
		// Open a prototype image stored as "data/A.png", "data/B.png", etc.
		const auto charFileName = "data"_p / c + ".png"_p;
		auto charimage = crn::SImage{};
		try
		{
			charimage = crn::NewImageFromFile(charFileName);
		}
		catch (...)
		{
			CRNError(U"Cannot open database.");
			return -2;
		}
		// Embed the image in a block structure.
		auto charblock = crn::Block::New(charimage);
		// Extract the features and store it in the database.
		database->PushBack(featureExtractor->Extract(*charblock));
	}
	crn::Timer::Split(U"OCR4dummies", U"Database");
	/**************************************************************************/
	/* 2. Document                                                            */
	/**************************************************************************/
	// Open the document image file.
	auto imageFileName = crn::Path(argv[1]);
	auto pageimage = crn::SImage{};
	try
	{
		pageimage	= crn::NewImageFromFile(imageFileName);
	}
	catch (...)
	{
		CRNError(U"Cannot open document image");
		return -3;
	}
	/**************************************************************************/
	/* 2.1 Segmentation                                                       */
	/**************************************************************************/
	// Embed the image in a block structure.
	auto pageblock = crn::Block::New(pageimage);
	// Estimate the mean stroke width.
	const auto sw = StrokesWidth(*pageblock->GetGray());
	// Now we will agglomerate the characters on each line to extract them:
	// Create a line matrix filled with 1.
	auto xsmear = crn::MatrixInt(1, 6 * sw + 1, 1);
	// Smear the black and white version of the image.
	pageblock->GetBW()->Dilate(xsmear);
	// Extract connected components (lines).
	pageblock->ExtractCC(U"Lines");
	// Remove false detections (very small lines).
	pageblock->FilterMinOr(U"Lines", 2*sw, 2*sw);
	// Sort lines from top to bottom.
	pageblock->SortTree(U"Lines", crn::Direction::TOP);
	// Reset the smeared black and white image.
	pageblock->FlushBW();
	/**************************************************************************/
	/* 2.2 Recognition                                                        */
	/**************************************************************************/
	auto s = crn::String{};
	// For each line...
	for (auto oline : pageblock->GetTree(U"Lines"))
	{
		auto line = std::static_pointer_cast<crn::Block>(oline);
		// Extract connected components in the line (characters).
		// To do that, a new black and white image (that is not smeared) 
		// is automatically computed.
		line->ExtractCC(U"Characters");
		// Remove false detections (spots and dots).
		line->FilterMinOr(U"Characters", sw, sw);
		// Sort characters from left to right.
		line->SortTree(U"Characters", crn::Direction::LEFT);
		// For each character...
		for (auto ocharacter : line->GetTree(U"Characters"))
		{
			auto character = std::static_pointer_cast<crn::Block>(ocharacter);
			// Extract the features.
			auto features = featureExtractor->Extract(*character);
			// Classify the character using the database.
			auto res = crn::BasicClassify::NearestNeighbor(features, database->begin(), database->end());
			// Print the characters label.
			s += char32_t(U'A' + res.class_id);
		}
		// End of line
		s += U'\n';
	}
	CRNVerbose(s);
	crn::Timer::Split(U"OCR4dummies", U"Recognition");
	CRNVerbose(crn::Timer::Stats(U"OCR4dummies"));

	return 0;
}

