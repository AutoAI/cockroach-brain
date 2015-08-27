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
** This sample demonstrates how to grab images and disparity map with the ZED SDK                **
** The GPU buffer is ingested directly into OpenGL texture for avoiding  GPU->CPU readback time  **
** For the Left image, a GLSL shader is used for RGBA-->BGRA transformation , just as an example **
***************************************************************************************************/


///Standard includes
#include <stdio.h>
#include <string.h>
#include <ctime>

//ZED include
#include <zed/Camera.hpp>

/// Glut and OpenGL extension (GLEW) for shaders
#include "GL/glew.h"
#include "GL/glut.h"
 
//Cuda includes
#include "cuda.h"
#include "cuda_runtime.h"
#include "cuda_gl_interop.h"

#include "FPS.h"

using namespace sl::zed;
using namespace std;

//declare some ressources (GL texture ID, GL shader ID...)
GLuint imageTex;
GLuint depthTex;
cudaGraphicsResource* pcuImageRes;
cudaGraphicsResource* pcuDepthRes;
Camera* zed;
int w;
int h;

GLuint shaderF;
GLuint program;

//ressources for ZED
int reliabilityIdx;
int ViewID;
int DisplayMode;
std::string DisplayModeStr;
bool quit;
sl::zed::SENSING_MODE dm_type;

bool PrintFPS;
bool ShortRange;
int short_dist_val;


Fps fps;

int n_x;
int n_y;

int r_x;
int r_y;

//Here is the very simple fragment shader for flipping Red and Blue
std::string strFragmentShad = ("uniform sampler2D texImage;\n"
							   " void main() {\n"
							   " vec4 color = texture2D(texImage, gl_TexCoord[0].st);\n"
							   " gl_FragColor = vec4(color.b, color.g, color.r, color.a);\n}");

 
//Keyboard interaction callback fct
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
			case 'b':
                reliabilityIdx -= 10;
                break;
			case 'n':
                reliabilityIdx += 10;
                break;
			
			//re-compute alignment
			case 'a' : 
                cout<<"reset Alignment"<<endl;
				zed->reset();
				break;

			// ______________  VIEW __________________
            case '0': // left
                ViewID = 0;
                break;
            case '1': // right
                ViewID = 1;
                break;
            case '2': // anaglyph
                ViewID = 2;
                break;
            case '3': // gray scale diff
                ViewID = 3;
                break;
            case '4': // Side by side
                ViewID = 4;
                break;
            case '5': // overlay
                ViewID = 5;
                break;

                   
			case 'r':
                dm_type = SENSING_MODE::RAW;
                cout << "SENSING_MODE: Raw" << endl;
                break;

			case 'f':
                dm_type = SENSING_MODE::FULL;
                cout << "SENSING_MODE: FULL" << endl;
                break;

			case 'q':
				quit = true;
				break;

			case 'd': // switch from depth to disparity to score 
				DisplayMode++;
				DisplayMode=DisplayMode%3;
				break;

			case 'g':
				PrintFPS = !PrintFPS;
				break;

            case 'c' :
                {
                  ShortRange=!ShortRange;

                  if (ShortRange)
                     zed->setDepthClampValue(short_dist_val);
                  else
                     zed->setDepthClampValue(20000);
                }
                break;


            case '+' :
                {
                     if (ShortRange)
                     {
                         short_dist_val+=100;

                     if (short_dist_val>20000)
                         short_dist_val=20000;

                      zed->setDepthClampValue(short_dist_val);
                     }
                }
                break;

            case '-' :
                {
                     if (ShortRange)
                     {
                         short_dist_val-=100;

                     if (short_dist_val<1000)
                         short_dist_val=1000;

                        zed->setDepthClampValue(short_dist_val);

                     }

                }
                break;

	}
}


void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			
			r_x = x;
			r_y = y;

			int w_wnd = glutGet(GLUT_WINDOW_WIDTH);
			int h_wnd = glutGet(GLUT_WINDOW_HEIGHT);

			w_wnd /= 2;
			n_x = (float) x / w_wnd * w;
			if (n_x > w) n_x -= w;
			n_y = (float)y / h_wnd * h;
			
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			n_x =-1;
			n_y = -1;
		}
	}
}

void printtext(int x, int y, string String)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w_wnd = glutGet(GLUT_WINDOW_WIDTH);
	int h_wnd = glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0, w_wnd, 0, h_wnd, -1.0f, 1.0f);
	glColor3f(0.098f, 0.725f, 0.925f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	int w_wnd_2 = (w_wnd / 2);

	if (x < w_wnd_2)
		x += w_wnd_2;
	y = h_wnd - y;
	glRasterPos2i(x - (w_wnd * 0.035), y + (h_wnd * 0.02));
	for (int i = 0; i < String.size(); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, String[i]);
	
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	float r_ = 0.01;
	float d_ = (r_ * w_wnd) / w_wnd;

	float n_x_ = (float)x / w_wnd;
	float n_y_ = (float)y / h_wnd;

	float x_p1 = (n_x_ - d_) * 2 - 1;
	float x_p2 = (n_x_ + d_) * 2 - 1;
	float y_p1 = (n_y_ - d_) * 2 - 1;
	float y_p2 = (n_y_ + d_) * 2 - 1;

	glColor3f(0.098f, 0.725f, 0.925f);
	glBegin(GL_LINES);
	glVertex2f(x_p1, y_p1);
	glVertex2f(x_p2, y_p2);
	glVertex2f(x_p2, y_p1);
	glVertex2f(x_p1, y_p2);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

//glut main loop : grab --> extract GPU Mat --> send to OpenGL and quad.
void draw(){
	
	//zed->setDispReliability(reliabilityIdx); !!function name has been change in Release 0.8 --see ChangeLog
    zed->setConfidenceThreshold(reliabilityIdx);





   // INIT_TIMER
    char WndTitle[256];
    int res = zed->grab(dm_type,true,true);

    if (res==0)
	{

       // STOP_TIMER("Grab Process Time : ")


		fps.update();


		// Map GPU Ressource for Image
		// With Gl texture, we have to use the cudaGraphicsSubResourceGetMappedArray cuda functions. It will link the gl texture with a cuArray. 
		// Then, we just have to copy our GPU Buffer to the CudaArray (D2D copy). 
		// Between , you can do some other GPU stuffs.

        Mat gpuImage = zed->getView_gpu(static_cast<VIEW_MODE>(ViewID));
		cudaArray_t ArrIm;
		cudaGraphicsMapResources(1, &pcuImageRes, 0);
		cudaGraphicsSubResourceGetMappedArray(&ArrIm, pcuImageRes, 0, 0);
		cudaMemcpy2DToArray(ArrIm, 0, 0, gpuImage.data, gpuImage.step, w * 4, h, cudaMemcpyDeviceToDevice);
		cudaGraphicsUnmapResources(1, &pcuImageRes, 0);

		//map GPU Ressource for Depth !! Image of depth == 8U 4channels
        Mat gpuDepth;
		


        switch (DisplayMode)
		{
		case 0 : 
            gpuDepth = zed->normalizeMeasure_gpu(MEASURE::DEPTH);
			DisplayModeStr = "DEPTH";
			break;

		case 1 : 
            gpuDepth = zed->normalizeMeasure_gpu(MEASURE::DISPARITY);
			DisplayModeStr = "DISPARITY";
			break;

		case 2 : 
			gpuDepth = zed->normalizeMeasure_gpu(MEASURE::CONFIDENCE);
			DisplayModeStr = "CONFIDENCE";
			break;

		default : 
			DisplayModeStr="N/A";
			break;
        }


		 
		if (PrintFPS)
			sprintf(WndTitle, "ZED %s Viewer - (dist max : %d mm) - %d fps", DisplayModeStr.c_str(),  zed->getDepthClampValue(),fps.get());
		else
			sprintf(WndTitle, "ZED %s Viewer - (dist max : %d mm)", DisplayModeStr.c_str(), zed->getDepthClampValue());

		glutSetWindowTitle(WndTitle);
	 
        cudaArray_t ArrDe;
		cudaGraphicsMapResources(1, &pcuDepthRes, 0);
		cudaGraphicsSubResourceGetMappedArray(&ArrDe, pcuDepthRes, 0, 0);
		cudaMemcpy2DToArray(ArrDe, 0, 0, gpuDepth.data, gpuDepth.step, w * 4, h, cudaMemcpyDeviceToDevice);
        cudaGraphicsUnmapResources(1, &pcuDepthRes, 0);

		//OpenGL Part
		glDrawBuffer(GL_BACK);      //write to both BACK_LEFT & BACK_RIGHT
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f,1.0f);
		//Draw Image Texture in Left Part of Side by Side
		glBindTexture(GL_TEXTURE_2D, imageTex);
		//flip R and B with GLSL Shader
		glUseProgram(program);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0,1.0);
			glVertex2f(-1.0,-1.0);
		glTexCoord2f(1.0,1.0);
			glVertex2f(0.0,-1.0);
		glTexCoord2f(1.0,0.0);
			glVertex2f(0.0,1.0);
		glTexCoord2f(0.0,0.0);
			glVertex2f(-1.0,1.0);
		glEnd();
	
		glUseProgram(0);

		//Draw Depth Texture in Right Part of Side by Side
        glBindTexture(GL_TEXTURE_2D, depthTex);
		 
		glBegin(GL_QUADS);
		glTexCoord2f(0.0,1.0);
			glVertex2f(0.0,-1.0);
		glTexCoord2f(1.0,1.0);
			glVertex2f(1.0,-1.0);
		glTexCoord2f(1.0,0.0);
			glVertex2f(1.0,1.0);
		glTexCoord2f(0.0,0.0);
			glVertex2f(0.0,1.0);
        glEnd();
		
        if (n_x >= 0){
			float depth_click = 0.f;
			cudaMemcpy(&depth_click, (float*)(zed->retrieveMeasure_gpu(DEPTH).data + n_y * zed->retrieveMeasure_gpu(DEPTH).step + (n_x * 4)), 4, cudaMemcpyDeviceToHost);

			int depth_clamp = depth_click / 10.;
			ostringstream ostr;
			if (depth_clamp > 0)
				ostr << "Depth : " << depth_clamp / 100. << "m";
			else
				ostr << "Depth : NaN";
			printtext(r_x, r_y, ostr.str().c_str());
			glColor3f(1.0f, 1.0f, 1.0f);
        }



		//swap.
        glutSwapBuffers();
	}
	else
	{
		zed->setSVOPosition(0); //in case you read SVO, it will loop the SVO -- otherwise it will do nothing (LIVE).		
#ifdef WIN32
		Sleep(1);
#else
        usleep(10);
#endif
	}

	glutPostRedisplay();



	if (quit){
		glutDestroyWindow(1);

		delete zed;
		glDeleteShader(shaderF);
		glDeleteProgram(program);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


//main function
int main(int argc, char **argv) {

	if (argc > 2){
		std::cout << "Only the path of a SVO can be passed in arg" << std::endl;
		return -1;
	}
	//init glut
	glutInit(&argc, argv);

	/*Setting up  The Display  */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    //Configure Window Postion
    glutInitWindowPosition(50, 25);

    //Configure Window Size
    glutInitWindowSize(1280, 480);

    //Create Window
    glutCreateWindow("ZED DEPTH Viewer");
	DisplayModeStr = "DEPTH";

    //if youwant to have it in Fullscreen, you can uncomment the following line
	//glutFullScreen();

	//init GLEW Library
	glewInit();
	
	//Setup our ZED Camera (construct and Init)

	if (argc == 1 ) // Use in Live Mode
	{

        zed = new Camera(sl::zed::HD720,15.0);

        //other options :
        //zed = new Camera(sl::zed::VGA,15.0);
        //zed = new Camera(sl::zed::VGA,30.0);
        //zed = new Camera(sl::zed::HD720,15.0);



	}
	else			// Use in SVO playback mode
		zed = new Camera(argv[1]);
	

    ERRCODE err = zed->init(MODE::PERFORMANCE, 0,true,false); //last parameter to true in the camera is "reverted"
    std::cout<<"ZED Serial Number : "<<zed->getZEDSerial()<<std::endl;
    // ERRCODE display
    std::cout << "ZED Init Err : " << errcode2str(err) << std::endl;

   if (err != SUCCESS) {
		delete zed;
		return -1;
	}

	ViewID = 0;
	DisplayMode = 1;
	quit = false;
	dm_type = SENSING_MODE::FULL;
	PrintFPS = false;
    ShortRange =false;
    short_dist_val =3500; //short distance = 3.5 meters this can be changed with keyboard +/-

	n_x = -1;
	n_y = -1;

	//print everything
    reliabilityIdx = 100;
	//zed->setDispReliability(reliabilityIdx); !!function name has been change in Release 0.8 --see ChangeLog
	zed->setConfidenceThreshold(reliabilityIdx);


	//get Image Size 
	w = zed->getImageSize().width;
	h = zed->getImageSize().height;
 
    cudaError_t err1, err2;

	//Create and Register OpenGL Texture for Image (RGBA -- 4channels)
	glEnable(GL_TEXTURE_2D);  	
	glGenTextures(1,&imageTex);
	glBindTexture(GL_TEXTURE_2D, imageTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
    err1 = cudaGraphicsGLRegisterImage(&pcuImageRes, imageTex, GL_TEXTURE_2D,cudaGraphicsMapFlagsNone);
	
	
	//Create and Register a OpenGL texture for Depth (RGBA- 4 Channels)
	glGenTextures(1,&depthTex);
    glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
    err2 = cudaGraphicsGLRegisterImage(&pcuDepthRes, depthTex, GL_TEXTURE_2D, cudaGraphicsMapFlagsNone);

	if (err1!=0 || err2!=0)		return -1;
 
	//Create GLSL fragment Shader for future processing (and here flip R/B)
	GLuint shaderF = glCreateShader(GL_FRAGMENT_SHADER); //fragment shader
	const char* pszConstString = strFragmentShad.c_str ();
	glShaderSource(shaderF, 1, (const char**)&pszConstString, NULL);

    // compile the shader source code and check
	glCompileShader(shaderF);
    GLint compile_status = GL_FALSE;
    glGetShaderiv(shaderF, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE) return -2;
	 
	//create the progamm for both V and F Shader
	program = glCreateProgram();
	glAttachShader(program, shaderF);
	//and link + Check
	glLinkProgram(program);
    GLint link_status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status!=GL_TRUE)  return -2;
	  
	glUniform1i(glGetUniformLocation(program, "texImage"), 0);
	
	//ptr_Depth = zed->retrieveMeasure_gpu(DEPTH);


	//Set Draw Loop
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(draw);
	glutMouseFunc(mouse);
	glutMainLoop();
	
    return 0;
}



