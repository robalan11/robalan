#include <iostream> 
#include <ctime> 
#include <cstdlib>

#include "argparser.h"
#include "glCanvas.h"

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

// =========================================
// =========================================

int main(int argc, char *argv[]) {
  // deterministic (repeatable) randomness
  srand(37);
  // "real" randomness
  //srand((unsigned)time(0));

  ArgParser *args = new ArgParser(argc, argv);
  glutInit(&argc, argv);
  
  if (args->cloth_file == "") {
    cout << "no simulation specified" << endl;
    delete args;
    return 0;
  }

  GLCanvas glcanvas;
  glcanvas.initialize(args);

  // well it never returns from the GLCanvas loop...
  delete args;
  return 0;
}

// =========================================
// =========================================
