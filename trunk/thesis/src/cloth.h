#ifndef _CLOTH_H_
#define _CLOTH_H_

#include "argparser.h"
#include "boundingbox.h"

#include <vector>

// =====================================================================================
// Cloth Springs
// =====================================================================================

class ClothSpring {
public:
	int getTarget() { return target; }
	float getGoal() { return goal; }
	float getOrig() { return orig; }
	void setTarget(int newtarget) { target = newtarget; }
	void setGoal(float newgoal) { goal = newgoal; }
	void setOrig(float init) { orig = init; }

private:
	int target;
	float goal;
	float orig;
};

// =====================================================================================
// Cloth Particles
// =====================================================================================

class ClothParticle {
public:
  // ACCESSORS
  const Vec3f& getOriginalPosition() const{ return original_position; }
  const Vec3f& getPosition() const{ return position; }
  const Vec3f& getVelocity() const{ return velocity; }
  const Vec3f& getAcceleration() const { return acceleration; }
  Vec3f getForce() const { return mass*acceleration; }
  float getMass() const { return mass; }
  bool isFixed() const { return fixed; }
  bool isEnabled() const { return enabled; }
  int getI() { return i; }
  int getJ() { return j; }
  int getP() { return p; }
  vector<ClothSpring> getStructural() { return structural; }
  vector<ClothSpring> getShear() { return shear; }
  std::vector<ClothSpring> structural;
  std::vector<ClothSpring> shear;
  const vector<vector<int> > getFaces() const{ return faces; }
  Vec3f getNormal() { return normal; }

  // MODIFIERS
  void setOriginalPosition(const Vec3f &p) { original_position = p; }
  void setPosition(const Vec3f &p) { position = p; }
  void setVelocity(const Vec3f &v) { velocity = v; }
  void setAcceleration(const Vec3f &a) { acceleration = a; }
  void setMass(float m) { mass = m; }
  void setFixed(bool b) { fixed = b; }
  void setIJ(int _i, int _j) { i = _i; j = _j; }
  void setP(int _p) { p = _p; }
  void enable() { enabled = true; }
  void disable() { enabled = false; }
  void add_structural(int index, float length) { 
	  ClothSpring newspring;
	  newspring.setTarget(index);
	  newspring.setGoal(length);
	  newspring.setOrig(length);
	  structural.push_back(newspring); }
  void add_shear(int index, float length) { 
	  ClothSpring newspring;
	  newspring.setTarget(index);
	  newspring.setGoal(length);
	  newspring.setOrig(length);
	  shear.push_back(newspring); }
  void add_face(std::vector<int> face) { faces.push_back(face); }
  void tighten_structural(int index, float length) { structural[index].setGoal(length); }
  void tighten_shear(int index, float length) { shear[index].setGoal(length); }
  void set_normal(Vec3f norm) { normal = norm; }
  
  void copyOther(ClothParticle other) {
	  original_position = other.getOriginalPosition();
	  position = other.getPosition();
	  velocity = other.getVelocity();
	  acceleration = other.getAcceleration();
	  structural = other.getStructural();
	  shear = other.getShear();
	  mass = other.getMass();
	  fixed = other.isFixed();
	  enabled = other.isEnabled();
	  i = other.getI();
	  j = other.getJ();
  }

  float spam;
  Vec3f normal;
  std::vector<vector<int> > faces;
private:
  // REPRESENTATION
  Vec3f original_position;
  Vec3f position;
  Vec3f velocity;
  Vec3f acceleration;
  float mass;
  bool fixed;
  bool enabled;
  int i;
  int j;
  int p;
};

// =====================================================================================
// Cloth System
// =====================================================================================

class Cloth {

public:
  Cloth(ArgParser *args, bool parent);

  ~Cloth() { }

  // ACCESSORS
  const BoundingBox& getBoundingBox() const { return box; }

  // PAINTING & ANIMATING
  void Paint();
  void Animate();
  
  ClothParticle *fixed_particles;
  int num_fixed;
  int width() { return nx; }
  int height() { return ny; }
  int getNumParticles() { return particles.size(); }
  bool isPointEnabled(int i) { return getParticle(i).isEnabled(); }

  void move_fixed(int i, float x, float y, float z) {
	ClothParticle &p = getParticle(i);
	p.setPosition(Vec3f(x, z, y));
  }
  void fix_particle(int p) { getParticle(p).setFixed(true); }
  void unfix_particle(int p) { getParticle(p).setFixed(false); }
  void enable_particle(int p) { getParticle(p).enable(); }
  void disable_particle(int p) { getParticle(p).disable(); }

  void save();
  void load(char* file);

  // ACCESSORS
  const ClothParticle& getParticle(int i, int j) const {
    assert (i >= 0 && i < nx && j >= 0 && j < ny);
    return particles[i + j*nx]; }
  ClothParticle& getParticle(int i, int j) {
    assert (i >= 0 && i < nx && j >= 0 && j < ny);
    return particles[i + j*nx]; }
  const ClothParticle& getParticle(int i) const {
	  assert (i >= 0 && i < nx*ny);
	  return particles[i]; }
  ClothParticle& getParticle(int i) {
	  assert (i >= 0 && i < nx*ny);
	  return particles[i]; }

private:

  // HELPER FUNCTIONS
  Vec3f getSpringForce(Cloth *mesh, int i);
  void insertGouraudNormal(ClothParticle &p1, int i, int j) const;
  Vec3f getNewGouraudNormal(int index) const;
  Vec3f getFaceNormal(std::vector<int> face) const;
  Vec3f getCornerNormal(std::vector<int> face, int index) const;
  void computeBoundingBox();
  void AdjustSpring(ClothParticle &a, ClothParticle &b, float correction);

  // REPRESENTATION
  ArgParser *args;
  // grid data structure
  int nx, ny;
  std::vector<ClothParticle> particles;
  std::vector<vector<int> > faces;
  BoundingBox box;
  // simulation parameters
  float damping;
  // spring constants
  float k_structural;
  float k_shear;
  float k_bend;
  // correction thresholds
  float provot_structural_correction;
  float provot_shear_correction;

  Cloth* second;
  Cloth* third;
};

#endif
