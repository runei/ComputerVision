#include <iostream>
#include <opencv2/opencv.hpp>

const double Threshold = 0.9;

cv::Mat readImage(char* file_name)
{
	cv::Mat result = cv::imread(file_name);
	if(result.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		exit(0);
	}

	// cv::resize(result, result, cv::Size(512, 512));

	return result;
}

cv::Mat matchImages(const cv::Mat & image, const cv::Mat & templ, const cv::TemplateMatchModes & match_mode)
{

	cv::Mat img_display(image.clone());

	const int result_cols = image.cols - templ.cols + 1;
	const int result_rows = image.rows - templ.rows + 1;

	cv::Mat result(result_cols, result_rows, CV_32FC1);

	cv::matchTemplate(image, templ, result, match_mode);
	cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	cv::threshold(result, result, Threshold, 1.0, cv::THRESH_TOZERO);

	while (true)
	{
		double min_val, max_val;
		cv::Point min_loc, max_loc, match_loc;

		cv::minMaxLoc(result, &min_val, &max_val, &min_loc, &max_loc, cv::Mat());

		if (match_mode == cv::TM_SQDIFF || match_mode == cv::TM_SQDIFF_NORMED)
		{
			match_loc = min_loc;
		}
		else
		{
			match_loc = max_loc;
		}

		if (max_val >= Threshold)
		{
			cv::rectangle(img_display, match_loc, cv::Point(match_loc.x + templ.cols, match_loc.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
			cv::rectangle(result, match_loc, cv::Point(match_loc.x + templ.cols, match_loc.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
		}
		else
		{
			break;
		}
	}


	cv::imshow("Image", img_display);
	cv::imshow("Result", result);

	return result;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " <Image> <Template>\n";
		return -1;
	}

	const cv::Mat image = readImage(argv[1]);
	const cv::Mat templ = readImage(argv[2]);

	matchImages(image, templ, cv::TM_CCOEFF_NORMED);

	// cv::imshow("Img", image);
	cv::imshow("templ", templ);
	// cv::imshow("Result", result);

	cv::waitKey();

	return 0;
}