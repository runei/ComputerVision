#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

const int Depth = CV_32F;
const int FilterSize = 3;
// const int MaxThreshold = 255;
const int thresh = 100; //  http://answers.opencv.org/question/91462/trackbar-pass-variable/

cv::Mat readImage(char* file_name)
{
	cv::Mat result = cv::imread(file_name);
	if(result.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		exit(0);
	}

	cv::resize(result, result, cv::Size(512, 512));

	return result;
}

void calculateDerivatives(const cv::Mat & image, cv::Mat & dx, cv::Mat & dy)
{
	//calculate the derivativers
	cv::Sobel(image, dx, Depth, 1, 0);
	cv::Sobel(image, dy, Depth, 0, 1);
}

cv::Mat structureTensorMatrix(const int i, const int j, const cv::Mat & dx2, const cv::Mat & dy2, const cv::Mat & dxy)
{
	const cv::Mat dx2_aux = dx2.rowRange(i - FilterSize, i + FilterSize).colRange(j - FilterSize, j + FilterSize);
	const cv::Mat dy2_aux = dy2.rowRange(i - FilterSize, i + FilterSize).colRange(j - FilterSize, j + FilterSize);
	const cv::Mat dxy_aux = dxy.rowRange(i - FilterSize, i + FilterSize).colRange(j - FilterSize, j + FilterSize);

	const double sum_dx2 = cv::sum(dx2)[0];
	const double sum_dy2 = cv::sum(dy2)[0];
	const double sum_dxy = cv::sum(dxy)[0];

	double temp[2][2] = {{sum_dx2, sum_dxy}, {sum_dxy, sum_dy2}};

	cv::Mat tensor_matrix(2, 2, CV_32F, temp);

	return tensor_matrix;
}

double calculateHarrisResponse(const cv::Mat & tensor_matrix)
{
	const static double k = 0.06; // defined empirically k = 0.04 or 0.06

	return cv::determinant(tensor_matrix) - k * std::pow(cv::trace(tensor_matrix)[0], 2.0);
}

void calculateProductsOfDerivatives(const cv::Mat & dx, const cv::Mat & dy, cv::Mat & dx2, cv::Mat & dy2, cv::Mat & dxy)
{
	cv::multiply(dx, dx, dx2);
	cv::multiply(dy, dy, dy2);
	cv::multiply(dx, dy, dxy);
}

cv::Mat harrisDetector(const cv::Mat & image)
{
	/*cv::Mat dx, dy;
	calculateDerivatives(image, dx, dy);

	cv::Mat dx2, dy2, dxy;
	calculateProductsOfDerivatives(dx, dy, dx2, dy2, dxy);

	cv::Mat final = cv::Mat::zeros(image.size(), CV_32FC1);


	for (int i = FilterSize; i < final.rows - FilterSize; ++i)
	{
		for (int j = FilterSize; j < final.cols - FilterSize; ++j)
		{
			const cv::Mat tensor_matrix = structureTensorMatrix(i, j, dx2, dy2, dxy);

			const double res = calculateHarrisResponse(tensor_matrix);

			if (res > Threshold)
			{
				final.at<float>(i, j) = res;
			}
		}
	}*/

	const static int block_size = 3;
	const static int ksize = 3;
	const static double k = 0.04;

	cv::Mat final = cv::Mat::zeros(image.size(), CV_32FC1);
	cv::cornerHarris(image, final, block_size, ksize, k);

	cv::Mat final_norm, final_norm_scaled;
	cv::normalize(final, final_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
	cv::convertScaleAbs(final_norm, final_norm_scaled);

	for (int i = 0; i < final_norm.rows; ++i)
	{
		for (int j = 0; j < final_norm.cols; ++j)
		{
			if ((int) final_norm.at<float>(i, j) > thresh)
			{
				cv::circle(final_norm_scaled, cv::Point(j, i), 5, cv::Scalar(0), 2, 8, 0);
			}
		}
	}

	return final_norm_scaled;
	// cv::Mat tensor_matrix = structureTensorMatrix(dx, dy);
}

int main(int argc, char** argv)
{

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <Image>\n";
		return -1;
	}
	const cv::Mat image = readImage(argv[1]);
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

	// const char* source_window = "Original";
	// createTrackbar ( "Threshold: ", source_window, &thresh, MaxThreshold, harrisDetector, gray_image );

	cv::Mat corners = harrisDetector(gray_image);

	cv::imshow("Original", image);
	cv::imshow("Final", corners);
	// cv::imshow("GrayCurve", solarised.gray_curve_image);



	cv::waitKey();
	return 0;

}