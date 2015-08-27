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


#ifndef __GLOBAL_DEFINE__
#define __GLOBAL_DEFINE__

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctype.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <memory.h>
#include <cmath>

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#else   /* _WIN32 */
#include <unistd.h>
#include <limits>
#endif  /* _WIN32 */

#define TOO_FAR 1000.f
#define OCCLUSION_VALUE 50000.f
#define OUTLIER_VALUE	10*OCCLUSION_VALUE
#define OCCLUSION_THRESHOLD OCCLUSION_VALUE - (0.5*OCCLUSION_VALUE)
#define OUTLIER_THRESHOLD OUTLIER_VALUE - (0.5*OUTLIER_VALUE)



#ifdef WIN32
#define SLSTEREO_EXPORT_DLL __declspec(dllexport)
#else
#define SLSTEREO_EXPORT_DLL
#endif

namespace sl {

    typedef unsigned char uchar;

    typedef struct uchar3Struct {
        uchar c1;
        uchar c2;
        uchar c3;

        uchar3Struct(uchar c1_ = 0, uchar c2_ = 0, uchar c3_ = 0) {
            c1 = c1_;
            c2 = c2_;
            c3 = c3_;
        }

        inline void setValue(uchar c1_, uchar c2_ = 0, uchar c3_ = 0) {
            c1 = c1_;
            c2 = c2_;
            c3 = c3_;
        }

    } uchar3;

    typedef struct float3Struct {
        float f1;
        float f2;
        float f3;

        float3Struct(float f1_ = 0, float f2_ = 0, float f3_ = 0) {
            f1 = f1_;
            f2 = f2_;
            f3 = f3_;
        }

    } float3;

    namespace zed {

        /*! \defgroup Enumerations Public enumeration types */

        /**
         *  \enum MODE
         *  \ingroup Enumerations
         *  \brief Enumerate for the pre-defined  disparity map computation parameters
         *  \brief Since v0.8, MEDIUM and QUALITY mode have been combined to provide a single quality mode.
         */

        typedef enum {
            NONE, /*!< Disparity map not computed, only the rectified images will be available. \ingroup Enumerations*/
            PERFORMANCE, /*!< Fastest mode, also requires less GPU memory, the disparity map is less detailed \ingroup Enumerations*/
            QUALITY /*!< Better quality mode, the disparity map is more precise \ingroup Enumerations*/
        } MODE;

        /**
         *  \enum VIEW_MODE
         *  \ingroup Enumerations
         *  \brief Enumerate for available output views for monitoring
         */
        enum VIEW_MODE {
            STEREO_LEFT, /*!< Left view \ingroup Enumerations*/
            STEREO_RIGHT, /*!< Right view \ingroup Enumerations*/
            STEREO_ANAGLYPH, /*!< Anaglyph (Red & Blue) view \ingroup Enumerations*/
            STEREO_DIFF, /*!< View of the difference between the left image and right image in gray scale \ingroup Enumerations*/
            STEREO_SBS, /*!< Side by Side view (in single image resolution) \ingroup Enumerations*/
            STEREO_OVERLAY/*!< View of both images in 5O% transparency \ingroup Enumerations*/
        };

        /**
         *  \enum SIDE
         *  \ingroup Enumerations
         *  \brief Enumerate for the Left and Right side of stereo Camera
         */
        enum SIDE {
            LEFT,
            RIGHT
        };

        /**
         *  \enum SENSING_MODE
         *  \ingroup Enumerations
         *  \brief Enumerate for the different types of disparity map computation
         */
        enum SENSING_MODE {
            FULL, /*!< Occlusion filling, edge sharpening, advanced post-filtering.
                   * Application example : Refocusing, Multi-view generation \ingroup Enumerations*/
            RAW /*!< Structure conservative, no occlusion filling.
                 * Application example : Obstacle detection, 3D reconstructions \ingroup Enumerations*/
        };

        /**
         *  \enum MEASURE
         *  \ingroup Enumerations
         *  \brief Enumerate for the retrievable measure (each measure should be
         * normalized to be displayed)
         */
        enum MEASURE {
            DISPARITY, /*!< Disparity map \ingroup Enumerations*/
            DEPTH, /*!< Depth map \ingroup Enumerations*/
            CONFIDENCE /*!< Certainty/confidence of the diparity map \ingroup Enumerations*/
        };

        /**
         *  \enum ERRCODE
         *  \ingroup Enumerations
         *  \brief Enumerate for error code during the sl::zed::Camera::init
         */
        enum ERRCODE {
            SUCCESS, /*!< Every step went fine, the camera is ready to use \ingroup Enumerations*/
            NO_GPU_COMPATIBLE, /*!< No GPU found or the CUDA cupability of the device
                                * is not sufficient \ingroup Enumerations*/
            NOT_ENOUGH_GPUMEM, /*!< not enough GPU memory for this depth calculation mode
                                                           * please try a "lighter" mode (PERFORMANCE...) \ingroup Enumerations*/
            ZED_NOT_AVAILABLE, /*!< The ZED camera is not plugged or detected \ingroup Enumerations*/
            AUTO_CALIBRATION_FAILED, /*!< The scene seen by the camera  is not adapted
                                     * to do the automatic calibration
                                     * (simple untextured elements...) \ingroup Enumerations*/
            RECORDER_ERROR /*!< The recording failed (invalid path, out of space) \ingroup Enumerations*/
        };

        /**
         *  \enum ZEDResolution_mode
         *  \ingroup Enumerations
         *  \brief Enumerate for available resolutions for ZED Camera
         */
        enum ZEDResolution_mode {
            HD2K = 0, /*!< 2208*1242 \ingroup Enumerations*/
            HD1080 = 1, /*!< 1920*1080 \ingroup Enumerations*/
            HD720 = 2, /*!< 1280*720 \ingroup Enumerations*/
            VGA = 3 /*!< 640*480 \ingroup Enumerations*/
        };


		/**
         *  \enum ZEDCamera_settings
         *  \ingroup Enumerations
         *  \brief Enumerate the available camera settings for the ZED Camera( whitebalance, contrast, Hue , Saturation ...)
		 *  \brief Each enum defines one of those settings
         */
        enum ZEDCamera_settings {
           ZED_BRIGHTNESS=0, /*!< Defines the brightness control. Affected value should be between 0 and 8   \ingroup Enumerations*/
		   ZED_CONTRAST=1,/*!< Defines the constral control. Affected value should be between 0 and 8   \ingroup Enumerations*/
		   ZED_HUE=2,/*!< Defines the hue control. Affected value should be between 0 and 11   \ingroup Enumerations*/
		   ZED_SATURATION=3,/*!< Defines the saturation control. Affected value should be between 0 and 8   \ingroup Enumerations*/
		   ZED_GAIN=4,/*!< Defines the gain control. Affected value should be between 0 and 8   \ingroup Enumerations*/
		   ZED_WHITEBALANCE=5,/*!< Defines the while balance (color temperature) manual control. Affected value should be between 2800 and 6500  \ingroup Enumerations*/	  
        };



        /**
         * \struct resolution
         * \brief Width and Height of each left and right image.
         *
         */
        struct resolution {
            int width; /*!< Width of single image in pixels  */
            int height; /*!< Height of single image in pixels*/

            resolution(int w_, int h_) {
                width = w_;
                height = h_;
            }
        };

        static inline std::string errcode2str(ERRCODE err) {
            std::string output;
            switch (err) {
                case ERRCODE::SUCCESS:
                    output = "SUCCESS";
                    break;
                case ERRCODE::NO_GPU_COMPATIBLE:
                    output = "NO_GPU_COMPATIBLE";
                    break;
                case ERRCODE::NOT_ENOUGH_GPUMEM:
                    output = "NOT_ENOUGH_GPUMEM";
                    break;
                case ERRCODE::ZED_NOT_AVAILABLE:
                    output = "ZED_NOT_AVAILABLE";
                    break;
                case ERRCODE::AUTO_CALIBRATION_FAILED:
                    output = "AUTO_CALIBRATION_FAILED";
                    break;
                case ERRCODE::RECORDER_ERROR:
                    output = "RECORDER_ERROR";
                    break;
            }
            return output;
        }

        /**
         * \struct CamParameters
         * \brief Intrinsic parameters of one camera
         *
         */
        struct CamParameters {
            float fx; /*!< Focal x */
            float fy; /*!< Focal y */
            float cx; /*!< Optical center x */
            float cy; /*!< Optical center y */

            double disto[5]; /*!< Distortion factor : k1, k2, k3, r1, r2 */

            void SetUp(float fx_, float fy_, float cx_, float cy_) {
                fx = fx_;
                fy = fy_;
                cx = cx_;
                cy = cy_;
            }
        };

        /**
         * \struct StereoParameters
         * \brief Intrinsic parameters of each cameras, baseline and convergence (radians)
         *
         */
        struct StereoParameters {
            float baseline; /*!< Distance between the 2 cameras in mm  */
            float convergence; /*!< Convergence between the 2 cameras in radians  */
            CamParameters LeftCam; /*!< Intrinsic parameters of the left camera  */
            CamParameters RightCam; /*!< Intrinsic parameters of the right camera  */
        };

    } // zed namespace

};


#define TIMING 
#ifdef TIMING
#define INIT_TIMER auto start = std::chrono::high_resolution_clock::now();
#define START_TIMER start = std::chrono::high_resolution_clock::now();
#define DEF_START_TIMER auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIMER(name)  std::cout << name << " " << \
    std::chrono::duration_cast<std::chrono::milliseconds>( \
    std::chrono::high_resolution_clock::now()-start \
    ).count() << " ms "<<std::endl;
#else
#define INIT_TIMER
#define START_TIMER
#define DEF_START_TIMER
#define STOP_TIMER(name)
#endif

#endif /*__GLOBAL_DEFINE__*/
