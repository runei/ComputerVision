#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

const int MaxPoints = 2;

typedef class Transform
{
public:
	double a;
	double b;
	double c;
	cv::Point2d p0;

	Transform(const cv::Point2d &, const cv::Point2d &);
	double solarise(const double);

} Transform;

Transform::Transform(const cv::Point2d & p1, const cv::Point2d & p2)
{
	/*
		y = a(x - x_0)³ - bx + c
		y_1 = a(x_1 - x_0)³ - bx_1 + c
		y_2 = a(x_2 - x_0)³ - bx_2 + c

		derivative of the function
		0 = 3a(x - x_0)² - b
		0 = 3a(x_1 - x_0)² - b
		0 = 3a(x_2 - x_0)² - b

		b/3a = (x_1 - x_0)² = (x_2 - x_0)
		x_1 - x_0 = x_0 - x_2
		2x_0 = x_1 + x_2
		x_0 = (x_1 + x_2) / 2

		y_1 - y_2 = (b/3)*((x_1 - x_0) - (x_2 - x_0)) - b(x_1 - x_2)
		y_1 - y_2 = (b/3)*(x_1 - x_2) - b(x_1 - x_2)
		y_1 - y_2 = -2/3*b * (x_1 - x_2)


		->b
		y_1 - y_2 = -2/3*b * (x_1 - x_2)
		-2/3*b = (y_1 - y_2) / (x_1 - x_2)
		b = -3/2 * (y_1 - y_2) / (x_1 - x_2)

		->a
		b/3a = (x_1 - x_0)²
		a = b / 3 / (x_1 - x_0)²
		a = -3/2 * (y_1 - y_2) / (x_1 - x_2) / 3 / (x_1 - ((x_1 + x_2) / 2))²
		a = -(2* (y_1 - y_2)) / (x_1 - x_2)³ #wolframalpha

		->c
	*/


	const cv::Point2d diff = p1 - p2;
	this->p0 = (p1 + p2) / 2;

	this->a = (-2.0*diff.y / pow(diff.x, 3.0));
	this->b = (-3.0 / 2.0 * diff.y / diff.x);
	this->c = ((p1.y + p2.y) / 2.0 + b*diff.x);
}

double Transform::solarise(const double x)
{
	double y = this->a * std::pow((x - this->p0.x), 3.0) - this->b * x + this->c;
	if (y > 255.0)
	{
		y = 255;
	}
	else if (y < 0.0)
	{
		y = 0;
	}
	return y;
}

typedef struct SolarisedImage
{
	// cv::Mat gray_curve_image;
	cv::Mat image;
	std::vector<cv::Point2d> points;

	SolarisedImage(const cv::Mat &);
} SolarisedImage;

SolarisedImage::SolarisedImage(const cv::Mat & _image) //: gray_curve_image(_image.size(), CV_8U)
{
	this->image = _image.clone();
}

cv::Mat readImage(char* file_name)
{
	cv::Mat result = cv::imread(file_name);
	if(result.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		exit(0);
	}

	cv::resize(result, result, cv::Size(256, 256));

	return result;
}

static void onMouse(int event, int x, int y, int flags, void* params)
{
	if (event != cv::EVENT_LBUTTONDOWN) return;

	SolarisedImage* solarised = static_cast<SolarisedImage*>(params);

	if (solarised->points.size() < MaxPoints)
	{
		solarised->points.push_back(cv::Point2d(x, y));

		if (solarised->points.size() == MaxPoints)
		{
			Transform transform(solarised->points[0], solarised->points[1]);


			cv::Mat solarise_lut(1, 256, CV_8U);
			for (int i = 0; i < 256; ++i)
			{
				solarise_lut.at<uchar>(i) = transform.solarise(i);
			}

/*			cv::Mat gray(solarised->image.size(), CV_8U);
			gray = 0;
			cv::Point2d ant(0, transform.solarise(0));
			for (int i = 1; i < 256; ++i)
			{
				// std::cout << transform.solarise(i) << "\n";
				cv::Point2d np(i, transform.solarise(i));
				cv::line(gray, ant, np, CV_RGB(255, 255, 255));
				ant = np;
			}
*/
			cv::Mat final;
			cv::LUT(solarised->image, solarise_lut, final);
			cv::imshow("Final", final);

			// cv::line(solarised->gray_curve_image, solarised->points[0], solarised->points[1], CV_RGB(255, 255, 255));
			// cv::imshow("GrayCurve", gray);
			solarised->points.clear();
		}
	}
}

int main(int argc, char** argv)
{

	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <Image>\n";
		return -1;
	}
	const cv::Mat image = readImage(argv[1]);

	SolarisedImage solarised(image);

	cv::imshow("Original", image);
	// cv::imshow("GrayCurve", solarised.gray_curve_image);

	cv::setMouseCallback("Original", onMouse, &solarised);

	cv::waitKey();
	return 0;

}