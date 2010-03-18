#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include "string.h"
#include "stdio.h"
#include "assert.h"
#include "stdlib.h"
#include "vectors.h"

#include <string>

using namespace std;

// ================================================================================
// ================================================================================

class ArgParser {

public:

  ArgParser() { DefaultValues(); }

  ArgParser(int argc, char *argv[]) {
    DefaultValues();

    for (int i = 1; i < argc; i++) {
      if (!strcmp(argv[i],"-cloth")) {
        i++; assert (i < argc); 
        cloth_file = argv[i];
      } else if (!strcmp(argv[i],"-size")) {
	i++; assert (i < argc); 
	height = atoi(argv[i]);
	width = height/2*3;
      } else if (!strcmp(argv[i],"-timestep")) {
	i++; assert (i < argc); 
	timestep = float(atof(argv[i]));
        assert (timestep > 0);
      } else if (!strcmp(argv[i],"-gouraud")) {
    gouraud = true;
      } else {
	printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
	assert(0);
      }
    }
  }

  // ===================================
  // ===================================

  void DefaultValues() {
    width = 800;
    height = 800;
    gouraud = true;

    timestep = float(0.01);
    animate = true;

    particles = false;
    velocity = false;
    force = false;

    surface = true;

    wireframe = false;
    bounding_box = true;

    gravity = Vec3f(0,float(-9.8),0);
  }

  // ===================================
  // ===================================
  // REPRESENTATION
  // all public! (no accessors)

  string cloth_file;
  int width;
  int height;
  bool gouraud;

  // animation control
  float timestep;
  bool animate;
  Vec3f gravity;

  // display option toggles 
  // (used by both)
  bool particles;
  bool velocity;
  bool surface;
  bool bounding_box;

  // used by cloth
  bool force;
  bool wireframe;
};

#endif
