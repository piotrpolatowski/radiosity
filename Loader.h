#ifndef __LOADER_H__
#define __LOADER_H__

#include "DataFormat.h"
#include "X3DSceneGraph/SceneGraph.h"
#include "Components/ResourceLoader.h"
#include <vector>

void load_model(std::string& file_name, 
								std::vector<MyVertex*>& vertices, 
								std::vector<MyPolygon*>& polygons);

#endif // __LOADER_H__
