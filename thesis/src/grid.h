#ifndef _GRID_H_
#define _GRID_H_

class GLCanvas;
class Floorplan;

class Grid {
public:
	static void initialize(GLCanvas* canvas, Floorplan* _floorplan, int window);
	static void set_expanding(bool state) { expanding = state; }
	static void Render();
	static int get_window() { return my_window; }
	static void expand();

private:
	//Variables
	static GLCanvas* parent;
	static Floorplan* floorplan;
	static int my_window;
	static int mouseButton;
	static int mouseState;
	static bool toggleState;
	static bool expanding;

	static void display(void);
	static void reshape(int w, int h);
	static void mouse(int button, int state, int x, int y);
	static void motion(int x, int y);
	static void passiveMotion(int x, int y);
};

#endif