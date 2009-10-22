#include <iostream>
#include "Hemicube.h"
#include "Graphics/PPMBitmap.h"

using namespace CGLA;
using namespace std;

const int NEAR_PLANE = 1;
const int FAR_PLANE = 1000;

Hemicube::Hemicube(int _size) : size(_size) {
	rendersize = size*2;
	
	data = new GLubyte[rendersize*rendersize*3];
	for (int i=0;i<rendersize*rendersize*3;i++) data[i] = 0;
}

Hemicube::~Hemicube() {
	delete [] data;
}

void Hemicube::renderScene(Vec3f eye, Vec3f up, Vec3f direction, render_scene_callback callback) {
	int quater1_size = size/2;
	int quater2_size = 2 * quater1_size;
	int quater3_size = 3 * quater1_size;
	int quater4_size = 4 * quater1_size;

	glFinish();
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	// Center
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1,1,-1,1,NEAR_PLANE,FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glViewport(quater1_size,quater1_size,quater2_size,quater2_size);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],
						eye[0] + direction[0],eye[1] + direction[1],eye[2] + direction[2],
						up[0],up[1],up[2]); 
	callback(); // render scene;


	// Right
	Vec3f right = cross(up, direction);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(0,1,-1,1,NEAR_PLANE,FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,quater1_size,quater1_size,quater2_size);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],
						eye[0] + right[0],eye[1] + right[1],eye[2] + right[2],
						up[0],up[1],up[2]); 
	callback(); // render scene;

	// Left
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1,0,-1,1,NEAR_PLANE,FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glViewport(quater3_size,quater1_size,quater1_size,quater2_size);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],
						eye[0] - right[0],eye[1] - right[1],eye[2] - right[2],
						up[0],up[1],up[2]); 
	callback(); // render scene;


	// Down
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1,1,0,1,NEAR_PLANE,FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glViewport(quater1_size,0,quater2_size,quater1_size);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],
						eye[0] - up[0],eye[1] - up[1],eye[2] - up[2],
						direction[0],direction[1],direction[2]); 
	callback(); // render scene;

	// Up
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1,1,-1,0,NEAR_PLANE,FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glViewport(quater1_size,quater3_size,quater2_size,quater1_size);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],
						eye[0] + up[0],eye[1] + up[1],eye[2] + up[2],
						-direction[0],-direction[1],-direction[2]); 
	callback(); // render scene;

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glFinish();

}

void Hemicube::readIndexBuffer() {

	glReadPixels((GLint)0,(GLint)0,rendersize,rendersize,GL_RGB, GL_UNSIGNED_BYTE, data);	
	/****************
	GFX::PPMBitmap blob(rendersize,rendersize);
	glReadPixels((GLint)0,(GLint)0,rendersize,rendersize,GL_RGBA, GL_UNSIGNED_BYTE, blob.get());
	blob.write("blob.ppm");
	exit(0);
	***************/
}

unsigned int Hemicube::getIndex(int px, int py) {
			
	assert(px>=0 && px<rendersize);
	assert(py>=0 && py<rendersize);
		
 	unsigned char r = (unsigned char)data[(rendersize*(py)+px)*3+0]; 
 	unsigned char g = (unsigned char)data[(rendersize*(py)+px)*3+1]; 
 	unsigned char b = (unsigned char)data[(rendersize*(py)+px)*3+2]; 

	return ((r*256)+g)*256+b;
			
}

float Hemicube::getDeltaFormFactor(int px, int py) {
	float x,y,z;
	
	float rs2 = (float)rendersize/2.0f;
	
	int quater1_size = size/2;
	int quater2_size = 2 * quater1_size;
	int quater3_size = 3 * quater1_size;
	int quater4_size = 4 * quater1_size;

	if (px>=quater1_size && px<=quater3_size-1 && py>=0 && py<=quater1_size-1) { // Area 1 = top
		x=(float)(px-quater2_size)/(float)quater1_size;
		y=(float)(py)/(float)quater1_size;
		z=y;
	} else if (px>=quater3_size && px<=quater4_size-1 && py>=quater1_size && py<=quater3_size-1) { // Area 2 = right
		x=(float)(quater4_size-px)/(float)quater1_size;
		y=(float)(py-quater2_size)/(float)quater1_size;
		z=x;
	} else if (px>=quater1_size && px<=quater3_size-1 && py>=quater3_size && py<=quater4_size-1) { // Area 3 = bottom
		x=(float)(px-quater2_size)/(float)quater1_size;
		y=(float)(quater4_size-py)/(float)quater1_size;
		z=y;
	} else if (px>=0 && px<=quater1_size-1 && py>=quater1_size && py<=quater3_size-1) { // Area 4 = left
		x=(float)(px)/(float)quater1_size;
		y=(float)(py-quater2_size)/(float)quater1_size;
		z=x;
	} else if (px>=quater1_size && px<=quater3_size-1 && py>=quater1_size && py<=quater3_size-1) { // Area 5 = center
		x=(float)(px-quater2_size)/(float)quater1_size;
		y=(float)(py-quater2_size)/(float)quater1_size;
		z=1;
	} else {
		return 0;
	}

	float dist = x*x+y*y+1.0;
	float dff = z/(M_PI*dist*dist*quater1_size*quater1_size);
	
	return dff; 

}
