#ifndef __HEMICUBE_H__
#define __HEMICUBE_H__

#include "CGLA/Vec3f.h"
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.1415926
#endif

typedef void (*render_scene_callback)(void);

class Hemicube {
	int size;	
	GLubyte *data;
public:
	int rendersize;
	Hemicube(int size); // Constructor, size is side length of 
	                    // hemicube face.
	~Hemicube();        // Destructor (non-virtual)
	
	/** Render Scene to Hemicube.

	Args:

	viewing parameters. 
	eye       - is centre of patch.
	up        - vector that lies in patch plane.
	direction - is the shooting patch normal.
	callback  - pointer to function that renders scene.
	*/
	void renderScene(CGLA::Vec3f eye, CGLA::Vec3f up, CGLA::Vec3f direction, 
									 render_scene_callback callback);

	// Read back hemicube. Call after you have called render scene.
	void readIndexBuffer();

	// Get patch index for pixel (px,py) in hemicube image.
	unsigned int getIndex(int px, int py);

	// Get delta form factor for pixel (px,py) in hemicube image.
	float getDeltaFormFactor(int px, int py);
};

#endif // __HEMICUBE_H__
