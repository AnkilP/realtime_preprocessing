# realtime_preprocessing

This is a ROS workspace with a preprocessing package that preprocess multiple image streams given that the image sensors are exactly or approximately time synchronized. 

This is the general overview of the pipeline:
//dead pixel concealment using median filter
//black level compensation
//lens shading correction
//anti-aliasing noise noise filter
//awb gain control
//cfa interpolation
//gamma correction
//color correction
//color space conversion
//noise filter for chroma
//hue saturation control
//noise filter for luma
//edge enhancement
//contrast brightness control
//data formatter
