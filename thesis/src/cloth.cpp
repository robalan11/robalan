#include "boundingbox.h"

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

#include "cloth.h"
#include "argparser.h"
#include <fstream>
#include <time.h>
#include "vectors.h"

using namespace std;

// ============================================================================
// some helper drawing functions

void DrawSpring(const Vec3f &a_o, const Vec3f &b_o, const Vec3f &a, const Vec3f &b, float correction);
void drawForce(const Vec3f &p, const Vec3f &f);

// ============================================================================
// ============================================================================

Cloth::Cloth(ArgParser *_args, bool parent) {
  args =_args;

  // open the file
  ifstream istr(args->cloth_file.c_str());
  assert (istr != NULL);
  string token;

  // read in the simulation parameters
  // (units == N/m)  (N = kg*m/s^2)
  istr >> token >> k_structural; assert (token == "k_structural");  
  istr >> token >> k_shear; assert (token == "k_shear");
  istr >> token >> k_bend; assert (token == "k_bend");
  istr >> token >> damping; assert (token == "damping");
  // NOTE: correction factor == .1, means springs shouldn't stretch more 
  //    than 10%, correction factor == 100, means don't do any correction
  istr >> token >> provot_structural_correction; 
  assert (token == "provot_structural_correction");
  istr >> token >> provot_shear_correction; 
  assert (token == "provot_shear_correction");

  // the cloth dimensions
  istr >> token >> nx >> ny; // (units == meters)
  assert (token == "m");
  assert (nx >= 2 && ny >= 2);

  // the corners of the cloth
  Vec3f a,b,c,d;
  istr >> token >> a; assert (token == "p");
  istr >> token >> b; assert (token == "p");
  istr >> token >> c; assert (token == "p");
  istr >> token >> d; assert (token == "p");

  // fabric weight  (units == kg/m^2)
  // denim ~300 g/m^2
  // silk ~70 g/m^2
  float fabric_weight;
  istr >> token >> fabric_weight; assert (token == "fabric_weight");
  float area = AreaOfTriangle(a,b,c) + AreaOfTriangle(a,c,d);
/*
  // create the particles
  float mass = area*fabric_weight / float(nx*ny);
  float xdim = Vec3f(a, d).Length()/float(nx);
  float ydim = Vec3f(a, b).Length()/float(ny);
  float diag = sqrt(pow(xdim, 2) + pow(ydim, 2));
  for (int j = 0; j < nx; j++) {
    float x = j/float(nx-1);
    Vec3f ad = (1-x)*a + x*d;
    Vec3f bc = (1-x)*b + x*c;
    for (int i = 0; i < ny; i++) {
      float y = i/float(ny-1);
      ClothParticle p;
      Vec3f abdc = (1-y)*ad + y*bc;
      p.setOriginalPosition(abdc);
      p.setPosition(abdc);
	  p.setIJ(i,j);
      p.setVelocity(Vec3f(0,0,0));
      p.setMass(mass);
      p.setFixed(false);
	  p.enable();
	  
	  if (i > 0) p.add_structural((i-1) + j*nx, xdim);
	  if (i < nx-1) p.add_structural((i+1) + j*nx, xdim);
	  if (j > 0) p.add_structural(i + (j-1)*nx, ydim);
	  if (j < ny-1) p.add_structural(i + (j+1)*nx, ydim);
	  if (i > 0 && j > 0) p.add_shear((i-1) + (j-1)*nx, diag);
	  if (i > 0 && j < ny-1) p.add_shear((i-1) + (j+1)*nx, diag);
	  if (i < nx-1 && j > 0) p.add_shear((i+1) + (j-1)*nx, diag);
	  if (i < nx-1 && j < ny-1) p.add_shear((i+1) + (j+1)*nx, diag);

	  p.spam = 0;
	  particles.push_back(p);
    }
  }

  // make faces
  for (int j = 0; j < nx-1; j++) {
	  for (int i = 0; i < ny-1; i++) {
		  std::vector<int> face;
		  face.push_back(i + j*ny);
		  face.push_back((i+1) + j*ny);
		  face.push_back((i+1) + (j+1)*ny);
		  face.push_back(i + (j+1)*ny);

		  faces.push_back(face);

		  getParticle(i,j).add_face(face);
		  getParticle(i+1,j).add_face(face);
		  getParticle(i+1,j+1).add_face(face);
		  getParticle(i,j+1).add_face(face);
	  }
  }
  
  num_fixed = 0;

  fixed_particles = new ClothParticle[nx*ny];

  // the fixed particles
  while (istr >> token) {
    assert (token == "f");
    int i,j;
    float x,y,z;
    istr >> i >> j >> x >> y >> z;
    ClothParticle &p = getParticle(i*ny + j);
    p.setPosition(Vec3f(x,y,z));
	p.setIJ(i,j);
    p.setFixed(true);
	fixed_particles[num_fixed] = p;
	num_fixed ++;
  }
*/
  /* Further user study hackage */
  load("flat.obj");

  /*
  for (unsigned int i = 0; i < particles.size(); i++) {
	  if (particles[i].getStructural().size() > 2)
		  particles[i].setFixed(false);
  }
  /* User Study Hack incoming! 
	num_fixed = 0;

	fixed_particles = new ClothParticle[particles.size()];

	// the fixed particles
	for (unsigned int p=0; p < particles.size(); p++) {
		if (particles[p].isFixed()) {
			fixed_particles[num_fixed] = particles[p];
			num_fixed ++;
		}
	}
	/* All clear. 

  Vec3f max = getParticle(fixed_particles[2].getP()).getPosition();
  getParticle(fixed_particles[0].getP()).setPosition(max*0.1);
  fixed_particles[0].setPosition(max*0.1);
  getParticle(fixed_particles[1].getP()).setPosition(Vec3f(max.x()*0.9, max.y(), max.z()*0.1));
  fixed_particles[1].setPosition(Vec3f(max.x()*0.9, max.y(), max.z()*0.1));
  getParticle(fixed_particles[2].getP()).setPosition(max*0.9);
  fixed_particles[2].setPosition(max*0.9);
  getParticle(fixed_particles[3].getP()).setPosition(Vec3f(max.x()*0.1, max.y(), max.z()*0.9));
  fixed_particles[3].setPosition(Vec3f(max.x()*0.1, max.y(), max.z()*0.9));

  /* end hackage */

  computeBoundingBox();

  /* User study setup */

  if (parent) {
	  directory = time(NULL);
	  char command[32];
	  sprintf(command, "md user%i", directory);
	  system(command);
	  filenum = 0;
  }

  /* end User study setup */

  if (parent) {
	  second = new Cloth(args, false);
	  third = new Cloth(args, false);
  } else {
	  second = NULL;
	  third = NULL;
  }
}

/*Cloth::Cloth(const Cloth& other) {
	particles = other.particles;
	nx = other.nx;
	ny = other.ny;
}*/

// ============================================================================

void Cloth::computeBoundingBox() {
  box = BoundingBox(getParticle(0).getPosition());
  for (unsigned int i = 0; i < particles.size(); i++) {
      box.Extend(getParticle(i).getPosition());
  }
}

// ============================================================================
// ============================================================================
// ============================================================================

void Cloth::Paint() {
  // ==========================================================================
  // render the cloth surface
  // ==========================================================================

	if (args->surface) {
		for (unsigned int i = 0; i < particles.size(); i++) {
			particles[i].set_normal(getNewGouraudNormal(i));
		}

		for (unsigned int i = 0; i < faces.size(); i++) {
			glBegin(GL_POLYGON);
			glColor3f(1,1,1);
			//Vec3f norm = getFaceNormal(faces[i]);
			//glNormal3f(norm.x(), norm.y(), norm.z());
			std::vector<int> f = faces[i];
			for (unsigned int j = 0; j < f.size(); j++) {
				if (!particles[f[f.size()-j-1]].isEnabled()) continue;

				int index = f[f.size()-j-1];
				glNormal3f(particles[index].getNormal().x(), particles[index].getNormal().y(), particles[index].getNormal().z());
				Vec3f pos = particles[index].getPosition();
				glVertex3f(pos.x(), pos.y(), pos.z());
			}
			glEnd();
		}
	}

  // ==========================================================================
  // render the wireframe cloth (the structural and shear springs)
  // ==========================================================================

  if (args->wireframe) {
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glBegin(GL_LINES);
    for (unsigned int i = 0; i < particles.size(); i++) {
	    ClothParticle p = getParticle(i);
		for (unsigned int k = 0; k < p.getStructural().size(); k++) {
			const Vec3f &a_o = p.getOriginalPosition();
			const Vec3f &b_o = particles[p.getStructural()[k].getTarget()].getOriginalPosition();
			const Vec3f &a = p.getPosition();
			const Vec3f &b = particles[p.getStructural()[k].getTarget()].getPosition();
			DrawSpring(a_o, b_o, a, b, provot_structural_correction);
		}
		for (unsigned int k = 0; k < p.getShear().size(); k++) {
			const Vec3f &a_o = p.getOriginalPosition();
			const Vec3f &b_o = particles[p.getShear()[k].getTarget()].getOriginalPosition();
			const Vec3f &a = p.getPosition();
			const Vec3f &b = particles[p.getShear()[k].getTarget()].getPosition();
			DrawSpring(a_o, b_o, a, b, provot_shear_correction);
		}
    }
    glEnd();
    glEnable(GL_LIGHTING);
  }
}

// ============================================================================
// ============================================================================

Vec3f Cloth::getSpringForce(Cloth *mesh, int i) {
    Vec3f force(0,0,0);

    ClothParticle* p = &(mesh->getParticle(i));
	if (!p->isEnabled()) return force;

    //structural springs
	std::vector<ClothSpring> structural = p->getStructural();
	//printf("Point (%d, %d) has neighbors: ", p.getI(), p.getJ());
	for (unsigned int k = 0; k < structural.size(); k++) {
		//printf("(%d, %d), ", particles[structural[k]].getI(), particles[structural[k]].getJ());
		if (mesh->getParticle(structural[k].getTarget()).isEnabled()) {
			Vec3f dist(mesh->getParticle(structural[k].getTarget()).getPosition(), p->getPosition());
			Vec3f direction(dist);
			direction.Normalize();
			float len1 = structural[k].getGoal();
			float len2 = dist.Length();
			if (len2 > structural[k].getOrig()) {
				force += direction*(len2 - structural[k].getOrig())*mesh->k_structural;
				if (len2 > structural[k].getOrig()) {
					mesh->getParticle(i).tighten_structural(k, len1 - 0.25*(len2-structural[k].getOrig()));
				} else if (structural[k].getOrig() > len2) {
					mesh->getParticle(i).tighten_structural(k, len1 + 0.25*(structural[k].getOrig()-len2));
				}
			}
		}
	}

    //shear springs
	std::vector<ClothSpring> shear = p->getShear();
	for (unsigned int k = 0; k < shear.size(); k++) {
		if (mesh->getParticle(shear[k].getTarget()).isEnabled()) {
			Vec3f dist(mesh->getParticle(shear[k].getTarget()).getPosition(), p->getPosition());
			Vec3f direction(dist);
			direction.Normalize();
			float len1 = shear[k].getGoal();
			float len2 = dist.Length();
			if (len2 > len1) {
				force += direction*(len2 - len1)*mesh->k_shear;
				if (len2 > shear[k].getOrig()) {
					mesh->getParticle(i).tighten_shear(k, len1 - 0.05*(len2-shear[k].getOrig()));
				} else if (shear[k].getOrig() > len2) {
					mesh->getParticle(i).tighten_shear(k, len1 + 0.05*(shear[k].getOrig()-len2));
				}
			}
		}
	}	
    return force;
}

void Cloth::Animate() {
    //Calculate initial forces
	for (unsigned int i = 0; i < particles.size(); i++) {
        ClothParticle &p = this->getParticle(i);
        //gravity
        Vec3f force(0,10*p.getMass(),0);
        force += getSpringForce(this, i);
        
		second->getParticle(i).copyOther(p);
        second->getParticle(i).setAcceleration(force /= p.getMass());
        second->getParticle(i).setVelocity((p.getVelocity()*(1-damping)) + second->getParticle(i).getAcceleration()*args->timestep);
        second->getParticle(i).setPosition(p.getPosition() + second->getParticle(i).getVelocity()*(args->timestep / 2));  //midpoint
    }
    //Calculate forces at midpoint
	for (unsigned int i = 0; i < particles.size(); i++) {
        ClothParticle &p = second->getParticle(i);
        //gravity
        Vec3f force(0,10*p.getMass(),0);
        force += getSpringForce(second, i);
        
        this->getParticle(i).setAcceleration(force /= p.getMass());
        //third.getParticle(i).setAcceleration(force /= p.getMass());
        //third.getParticle(i).setVelocity((this->getParticle(i).getVelocity()*(1-damping)) + third.getParticle(i, j).getAcceleration()*args->timestep);
        //third.getParticle(i).setPosition(this->getParticle(i).getPosition() + third.getParticle(i, j).getVelocity()*(args->timestep / 2));  //midpoint
    }
    /*
    //Calculate forces at alternate midpoint
    Cloth fourth(args);
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            ClothParticle &p = third.getParticle(i, j);
            //gravity
            Vec3f force(0,-10*p.getMass(),0);
            force += getSpringForce(&third, i, j);
            
            fourth.getParticle(i, j).setAcceleration(force /= p.getMass());
            fourth.getParticle(i, j).setVelocity((this->getParticle(i, j).getVelocity()*(1-damping)) + fourth.getParticle(i, j).getAcceleration()*args->timestep);
            fourth.getParticle(i, j).setPosition(this->getParticle(i, j).getPosition() + fourth.getParticle(i, j).getVelocity()*(args->timestep));  //midpoint
        }
    }
    
    //Calculate final forces
    Cloth fifth(args);
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            ClothParticle &p = fourth.getParticle(i, j);
            //gravity
            Vec3f force(0,-10*p.getMass(),0);
            force += getSpringForce(&fourth, i, j);
            
            fifth.getParticle(i, j).setAcceleration(force /= p.getMass());
        }
    }
    
    //Collate forces
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            ClothParticle &p = getParticle(i, j);
            p.setAcceleration(second.getParticle(i, j).getAcceleration()*(1.0/6.0) +
                              third.getParticle(i, j).getAcceleration()*(1.0/3.0) +
                              fourth.getParticle(i, j).getAcceleration()*(1.0/3.0) +
                              fifth.getParticle(i, j).getAcceleration()*(1.0/6.0));
        }
    }
    */
    //Move particles
	for (unsigned int i = 0; i < particles.size(); i++) {
        ClothParticle &p = getParticle(i);
		if (p.isFixed()) {
			//p.setPosition(p.getOriginalPosition() + Vec3f(0,0,cos(p.spam)));
			//p.spam += 0.1;
			continue;
		}
		p.setVelocity(Vec3f(0,0,0));
        p.setVelocity((p.getVelocity()*(1-damping)) + p.getAcceleration()*args->timestep);
        //if (i == 1 && j == 1) { cout << p.getAcceleration() << p.getVelocity() << endl; }
        p.setPosition(p.getPosition() + p.getVelocity()*args->timestep);
    }
    //Adjust particles
    for (int k = 0; k < max(max(nx, ny)/2, 7); k++) {
		for (unsigned int i = 0; i < particles.size(); i++) {
			ClothParticle &a = getParticle(i);
			for (unsigned int j = 0; j < particles[i].structural.size(); j++) {      
                ClothParticle &b = getParticle(particles[i].structural[j].getTarget());
				if (a.isEnabled() && b.isEnabled())
	                AdjustSpring(a,b, provot_structural_correction);
            }
			for (unsigned int j = 0; j < particles[i].shear.size(); j++) {
				ClothParticle &b = getParticle(particles[i].shear[j].getTarget());
				if (a.isEnabled() && b.isEnabled())
					AdjustSpring(a,b, provot_shear_correction);
			}
        }
    }
}

void Cloth::AdjustSpring(ClothParticle &a, ClothParticle &b, float correction) {
    Vec3f ab_o = b.getOriginalPosition()-a.getOriginalPosition();
    Vec3f ab = b.getPosition()-a.getPosition();
    float length_o = ab_o.Length();
    float length = ab.Length();
    Vec3f direction = ab;
    direction.Normalize();
    if (length > (1+correction)*length_o) {
        if (a.isFixed()) {
            b.setPosition(a.getPosition() + direction*(1+correction)*length_o);
        } else if (b.isFixed()) {
            a.setPosition(b.getPosition() - direction*(1+correction)*length_o);
        } else {
            Vec3f midpoint((a.getPosition().x()+b.getPosition().x())/2,
                           (a.getPosition().y()+b.getPosition().y())/2,
                           (a.getPosition().z()+b.getPosition().z())/2);
            a.setPosition(midpoint - direction*(1+correction)*(length_o/2));
            b.setPosition(midpoint + direction*(1+correction)*(length_o/2));
        }
    } else { return; }
}

// ============================================================================
// ============================================================================
// some helper drawing functions
// ============================================================================

void DrawSpring(const Vec3f &a_o, const Vec3f &b_o, const Vec3f &a, const Vec3f &b, float correction) {
  /*Vec3f ab_o = b_o-a_o;
  Vec3f ab = b-a;
  float length_o = ab_o.Length(); // the original length
  float length = ab.Length();     // the current length
  if (length > (1+correction) * length_o ||
      length < (1-correction) * length_o) {
    // draw the spring in cyan if it's over-stretched
    glColor3f(0,1,1);
  } else {
    // otherwise draw it black
    glColor3f(0,0,0);
  }  */
  glColor3f(0,0,0);
  glVertex3f(a.x(),a.y(),a.z());
  glVertex3f(b.x(),b.y(),b.z());
}

Vec3f computeNormal(const Vec3f &p1, const Vec3f &p2, const Vec3f &p3, const Vec3f &p4) {
	// compute the normal of a non-planar quadrilateral (not well-defined)
	Vec3f v12 = p2 - p1;
	Vec3f v23 = p3 - p2;
	Vec3f v34 = p4 - p3;
	Vec3f v41 = p1 - p4;

	// compute normals at each corner and average them
	Vec3f normal1, normal2, normal3, normal4;
	Vec3f::Cross3(normal1,v41,v12);
	Vec3f::Cross3(normal2,v12,v23);
	Vec3f::Cross3(normal3,v23,v34);
	Vec3f::Cross3(normal4,v34,v41);
	Vec3f normal = normal1+normal2+normal3+normal4;

	normal.Normalize();
	return normal;
}

Vec3f Cloth::getFaceNormal(std::vector<int> face) const {
	Vec3f norm(0,0,0);
	Vec3f points[16];
	Vec3f vectors[16];

	for (unsigned int i = 0; i < face.size(); i++)
		points[i] = (particles[face[i]].getPosition());

	for (unsigned int i = 0; i < face.size(); i++)
		vectors[i] = (points[(i+1)%face.size()] - points[i]);

	for (unsigned int i = 0; i < face.size(); i++) {
		Vec3f temp_norm;
		Vec3f::Cross3(temp_norm, vectors[i], vectors[(i+1)%face.size()]);
		norm += temp_norm;
	}

	norm.Normalize();

	return -1*norm;
}

Vec3f Cloth::getCornerNormal(std::vector<int> face, int index) const {
	int center = 0;
	for (unsigned int i = 0; i < face.size(); i++)
		if (face[i] == index) center = i;

	Vec3f p1 = particles[face[(center-1)%face.size()]].getPosition();
	Vec3f p2 = particles[face[center]].getPosition();
	Vec3f p3 = particles[face[(center+1)%face.size()]].getPosition();

	Vec3f v1 = p2-p1;
	Vec3f v2 = p3-p2;

	Vec3f normal;
	Vec3f::Cross3(normal, v2, v1);

	normal.Normalize();
	return normal;
}

Vec3f Cloth::getNewGouraudNormal(int index) const {
	Vec3f norm(0,0,0);
	for (unsigned int i = 0; i < particles[index].faces.size(); i++) {
		norm += getCornerNormal(particles[index].faces[i], index);
	}
	norm.Normalize();
	return norm;
}

void drawForce(const Vec3f &p, const Vec3f &f) {
  Vec3f tmp = p+f;
  glVertex3f(p.x(),p.y(),p.z());
  glVertex3f(tmp.x(),tmp.y(),tmp.z());
}

// ============================================================================
// ============================================================================

void Cloth::save() {
	printf("Saving...\n");
	ofstream outstream;

	char filename[64];
	sprintf(filename, "user%i/output%i.obj", directory, filenum);
	outstream.open(filename);
	filenum++;

	Vec3f part;
	for (unsigned int i = 0; i < particles.size(); i++) {
		part = particles[i].getPosition();
		outstream << "v " << part.x() << " " << part.z() << " " << part.y() << std::endl;
	}
	
	outstream << std::endl;

	for (unsigned int i = 0; i < faces.size(); i++) {
		outstream << "f";
		for (unsigned int j = 0; j < faces[i].size(); j++)
			outstream << " " << (faces[i][j] + 1);
		outstream << std::endl;
	}

	printf("Saved!\n");
}

void Cloth::load(char* file) {
	ifstream instream;
	instream.open(file);
	char* buffer = (char*)malloc(128 * sizeof(char));
	float x, y, z;
	int count;
	particles.clear();
	faces.clear();
	k_structural = 10;
	k_shear = 5;
	provot_structural_correction = 0;
	provot_shear_correction = 0;

	instream.getline(buffer, 128);
	while (!instream.eof()) {
		if (buffer[0] == 'v' && buffer[1] == ' ') {
			int i, j, k;
			char* xstr = (char*)malloc(16 * sizeof(char));
			for (i = 2; buffer[i] != ' '; i++) {
				xstr[i-2] = buffer[i];
			}
			x = atof(xstr);
			free(xstr);
			char* ystr = (char*)malloc(16 * sizeof(char));
			for (j = i+1; buffer[j] != ' '; j++)
				ystr[j-(i+1)] = buffer[j];
			y = atof(ystr);
			free(ystr);
			char* zstr = (char*)malloc(16 * sizeof(char));
			for (k = j+1; buffer[k] != '\0'; k++)
				zstr[k-(j+1)] = buffer[k];
			z = atof(zstr);
			free(zstr);

			ClothParticle temp_particle;
			temp_particle.setOriginalPosition(Vec3f(x, z, y));
			temp_particle.setPosition(Vec3f(x, z, y));
			temp_particle.setVelocity(Vec3f(0,0,0));
			temp_particle.setAcceleration(Vec3f(0,0,0));
			temp_particle.setMass(0.1);
			temp_particle.enable();
			temp_particle.setIJ(0,0);
			temp_particle.setP(particles.size());
			temp_particle.getShear().clear();
			temp_particle.getStructural().clear();
			particles.push_back(temp_particle);
		}
		
		else if (buffer[0] == 'f') {
			std::vector<int> indices;
			int i = 2;
			int j, k;
			while (i < count) {
				if (buffer[i] == '\0') break;
				if (buffer[i] == ' ') { j++; continue; }
				char* tempstr = (char*)malloc(16 * sizeof(char));
				char* tempstr2 = (char*)malloc(8 * sizeof(char));
				for (j = i; buffer[j] != ' ' && buffer[j] != '\0'; j++)
					tempstr[j-i] = buffer[j];
				tempstr[j-i] = '\0';
				for (k = 0; tempstr[k] != '/' && tempstr[k] != '\0'; k++)
					tempstr2[k] = tempstr[k];
				tempstr2[k] = '\0';
				indices.push_back(atoi(tempstr2)-1);
				i += (j-i) + 1;
				free(tempstr);
				free(tempstr2);
			}
			for (unsigned int i = 0; i < indices.size(); i++) {
				int p1 = indices[i];
				int p2 = indices[(i+1)%(indices.size())];
				bool found = false;
				for (unsigned int j = 0; j < particles[indices[i]].getStructural().size(); j++) {
					if (p2 == particles[p1].getStructural()[j].getTarget())
						found = true;
				}

				if (!found) {				
					particles[p1].add_structural(p2, Vec3f(particles[p1].getPosition(), particles[p2].getPosition()).Length());
					particles[p2].add_structural(p1, Vec3f(particles[p1].getPosition(), particles[p2].getPosition()).Length());
				}

				for (unsigned int j = i + 2; j < indices.size(); j++) {
					if (j != (i-1)%indices.size()) {
						particles[indices[i]].add_shear(indices[j], Vec3f(particles[indices[i]].getPosition(), particles[indices[j]].getPosition()).Length());
						particles[indices[j]].add_shear(indices[i], Vec3f(particles[indices[i]].getPosition(), particles[indices[j]].getPosition()).Length());
					}
				}
			}

			for (unsigned int i = 0; i < indices.size(); i++) {
				particles[indices[i]].add_face(indices);
			}
			faces.push_back(indices);
		}

		instream.getline(buffer, 128);
		count = instream.gcount();
	}

	for (unsigned int i = 0; i < particles.size(); i++) {
		bool fixed = true;
		for (unsigned int j = 0; j < particles[i].getStructural().size(); j++) {
			if (particles[particles[i].getStructural()[j].getTarget()].getPosition().y() < particles[i].getPosition().y()) fixed = false;
		}
		particles[i].setFixed(fixed);
	}

	num_fixed = 0;

	fixed_particles = new ClothParticle[particles.size()];

	// the fixed particles
	for (unsigned int p=0; p < particles.size(); p++) {
		if (particles[p].isFixed()) {
			fixed_particles[num_fixed] = particles[p];
			num_fixed ++;
		}
	}
	computeBoundingBox();

	if (file == "flat.obj" || file == "flat16.obj") {
		for (unsigned int i = 0; i < particles.size(); i++) {
		  if (particles[i].getStructural().size() > 2)
			  particles[i].setFixed(false);
		}
		/* User Study Hack incoming! */
		num_fixed = 0;

		fixed_particles = new ClothParticle[particles.size()];

		// the fixed particles
		for (unsigned int p=0; p < particles.size(); p++) {
			if (particles[p].isFixed()) {
				fixed_particles[num_fixed] = particles[p];
				num_fixed ++;
			}
		}
		/* All clear. */

		Vec3f max = getParticle(fixed_particles[2].getP()).getPosition();
		getParticle(fixed_particles[0].getP()).setPosition(max*0.1);
		fixed_particles[0].setPosition(max*0.1);
		getParticle(fixed_particles[1].getP()).setPosition(Vec3f(max.x()*0.9, max.y(), max.z()*0.1));
		fixed_particles[1].setPosition(Vec3f(max.x()*0.9, max.y(), max.z()*0.1));
		getParticle(fixed_particles[2].getP()).setPosition(max*0.9);
		fixed_particles[2].setPosition(max*0.9);
		getParticle(fixed_particles[3].getP()).setPosition(Vec3f(max.x()*0.1, max.y(), max.z()*0.9));
		fixed_particles[3].setPosition(Vec3f(max.x()*0.1, max.y(), max.z()*0.9));
	}

	printf("Loaded!\n");
}