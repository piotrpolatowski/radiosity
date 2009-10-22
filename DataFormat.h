#ifndef __DATAFORMAT_H__
#define __DATAFORMAT_H__

#include "CGLA/Vec3f.h"

class MyVertex {
public:
	CGLA::Vec3f position;
	
	// The variables below should be used in nodal averaging.
	int colorcount;     // Number of polygons incident on this vertex.
	CGLA::Vec3f color;  // Final (averaged) colour of vertex.
};

class MyPolygon {
public:
	int vertices;          // Number of vertices
	int vertex[4];         // Vertices
	CGLA::Vec3f diffuse;   // R,G,B reflectivity
	CGLA::Vec3f emissive;  // R,G,B emission
	CGLA::Vec3f normal;    
	CGLA::Vec3f center;
	float area;
	float formF;           // Form factor
		
	CGLA::Vec3f rad;        // Radiosity (converges to polygon colour)
	CGLA::Vec3f unshot_rad; // Radiosity that needs to be shot.

	CGLA::Vec3f color;      // Radiosity times scale factor.
};

#endif // __DATAFORMAT_H__
