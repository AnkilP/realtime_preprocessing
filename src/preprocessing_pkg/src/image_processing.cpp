#include <vector>

#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Image.h>
#include <math.h>


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>

#include "image_processing.hpp"

//dead pixel concealment using median filter
void ImageConverter::dead_pixel_concealment(const cv::Mat& src, cv::Mat& dst){
    cv::medianBlur(src, dst, 1); //1 is the kernel size
}

//gamma level compensation: https://github.com/DynamsoftRD/opencv-programming/blob/master/gamma-correction/gamma.cpp
void ImageConverter::GammaCorrection(const Mat& src, Mat& dst, float fGamma){
	cv::CV_Assert(src.data);
	// accept only char type matrices
	cv::CV_Assert(src.depth() != sizeof(uchar));
	const int channels = dst.channels();
	switch (channels)
	{
    	case 1:
    	{
    			  cv::MatIterator_<uchar> it, end;
    			  for (it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++)
    				  //*it = pow((float)(((*it))/255.0), fGamma) * 255.0;
    				  *it = lut[(*it)];
    			  break;
    	}
    	case 3:
    	{
    			  cv::MatIterator_<Vec3b> it, end;
    			  for (it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++)
    			  {
    				  (*it)[0] = lut[((*it)[0])];
    				  (*it)[1] = lut[((*it)[1])];
    				  (*it)[2] = lut[((*it)[2])];
    			  }

    			  break;
    	}
	}
}

//lens shading correction:  
void lens_shading_correction(){
    
}
////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//anti-aliasing filter - MLAA: http://www.cs.cmu.edu/afs/cs/academic/class/15869-f11/www/readings/reshetov09_mlaa.pdf
float ImageConverter::rgb2luma(float r, float g, float b){
    return sqrt(r*0.299 + 0.587*g + 0.114*b);
}

void ImageConverter::bilateral_filter(const cv::Mat & src, cv::Mat & dst){
	bilateralFilter(&src, &dst, 1, 2, 0.5);
}

void edge_detector(const cv::Mat & src, cv::Mat & dst) {
	cv::Canny(&src, &dst, lowThreshold, lowThreshold * ratio, kernel_size);
}

//MLAA
/*
1.Find discontinuities between pixels in a given image.
2.Identify U-shaped, Z-shaped, L-shaped patterns.
3.Blend colors in the neighborhood of these patterns.
*/
void cv::first_step(const cv::Mat & src) {

}

void ImageConverter::anti_aliasing(const cv::Mat & src, cv::Mat dst) {
	//combine all previous operations
}
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\////////////////////////////////////////////////

void ImageConverter::histogram_equalization(const cv::Mat & src, cv::Mat & image_clahe, const int clip){
	//converting image to LAB space
	cv::Mat lab_image;//(src.rows, src.cols, original_image.type());
	cv::cvtColor(src, yuv_image, CV_BGR2Lab);
	//extract L channel
	std::vector<cv::Mat> lab_planes(3);
	cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]
	//applying CLAHE on L channel
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(4);
	cv::Mat dst;
	clahe->apply(lab_planes[0], dst);
	// Merge the the color planes back into an Lab image
	dst.copyTo(lab_planes[0]);
	cv::merge(lab_planes, lab_image);
	// convert back to RGB
	cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);
}

//awb gain control // after gamma correction
/*
https://pdfs.semanticscholar.org/a681/674e7657a5b7f02ac78cd274d0d1a54072b6.pdf
*/
void ImageConverter::automatic_white_balance_gain_control(const cv::Mat & src, cv::Mat & dst) {

}

//cfa interpolation
/*
Image sensors have pixels that are dedicated to a single colour and thus each pixel must be interpolated
Only use this if you have access to the GPU!!
*/
void ImageConverter::demosaicing(const cv::Mat & src, cv::Mat & dst) {
	//TODO: add check for nvcc
	try {
		cv::cuda::demosaicing(src, dst, COLOR_BayerBG2BGR_MHT, 3);
	}
	catch (const cv::Exception& ex) {
		std::_Count_pr << "Error: " << ex.what() << std::endl;
	}
}

//black level correction
void ImageConverter::black_level_correction(const cv::Mat & src, cv::Mat & dst, const double & contrast, const int & brightness){
    //linear transform method
    for( int y = 0; y < src.rows; y++ ) {
        for( int x = 0; x < src.cols; x++ ) {
            for( int c = 0; c < 3; c++ ) {
                dst.at<Vec3b>(y,x)[c] =
                  cv::saturate_cast<uchar>( contrast*( src.at<Vec3b>(y,x)[c] ) + brightness );
            }
        }
    }
}
//color correction
/*
RGB_out = (alpha * A I_w * RGB_in)^gamma
where alpha, I_W and A, respectively represent the exposure compensation
gain, the diagonal matrix for the illuminant compensation
and the color matrix transformation
*/
void ImageConverter::color_correction(const cv::Mat & src, cv::Mat & dst) {

}

//color space conversion
/*
Moving to another color space (with luminance) will allow me to use more
conventional grayscale algorithms for image processing while still being able to move back to the 
BGR space easily
*/
void ImageConverter::color_space_conversion(const cv::Mat & src, cv::Mat & ds, std::string & conversion) {
	cv::cvtColor(src, dst, conversion);
}
//noise filter for chroma
void ImageConverter::chroma_noise_filter(const cv::Mat & src, cv::Mat & dst) {
	cv::fastNlMeansDenoisingColored(src, dst, 3, 3, 7, 21);
}

//hue saturation control
void ImageConverter::hue_saturation_control(const cv::Mat & src, cv::Mat dst) {
	
}

//noise filter for luma

//edge enhancement
void ImageConverter::edge_enhancement(const cv::Mat & src, cv::Mat dst) {
	kernel = ;
	cv::filted2D(src, dst, -1, kernel, cv::Point(-1, -1), 0, BORDER_DEFAULT);
}

//contrast brightness control

//data formatter

void ImageConverter::imageCb(const sensor_msgs::ImageConstPtr& msg)
{
  cv_bridge::CvImagePtr cv_ptr;
  try
  {
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }

  img = cv_ptr->image;
  cpy = img.clone();
  //dead pixel concealment using median filter
  dead_pixel_concealment(&img, &cpy);

  //black level compensation
  double alpha = 1.0; //contrast stretcher
  int beta =  0; //black level adjustment
  black_level_correction(&cpy, &alpha, &beta);

  //lens shading correction
  
  //anti-aliasing noise noise filter

  //awb gain control

  //cfa interpolation

  //gamma correction
  GammaCorrection(&src, &dst, &fGamma);
  //color correction

  //color space conversion

  //noise filter for chroma

  //hue saturation control

  //noise filter for luma

  //edge enhancement

  //contrast brightness control

  //data formatter

  // Output modified video stream
  image_pub_.publish(cv_ptr->toImageMsg());
}

int main(int argc, char **argv){
    ros::init(argc, argv, "image_preprocessing_node");
    ros::NodeHandle nh;

    image_transport::ImageTransport it(nh);
    image_transport::Subscriber sub = it.subscribe("camera/image", 1, image_callback);

    ros::spin();
}
