#ifndef __MESHER_H__
#define __MESHER_H__

#include "DataFormat.h"
#include "X3DSceneGraph/SceneGraph.h"
#include <vector>

namespace Mesher {

	/** 
			Function mesh divides a polygonal (quad or tri) mesh into a 
			larger number of smaller polygons. The emissive colour of the
			original polygons will be assigned as unshot radiosity to the
			newly created polygons.

			Arguments
			inv              - input vertices
			inp              - input polygons
			outv             - output vertices
			outp             - output polygons
			subdivsize       - desired side length of polygons.
			light_subdivsize - desired side length of (emissive) polygons
	b*/
	
	void mesh(std::vector<MyVertex*> inv, 
						std::vector<MyPolygon*> inp, 
						std::vector<MyVertex*> &outv, 
						std::vector<MyPolygon*> &outp, 
						float subdivsize, 
						float light_subdivsize);

};


#endif // __MESHER_H__

