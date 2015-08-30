//CloudViewer.hpp

#ifndef __CLOUD_VIEWER_INCLUDE__
#define __CLOUD_VIEWER_INCLUDE__

#include "PointCloud.hpp"
#include "utils.hpp"
#include <math.h>
#include "GL/glut.h"    /* OpenGL Utility Toolkit header */

class TrackBallCamera {
public:
	TrackBallCamera() {};
	TrackBallCamera(vect3 p, vect3 la);
	void applyTransformations();
	void show();
	void rotation(float angle, vect3 v);
	void rotate(float speed, vect3 v);
	void translate(vect3 v);
	void translateLookAt(vect3 v);
	void translateAll(vect3 v);
	void zoom(float z);

	vect3 getPosition();
	vect3 getPositionFromLookAt();
	vect3 getLookAt();
	vect3 getForward();
	vect3 getUp();
	vect3 getLeft();

	void setPosition(vect3 p);
	void setLookAt(vect3 p);

private:
	vect3 position;
	vect3 lookAt;
	vect3 forward;
	vect3 up;
	vect3 left;
	float angleX;

	void setAngleX();
};

class CloudViewer {
public:
	CloudViewer();
	virtual ~CloudViewer();
	void AddData(PointCloud *cloud);
	void UpDate();
	void StopDraw();
	unsigned char getKey();

private:
	void Init();

	static void RedrawCallback();
	static void mouseCallback(int button, int state, int x, int y);
	static void keyCallback(unsigned char c, int x, int y);
	static void motionCallback(int x, int y);
	static void reshapeCallback(int width, int height);

	void setupCallback();

	PointCloud *cloud;
	bool ptr_points_locked;

	void Redraw();
	void mouse(int button, int state, int x, int y);
	void key(unsigned char c, int x, int y);
	void motion(int x, int y);
	void reshape(int width, int height);
	int windowWidth, windowHeight;
	bool resized;

	//! Mouse Save Position
	bool Rotate;
	bool Translate;
	bool Zoom;
	int startx;
	int starty;

	TrackBallCamera camera;

	GLfloat point_size;

	void Visualize();
	void DrawRepere(vect3 pos);
	void DrawTrapeze();

	bool data_point;

	unsigned char keyPressed;
	bool invertMotion;

	//!Thread unix
	static void* glutThreadFunc(void* v);
};


#endif	/* __CLOUD_VIEWER_INCLUDE__ */