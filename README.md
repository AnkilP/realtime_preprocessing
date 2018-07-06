# realtime_preprocessing

This is a ROS workspace with a preprocessing package that preprocess multiple image streams given that the image sensors are exactly or approximately time synchronized. 

This is the general overview of the pipeline: <br />
-dead pixel concealment using median filter <br />
-black level compensation <br /> 
-lens shading correction <br />
-anti-aliasing noise noise filter <br />
-awb gain control <br />
-cfa interpolation <br />
-gamma correction <br />
-color correction <br />
-color space conversion <br />
-noise filter for chroma <br />
-hue saturation control <br />
-noise filter for luma <br />
-edge enhancement <br />
-contrast brightness control <br />
-data formatter
