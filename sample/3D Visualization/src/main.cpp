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

//standard Include
#include <stdio.h>
#include <string.h>
#include <chrono>

//camera Include
#include <zed/Mat.hpp>
#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

//our point cloud generator and viewer.
#include "PointCloud.hpp"
#include "CloudViewer.hpp"

using namespace sl::zed;
using namespace std;

//main Loop

int main() {
	
    Camera* camera = new Camera(VGA,15.0);
    ERRCODE err = camera->init(MODE::PERFORMANCE, 0);

    int width = camera->getImageSize().width;
    int height = camera->getImageSize().height;

    // ERRCODE display
    cout << errcode2str(err) << endl;

    // Quit if an error occurred
    if (err != SUCCESS) {
        delete camera;
        return 1;
    }

    int key = ' ';

    Mat depth, imLeft;

    PointCloud *cloud = new PointCloud(width, height);
    CloudViewer *viewer = new CloudViewer();

    // application quits when user stikes 'q'
    while (key != 'q') {
        // DisparityMap filtering
		camera->setConfidenceThreshold(96); // parameter is reliability index ~[1,100] with 100 as no filtering

        // Get frames and launch the computation
        bool res = camera->grab(SENSING_MODE::RAW);

        depth = camera->retrieveMeasure(MEASURE::DEPTH);
        imLeft = camera->retrieveImage(SIDE::LEFT);
        cloud->fill(imLeft.data, (float*) depth.data, camera->getParameters());
        viewer->AddData(cloud);

        // Update the value of key so that we can quit when the user strikes 'q'
        key = viewer->getKey();
    }

    delete camera;
    delete cloud;
    delete viewer;
    return 0;
}
