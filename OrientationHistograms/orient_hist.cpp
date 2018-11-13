#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

const int Depth = CV_32F;
const int Bins = 9;
const double BinInterval = 180.0 / Bins;

int isInteger(double n)
{
	return std::trunc(n) == n;
}

cv::Mat readImage(char* file_name)
{
	cv::Mat result = cv::imread(file_name, 0);
	if(result.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		exit(0);
	}

	result.convertTo(result, Depth);

	// cv::resize(result, result, cv::Size(256, 256));

	return result;
}

void createHistogram(const cv::Mat & magnitude, const cv::Mat & angle)
{
	std::vector<double> result(Bins, 0.0);
	std::ofstream text_file("histogram.txt");

	// text_file << angle;

	for (int i = 0; i < angle.rows; ++i)
	{
		for (int j = 0; j < angle.cols; ++j)
		{
			const double val_angle = angle.at<float>(i, j) / BinInterval;
			const int low = std::floor(val_angle);
			const int high = std::ceil(val_angle);
			const double perc_low = 1.0 - (val_angle - low);

			// text_file << magnitude.at<double>(i, j) << "\t" << val_angle << "\n";

			result[low] += magnitude.at<float>(i, j) * perc_low;
			result[high % Bins] += magnitude.at<float>(i, j) * (1 - perc_low);
		}
	}

	for (int i = 0; i < result.size(); ++i)
	{
		text_file << (i * BinInterval) << "\t" << result[i] << "\n";
	}

	// return result;
}

void createGradient(const cv::Mat & image)
{
	//calculate gradient
	cv::Mat gx, gy, abs_gx, abs_gy;
	cv::Sobel(image, gx, Depth, 1, 0, 1);
	cv::Sobel(image, gy, Depth, 0, 1, 1);
	cv::convertScaleAbs(gx, abs_gx);
	cv::convertScaleAbs(gy, abs_gy);
	abs_gx.convertTo(abs_gx, Depth);
	abs_gx.convertTo(abs_gy, Depth);

	// magnitude and direction
	cv::Mat magnitude, angle;
	cv::cartToPolar(abs_gx, abs_gy, magnitude, angle, true);

	// std::cout << angle;

	createHistogram(magnitude, angle);

	/*
	//gradient X
	cv::Mat grad_x, abs_grad_x;
	cv::Sobel(image, grad_x, CV_16S, 1, 0);

	//gradient Y
	cv::Mat grad_y, abs_grad_y;
	cv::Sobel(image, grad_y, CV_16S, 0, 1);
	cv::convertScaleAbs(grad_y, abs_grad_y);

	//absolute gradient result
	cv::Mat final_grad;
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, final_grad);

	//orientation
	cv::Mat orientation = cv::Mat::zeros(abs_grad_x.rows, abs_grad_y.cols, CV_32F);
	grad_x.convertTo(grad_x, CV_32F);
	grad_y.convertTo(grad_y, CV_32F);
	cv::phase(grad_x, grad_y, orientation);
	cv::normalize(orientation, orientation, 0x00, 0xFF, cv::NORM_MINMAX, CV_8U);

	getHistogram(orientation);
*/
	//show images
	cv::imshow("original", image);
	cv::imshow("magnitude", magnitude);
	cv::imshow("angle", angle);
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <Image>\n";
		return -1;
	}
	const cv::Mat image = readImage(argv[1]);

	createGradient(image);

	cv::waitKey();

	return 0;
}