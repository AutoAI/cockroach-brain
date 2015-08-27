/*
 * SOFTWARE LICENSE
 * BY USING YOUR ZED CAMERA YOU AGREE TO THIS SOFTWARE LICENSE. BEFORE SETTING IT UP, 
 * PLEASE READ THIS SOFTWARE LICENSE CAREFULLY. IF YOU DO NOT ACCEPT THIS 
 * SOFTWARE LICENSE, DO NOT USE YOUR ZED CAMERA. RETURN IT TO UNUSED TO 
 * STEREOLABS FOR A REFUND. Contact STEREOLABS at contact@stereolabs.com
 *
 * 1. Definitions
 *
 * "Authorized Accessory" means a STEREOLABS branded ZED, and a STEREOLABS 
 * licensed, third party branded, ZED hardware accessory whose packaging 
 * bears the official "Licensed for ZED" logo. The ZED Camera is an Authorized
 *  Accessory solely for purpose of this Software license.
 * "Software" means the Software Development Kit, pre-installed in the ZED 
 * USB flash drive included in the ZED packaging, and including any 
 * updates STEREOLABS may make available from time to time.
 * "Unauthorized Accessories" means all hardware accessories other than 
 * an Authorized Accessory.
 * "Unauthorized Software" means any software not distributed by STEREOLABS.
 * "You" means the user of a ZED Camera.
 * 
 * 2. License
 * 
 * a. The Software is licensed to You, not sold. You are licensed to use the 
 * Software only as pre-installed in Your ZED USB flash drive, and updated by 
 * STEREOLABS from time to time. You may not copy or reverse engineer the Software.
 * 
 * b. As conditions to this Software license, You agree that:
 *       i. You will use Your Software with ZED Camera only and not with any 
 * other device (including). You will not use Unauthorized Accessories. 
 * They may not work or may stop working permanently after a Software update.
 *       ii. You will not use or install any Unauthorized Software. 
 * If You do, Your ZED Camera may stop working permanently at that time 
 * or after a later Software update.
 *       iii. You will not attempt to defeat or circumvent any Software 
 * technical limitation, security, or anti-piracy system. If You do, 
 * Your ZED Camera may stop working permanently at that time or after a 
 * later Software update.
 *       iv. STEREOLABS may use technical measures, including Software 
 * updates, to limit use of the Software to the ZED Camera, to prevent 
 * use of Unauthorized Accessories, and to protect the technical limitations, 
 * security and anti-piracy systems in the ZED Camera.
 *       v. STEREOLABS may update the Software from time to time without 
 * further notice to You, for example, to update any technical limitation, 
 * security, or anti-piracy system.
 *
 * 3. Warranty
 * The Software is covered by the Limited Warranty for Your ZED Camera, 
 * and STEREOLABS gives no other guarantee, warranty, or condition for 
 * the Software. No one else may give any guarantee, warranty, or condition 
 * on STEREOLABS's behalf.
 * 
 * 4. EXCLUSION OF CERTAIN DAMAGES
 * STEREOLABS IS NOT RESPONSIBLE FOR ANY INDIRECT, INCIDENTAL, SPECIAL, OR 
 * CONSEQUENTIAL DAMAGES; ANY LOSS OF DATA, PRIVACY, CONFIDENTIALITY, OR 
 * PROFITS; OR ANY INABILITY TO USE THE SOFTWARE. THESE EXCLUSIONS APPLY 
 * EVEN IF STEREOLABS HAS BEEN ADVISED OF THE POSSIBILITY OF THESE DAMAGES, 
 * AND EVEN IF ANY REMEDY FAILS OF ITS ESSENTIAL PURPOSE.
 * 
 * 5. Choice of Law
 * French law governs the interpretation of this Software license and any 
 * claim that STEREOLABS has breached it, regardless of conflict of 
 * law principles.
 *
 */

#ifndef CAMERA_HPP
#define	CAMERA_HPP

#include "zed/Mat.hpp"
#include "zed/utils/GlobalDefine.hpp"
#include <cuda.h>

namespace sl {
    namespace zed {

        /*! Available mode for the ZED camera sensors */
        static std::vector<resolution> zedResolution = [] {
            std::vector<resolution> v;
            v.push_back(resolution(2208, 1242));
            v.push_back(resolution(1920, 1080));
            v.push_back(resolution(1280, 720));
            v.push_back(resolution(640, 480));
            return v;
        }();

        /*! \class Camera
         *  \brief The main class to use the ZED camera
         */
        class SLSTEREO_EXPORT_DLL Camera {
        public:
            /*!
             *  \brief Camera constructor. The ZEDResolution_mode sets the sensor
             * resolution and defines the size of the output images, including the
             * measures (disparity map, confidence map..).
             * 
             * All computation is done on a CUDA capable device
             * (That means that every CPU computation will need a memory retrieve 
             * of the images, which takes some time). If the performance is the main focus,
             * all external computation should run on GPU. The retrieve*_gpu gives
             * directly access to the gpu buffer.
             * 
             *  \param zedRes_mode : the chosen ZED resolution
			 *  \param fps : a requested fps for this resolution. set as 0.0 will choose the default fps for this resolution ( see User guide)
             * 
             */
            Camera(ZEDResolution_mode zedRes_mode = ZEDResolution_mode::HD1080,float fps=0.0);


            /*!
             *  \brief Camera constructor, from svo file previously recorded. 
             * The size of the output is defined by the recorded images.
             * 
             *  \param zed_record_filename : path with filename to the recorded svo file
             * 
             */
            Camera(std::string zed_record_filename);

            ~Camera();

            /*!
             *  \brief The init function must be called after the instantiation.
             * The function checks if the ZED camera is plugged and opens it, if the graphic card
             * is compatible, allocates the memory and launches the automatic calibration.
             *              
             *  \param quality  : defines the quality if the disparity map,
             * affect the level of details and also the computation time. 
             * The MODE::QUALITY require a quite powerful graphic card and more 
             * gpu memory, if the frame rate is too low try a lower quality
             *  \param device : defines the graphic card on which the
             * computation will be done. The default value 0 takes the first card.
             * The computation should be launched on the fastest card.
             *  \param verbose : if set to true, it will output some information about the current status of initialization
             *  \param vflip : if set to true, it will vertically flipped frames coming from the ZED. (for "Hang out" mode)
             * \retval ERRCODE : The error code gives information about the
             * internal process, if SUCCESS is returned, the camera is ready to use.
             * Every other code indicates an error and the program should be stopped.
             * For more details see sl::zed::ERRCODE.
             *  
             */
            ERRCODE init(MODE quality, int device = 0, bool verbose = false, bool vflip=false);

            /*!
             *  \brief The function grabs a new image, rectifies it and computes the 
             * disparity map and optionally the depth map.
             * The grabbing function is typically called in the main loop.
             * 
             *  \param dm_type : defined the type of disparity map, more info : sl::zed::SENSING_MODE definition
             *  \param computeMeasure : defined if the depth map should be computed. 
             * If false, only the disparity map is computed.
             *  \param computeDisparity : defined if the disparity map should be computed. 
             * If false, the depth map can't be computed and only the camera images can be retrieved.
             * 
             *  \retval error boolean value : the function return false if no problem was encountered, 
             * true otherwise.
             * 
             * \pre The init function must have been called (once for the lifetime of the Camera object)
             *  before calling grab.
             */
            bool grab(SENSING_MODE dm_type = SENSING_MODE::FULL, bool computeMeasure = 1, bool computeDisparity = 1);



            /*!
             *  \brief The reset function can be called at any time AFTER the Camera::init function has been called
             *  It will reset and calculate again correction for misalignment, convergence and color mismatch.
             *  It can be called after changing camera parameters via the tool ZEDCalibrationUpdater without needing to restart your executable.
             *
             * \retval ERRCODE : error boolean value : the function return false if no problem was encountered, 
             * true otherwise.
             * if no problem was encountered, the camera will use new parameters. Otherwise, it will be the old ones
             */
            bool reset();


            /*!
             *  \brief Generate a CPU image to display. Several modes available
             * SidebySide, anaglyph... (for more see sl::zed::VIEW_MODE) 
             * 
             *  \param view : the wanted mode
             *  \retval View : the return value is a CPU sl::zed::Mat. 
             * 
             * \warning The buffer must be duplicated if another CPU retrieve has to be done, it will be
             * overwriten otherwise. The buffer must not be freed.
             */
            Mat getView(VIEW_MODE view);

            /*!
             *  \brief Generate a GPU image to display. Several modes available
             * SidebySide, anaglyph... (for more see sl::zed::VIEW_MODE) 
             * 
             *  \param view : the wanted mode
             *  \retval View : the return value is a GPU sl::zed::Mat. 
             * 
             * \warning The buffer must be duplicated if another GPU retrieve has to be done, it will be
             * overwriten otherwise. The buffer must not be freed.
             */
            Mat getView_gpu(VIEW_MODE view);

            /*!
             *  \brief Download the rectified image from the device and return the CPU buffer. 
             * The retrieve function should be called after the function grab
             * 
             *  \param SIDE  : defined the image side wanted (see sl::zed::SIDE)
             *  \retval Image : the return value is a CPU sl::zed::Mat. The size is given by 
             * the input parameters of the constructor Camera.
             * 
             * \warning The buffer must be duplicated if another CPU retrieve has to be done, it'll be
             * overwriten otherwise. The buffer must not be freed.
             */
            Mat retrieveImage(SIDE side);

            /*!
             *  \brief Download the measure (disparity, depth or confidence of disparity) 
             * from the device and return the CPU buffer. 
             *  The retrieve function should be called after the function grab
             * 
             *  \param MEASURE : defined the type wanted, such as disparity map, 
             * depth map or the confidence (see sl::zed::MEASURE)
             *  \retval Measure : the return value is a CPU sl::zed::Mat. For Depth measure, values are given in mm. For Confidence map, 
			 * a value close to 1 means a good precision, a value close to 100 means less precision.
             *  
             * \warning The buffer must be duplicated if another CPU retrieve has to be done, it'll be
             * overwriten otherwise. The buffer must not be freed.
             */
            Mat retrieveMeasure(MEASURE measure);

            /*!
             *  \brief Get the rectified image GPU buffer. 
             * The retrieve function should be called after the function grab
             * 
             *  \param SIDE  : defined the image side wanted (see sl::zed::SIDE)
             *  \retval Image : the return value is a GPU sl::zed::Mat. The size is given by 
             * the input parameters of the constructor Camera.
             * \warning The buffer must not be freed.
             */
            Mat retrieveImage_gpu(SIDE side);

            /*!
             *  \brief Get the measure (disparity, depth or certainty of disparity) GPU buffer. 
             * The retrieve function should be called after the function grab
             * 
             *  \param MEASURE : defined the type wanted, such as disparity map, 
             * depth map or the disparity map certainty (see sl::zed::MEASURE)
             *  \retval Measure : the return value is a GPU sl::zed::Mat. The size is given by 
             * the input parameters of the constructor Camera. For Depth measure, values are given in mm.
             * \warning The buffer must not be freed.
             */
            Mat retrieveMeasure_gpu(MEASURE measure);

            /*!
             *  \brief GPU Normalization of the measure value and download the result as an CPU image
             *  \param MEASURE : defined the requested output (see sl::zed::MEASURE)
             *  \param min : defined the lower bound of the normalization, default (except for DEPTH) : automatically found
             *  \param max : defined the upper bound of the normalization, default (except for DEPTH) : automatically found
             * 
             *  \retval normalized_measure : the CPU buffer. 
             *  \warning The buffer must be duplicated if another CPU retrieve has to be done, it'll be
             * overwriten otherwise. The buffer must not be freed.
             * 
             */
            Mat normalizeMeasure(MEASURE measure, float min = 0, float max = 0);

            /*!
             *  \brief GPU Normalization of the measure value and get the result as an GPU image
             *  \param MEASURE : defined the requested output (see sl::zed::MEASURE)
             *  \param min : defined the lower bound of the normalization, default (except for DEPTH) : automatically found
             *  \param max : defined the upper bound of the normalization, default (except for DEPTH) : automatically found
             * 
             *  \retval normalized_measure : the GPU buffer. 
             *  \warning The buffer must be duplicated if another GPU retrieve has to be done, it'll be
             * overwriten otherwise. The buffer must not be freed.
             * 
             */
            Mat normalizeMeasure_gpu(MEASURE measure, float min = 0, float max = 0);

            /*!
             *  \brief Getter of the ZED parameters
             *  \retval StereoParameters pointer containing the intrinsic parameters of each camera 
             * and the baseline (mm) and convergence (radian) of the ZED.
             */
            StereoParameters* getParameters();

            /*!
             *  \brief Set a filtering value for the disparity map 
             * (and by extension the depth map). The function should be called before
             * the grab to be taken into account.
             * \param ThresholdIdx : a value in [1,100]. A lower value means more confidence and precision 
             * (but less density), an upper value reduces the filtering (more density, less certainty).
             * Other value means no filtering.
             * 
             */
            void setConfidenceThreshold(int ThresholdIdx);

            /*!
             *  \brief Get the current confidence threshold value for the disparity map
             * (and by extension the depth map).
             * \retval :   current filtering value between 0 and 100.
             *
             */
            int getConfidenceThreshold();


            /*!
             *  \brief Getter of the CUDA context used for all the computation
             *  \retval CUDA_context : the CUcontext
             */
            CUcontext getCUDAContext();

            /*!
             *  \brief Get the image size
             *  \retval resolution : the image size
             */
            resolution getImageSize();


            /*!
             *  \brief Get the ZED Serial Number
             *  \retval Return the ZED Serial Number (as uint), 0 if the ZED is not connected.
             */
            unsigned int getZEDSerial();

            // Recorder functions:
            /*!
             *  \brief Initializes the recorder
             *  \param filepath : path with absolute or relative filename of the recorded file
             *  \param avi_file : record in avi (not compatible with the ZED SDK as an input) rather than svo 
             *  \param side_by_side : record one avi file with side by side images, left and right otherwise
             * 
             * \note The disparity/depth map is neither recorded nor computed
             *
             * \pre The init function must have been called (once for the lifetime of the Camera object)
             *  before calling this function.
             */
            ERRCODE init_recording(std::string filepath, bool avi_file = false, bool side_by_side = true);



            /*!
             *  \brief Set the position of the SVO file to a desired frame.
             * \param frame : the number of the desired frame to be decoded.
             * \retval true if succes, false if failed (i.e. if the ZED is currently used in live and not playing a SVO file)
             *
             */
            bool setSVOPosition(int frame);

			 /*!
             *  \brief Get the current position of the SVO file
             * \retval SVO Style Only : the current position in the SVO file as int (-1 if the SDK is not reading a SVO)
             *
             */
            int getSVOPosition();

            /*!
             *  \brief Get the number of frames in the SVO file
             * \retval : SVO Style Only : the total number of frames in the SVO file (-1 if the SDK is not reading a SVO)
             *
             */
            int getSVONumberOfFrames();




            /*!
             *  \brief Set the maximum distance of depth/disarity estimation (all values after this limit will be reported as TOO_FAR value)
             *  \param distanceMax : maximum distance in mm (or Baseline unit --see ZED Settings App)
             *
             */
            void setDepthClampValue(int distanceMax);

            /*!
             *  \brief Get the current maximum distance of depth/disarity estimation
             *  \retval : current maximum distance in mm (or Baseline unit --see ZED Settings App)
             *
             */
            int  getDepthClampValue();


            /*!
             *  \brief Record a camera frame (typically called in a loop)
             */
            bool record();

            /*!
             *  \brief Stop the recording and close the file
             */
            bool stop_recording();

            /*!
             *  \brief Display the current recorded frame
             */
            void display_recorded();


            /*!
             *  \brief Set the value to the corresponding Camera Settings mode (Gain, brightness, hue, white balance....)
             *  \param ZEDCamera_settings : enum for the control mode 
			 *  \param value (int) : value to set for the corresponding control 
			 *  \param usedefault(bool) : will set default (or automatic) value if set to true (value (int) will not be taken into account)
             *
             */
			void setCameraSettingsValue(ZEDCamera_settings mode, int value, bool usedefault=false);

			
			         
			
			 /*!
             *  \brief Get the current value to the corresponding Camera Settings mode (Gain, brightness, hue, white balance....)
             *  \param ZEDCamera_settings : enum for the control mode 
			 *  \retval Current value for the corresponding control (-1 if something wrong happened).
             *
             */
			int getCameraSettingsValue(ZEDCamera_settings mode);




        private:
            bool openCamera(bool);
            bool nextImage();
            void convertImage(Mat sbsYUV_cpu, bool align=false);
            void thresDMScore();
            void computeDepth();
            int initMemory();
            bool initRectifier(bool);
            void loadParameters();

            int* members;
        };
    }
}

#endif	/* CAMERA_HPP */

