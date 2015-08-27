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
 ** This sample demonstrates how to grab images and depth map with the ZED SDK                    **
 ** and apply the result in a 3D view "point cloud style" with OpenGL /freeGLUT                   **
 ** Some of the functions of the ZED SDK are linked with a key press event				         **
 ***************************************************************************************************/


//standard Include
#include <stdio.h>
#include <string.h>
#include <chrono>

//ZED Include
#include <zed/Mat.hpp>
#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

//our point cloud generator and viewer.
#include "PointCloud.hpp"
#include "CloudViewer.hpp"

using namespace sl::zed;
using namespace std;

//main Loop

int main(int argc, char **argv) {

	if (argc > 2){
		std::cout << "Only the path of a SVO can be passed in arg" << std::endl;
		return -1;
	}
	
    Camera* zed ;
	
	if (argc == 1 ) // Use in Live Mode
        zed = new Camera(HD720,15.0);
	else			// Use in SVO playback mode
		zed = new Camera(argv[1]);
	
	
    ERRCODE err = zed->init(MODE::QUALITY, 0); //need quite a powerful graphic card in QUALITY

    int width = zed->getImageSize().width;
    int height = zed->getImageSize().height;

    // ERRCODE display
    cout << errcode2str(err) << endl;

    // Quit if an error occurred
    if (err != SUCCESS) {
        delete zed;
        return 1;
    }

    SENSING_MODE dm_type = RAW;
    bool short_range=false;		 //20 meters
    int short_range_dist = 3500; //mm

    int key = ' ';
    int ViewID = 0;
    int reliabilityIdx = 96;

    Mat depth, imLeft;

    PointCloud *cloud = new PointCloud(width, height);
    CloudViewer *viewer = new CloudViewer();

    while ((key != 'q')) {
        // DisparityMap filtering
        //zed->setDispReliability(reliabilityIdx); !! this function name has been changed in Release 0.8 --see change log
		zed->setConfidenceThreshold(reliabilityIdx);

        // Get frames and launch the computation
        bool res = zed->grab(dm_type);

        depth = zed->retrieveMeasure(MEASURE::DEPTH);
        imLeft = zed->retrieveImage(SIDE::LEFT);
        cloud->fill(imLeft.data, (float*) depth.data, zed->getParameters());
        viewer->AddData(cloud);

        key = viewer->getKey();
        switch (key) {

            case 'b': // decrease the value of the reliability 
                reliabilityIdx -= 1;
                break;

            case 'n': // increase the value of the reliability
                reliabilityIdx += 1;
                break;

            case 'r': // switch to RAW sensing mode
                dm_type = RAW;
                break;

            case 'f': // switch to FULL sensing mode
                dm_type = FULL;
                break;

            case 's': // save the current point cloud in a PCD file
                cloud->WritePCDFile("PointCloud");
                break;

            case 'c' : // clamp/unclamp to short nmode (4m /20m)
                {

                    short_range=!short_range;
                    if (short_range)
                    {
                     //  std::cout<<" Clamp Value : "<<short_range_dist<<std::endl;
                      zed->setDepthClampValue(short_range_dist);
                    }
                    else
                    {
                      //  std::cout<<"ZED Clamp Value : "<<20000<<std::endl;
                       zed->setDepthClampValue(20000);
                    }


                }
                break;

            case '+':
                {
                    if (short_range)
                    {
                        short_range_dist+=100;
                        if (short_range_dist>20000) short_range_dist=20000;

                        //std::cout<<"ZED Clamp Value : "<<short_range_dist<<std::endl;
                        zed->setDepthClampValue(short_range_dist);
                    }
                }
                break;

            case '-':
            {
                if (short_range)
                {
                    short_range_dist-=100;
                    if (short_range_dist<1000) short_range_dist=1000;

                   // std::cout<<"ZED Clamp Value : "<<short_range_dist<<std::endl;
                    zed->setDepthClampValue(short_range_dist);
                }
            }
            break;
        }
    }

    delete zed;
    delete cloud;
    delete viewer;
    return 0;
}
