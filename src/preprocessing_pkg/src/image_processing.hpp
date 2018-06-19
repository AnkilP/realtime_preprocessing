#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

//static const std::string OPENCV_WINDOW = "Image window";

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  unsigned char lut[256];
  float fGamma = 0.5;
  cv::Mat img;
  cv::Mat cpy;

  int lowThreshold = 1;
  int ratio = 3;

public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/camera/image_raw", 1,
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    //cv::namedWindow(OPENCV_WINDOW);
  }
  ImageConverter() :{
	  std::cout << "Not using ros" << std::endl;
  }
  ~ImageConverter()
  {
    std::cout << "/* Destroyed ImageConverter */" << '\n';
  }
  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  void dead_pixel_concealment(cv::Mat& src, cv::Mat&dst, cv::Mat&dst);
  void black_level_correction(const cv::Mat &src, cv::Mat &dst, const double & contrast, const int & brightness);
  void GammaCorrection(const Mat& src, cv::Mat& dst, float & fGamma);
  void lut_builder(float fGamma);
  float rgb2luma(float r, float g, float b);
  void bilateral_filter(const cv::Mat & src, cv::Mat dst);
  void anti_aliasing(const cv::Mat & src, cv::Mat dst);
    
  void histogram_equalization(const cv::Mat & src, cv::Mat & image_clahe, const int clip);
  void chroma_noise_filter(const cv::Mat & src, cv::Mat & dst);
  void hue_saturation_control(const cv::Mat & src, cv::Mat & dst);
  void automatic_white_balance_gain_control(const cv::Mat & src, cv::Mat & dst);
};

void ImageConverter::lut_builder(float fGamma){
    for (int i = 0; i < 256; i++)
	{
		lut[i] = cv::saturate_cast<uchar>(pow((float)(i / 255.0), fGamma) * 255.0f);
	}
}

// don't forget to include related head files - not sure if the opengl route might be easier given this problem's ubiquity in this space
//https://gist.github.com/zhangzhensong/03f67947c22acb5ee922
void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture)
{
   if(image.empty()){
      std::cout << "image empty" << std::endl;
  }else{
      //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glGenTextures(1, &imageTexture1);
      glBindTexture(GL_TEXTURE_2D, imageTexture1);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set texture clamping method
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

      cv::cvtColor(image, image, CV_RGB2BGR);

      glTexImage2D(GL_TEXTURE_2D,         // Type of texture
                     	0,                   // Pyramid level (for mip-mapping) - 0 is the top level
			GL_RGB,              // Internal colour format to convert to
                     	image.cols,          // Image width  i.e. 640 for Kinect in standard mode
                     	image.rows,          // Image height i.e. 480 for Kinect in standard mode
                     	0,                   // Border width in pixels (can either be 1 or 0)
			GL_RGB,              // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
			GL_UNSIGNED_BYTE,    // Image data type
			image.ptr());        // The actual image data itself
	}
} 