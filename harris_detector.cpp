#include <iostream>
#include <opencv2/opencv.hpp>

const int Depth = CV_32F;

cv::Mat readImage(char* file_name)
{
	cv::Mat result = cv::imread(file_name);
	if(result.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		exit(0);
	}

	// cv::resize(result, result, cv::Size(256, 256));

	return result;
}

void calculateDerivatives(const cv::Mat & image, cv::Mat & dx, cv::Mat & dy)
{
	//calculate the derivativers
	cv::Sobel(image, dx, Depth, 1, 0);
	cv::Sobel(image, dy, Depth, 0, 1);
}

cv::Mat structureTensorMatrix(const cv::Mat & dx, const cv::Mat & dy)
{
	cv::Mat dx2, dy2, dxy;

	cv::multiply(dx, dx, dx2);
	cv::multiply(dy, dy, dy2);
	cv::multiply(dx, dy, dxy);

	const double sum_dx2 = cv::sum(dx2)[0];
	const double sum_dy2 = cv::sum(dy2)[0];
	const double sum_dxy = cv::sum(dxy)[0];

	double temp[2][2] = {{sum_dx2, sum_dxy}, {sum_dxy, sum_dy2}};

	cv::Mat tensor_matrix(2, 2, CV_32F, temp);

	return tensor_matrix;
}

void calculateHarrisResponse()

void harrisDetector(const cv::Mat & image)
{
	cv::Mat dx, dy;

	calculateDerivatives(image, dx, dy);

	cv::Mat tensor_matrix = structureTensorMatrix(dx, dy);
}

int main(int argc, char** argv)
{

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <Image>\n";
		return -1;
	}
	const cv::Mat image = readImage(argv[1]);

	cv::imshow("Original", image);
	// cv::imshow("GrayCurve", solarised.gray_curve_image);

	cv::waitKey();
	return 0;

}