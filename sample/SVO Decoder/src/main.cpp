///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015, STEREOLABS.
// 
// All rights reserved.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////




/**************************************************************************************************
 ** This sample demonstrates how to grab images and depth with the ZED SDK from a SVO file,       **
 ** display aligned Left and Right images, and compute depth                                      **
 ** A SVO file must be specified in cmd arguments //See Record sample or ZEDExplorer to save SVO  **
 ***************************************************************************************************/

/* This software uses BOOST for program arguments options */

//Standard include
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <cstdlib>
#include <chrono>

#ifdef _WIN32
#include <windows.h> 
#endif

//opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//include boost program options
#include <boost/program_options.hpp>

//ZED include
#include <zed/Mat.hpp>
#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

using namespace sl::zed;
using namespace std;
namespace po = boost::program_options;


//Utils fct : how to convert sl::zed::Mat to cv::Mat

cv::Mat slMat2cvMat(sl::zed::Mat slMat) {
    int cvType;
    if (slMat.data_type == DATA_TYPE::UCHAR)
        cvType = CV_8U;
    else if (slMat.data_type == DATA_TYPE::FLOAT)
        cvType = CV_32F;
    else
        std::cerr << "slMat2cvMat: mat type currently not supported " <<
            slMat.channels << " " << slMat.getDataSize() << std::endl;
    return cv::Mat(slMat.height, slMat.width, CV_MAKETYPE(cvType, slMat.channels), slMat.data);
}

void displayParam(sl::zed::Camera* zed_ptr) {
    StereoParameters* zed_param = zed_ptr->getParameters();
    cout << endl << "Parameters : " << endl << "fx: " << zed_param->LeftCam.fx \
 << endl << "fy: " << zed_param->LeftCam.fy << endl << "cx: " << zed_param->LeftCam.cx \
 << endl << "cy: " << zed_param->LeftCam.cy << endl << endl;
}


//Main func

int main(int argc, char **argv) {
    ERRCODE err;
    std::string filename = "";
    std::string output_filename = "output";
    std::string output_video_filename = "output.mp4";
    bool display = 1, disparity_computation = 0;
    int quality_mode;
    bool record_output = 0;
    bool record_video_output = 0;
    int device = 0;
    cv::VideoWriter* video_writer;
    Camera* zed_ptr;

    // Option handler (boost program_options)
    int opt;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("filename,f", po::value< std::string >(), "SVO recorded filename")
            ("record,r", po::value< std::string >(), "Record into a sequence of image")
            ("video,v", po::value< std::string >(), "Record into a video file (a name WITH EXTENSION must be given)")
            ("nodisplay,s", "Toggle image display (if set, no image will be displayed)")
            ("disparity,z", "Compute disparity, if record option activated : Record Left image and Disparity map, SbS L+R otherwise")
            ("device,d", po::value<int>(&opt)->default_value(0), "CUDA device (only with disparity option)")
            ("quality,q", po::value<int>(&opt)->default_value(MODE::PERFORMANCE),
            "Disparity Map quality factor [1-3] (only with disparity option)")
            ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    //if no filename specified , still print help
    if (vm.count("filename")) {
        filename = vm["filename"].as<std::string>();
        cout << "Opening file " << filename << endl;
    } else {
        cout << "  !!!! You must specify at least the SVO filename  -- see allowed options !!!!\n" << std::endl;
        cout << desc << "\n";

#ifdef WIN32
        system("pause"); //just in case of this sample is launched without cmd line...
#endif
        return 0;
    }

    if (vm.count("nodisplay")) {
        cout << "Display off" << endl;
        display = 0;
    }
    if (vm.count("disparity")) {
        cout << "Disparity map computing" << endl;
        disparity_computation = 1;
    }
    if (vm.count("record")) {
        cout << "Image sequence recording on" << endl;
        output_filename = vm["record"].as<std::string>();
        record_output = 1;
    }
    if (vm.count("video")) {
        cout << "Video recording on" << endl;
        output_video_filename = vm["video"].as<std::string>();
        record_video_output = 1;
    }
    if (vm.count("quality")) {
        quality_mode = vm["quality"].as<int>();
        if (quality_mode > MODE::QUALITY) quality_mode = MODE::QUALITY;
        if (quality_mode < MODE::PERFORMANCE) quality_mode = MODE::PERFORMANCE;
    }
    if (vm.count("device")) {
        device = vm["device"].as<int>();
    }

    // Instead of a resolution parameter, we just have to tell the filename of SVO file (with extension .svo)
    // this will grab images from SVO file as if it was coming from the camera itself.
    zed_ptr = new Camera(filename);
    cout << "SVO recorded by ZED Camera S/N" << zed_ptr->getZEDSerial() << std::endl;

    err = zed_ptr->init(static_cast<MODE> (quality_mode), device);

    // ERRCODE display
    cout << errcode2str(err) << endl;
    // Quit if an error occurred
    if (err != SUCCESS) {
        delete zed_ptr;
        return 1;
    }

    // Getting new parameters (tweaked by the live rectification)
    displayParam(zed_ptr);

    // Get recorded image size
    resolution size = zed_ptr->getImageSize();

    int key = ' ';

    if (record_video_output) {
        video_writer = new cv::VideoWriter(output_video_filename, CV_FOURCC('M', '4', 'S', '2'), 25, cv::Size(size.width * 2, size.height));
    }

    cv::Mat leftIm(size.height, size.width, CV_8UC4);
    cv::Mat rightIm(size.height, size.width, CV_8UC4);
    cv::Mat sbsIm(size.height, size.width * 2, CV_8UC4);

    // Main loop
    // Get frames and launch the computation
    std::string tmp;
    int frame_count = 0;

    cout << "Running..." << endl;

    // same grab function //
    while (!zed_ptr->grab(SENSING_MODE::FULL, 0, disparity_computation)) {

        // Left
        slMat2cvMat(zed_ptr->retrieveImage(SIDE::LEFT)).copyTo(leftIm);
        // Right (Disparity Map or Right image)
        if (disparity_computation) slMat2cvMat(zed_ptr->normalizeMeasure(MEASURE::DISPARITY)).copyTo(rightIm);
        else slMat2cvMat(zed_ptr->retrieveImage(SIDE::RIGHT)).copyTo(rightIm);

        // SbS
        sbsIm.adjustROI(0, 0, 0, -size.width);
        leftIm.copyTo(sbsIm);
        sbsIm.adjustROI(0, 0, -size.width, size.width);
        rightIm.copyTo(sbsIm);
        sbsIm.adjustROI(0, 0, size.width, 0);

        // Record
        if (record_output) {
            cv::Mat sbs_rgb;
            cv::cvtColor(sbsIm, sbs_rgb, CV_RGBA2RGB);
            tmp = output_filename + std::string("_%07d.png");
            char imageName[128];
            sprintf(imageName, tmp.c_str(), frame_count);
            cv::imwrite(std::string(imageName), sbs_rgb);
        }

        // Record Video
        if (record_video_output) {
            cv::Mat sbs_rgb;
            cv::cvtColor(sbsIm, sbs_rgb, CV_RGBA2RGB);
            video_writer->write(sbs_rgb);
        }

        if (display) {
            //resize for display -- see if needed depending on your resolution (screen/camera)
            cv::Mat sbsIm_LowRes;
            cv::resize(sbsIm, sbsIm_LowRes, cv::Size(2 * 640, 360));
            imshow("ZED converter display", sbsIm_LowRes);
            key = cv::waitKey(2);
        }

        frame_count++;
    }

    cout << "Done" << endl;
    if (record_video_output) delete video_writer;
    delete zed_ptr;
    return 0;
}
