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

//anti-aliasing noise filter: http://blog.simonrodriguez.fr/articles/30-07-2016_implementing_fxaa.html
float ImageConverter::rgb2luma(float r, float g, float b){
    return sqrt(r*0.299 + 0.587*g + 0.114*b);
}

void ImageConverter::bilateral_filter(const cv::Mat & src, cv::Mat dst){
    
}

//awb gain control

//cfa interpolation

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

//color space conversion

//noise filter for chroma

//hue saturation control

//noise filter for luma

//edge enhancement

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
