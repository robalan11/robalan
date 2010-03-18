#include <vector>
#include "glCanvas.h"
#include "cloth.h"

#ifndef _FLOORPLAN_H_
#define _FLOORPLAN_H_

class GLCanvas;
class Vec3f;
class ClothParticle;
class Grid;

class Point {
public:
	void set_pos(int _x, int _y) { x = _x; y = _y; }
	void set_original_pos(int _x, int _y) { orig_x = _x; orig_y = _y; }
	void set_p(int _p) { p = _p; }
	void set_next(int n) { next = n; }
	void fix() { fixed = true; }
	void unfix() { fixed = false; }
	void set_bezier(bool state) { bezier = state; }
	void init_bezier(vector<Point> points) { control_pts = points; }
	void move_bezier_point(int index, int x, int y) { control_pts[index].set_pos(x, y); }

	int get_x() { return x; }
	int get_y() { return y; }
	int get_p() { return p; }
	int get_next() { return next; }
	bool is_fixed() { return fixed; }
	bool is_bezier() { return bezier; }
	vector<Point> get_bezier_points() { return control_pts; }

private:
	int x;
	int y;
	int orig_x;
	int orig_y;
	int p;
	int next;
	bool fixed;
	bool bezier;
	vector<Point> control_pts;
};

class Floorplan {
public:
	static void initialize(GLCanvas* canvas, int window);
	static void add_point(int x, int y, int p=-1);
	static void add_cloth_point(ClothParticle point, Cloth* cloth);
	static void get_new_points();
	static void set_display_window(int disp);
	static void set_grid(Grid* thegrid);
	static void set_selected_point(int p);
	static int get_selected_point_int() { return selected_point; }
	static Point get_selected_point() { return coords[selected_point]; }
	static int get_window() { return my_window; }
	static void set_expanding(bool state) { expanding = state; }
	static void expand();
	static void Render();

private:
	//Variables
	static GLCanvas* parent;
	static std::vector<Point> coords;
	static int selected_point;
	static int selected_line;
	static int selected_control_point;
	static int display_window;
	static int my_window;
	static int grid_window;
	static Grid* my_grid;
	static int default_menu_id;
	static int line_menu_id;
	static int point_menu_id;
	static bool expanding;

	// state of the mouse cursor
	static int mouseButton;
	static int mouseStatus;
	static int mouseX;
	static int mouseY;
	static bool controlPressed;

	static void display(void);
	static void reshape(int w, int h);
	static void mouse(int button, int state, int x, int y);
	static void motion(int x, int y);
	static void passiveMotion(int x, int y);
	static void keyboard(unsigned char key, int x, int y);

	static void default_menu(int op);
	static void line_menu(int op);
	static void point_menu(int op);

	static void toggle_line_fixed(int line, bool state);
	static void setup_bezier(int index);

	static Point bezier(std::vector<Point> points, float t);
};

#endif