#include "floorplan.h"
#include <iostream>
#include "math.h"
#include "vectors.h"
#include "grid.h"

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

GLCanvas* Floorplan::parent;
std::vector<Point> Floorplan::coords;
int Floorplan::selected_point;
int Floorplan::selected_line;
int Floorplan::selected_control_point;
int Floorplan::display_window;
int Floorplan::my_window;
int Floorplan::grid_window;
Grid* Floorplan::my_grid;
int Floorplan::default_menu_id;
int Floorplan::line_menu_id;
int Floorplan::point_menu_id;
bool Floorplan::expanding;

int Floorplan::mouseButton;
int Floorplan::mouseStatus;
int Floorplan::mouseX;
int Floorplan::mouseY;
bool Floorplan::controlPressed;

float distance(float x1, float y1, float x2, float y2);
float point_to_line(float x0, float y0, float x1, float y1, float x2, float y2);

void Floorplan::initialize(GLCanvas* canvas, int window) {
	parent = canvas;
	my_window = glutCreateSubWindow(window, 800, 0, 400, 400);

	set_expanding(false);
	
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	for (int i = 0; i < parent->get_cloth()->num_fixed; i++) {
		add_cloth_point(parent->get_cloth()->fixed_particles[i], parent->get_cloth());
	}
	//coords[0].set_next(1);
	//coords[1].set_next(3);
	//coords[2].set_next(0);
	//coords[3].set_next(2);

	selected_line = -1;

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc(keyboard);

    default_menu_id = glutCreateMenu(default_menu);
	glutAddMenuEntry("Save as .obj", 's');
	glutAddMenuEntry("Load input.obj", 'l');
    glutAddMenuEntry("Quit", 'q');
    glutAttachMenu(GLUT_RIGHT_BUTTON);

	line_menu_id = glutCreateMenu(line_menu);
	glutAddMenuEntry("Attach", 'a');
	glutAddMenuEntry("Detach", 'd');
	glutAddMenuEntry("Set Bezier", 'b');
	glutAddMenuEntry("Set Line", 'l');

	point_menu_id = glutCreateMenu(point_menu);
	glutAddMenuEntry("Set Height", 'h');
	glutAddMenuEntry("Delete", 'd');
}

void Floorplan::display(void) {
	float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
	float xmin = parent->get_cloth()->getBoundingBox().getMin().x();
	float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
	float zmin = parent->get_cloth()->getBoundingBox().getMin().x();

	glClearColor(0.06,0.04,0.15,0);
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glPointSize(5);
    glLineWidth(1.5);
	glColor3f(0.5f, 0.6f, 0.8f);
    glDisable(GL_LIGHTING);

	bool line = false;

	glBegin(GL_LINES);
	for(unsigned int i = 0; i < coords.size(); i++) {
		/*if (coords[i].is_bezier()) {
			Point p1, p2, point;
			std::vector<Point> points;
			p1 = coords[i];
			p2 = coords[coords[i].get_next()];

			points = coords[i].get_bezier_points();
			Point prev = bezier(points, 0);
			int length = max(abs(p1.get_i()-p2.get_i()), abs(p1.get_j()-p2.get_j()));
			for (int j = 1; j <= length; j++) {
				float t = 1.0/length*j;
				point = bezier(points, t);
				float x1 = float(prev.get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
				float y1 = float(prev.get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
				float x2 = float(point.get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
				float y2 = float(point.get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
				glVertex2f(x1, y1);
				glVertex2f(x2, y2);
				prev = point;
			}
			
			if (i == selected_line) {
				glColor3f(0.5f, 0.5f, 0.5f);
			} else {
				glColor3f(0.75f, 0.75f, 0.75f);
			}
			for (int j = 0; j < 3; j++) {
				float x1 = float(coords[i].get_bezier_points()[j].get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
				float y1 = float(coords[i].get_bezier_points()[j].get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
				float x2 = float(coords[i].get_bezier_points()[j+1].get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
				float y2 = float(coords[i].get_bezier_points()[j+1].get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
				glVertex2f(x1, y1);
				glVertex2f(x2, y2);
			}
			glEnd();

			glBegin(GL_POINTS);
			float x1 = float(coords[i].get_bezier_points()[1].get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
			float y1 = float(coords[i].get_bezier_points()[1].get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
			float x2 = float(coords[i].get_bezier_points()[2].get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
			float y2 = float(coords[i].get_bezier_points()[2].get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
			if (i == selected_line && 1 == selected_control_point) glColor3f(0.3f, 0.5f, 1.0f);
			else glColor3f(0.3f, 0.3f, 0.3f);
			glVertex2f(x1, y1);
			if (i == selected_line && 2 == selected_control_point) glColor3f(0.3f, 0.5f, 1.0f);
			else glColor3f(0.3f, 0.3f, 0.3f);
			glVertex2f(x2, y2);
			glEnd();

			glBegin(GL_LINES);
		} else {*/
			float x1 = 0.9*((coords[i].get_x()-xmin) / xconst * 2 - 1);
			float y1 = 0.9*((coords[i].get_y()-zmin) / zconst *-2 + 1);
			float x2 = 0.9*((coords[coords[i].get_next()].get_x()-xmin) / xconst * 2 - 1);
			float y2 = 0.9*((coords[coords[i].get_next()].get_y()-zmin) / zconst *-2 + 1);
			glVertex2f(x1, y1);
			glVertex2f(x2, y2);
		//}
	}
	glEnd();

	if (selected_line != -1) {
		glColor4f(1.0f, 1.0f, 0.5f, 0.7f);
		glLineWidth(4);
		glBegin(GL_LINES);

		/*if (coords[selected_line].is_bezier()) {
			Point p1, p2, point;
			std::vector<Point> points;
			p1 = coords[selected_line];
			p2 = coords[coords[selected_line].get_next()];

			points = coords[selected_line].get_bezier_points();
			Point prev = bezier(points, 0);
			int length = max(abs(p1.get_i()-p2.get_i()), abs(p1.get_j()-p2.get_j()));
			for (int j = 1; j <= length; j++) {
				float t = 1.0/length*j;
				point = bezier(points, t);
				float x1 = float(prev.get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
				float y1 = float(prev.get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
				float x2 = float(point.get_x()) / float(glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
				float y2 = float(point.get_y()) / float(glutGet(GLUT_WINDOW_HEIGHT)) * -2 + 1;
				glVertex2f(x1, y1);
				glVertex2f(x2, y2);
				prev = point;
			}
		} else {*/
			float x1 = 0.9*((coords[selected_line].get_x()-xmin) / xconst * 2 - 1);
			float y1 = 0.9*((coords[selected_line].get_y()-zmin) / zconst *-2 + 1);
			float x2 = 0.9*((coords[coords[selected_line].get_next()].get_x()-xmin) / xconst * 2 - 1);
			float y2 = 0.9*((coords[coords[selected_line].get_next()].get_y()-zmin) / zconst *-2 + 1);
			glVertex2f(x1, y1);
			glVertex2f(x2, y2);
		//}

		glEnd();
	}

	glBegin(GL_POINTS);
	for(unsigned int i = 0; i < coords.size(); i++) {
		float x = 0.9*((coords[i].get_x()-xmin) / xconst * 2 - 1);
		float y = 0.9*((coords[i].get_y()-zmin) / zconst *-2 + 1);
		
		if (i == selected_point) {
			glColor3f(1.0,0.95,0.3);
			glVertex2f(x, y);
		} else {
			glColor3f(0.6,0.7,0.9);
			glVertex2f(x, y);
		}
	}
	glEnd();

	glutSetWindow(display_window);
	parent->Render();
	glutSetWindow(my_window);

	glFlush();
	glutSwapBuffers();
}

void Floorplan::reshape(int w, int h) {
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glutPostRedisplay();
}

void Floorplan::mouse(int button, int state, int x, int y) {
	float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
	float xmin = parent->get_cloth()->getBoundingBox().getMin().x();
	float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
	float zmin = parent->get_cloth()->getBoundingBox().getMin().x();
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	mouseButton = button;
	mouseStatus = state;
	mouseX = x;
	mouseY = y;
	//controlPressed = glutGetModifiers() & GLUT_ACTIVE_CTRL;

	if (mouseButton == GLUT_LEFT_BUTTON && mouseStatus == GLUT_DOWN) {
		float tmp, min = 10;
		for (unsigned int i = 0; i < coords.size(); i++) {
			float px = (0.9*(coords[i].get_x()-xmin)/xconst + 0.05) * width;
			float py = (0.9*(coords[i].get_y()-zmin)/zconst + 0.05) * height;
			tmp = distance(x, y, px, py);
			if (tmp < min) {
				min = tmp;
			}
		}
		if (selected_line != -1 && coords[selected_line].is_bezier()) {
			tmp = distance(x, y, coords[selected_line].get_bezier_points()[1].get_x(), coords[selected_line].get_bezier_points()[1].get_y());
			if (tmp < min) min = tmp;
			tmp = distance(x, y, coords[selected_line].get_bezier_points()[2].get_x(), coords[selected_line].get_bezier_points()[2].get_y());
			if (tmp < min) min = tmp;
		}
		if (min > 5) {
			add_point(x, y, -1);
			selected_line = -1;
			if (coords.size() > 1) {
				selected_point = coords.size()-1;
				glutSetWindow(grid_window);
				my_grid->Render();
				glutSetWindow(my_window);
			}
			glutSetMenu(point_menu_id);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
		}
	}
	glutPostRedisplay();
}

void Floorplan::add_point(int x, int y, int p) {
	Point tmp;
	tmp.set_pos(x, y);
	tmp.set_original_pos(x, y);
	tmp.set_p(p);
	tmp.unfix();
	tmp.set_bezier(false);
	
	int closest = 0;
	int next = 0;
	float min_distance = ((coords.size() > 0) ? distance(x, y, coords[0].get_x(), coords[0].get_y()) : 100);
	for (unsigned int i = 1; i < coords.size(); i++) {
		next = coords[i].get_next();
		float dist = point_to_line(x, y, coords[i].get_x(), coords[i].get_y(), coords[next].get_x(), coords[next].get_y());
		if (dist < min_distance) {
			closest = i;
			min_distance = dist;
		}
	}
	coords.push_back(tmp);
	coords[coords.size()-1].set_next(coords[closest].get_next());
	coords[closest].set_next(coords.size()-1);
}

void Floorplan::add_cloth_point(ClothParticle point, Cloth* cloth) {
	int x = point.getPosition().x();
	int y = point.getPosition().z();
	add_point(x, y, point.getP());
}

void Floorplan::get_new_points() {
	coords.clear();
	for (int i = 0; i < parent->get_cloth()->num_fixed; i++) {
		add_cloth_point(parent->get_cloth()->fixed_particles[i], parent->get_cloth());
	}
}

void Floorplan::motion(int x, int y) {
	float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
	float xmin = parent->get_cloth()->getBoundingBox().getMin().x();
	float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
	float zmin = parent->get_cloth()->getBoundingBox().getMin().x();
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x > glutGet(GLUT_WINDOW_WIDTH )) x = glutGet(GLUT_WINDOW_WIDTH );
	if (y > glutGet(GLUT_WINDOW_HEIGHT)) y = glutGet(GLUT_WINDOW_HEIGHT);
	if (mouseButton == GLUT_LEFT_BUTTON) {
		if (selected_point != -1) {
			Point point = coords[selected_point];
			float transx = (x/float(width)-0.05)/0.9 * xconst;
			float transy = (y/float(height)-0.05)/0.9 * zconst;
			coords[selected_point].set_pos(transx, transy);
			if (point.is_bezier()) coords[selected_point].move_bezier_point(0, x, y);
			for (unsigned int i = 0; i < coords.size(); i++) {
				if (coords[i].is_bezier() && coords[i].get_next() == selected_point)
					coords[i].move_bezier_point(3, x, y);
			}
			glutPostRedisplay();
			if (point.get_p() != -1) {
				float new_x = point.get_x();
				float new_y = point.get_y();
				parent->get_cloth()->move_fixed(point.get_p(), new_x, new_y, 0);
				if (point.is_fixed())
					toggle_line_fixed(selected_point, true);
				for (unsigned int i = 0; i < coords.size(); i++) {
					if (coords[i].is_fixed() && coords[i].get_next() == selected_point)
						toggle_line_fixed(i, true);
				}
			}
			parent->Render();
		}
		if (selected_line != -1 && selected_control_point != -1) {
			coords[selected_line].move_bezier_point(selected_control_point, x, y);
			Render();
			if (coords[selected_line].is_fixed())
				toggle_line_fixed(selected_line, true);
			parent->Render();
		}
	}
}

void Floorplan::passiveMotion(int x, int y) {
	float xconst = fabs(parent->get_cloth()->getBoundingBox().getMax().x() - parent->get_cloth()->getBoundingBox().getMin().x());
	float xmin = parent->get_cloth()->getBoundingBox().getMin().x();
	float zconst = fabs(parent->get_cloth()->getBoundingBox().getMax().z() - parent->get_cloth()->getBoundingBox().getMin().z());
	float zmin = parent->get_cloth()->getBoundingBox().getMin().x();
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	set_expanding(true);
	my_grid->set_expanding(false);

	float tmp, base;
	if (selected_point != -1) base = distance(x, y, (0.9*(coords[selected_point].get_x()-xmin)/xconst + 0.05)*width, (0.9*(coords[selected_point].get_y()-zmin)/zconst + 0.05)*height);
	else base = glutGet(GLUT_WINDOW_WIDTH);
	
	int prev_point = selected_point;
	for (unsigned int i = 0; i < coords.size(); i++) {
		float cx = (0.9*(coords[i].get_x()-xmin)/xconst + 0.05)*width;
		float cy = (0.9*(coords[i].get_y()-zmin)/zconst + 0.05)*height;
		tmp = distance(x, y, cx, cy);
		if (tmp < base && tmp < 5) {
			selected_point = i;
			glutSetWindow(grid_window);
			my_grid->Render();
			glutSetWindow(my_window);
			glutSetMenu(point_menu_id);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
			selected_line = -1;
			Render();
			break;
		}
	}
	if (selected_point == prev_point && base > 15) {
		selected_point = -1;
		selected_line = -1;
		glutSetWindow(my_window);
		glutSetMenu(default_menu_id);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		for (unsigned int i = 0; i < coords.size(); i++) {
			/*if (coords[i].is_bezier()) {
				if (distance(x, y, coords[i].get_bezier_points()[1].get_x(), coords[i].get_bezier_points()[1].get_y()) < 5) {
					selected_control_point = 1;
					selected_line = i;
					selected_point = -1;
					glutSetMenu(line_menu_id);
					glutAttachMenu(GLUT_RIGHT_BUTTON);
					Render();
					break;
				}
				if (distance(x, y, coords[i].get_bezier_points()[2].get_x(), coords[i].get_bezier_points()[2].get_y()) < 5) {
					selected_control_point = 2;
					selected_line = i;
					selected_point = -1;
					glutSetMenu(line_menu_id);
					glutAttachMenu(GLUT_RIGHT_BUTTON);
					Render();
					break;
				}
				selected_control_point = 0;
				Point p1, p2, point;
				std::vector<Point> points;
				p1 = coords[i];
				p2 = coords[coords[i].get_next()];

				points = coords[i].get_bezier_points();
				Point prev = bezier(points, 0);
				int length = max(abs(p1.get_i()-p2.get_i()), abs(p1.get_j()-p2.get_j()));
				for (int j = 1; j <= length; j++) {
					float t = 1.0/length*j;
					point = bezier(points, t);
					float x1 = float(prev.get_x());
					float y1 = float(prev.get_y());
					float x2 = float(point.get_x());
					float y2 = float(point.get_y());

					if (point_to_line(x, y, x1, y1, x2, y2) < 4) {
						selected_line = i;
						selected_point = -1;
						glutSetMenu(line_menu_id);
						glutAttachMenu(GLUT_RIGHT_BUTTON);
						Render();
					} else {
						prev = point;
					}
				}
			} else {*/
				float cx1 = (0.9*(coords[i].get_x()-xmin)/xconst + 0.05)*width;
				float cy1 = (0.9*(coords[i].get_y()-zmin)/zconst + 0.05)*height;
				float cx2 = (0.9*(coords[coords[i].get_next()].get_x()-xmin)/xconst + 0.05)*width;
				float cy2 = (0.9*(coords[coords[i].get_next()].get_y()-zmin)/zconst + 0.05)*height;
				if (point_to_line(x, y, cx1, cy1, cx2, cy2) < 4) {
					selected_line = i;
					selected_point = -1;
					selected_control_point = -1;
					glutSetMenu(line_menu_id);
					glutAttachMenu(GLUT_RIGHT_BUTTON);
					Render();
				}
			//}
		}
	}
}

void Floorplan::keyboard(unsigned char key, int x, int y) {
	switch(key) {
		/*case 'r': case 'R':
			// Reset points
			coords.clear();
			glutPostRedisplay();
			break;*/
		case 'u': case 'U':
			// Undo
			// Disabled for now; use delete instead
			/*
			if (coords[coords.size()-1].get_i() != -1)
				parent->get_cloth()->unfix_particle(coords[coords.size()-1].get_i(), coords[coords.size()-1].get_j());
			for (unsigned int i = 0; i < coords.size(); i++)
				if (coords[i].get_next() == coords.size()-1)
					coords[i].set_next(coords[coords.size()-1].get_next());
			coords.pop_back();
			glutPostRedisplay();
			*/
			break;
		case 'q': case 'Q':
			parent->quit();
			break;
		default:
			printf("UNKNOWN KEYBOARD INPUT  '%c'\n", key);
	}
}

void Floorplan::set_display_window(int disp) {
	display_window = disp;
}

void Floorplan::set_grid(Grid* thegrid) {
	my_grid = thegrid;
	grid_window = thegrid->get_window();
}

void Floorplan::set_selected_point(int p) {
	if (selected_point == -1) return;  // If no point is selected in the floorplan, return

	if (coords[selected_point].get_p() != -1)   // If the selected point is associated with a point already, unfix that one.
		parent->get_cloth()->unfix_particle(coords[selected_point].get_p());

	int toreset = -1;
	if (coords[selected_point].is_fixed()) {
		toggle_line_fixed(selected_point, false);
		toreset = selected_point;
	}
	for (unsigned int itr = 0; itr < coords.size(); itr++) {
		if (coords[itr].is_fixed() && coords[itr].get_next() == selected_point) {
			toggle_line_fixed(itr, false);
			toreset = itr;
		}
	}

	coords[selected_point].set_p(p);
	parent->get_cloth()->fix_particle(p);
	glutSetWindow(my_window);
	float new_x = coords[selected_point].get_x() / float(glutGet(GLUT_WINDOW_WIDTH)) / 2 * (parent->get_cloth()->width() - 1);
	float new_y = coords[selected_point].get_y() / float(glutGet(GLUT_WINDOW_HEIGHT)) / 2 * (parent->get_cloth()->height()- 1);
	glutSetWindow(grid_window);
	parent->get_cloth()->move_fixed(p, new_x, new_y, 0);

	if (toreset != -1) toggle_line_fixed(toreset, true);
}

void Floorplan::default_menu(int op) {
	switch(op) {
		case 'S': case 's':
			parent->get_cloth()->save();
			break;
		case 'L': case 'l':
			parent->get_cloth()->load("input.obj");
			get_new_points();
			break;
		case 'Q': case 'q':
			parent->quit();
			break;
  }
}

void Floorplan::line_menu(int op) {
	switch(op) {
		case 'a':
			toggle_line_fixed(selected_line, true);
			break;
		case 'd':
			toggle_line_fixed(selected_line, false);
			break;
		case 'b':
			coords[selected_line].set_bezier(true);
			setup_bezier(selected_line);
			if (coords[selected_line].is_fixed()) toggle_line_fixed(selected_line, true);
			Render();
			break;
		case 'l':
			coords[selected_line].set_bezier(false);
			if (coords[selected_line].is_fixed()) toggle_line_fixed(selected_line, true);
			Render();
			break;
	}
}

void Floorplan::point_menu(int op) {
	switch(op) {
		case 'h':

			break;
		case 'd':
			if (coords[selected_point].get_p() != -1)
				parent->get_cloth()->unfix_particle(coords[selected_point].get_p());
			for (unsigned int i = 0; i < coords.size(); i++)
				if (coords[i].get_next() == selected_point)
					coords[i].set_next(coords[selected_point].get_next());
			coords[selected_point].set_next(selected_point);
			coords[selected_point].set_pos(-10, -10);
			//coords.erase(coords.begin() + selected_point);
			glutPostRedisplay();
			break;
	}
}

void Floorplan::toggle_line_fixed(int line, bool state) {
	Point p1 = coords[line];
	Point p2 = coords[coords[line].get_next()];

	/* Temporarily disabled.

	int diff_i = p1.get_i()-p2.get_i();
	int diff_j = p1.get_j()-p2.get_j();
	int steps = max(abs(diff_i), abs(diff_j));

	float diff_x = p2.get_x() - p1.get_x();
	float diff_y = p2.get_y() - p1.get_y();

	for (int i = 1; i < steps; i++) {
		int coord_i;
		int coord_j;
		if (p1.get_i() < p2.get_i()) {
			if (p1.get_j() < p2.get_j()) {
				if (abs(diff_i) > abs(diff_j)) {
					coord_i = p1.get_i() + i;
					coord_j = p1.get_j() + int(i*(float(diff_j)/float(diff_i)));
				} else {
					coord_i = p1.get_i() + int(i*(float(diff_i)/float(diff_j)));
					coord_j = p1.get_j() + i;
				}
			} else {
				if (abs(diff_i) > abs(diff_j)) {
					coord_i = p1.get_i() + i;
					coord_j = p1.get_j() - int(i*(float(diff_j)/float(diff_i)));
				} else {
					coord_i = p1.get_i() + int(i*(float(diff_i)/float(diff_j)));
					coord_j = p1.get_j() - i;
				}
			}
		} else {
			if (p1.get_j() < p2.get_j()) {
				if (abs(diff_i) > abs(diff_j)) {
					coord_i = p1.get_i() - i;
					coord_j = p1.get_j() + int(i*(float(diff_j)/float(diff_i)));
				} else {
					coord_i = p1.get_i() - int(i*(float(diff_i)/float(diff_j)));
					coord_j = p1.get_j() + i;
				}
			} else {
				if (abs(diff_i) > abs(diff_j)) {
					coord_i = p1.get_i() - i;
					coord_j = p1.get_j() - int(i*(float(diff_j)/float(diff_i)));
				} else {
					coord_i = p1.get_i() - int(i*(float(diff_i)/float(diff_j)));
					coord_j = p1.get_j() - i;
				}
			}
		}
		float x_pos, y_pos;
		if (coords[line].is_bezier()) {
			Point point;
			std::vector<Point> points;
			points = coords[line].get_bezier_points();
			float t = float(i)/steps;
			point = bezier(points, t);
			x_pos = point.get_x() / float(glutGet(GLUT_WINDOW_WIDTH)) / 2.0 * (parent->get_cloth()->width() - 1);
			y_pos = point.get_y() / float(glutGet(GLUT_WINDOW_HEIGHT))/ 2.0 * (parent->get_cloth()->height()- 1);
		} else {
			x_pos = float(p1.get_x() + (diff_x * (float(i)/float(steps)))) / float(glutGet(GLUT_WINDOW_WIDTH)) / 2.0 * (parent->get_cloth()->width() - 1);
			y_pos = float(p1.get_y() + (diff_y * (float(i)/float(steps)))) / float(glutGet(GLUT_WINDOW_HEIGHT))/ 2.0 * (parent->get_cloth()->height()- 1);
		}
		if (state) {
			parent->get_cloth()->move_fixed(coord_i, coord_j, x_pos, y_pos, 0);
			parent->get_cloth()->fix_particle(coord_i, coord_j);
			coords[line].fix();
		} else {
			parent->get_cloth()->unfix_particle(coord_i, coord_j);
			coords[line].unfix();
		}
	}
	*/
}

void Floorplan::expand() {
	glutSetWindow(my_window);
	
	if (expanding && glutGet(GLUT_WINDOW_WIDTH) < 600) {
		glutPositionWindow(600, 0);
		glutReshapeWindow(600, 600);
		/*for (unsigned int i = 0; i < coords.size(); i++) {
			coords[i].set_pos(coords[i].get_x()*(3.0/2.0), coords[i].get_y()*(3.0/2.0));
			if (coords[i].is_bezier()) {
				std::vector<Point> bezier = coords[i].get_bezier_points();
				for (int j = 0; j < 4; j++)
					coords[i].move_bezier_point(j, bezier[j].get_x()*(3.0/2.0), bezier[j].get_y()*(3.0/2.0));
			}
		}*/
	} else if (!expanding && glutGet(GLUT_WINDOW_WIDTH) > 400) {
		glutPositionWindow(800, 0);
		glutReshapeWindow(400, 400);
		/*for (unsigned int i = 0; i < coords.size(); i++) {
			coords[i].set_pos(coords[i].get_x()*(2.0/3.0), coords[i].get_y()*(2.0/3.0));
			if (coords[i].is_bezier()) {
				std::vector<Point> bezier = coords[i].get_bezier_points();
				for (int j = 0; j < 4; j++)
					coords[i].move_bezier_point(j, bezier[j].get_x()*(2.0/3.0), bezier[j].get_y()*(2.0/3.0));
			}
		}*/
	}
}

void Floorplan::setup_bezier(int index) {
	Point pt1, pt2, pt3, pt4, point;
	std::vector<Point> points;
	pt1 = coords[index];
	pt4 = coords[coords[index].get_next()];

	pt2.set_pos(pt1.get_x() + (pt4.get_x()-pt1.get_x()) * (1.0 / 3), pt1.get_y() + (pt4.get_y()-pt1.get_y()) * (1.0 / 3));
	pt3.set_pos(pt1.get_x() + (pt4.get_x()-pt1.get_x()) * (2.0 / 3), pt1.get_y() + (pt4.get_y()-pt1.get_y()) * (2.0 / 3));

	points.push_back(pt1); points.push_back(pt2); points.push_back(pt3); points.push_back(pt4);
	coords[index].init_bezier(points);
}

Point Floorplan::bezier(std::vector<Point> points, float t) {
	if (points.size() == 1) {
		return points[0];
	}
	std::vector<Point> next_step;
	for (unsigned int i = 0; i < points.size() - 1; i++) {
		Point p1, p2, point;
		p1.set_pos(points[i].get_x(), points[i].get_y());
		p2.set_pos(points[i+1].get_x(), points[i+1].get_y());
		point.set_pos(p1.get_x() + (p2.get_x() - p1.get_x()) * t,
					  p1.get_y() + (p2.get_y() - p1.get_y()) * t);
		next_step.push_back(point);
	}
	return bezier(next_step, t);
}

void Floorplan::Render() {
	glutPostRedisplay();
}

float distance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

float point_to_line(float x0, float y0, float x1, float y1, float x2, float  y2) {
	float num = fabs((x2-x1)*(y1-y0)-(x1-x0)*(y2-y1));
	float den = distance(x2, y2, x1, y1);
	float d0 = num / den;
	float d1 = distance(x0, y0, x1, y1);
	float d2 = distance(x0, y0, x2, y2);
	float len = distance(x1, y1, x2, y2);

	if ((pow(d1, 2) - pow(d0, 2) > pow(len, 2)) || (pow(d2, 2) - pow(d0, 2) > pow(len, 2)))
		return min(d1, d2);
	else
		return d0;
}