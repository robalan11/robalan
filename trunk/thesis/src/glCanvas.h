// ====================================================================
// GLCanvas class by Rob Jagnow.
// ====================================================================

#ifndef _GL_CANVAS_H_
#define _GL_CANVAS_H_

#include <stdlib.h>
#include <assert.h>

class ArgParser;
class Camera;
class Cloth;
class Fluid;
class Floorplan;
class Grid;

// ====================================================================
// NOTE:  All the methods and variables of this class are static
// ====================================================================

class GLCanvas {

public:

  // Set up the canvas and enter the rendering loop
  // Note that this function will not return but can be
  // terminated by calling 'exit(0)'
  void initialize(ArgParser *_args);
  static void Render();
  static void quit();
  static Cloth* get_cloth() { return cloth; }

private:

  static void InitLight();
  static void Load();

  // various static variables
  static ArgParser *args;
  static Camera *camera;
  static Cloth *cloth;
  static Fluid *fluid;
  static Floorplan *floorplan;
  static Grid *grid;
  static int display_list_index;
  static int my_window;

  // state of the mouse cursor
  static int mouseButton;
  static int mouseX;
  static int mouseY;
  static bool controlPressed;

  // Callback functions for mouse and keyboard events
  static void display(void);
  static void reshape(int w, int h);
  static void mouse(int button, int state, int x, int y);
  static void motion(int x, int y);
  static void passiveMotion(int x, int y);
  static void keyboard(unsigned char key, int x, int y);
  static void idle();
};

// ====================================================================

int HandleGLError();

// ====================================================================

#endif
