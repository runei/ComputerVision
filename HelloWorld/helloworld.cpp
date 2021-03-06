#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        std::cout << "usage: ./helloworld <Image_Path>\n";
        return -1;
    }
    cv::Mat image;
    image = cv::imread( argv[1], 1 );
    if ( !image.data )
    {
        std::cout << "No image data \n";
        return -1;
    }
    namedWindow("Hello Lena", cv::WINDOW_AUTOSIZE );
    imshow("Hello Lena", image);
    cv::waitKey(0);
    return 0;
}