#include "grid.h"
#include "floorplan.h"
#include "cloth.h"

// Included files for OpenGL Rendering
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

GLCanvas* Grid::parent;
Floorplan* Grid::floorplan;
int Grid::my_window;
int Grid::mouseButton;
int Grid::mouseState;
bool Grid::toggleState;
bool Grid::expanding;

void Grid::initialize(GLCanvas* canvas, Floorplan* _floorplan, int window) {
	parent = canvas;
	floorplan = floorplan;
	my_window = glutCreateSubWindow(window, 800, 400, 400, 400);
	set_expanding(false);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);
}

void Grid::display(void) {
	float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
	float xmin = parent->get_cloth()->getBoundingBox().getMin().x();
	float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
	float zmin = parent->get_cloth()->getBoundingBox().getMin().x();

	glClearColor(0.2,0.4,0.3,0);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();

	glPointSize(5);
    glLineWidth(2);
    glDisable(GL_LIGHTING);

	glColor3f(0.8f,0.8f,0.6f);
	glBegin(GL_LINES);
	for (int i = 0; i < parent->get_cloth()->getNumParticles(); i++) {
		for (unsigned int j = 0; j < parent->get_cloth()->getParticle(i).getStructural().size(); j++) {
			glVertex2f(0.9*((((parent->get_cloth()->getParticle(i).getPosition().x()-xmin) / xconst) * 2) - 1),
				       0.9*((((parent->get_cloth()->getParticle(i).getPosition().z()-zmin) / zconst) *-2) + 1));
			glVertex2f(0.9*((((parent->get_cloth()->getParticle(parent->get_cloth()->getParticle(i).getStructural()[j].getTarget()).getPosition().x()-xmin) / xconst) * 2) - 1),
				       0.9*((((parent->get_cloth()->getParticle(parent->get_cloth()->getParticle(i).getStructural()[j].getTarget()).getPosition().z()-zmin) / zconst) *-2) + 1));
		}
	}

	glEnd();

	glBegin(GL_POINTS);
	for (int i = 0; i < parent->get_cloth()->getNumParticles(); i++) {
		if (floorplan->get_selected_point_int() != -1 && floorplan->get_selected_point().get_p() == i) glColor3f(1.0,0.6,0.2);
		else if (parent->get_cloth()->isPointEnabled(i)) glColor3f(0,0,0);
		else glColor3f(0.5f, 0.5f, 0.5f);
		glVertex2f(0.9*((((parent->get_cloth()->getParticle(i).getPosition().x()-xmin) / xconst) * 2) - 1),
			       0.9*((((parent->get_cloth()->getParticle(i).getPosition().z()-zmin) / zconst) *-2) + 1));
	}
	glEnd();

	glutSetWindow(floorplan->get_window());
	floorplan->Render();
	glutSetWindow(my_window);

	glutSwapBuffers();
}

void Grid::reshape(int w, int h) {
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glutPostRedisplay();
}

void Grid::mouse(int button, int state, int x, int y) {
	mouseButton = button;
	mouseState = state;
	float xpos, ypos;
	if (mouseButton == GLUT_LEFT_BUTTON && mouseState == GLUT_DOWN) {
		float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
		float xmin = parent->get_cloth()->getBoundingBox().getMin().x();
		float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
		float zmin = parent->get_cloth()->getBoundingBox().getMin().x();
		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_WIDTH);
		for (int i = 0; i < parent->get_cloth()->getNumParticles(); i++) {
			xpos = (0.9*((parent->get_cloth()->getParticle(i).getPosition().x()-xmin) / xconst)+0.05) * width;
			ypos = (0.9*((parent->get_cloth()->getParticle(i).getPosition().z()-zmin) / zconst)+0.05) * height;
			if (sqrt(pow(xpos-x, 2) + pow(ypos-y, 2)) < 3 && parent->get_cloth()->isPointEnabled(i))
				floorplan->set_selected_point(i);
		}
	}
	if (mouseButton == GLUT_RIGHT_BUTTON && mouseState == GLUT_DOWN) {
		float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
		float xmin = parent->get_cloth()->getBoundingBox().getMin().x();
		float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
		float zmin = parent->get_cloth()->getBoundingBox().getMin().x();
		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_WIDTH);
		for (int i = 0; i < parent->get_cloth()->getNumParticles(); i++) {
			xpos = (0.9*((parent->get_cloth()->getParticle(i).getPosition().x()-xmin) / xconst)+0.05) * width;
			ypos = (0.9*((parent->get_cloth()->getParticle(i).getPosition().z()-zmin) / zconst)+0.05) * height;
			if (sqrt(pow(xpos-x, 2) + pow(ypos-y, 2)) < 3) {
				if (parent->get_cloth()->isPointEnabled(i)) {
					parent->get_cloth()->disable_particle(i);
					toggleState = false;
				} else {
					parent->get_cloth()->enable_particle(i);
					toggleState = true;
				}
				
			}
		}
	}
	glutPostRedisplay();
}

void Grid::motion(int x, int y) {
	if (mouseButton == GLUT_RIGHT_BUTTON && mouseState == GLUT_DOWN) {
		float xpos, ypos;
		float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
		float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_WIDTH);
		for (int i = 0; i < parent->get_cloth()->getNumParticles(); i++) {
			xpos = parent->get_cloth()->getParticle(i).getPosition().x() / xconst * width;
			ypos = parent->get_cloth()->getParticle(i).getPosition().z() / zconst * height;
			if (sqrt(pow(xpos-x, 2) + pow(ypos-y, 2)) < 3) {
				if (toggleState == true) {
					parent->get_cloth()->enable_particle(i);
				} else {
					parent->get_cloth()->disable_particle(i);
				}
			}
		}
		glutPostRedisplay();
	}
}

void Grid::passiveMotion(int x, int y) {
	floorplan->set_expanding(false);
	set_expanding(true);
}

void Grid::expand() {
	glutSetWindow(my_window);
	
	if (expanding && glutGet(GLUT_WINDOW_WIDTH) < 600) {
		glutPopWindow();
		glutPositionWindow(600, 200);
		glutReshapeWindow(600, 600);
	} else if (!expanding && glutGet(GLUT_WINDOW_WIDTH) > 400) {
		glutPositionWindow(800, 400);
		glutReshapeWindow(400, 400);
		glutPushWindow();
	}
}

void Grid::Render() {
	glutPostRedisplay();
}