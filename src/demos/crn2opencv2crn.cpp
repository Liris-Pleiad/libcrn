#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

int opencv(int argc, char *argv[])
{
	// check argument
	if (argc < 2)
	{
		printf("Usage: %s <document_image_name>\n", argv[0]);
		return -1;
	}

	cv::Mat src, dst;

	// load image
	src = cv::imread(argv[1], /*CV_LOAD_IMAGE_COLOR*/CV_LOAD_IMAGE_GRAYSCALE);

	if (!src.data) // check for invalid input
	{
		std::cout << "Cannot open document image" << std::endl;
		return -2;
	}

	// apply bilateral filter
	cv::bilateralFilter(src, dst, 15, 80, 80);

	// apply Gaussian filter
	//cv::GaussianBlur(src, dst, Size(5, 5), 0, 0);

	// apply median filter
	//cv::medianBlur(src, dst, 15);

	// display images
	cv::imshow("source", src);
	cv::imshow("result", dst);

	// save image
	cv::imwrite("result.jpg", dst); // it will store the image in name "result.jpg"

	cv::waitKey(0); // wait for a keystroke in the window

	return 0;
}

#include <CRNIO/CRNIO.h>
#include <CRNUtils/CRNTimer.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNBlock.h>

int main(int argc, char *argv[])
{
	// check argument
	if (argc < 2)
	{
		printf("Usage: %s <document_image_name>\n", argv[0]);
		return -1;
	}
	crn::IO::IsVerbose() = true;
	crn::IO::IsQuiet() = false;

	crn::Timer::Start(U"crn2opencv2crn");

	// open the document image file
	auto imageFileName = crn::Path(argv[1]);
	auto pageimage = crn::SImage{};
	try
	{
		pageimage = crn::NewImageFromFile(imageFileName);
	}
	catch (...)
	{
		CRNError(U"Cannot open document image");
		return -2;
	}

	//opencv(argc, argv); // TEMP
	
	// 'make' a gray image
	crn::SBlock b(crn::Block::New(pageimage));
	crn::SImageGray ig(b->GetGray());

	// [OpenCV] : import the gray image from libcrn
	cv::Mat src(ig->GetHeight(), ig->GetWidth(), CV_8UC1, (unsigned char*)ig->GetPixels());

	// [OpenCV] : apply a bilateral filter
	cv::Mat dst;
	cv::bilateralFilter(src, dst, 15, 80, 80);

#if(0)
	// [OpenCV] : display images
	cv::imshow("source", src);
	cv::imshow("result", dst);

	cv::waitKey(0); // wait for a keystroke in the window
#endif
	
	// save the source gray image
	ig->SavePNG("gray_img_src.png");
	
	// [OpenCV] : copy result image to source image
	dst.copyTo(src);

	// save the result gray image
	ig->SavePNG("gray_img_dst_after_OpenCV_bilateral_filter.png");

	crn::Timer::Split(U"crn2opencv2crn", U"Document");
	CRNVerbose(crn::Timer::Stats(U"crn2opencv2crn"));

	return 0;
}
