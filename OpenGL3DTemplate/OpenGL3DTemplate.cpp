#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

double transX = 0.0;
double transY = 0.0;
double rotatVal = 0.0;
bool transFlag = false;
bool colorFlag = false;
double r = 0;
double g = 0;
double b = 0;
double gtx = 0;
double gty = 0;
double gtz = 0;
double gTransX = 0;
double gTransY = 0;
double gTransZ = 0;
bool gFlag = false;
bool flag = false;
bool flagAppearZ = false;
double gScalVal = 0.0;
double roomRotate = 0.0;
bool room = false;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f &v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f &v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 2.4f, float eyeY = 0.5f, float eyeZ = 0.0f,
		float centerX = 0.5f, float centerY = 0.5f, float centerZ = 0.0f,
		float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void top() {
		eye = Vector3f(0.4, 2.5, 0.0);
		center = Vector3f(0.0, 0.0, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void left() {
		eye = Vector3f(0.5, 0.6, 2.0);
		center = Vector3f(0.5, 0.5, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void right() {
		eye = Vector3f(0.5, 0.6, -2.0);
		center = Vector3f(0.5, 0.5, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void front() {
		eye = Vector3f(2.4, 0.5, 0.0);
		center = Vector3f(0.5, 0.5, 0.0);
		up = Vector3f(0.0, 1.0, 0.0);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}


void drawWall(double thickness) {
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
}

void drawSofa_ChairLeg(GLfloat radius,
	GLfloat height)
{
	GLfloat x = 0.0;
	GLfloat y = 0.0;
	GLfloat angle = 0.0;
	GLfloat angle_stepsize = 0.1;

	/** Draw the tube */
	//glColor3ub(R - 40, G - 40, B - 40);
	glBegin(GL_QUAD_STRIP);
	angle = 0.0;
	while (angle < 2 * 3.1415927) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y, height);
		glVertex3f(x, y, 0.0);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glVertex3f(radius, 0.0, 0.0);
	glEnd();

	/** Draw the circle on top of cylinder */
	glBegin(GL_POLYGON);
	angle = 0.0;
	while (angle < 2 * 3.1415927) {
		x = radius * cos(angle);
		y = radius * sin(angle);
		glVertex3f(x, y, height);
		angle = angle + angle_stepsize;
	}
	glVertex3f(radius, 0.0, height);
	glEnd();
}
void drawSofa() {

	glTranslated(0.2, 0.0, 0.4);

	//Sofa base
	glPushMatrix();
	glTranslated(0, 0.3, 0);
	glScaled(0.2, 0.1, 0.6);
	glutSolidCube(1.0);
	glPopMatrix();

	//Sofa back
	glPushMatrix();
	glTranslated(-0.1, 0.3 + 0.1, 0);
	glScaled(0.03, 0.3, 0.6);
	glutSolidCube(1.0);
	glPopMatrix();

	//Sofa arms
	glPushMatrix();
	glTranslated(0, 0.3 + 0.1, 0.285);
	glScaled(0.2, 0.1, 0.03);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.3 + 0.1, -0.285);
	glScaled(0.2, 0.1, 0.03);
	glutSolidCube(1.0);
	glPopMatrix();

	//Sofa legs
	glColor3f(0.6, 0.4, 0.2);
	glPushMatrix();
	glTranslated(0.07, 0.3, 0.25);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.075, 0.3, 0.25);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.07, 0.3, -0.25);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.075, 0.3, -0.25);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();

}

void drawChair() {
	glTranslated(0.6, 0.0, 0.8);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

	//Chair base
	glPushMatrix();
	glTranslated(0, 0.3, 0);
	glScaled(0.2, 0.1, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();

	//Chair back
	glPushMatrix();
	glTranslated(-0.1, 0.3 + 0.1, 0);
	glScaled(0.03, 0.3, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();

	//Chair arms
	glPushMatrix();
	glTranslated(0, 0.3 + 0.1, 0.085);
	glScaled(0.2, 0.1, 0.03);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.3 + 0.1, -0.085);
	glScaled(0.2, 0.1, 0.03);
	glutSolidCube(1.0);
	glPopMatrix();

	//chair legs
	glColor3f(0.6, 0.4, 0.2);
	glPushMatrix();
	glTranslated(0.07, 0.3, 0.05);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.075, 0.3, 0.05);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.07, 0.3, -0.05);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.075, 0.3, -0.05);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawSofa_ChairLeg(0.01, 0.3);
	glPopMatrix();
}

void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawTable() {
	double topWid = 0.2;
	double topThick = 0.02;
	double legThick = 0.01;
	double legLen = 0.3;

	glTranslatef(0.55, 0.0, 0.39);


	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95*topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
}

void drawClock() {

	//clock base
	glPushMatrix();
	glRotated(90, 0.0, 1.0, 0.0);
	glTranslated(-0.4, 0.7, 0.0);
	drawSofa_ChairLeg(0.1, 0.01);
	glPopMatrix();

	//clock center
	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.01, 0.7, 0.4);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();

	//Clockwise
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glTranslated(0.01, 0.7, 0.3);
	drawSofa_ChairLeg(0.01, 0.096);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glTranslated(0.01, 0.6, 0.3);
	glRotated(90.0, 1.0, 0.0, 0.0);
	glTranslated(-0.01, -0.6, -0.3);
	glTranslated(0.01, 0.7, 0.1);
	drawSofa_ChairLeg(0.01, 0.096);
	glPopMatrix();
}

void drawPicture() {
	glTranslated(-0.5, -0.7, -0.3);
	glRotated(-90.0, 0.0, 1.0, 0.0);
	glTranslated(-0.2, 0.7, -1.3);

	//Picture base
	glPushMatrix();
	glTranslated(0.5, 0.7, 0.3);
	glScaled(0.01, 0.25, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	//Picture center 
	glPushMatrix();
	glColor3f(0.2, 0.5, 0.5);
	glTranslated(0.53, 0.7 + 0.02, 0.3);
	glutSolidSphere(0.03, 50, 50);
	glPopMatrix();

	//Picture Arms
	glPushMatrix();
	glColor3f(0.2, 0.0, 0.9);
	glTranslated(0.53, 0.7 + 0.02, 0.3);
	drawSofa_ChairLeg(0.01, 0.2);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.2, 0.0, 0.9);
	glTranslated(0.53, 0.7 + 0.02, 0.1);
	drawSofa_ChairLeg(0.01, 0.2);
	glPopMatrix();
}


void drawBed() {
	double topWid = 0.65;
	double topThick = 0.03;
	double topLen = 0.3;
	double legThick = 0.01;
	double legLen = 0.18;

	glTranslated(0.4, 0.0, -0.2);

	//Bed base
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topLen);
	glutSolidCube(1.0);
	glPopMatrix();

	//Bed Back
	glPushMatrix();
	glTranslated(-0.3, legLen + topThick + 0.08, 0);
	glScaled(0.05, 0.19, topLen);
	glutSolidCube(1.0);
	glPopMatrix();

	//Bed legs
	glPushMatrix();
	glTranslated(-0.3, 0.0, -0.1);
	drawTableLeg(legThick, legLen);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.3, 0, 0.1);
	drawTableLeg(legThick, legLen);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.3, 0, -0.1);
	drawTableLeg(legThick, legLen);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.3, 0, 0.1);
	drawTableLeg(legThick, legLen);
	glPopMatrix();

	//Bed Martaba
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslated(0, legLen + topThick, 0);
	glScaled(topWid, topThick, topLen);
	glutSolidCube(1.0);
	glPopMatrix();

	//Bed cover
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.6);
	glTranslated(0.165, legLen + topThick + topThick - 0.01, -0.001);
	glScaled(topWid / 2, 0.01, topLen);
	glutSolidCube(1.0);
	glPopMatrix();

	//Bed Pillow
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.6);
	glTranslated(-0.255, 0.245, -0.15);
	drawSofa_ChairLeg(0.02, topLen);
	glPopMatrix();
}

void drawCommod() {
	double topWid = 0.2;
	double topThick = 0.05;
	double legThick = 0.01;
	double legLen = 0.15;

	glTranslated(0.18, 0.0, -0.5);

	//Commod drawers
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, legLen + topThick, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95*topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();

	//drawer knob
	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.1, 0.15, 0.0);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.1, 0.15 + topThick, 0.0);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();
}

void drawCupBoard() {
	glRotated(-90.0, 0.0, 1.0, 0.0);
	glTranslated(-1.3, 0.0, 0.3);
	glPushMatrix();
	glTranslated(0.5, 0.42, -0.6);
	glScaled(0.2, 0.8, 0.4);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.6, 0.44, -0.58);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.6, 0.44, -0.64);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();
}

void drawUnit() {
	double topWid = 0.2;
	double topThick = 0.05;
	double legThick = 0.01;
	double legLen = 0.15;

	glTranslated(0.5, 0.0, -0.5);

	glTranslated(0.5, 0.0, -0.5);
	glRotated(-90.0, 0.0, 1.0, 0.0);
	glTranslated(-0.5, 0.0, 0.5);

	glTranslated(0.7, 0.0, -0.2);

	//Unit's drawers
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, legLen + topThick, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, legLen + topThick + topThick, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, legLen + topThick + topThick + topThick, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	//Unit's legs
	double dist = 0.95*topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();

	//Unit's Mirror
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslated(-0.095, legLen + (4 * topThick) + 0.09, 0);
	glScaled(0.01, 0.25, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	//drawer knob
	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.1, 0.15, 0.0);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.1, 0.15 + topThick, 0.0);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.1, 0.15 + topThick + topThick, 0.0);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.6, 0.3, 0.5);
	glTranslated(0.1, 0.15 + topThick + topThick + topThick, 0.0);
	glutSolidSphere(0.01, 50, 50);
	glPopMatrix();

}

void drawShelf() {
	glPushMatrix();
	glTranslated(0.0, 0.5 + 0.09, -0.3);
	glScaled(0.01, 0.25, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0485, 0.5 + 0.09, -0.3);
	glScaled(0.097, 0.01, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0485, 0.5 + 0.09 + 0.125, -0.3);
	glScaled(0.097, 0.01, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0485, 0.5 + 0.09 - 0.125, -0.3);
	glScaled(0.097, 0.01, 0.5);
	glPopMatrix();
}

void drawGhost() {
	glColor3f(0.5, 0.1, 0.5);
	glTranslated(0.8, 0.5, 0.5);

	//Ghost Base
	glPushMatrix();
	glTranslated(0.0, -0.3, 0.0);
	glScaled(0.16, 0.08, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();

	//Ghost Hands
	glPushMatrix();
	glTranslated(0.0, -0.1, 0.0);
	drawSofa_ChairLeg(0.02, 0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, -0.1, -0.2);
	drawSofa_ChairLeg(0.02, 0.2);
	glPopMatrix();

	//Ghost head
	glPushMatrix();
	glutSolidSphere(0.1, 50, 50);
	glPopMatrix();

	//Ghost body
	glPushMatrix();
	glColor3f(0.5, 0.1, 0.5);
	glRotated(90.0, 1.0, 0.0, 0.0);
	glTranslated(0.0, 0.0, 0.03);
	drawSofa_ChairLeg(0.08, 0.3);
	glPopMatrix();

	//Ghost Eye
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslated(0.07, 0.02, 0.0);
	glutSolidSphere(0.04, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glTranslated(0.1, 0.02, 0.0);
	glutSolidSphere(0.02, 50, 50);
	glPopMatrix();


}

void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glRotated(roomRotate, 1.0, 0.0, 0.0);

	//Ghost
	glPushMatrix();
	glTranslated(gtx, gty, gtz);
	glTranslated(gTransX, gTransY, gTransZ);
	glScaled(gScalVal, gScalVal, gScalVal);
	if (gFlag == true) {
		drawGhost();
	}
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, transY, 0.0);
	glTranslated(-0.5, 0.5, -0.5);
	glRotated(rotatVal, 0.0, 1.0, 0.0);
	glTranslated(0.5, -0.5, 0.5);
	//Bedroom components
	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.4, 0.2);
	}
	drawBed();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.4, 0.2);
	}
	drawCommod();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.4, 0.2);
	}drawUnit();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.4, 0.2);
	}
	drawCupBoard();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.4, 0.2);
	}
	drawShelf();
	glPopMatrix();


	glPopMatrix();

	//Bedroom walls
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.3, 0.3);
	}
	glPushMatrix();
	glTranslated(0.0, 0.0, -1.0);
	drawWall(0.02);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.0, 0.0, -1.0);
	glRotated(90, 0, 0, 1.0);
	drawWall(0.02);
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWall(0.02);
	glPopMatrix();


	//Livingroom wall
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	drawWall(0.02);
	glPopMatrix();

	glPushMatrix();
	drawWall(0.02);
	glPopMatrix();

	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWall(0.02);
	glPopMatrix();

	glTranslated(0.0, transY, 0.0);
	glTranslated(0.5, 0.5, 0.5);
	glRotated(rotatVal, 0.0, 1.0, 0.0);
	glTranslated(-0.5, -0.5, -0.5);
	//Livingroom components
	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.4, 0.2);
	}
	drawTable();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.7, 0.9);
	}
	drawSofa();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.6, 0.7, 0.9);
	}
	drawChair();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.2, 0.0, 0.0);
	}
	drawClock();
	glPopMatrix();

	glPushMatrix();
	if (colorFlag == true) {
		glColor3f(r, g, b);
	}
	else {
		glColor3f(0.2, 0.6, 0.1);
	}
	drawPicture();
	glPopMatrix();

	glFlush();
}


void Anim(void) {

	//furniture animation
	if (transFlag == true && transY<1) {
		transY += 0.001;
		rotatVal += 0.1;
	}
	else {
		transY = 0;
		rotatVal = 0;
	}

	//Room animation
	if (room == true && roomRotate<30) {
		roomRotate += 0.1;
	}
	else {
		roomRotate = 0;
	}

	//Random Colors
	if (colorFlag == true) {
		r = rand() / double(RAND_MAX);
		g = rand() / double(RAND_MAX);
		b = rand() / double(RAND_MAX);
	}

	//Ghost Animation
	if (gFlag == true && gTransZ <= 1.0 && gTransZ >= -1
		&& gTransY <= 1.0 && gScalVal <= 1.0
		//&& gTransX <= 1.0
		) {
		if (flag == true) {
			gTransZ += 0.004;
		}
		else {
			gTransZ -= 0.004;
		}
		gTransY += 0.004;
		gScalVal += 0.004;
		//gTransX += 0.004;
	}
	else {
		gScalVal = 0;
		gTransZ = 0;
		gTransY = 0;
		//gTransX = 0;
		gFlag = false;
	}
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;
	float a = 1.0;
	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 'i':
		camera.rotateX(a);
		break;
	case 'k':
		camera.rotateX(-a);
		break;
	case 'l':
		camera.rotateY(-a);
		break;
	case 'j':
		camera.rotateY(a);
		break;
	case ' ':
		transFlag = true;
		break;
	case 'r':
		transFlag = false;
		break;
	case 'c':
		colorFlag = true;
		break;
	case 'x':
		colorFlag = false;
		break;
	case 'y':
		room = true;
		break;
	case 'u':
		room = false;
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}
	glutPostRedisplay();
}
void Special(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_UP:
		camera.top();
		break;
	case GLUT_KEY_DOWN:
		camera.front();
		break;
	case GLUT_KEY_LEFT:
		camera.left();
		break;
	case GLUT_KEY_RIGHT:
		camera.right();
		break;
	}

	glutPostRedisplay();
}

void ghostTimer(int value) {
	gFlag = true;
	gtx = rand() / double(RAND_MAX);
	gty = rand() / double(RAND_MAX);
	gtz = rand() / double(RAND_MAX);
	if (flagAppearZ == true) {
		gtz = gtz - 1;
	}
	if (flag == true) {
		flag = false;
	}
	else {
		flag = true;
	}

	glutPostRedisplay();
	glutTimerFunc(2 * 1000, ghostTimer, 0);
}

void ghostAppear(int value) {
	if (flagAppearZ == true) {
		flagAppearZ = false;
	}
	else {
		flagAppearZ = true;
	}

	glutPostRedisplay();
	glutTimerFunc(3 * 1000, ghostAppear, 0);
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Haunted House");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutIdleFunc(Anim);
	glutTimerFunc(0, ghostTimer, 0);
	glutTimerFunc(0, ghostAppear, 0);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
