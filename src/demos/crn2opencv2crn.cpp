//#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
//using namespace std;

int opencv(int argc, char *argv[])
{
	// check argument
	if (argc < 2)
	{
		printf("Usage: %s <document_image_name>\n", argv[0]);
		return -1;
	}

	Mat src, dst;

	// load image
	src = imread(argv[1], /*CV_LOAD_IMAGE_COLOR*/CV_LOAD_IMAGE_GRAYSCALE);

	if (!src.data) // check for invalid input
	{
		std::cout << "Cannot open document image" << std::endl;
		return -2;
	}

	// apply bilateral filter
	bilateralFilter(src, dst, 15, 80, 80);

	// apply Gaussian filter
	//GaussianBlur(src, dst, Size(5, 5), 0, 0);

	// apply median filter
	//medianBlur(src, dst, 15);

	// display image
	imshow("source", src);
	imshow("result", dst);

	// save image
	imwrite("result.jpg", dst); // it will store the image in name "result.jpg"

	waitKey(0); // wait for a keystroke in the window

	return 0;
}

//#include <CRNBlock.h>
//#include <CRNFeature/CRNFeatureSet.h>
//#include <CRNFeature/CRNFeatureExtractorProfile.h>
//#include <CRNFeature/CRNFeatureExtractorProjection.h>
//#include <CRNFeature/CRNBlockTreeExtractorTextLinesFromProjection.h>
//#include <CRNAI/CRNBasicClassify.h>

#include <CRNIO/CRNIO.h>
#include <CRNUtils/CRNTimer.h>
#include <CRNImage/CRNImageGray.h>

//using namespace crn::literals;

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

	crn::Timer::Start(U"crn2opencv2crn");

	////////////////////////////////////////////////////////////////////////////
	// 1. Document                                                            //
	////////////////////////////////////////////////////////////////////////////
	// Open the document image file.
	auto imageFileName = crn::Path(argv[1]);
	auto pageimage = crn::SImage{};
	try
	{
		pageimage	= crn::NewImageFromFile(imageFileName);

		opencv(argc, argv); // TEMP
	}
	catch (...)
	{
		CRNError(U"Cannot open document image");
		return -2;
	}

	crn::Timer::Split(U"crn2opencv2crn", U"Document");
	CRNVerbose(crn::Timer::Stats(U"crn2opencv2crn"));

	return 0;
}
