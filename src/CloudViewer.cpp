// CloudViewer.cpp

#include "CloudViewer.hpp"

#include <iostream>

using namespace sl::zed;

CloudViewer* ptr;

void idle(void) {
	glutPostRedisplay();
}

TrackBallCamera::TrackBallCamera(vect3 p, vect3 la) {
	position.x = p.x;
	position.y = p.y;
	position.z = p.z;

	lookAt.x = la.x;
	lookAt.y = la.y;
	lookAt.z = la.z;
	angleX = 0.0f;
	applyTransformations();
}

void TrackBallCamera::applyTransformations() {
	forward = vect3(lookAt.x - position.x,
			lookAt.y - position.y,
			lookAt.z - position.z);
	left = vect3(forward.z,
			0,
			-forward.x);
	up = vect3(left.y * forward.z - left.z * forward.y,
			left.z * forward.x - left.x * forward.z,
			left.x * forward.y - left.y * forward.x);
	forward.normalise();
	left.normalise();
	up.normalise();
}

void TrackBallCamera::show() {
	gluLookAt(position.x, position.y, position.z,
			lookAt.x, lookAt.y, lookAt.z,
			0.0, 1.0, 0.0);
}

void TrackBallCamera::rotation(float angle, vect3 v) {
	translate(vect3(-lookAt.x, -lookAt.y, -lookAt.z));
	position.rotate(angle, v);
	translate(vect3(lookAt.x, lookAt.y, lookAt.z));
	setAngleX();
}

void TrackBallCamera::rotate(float speed, vect3 v) {
	float tmpA;
	float angle = speed / 360.0f;
	(v.x != 0.0f) ? tmpA = angleX - 90.0f + angle : tmpA = angleX - 90.0f;
	if (tmpA < 89.5f && tmpA > -89.5f) {
		translate(vect3(-lookAt.x, -lookAt.y, -lookAt.z));
		position.rotate(angle, v);
		translate(vect3(lookAt.x, lookAt.y, lookAt.z));
	}
	setAngleX();
}

void TrackBallCamera::translate(vect3 v) {
	position.x += v.x;
	position.y += v.y;
	position.z += v.z;
}

void TrackBallCamera::translateLookAt(vect3 v) {
	lookAt.x += v.x;
	lookAt.y += v.y;
	lookAt.z += v.z;
}

void TrackBallCamera::translateAll(vect3 v) {
	translate(v);
	translateLookAt(v);
}

void TrackBallCamera::zoom(float z) {
	float dist = vect3::length(vect3(position.x - lookAt.x, position.y - lookAt.y, position.z - lookAt.z));
	if (dist - z > z) {
		translate(vect3(forward.x * z, forward.y * z, forward.z * z));
	}
}

vect3 TrackBallCamera::getPosition() {
	return vect3(position.x, position.y, position.z);
}

vect3 TrackBallCamera::getPositionFromLookAt() {
	return vect3(position.x - lookAt.x, position.y - lookAt.y, position.z - lookAt.z);
}

vect3 TrackBallCamera::getLookAt() {
	return vect3(lookAt.x, lookAt.y, lookAt.z);
}

vect3 TrackBallCamera::getForward() {
	return vect3(forward.x, forward.y, forward.z);
}

vect3 TrackBallCamera::getUp() {
	return vect3(up.x, up.y, up.z);
}

vect3 TrackBallCamera::getLeft() {
	return vect3(left.x, left.y, left.z);
}

void TrackBallCamera::setPosition(vect3 p) {
	position.x = p.x;
	position.y = p.y;
	position.z = p.z;
	setAngleX();
}

void TrackBallCamera::setLookAt(vect3 p) {
	lookAt.x = p.x;
	lookAt.y = p.y;
	lookAt.z = p.z;
	setAngleX();
}

void TrackBallCamera::setAngleX() {
	angleX = vect3::getAngle(vect3(position.x, position.y + 1, position.z),
			vect3(position.x, position.y, position.z),
			vect3(lookAt.x, lookAt.y, lookAt.z));
}

CloudViewer::CloudViewer() {
	camera = TrackBallCamera(vect3(0.0f, .1f, .5f), vect3(0.0f, 0.0f, 0.0f));
	point_size = 5;
	data_point = false;
	ptr_points_locked = false;
	Translate = false;
	Rotate = false;
	Zoom = false;
	invertMotion = false;

	setupCallback();

	pthread_t glutThreadId;
	pthread_create(&glutThreadId, NULL, glutThreadFunc, 0);
}

CloudViewer::~CloudViewer() {

}

void CloudViewer::setupCallback() {
	ptr = this;
}

void CloudViewer::RedrawCallback() {
	ptr->Redraw();
}

void CloudViewer::mouseCallback(int button, int state, int x, int y) {
	ptr->mouse(button, state, x, y);
}

void CloudViewer::keyCallback(unsigned char c, int x, int y) {
	ptr->key(c, x, y);
}

void CloudViewer::motionCallback(int x, int y) {
	ptr->motion(x, y);
}

void CloudViewer::reshapeCallback(int width, int height) {
	ptr->reshape(width, height);
}

void* CloudViewer::glutThreadFunc(void* v) {
	ptr->Init();
	glutMainLoop();
	return v;
}

void CloudViewer::Init() {
	char *argv[1];
	int argc = 1;

	argv[0] = strdup("ZED View");

	glutInit(&argc, argv);

	glutInitWindowSize(1000, 1000);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("ZED 3D Viewer");

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(75.0,
			1.0,
			.20, 25000.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 6.0,
			0.0, 0.0, 0.0,
			0.0, .10, 0.0);

	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glutDisplayFunc(RedrawCallback);
	glutMouseFunc(mouseCallback);
	glutKeyboardFunc(keyCallback);
	glutMotionFunc(motionCallback);
	glutReshapeFunc(reshapeCallback);
	glutIdleFunc(idle);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void CloudViewer::UpDate() {
	glutPostRedisplay();
}

void CloudViewer::DrawRepere(vect3 pos) {

	float length_ = 0.1;

	glBegin(GL_LINES);
	//! X
	glColor3f(1, 0, 0);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x + length_, pos.y, pos.z);

	//! Y
	glColor3f(0, 1, 0);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x, pos.y + length_, pos.z);

	//! Z
	glColor3f(0.3f, 0.3f, 1);
	glVertex3f(pos.x, pos.y, pos.z);
	glVertex3f(pos.x, pos.y, pos.z + length_);
	glEnd();
}

void CloudViewer::DrawTrapeze() {
	glBegin(GL_LINES);
	glColor3f(0.2f, 0.2f, 0.2f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-3.5f, -2.5f, -5.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(3.5f, -2.5f, -5.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-3.5f, 2.5f, -5.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(3.5f, 2.5f, -5.0f);

	glVertex3f(-3.5f, -2.5f, -5.0f);
	glVertex3f(3.5f, -2.5f, -5.0f);

	glVertex3f(3.5f, -2.5f, -5.0f);
	glVertex3f(3.5f, 2.5f, -5.0f);

	glVertex3f(3.5f, 2.5f, -5.0f);
	glVertex3f(-3.5f, 2.5f, -5.0f);

	glVertex3f(-3.5f, 2.5f, -5.0f);
	glVertex3f(-3.5f, -2.5f, -5.0f);

	glEnd();
}

void CloudViewer::Visualize() {
	glPointSize(point_size);

	if (data_point && !ptr_points_locked) {
		ptr_points_locked = true;
		glBegin(GL_LINES);
		int skipped = 0;
		for (int i = 0; i < cloud->GetNbPointsHM() + skipped; i++) {
			POINT3D temp = cloud->PointHM(i);
			if (temp.r + temp.b + temp.g == 0) {
				skipped++;
				continue;
			}
			if (cloud->PointHM(i).z > 0) {
				glColor4f(cloud->PointHM(i).r, cloud->PointHM(i).g, cloud->PointHM(i).b, 0.75);
				glVertex3f(cloud->PointHM(i).x, 0,                    -cloud->PointHM(i).z);
				glVertex3f(cloud->PointHM(i).x, -cloud->PointHM(i).y, -cloud->PointHM(i).z);
			}
		}
		glEnd();
		ptr_points_locked = false;
	}
}

void CloudViewer::StopDraw() {
	data_point = false;
}

void CloudViewer::AddData(PointCloud *cloud) {
	if (!ptr_points_locked) {
		ptr_points_locked = true;
		this->cloud = cloud;
		data_point = true;
		ptr_points_locked = false;
	}
}

void CloudViewer::Redraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glPushMatrix();
	camera.applyTransformations();
	camera.show();
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.12f, 0.12f, 0.12f, 1.0f);

	Visualize();
	// DrawRepere(camera.getLookAt());
	// DrawTrapeze();
	glPopMatrix();
	glutSwapBuffers();
}

void CloudViewer::mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			Rotate = true;
			startx = x;
			starty = y;
		}
		if (state == GLUT_UP) {
			Rotate = false;
		}
	}
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			Translate = true;
			startx = x;
			starty = y;
		}
		if (state == GLUT_UP) {
			Translate = false;
		}
	}

	if (button == GLUT_MIDDLE_BUTTON) {
		if (state == GLUT_DOWN) {
			Zoom = true;
			startx = x;
			starty = y;
		}
		if (state == GLUT_UP) {
			Zoom = false;
		}
	}

	if ((button == 3) || (button == 4)) {
		if (state == GLUT_UP) return;
		if (button == 3) {
			camera.zoom(0.2);
		} else
			camera.zoom(-0.2);
	}
}

void CloudViewer::motion(int x, int y) {
	if (Translate) {
		float Trans_x = (x - startx) / 60.0f;
		float Trans_y = (y - starty) / 60.0f;
		vect3 left = camera.getLeft();
		vect3 up = camera.getUp();
		if (invertMotion) {
			camera.translateAll(vect3(left.x * -Trans_x, left.y * -Trans_x, left.z * -Trans_x));
			camera.translateAll(vect3(up.x * Trans_y, up.y * Trans_y, up.z * Trans_y));
		} else {
			camera.translateAll(vect3(left.x * Trans_x, left.y * Trans_x, left.z * Trans_x));
			camera.translateAll(vect3(up.x * -Trans_y, up.y * -Trans_y, up.z * -Trans_y));
		}
		startx = x;
		starty = y;
	}

	if (Zoom) {
		camera.zoom((float) (y - starty) / 5.0f);
		starty = y;
	}

	if (Rotate) {
		float sensitivity = 100.0f;
		float Rot = y - starty;
		vect3 tmp = camera.getPositionFromLookAt();
		tmp.y = tmp.x;
		tmp.x = -tmp.z;
		tmp.z = tmp.y;
		tmp.y = 0.0f;
		tmp.normalise();
		camera.rotate(Rot * sensitivity, tmp);

		Rot = x - startx;
		camera.rotate(-Rot * sensitivity, vect3(0.0f, 1.0f, 0.0f));

		startx = x;
		starty = y;
	}

	glutPostRedisplay();
}

void CloudViewer::reshape(int width, int height) {
	float windowWidth = width;
	float windowHeight = height;

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0,
			windowWidth / windowHeight,
			.20, 25000.0);
	glMatrixMode(GL_MODELVIEW);
}

void CloudViewer::key(unsigned char c, int x, int y) {

	keyPressed = c;
	switch (c) {
		case 'i':
			invertMotion = !invertMotion;
			break;
		case 'w':
			if (point_size > 0)
				point_size--;
			break;
		case 'x':
			point_size++;
			break;
		case 'o':
			camera.setPosition(vect3(0.0f, .1f, .5f));
			camera.setLookAt(vect3(0.0f, 0.0f, 0.0f));
			break;
		case 't':
			camera.setPosition(vect3(0.0f, 5.0f, -2.95f));
			camera.setLookAt(vect3(0.0f, 0.0f, -3.0f));
			break;
		default:
			break;
	}

	glutPostRedisplay();
}

unsigned char CloudViewer::getKey() {
	unsigned char key_swap = keyPressed;
	keyPressed = ' ';

	return key_swap;
}
