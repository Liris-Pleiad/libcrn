#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <CRNWrapper/CRNCV.h>
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
	
	// 'make' a gray image
	crn::SBlock b(crn::Block::New(pageimage));

	{
		crn::SImageGray ig(b->GetGray());

		// [OpenCV] : import the gray image from libcrn
		auto src = crn::WrapCVMat(*ig);

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

		crn::Timer::Split(U"crn2opencv2crn", U"Gray");
	}

	{
		auto irgb = b->GetRGB();

		// [OpenCV] : import the gray image from libcrn
		auto src = crn::WrapCVMat(*irgb);

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
		irgb->SavePNG("rgb_img_src.png");

		// [OpenCV] : copy result image to source image
		dst.copyTo(src);

		// save the result gray image
		irgb->SavePNG("rgb_img_dst_after_OpenCV_bilateral_filter.png");

		crn::Timer::Split(U"crn2opencv2crn", U"RGB");
	}

	CRNVerbose(crn::Timer::Stats(U"crn2opencv2crn"));

	return 0;
}
