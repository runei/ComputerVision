#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#define SIZE_RESIZE 0.3

/*
	1. Install OpenCV with debug libs on your system.

	2. Take two photos of a static (not changing) object, about 3m away from two view points, with about 10cm horizontal distance.

	3. Measure the overall brightness of the two images and adjust it to the mean value of the two images.

	4. Create a mixed image and put the red channel from the (adjusted) left image and the green and blue channel from the right image into said mixed image. Do this by (i) looping over all the pixels, (ii) by using opencv functions – compare the runtime!

	5. View the mixed image with red-cyan glasses ;-)
*/

int getOverallBrightness(const cv::Mat & image)
{
	int result = 0;
	// cv::Mat temp;

	// cv::cvtColor(image, temp, cv::COLOR_BGR2HSV);

	// std::cout << image.channels() << "\n";
	// exit(0);


	// not efficient
	for (auto it = image.begin<cv::Vec3b>(), end = image.end<cv::Vec3b>(); it != end; ++it)
	{
		int temp = 0;
		for (int c = 0; c < image.channels(); ++c)
		{
			temp += (*it)[c];
		}
		result += temp / image.channels();
	}


	/*for (auto it = temp.begin<cv::Vec3b>(), end = temp.end<cv::Vec3b>(); it != end; ++it)
	{
		result += (*it)[2];
	}*/
	return result / image.total();
}

cv::Mat readImage(char* file_name)
{
	cv::Mat result = cv::imread(file_name);
	if(result.empty())
	{
		std::cout << "Could not open or find the image!\n" << std::endl;
		exit(0);
	}

	cv::resize(result, result, cv::Size(), SIZE_RESIZE, SIZE_RESIZE, cv::INTER_AREA);

	return result;
}

void adjustBrightness(cv::Mat & image1, cv::Mat & image2)
{
	const int bright1 = getOverallBrightness(image1);
	const int bright2 = getOverallBrightness(image2);

	const int mean_brightness = (bright1 + bright2) / 2;

	// std::cout << bright1 << "\t" << bright2 << "\t" << mean_brightness << std::endl;

	// cv::Mat new_image1 = cv::Mat::zeros( image1.size(), image1.type() );
	// cv::Mat new_image2 = cv::Mat::zeros( image2.size(), image2.type() );

	// image1.convertTo(new_image1, -1, 1, -50);
	image1.convertTo(image1, -1, 1, mean_brightness - bright1);
	image2.convertTo(image2, -1, 1, mean_brightness - bright2);

	/*const int brighttemp1 = getOverallBrightness(new_image1);
	const int brighttemp2 = getOverallBrightness(new_image2);

	std::cout << brighttemp1 << "\t" << brighttemp2 << "\t" << mean_brightness << std::endl;**/
}

cv::Mat mixImages(const cv::Mat & image1, const cv::Mat & image2)
{
	std::vector<cv::Mat> channels_1, channels_2;
	cv::Mat result;

	cv::split(image1, channels_1);
	cv::split(image2, channels_2);

	// const std::vector<cv::Mat> final_channel = {channels_2[0], channels_2[1], channels_1[2]};
	const std::vector<cv::Mat> final_channel = {channels_1[0], channels_1[1], channels_2[2]};
	cv::merge(final_channel, result);

	return result;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " <Image1> <Image2>\n";
		return -1;
	}

	cv::Mat img1 = readImage(argv[1]);
	cv::Mat img2 = readImage(argv[2]);

	adjustBrightness(img1, img2);

	const cv::Mat mixed_image = mixImages(img1, img2);
	// cv::imshow("Original Image 1", img1);
	cv::imshow("Final", mixed_image);

/*
	cv::imshow("New Image 1", new_image1);
	cv::imshow("Original Image 2", img2);
	cv::imshow("New Image 2", new_image2);
*/	cv::waitKey();

	return 0;
}