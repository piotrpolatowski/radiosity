#include "Loader.h"
#include "X3DSceneGraph/SceneGraphLoader.h"
#include "X3DSceneGraph/SceneGraphTools.h"
#include "Components/ResourceLoader.h"

using namespace std;
using namespace CGLA;
using namespace std;
using namespace X3DSceneGraph;

namespace {
	vector<Mat4x4f> matrix_stack;
	Mat4x4f matrix_current = identity_Mat4x4f();
}

namespace SG = X3DSceneGraph;

namespace  {
	void convertToNewFormat(SG::Group* group, vector<MyVertex*> *vertices, vector<MyPolygon*> *polygons) {
		
		Mat4x4f matrix_translation;
		Mat4x4f matrix_rotation;

		MyVertex* vertex;
		MyPolygon *polygon;
		int nr=0;
		if (SG::Transform* tf=dynamic_cast<SG::Transform*>(group)) {
			matrix_stack.push_back(matrix_current);
			matrix_translation = translation_Mat4x4f(tf->translation);
			matrix_rotation = tf->rotation.get_mat4x4f();
			matrix_current *=  matrix_translation  * matrix_rotation;
		}
		for(vector<SG::Drawable*>::iterator i=group->children.begin();i!=group->children.end();++i) {
			if (SG::Group* gr=dynamic_cast<SG::Group*>(*i)) 
				convertToNewFormat(gr, vertices, polygons);

			if (SG::Shape* sh=dynamic_cast<SG::Shape*>(*i))  {
				Vec3f diffuse = Vec3f(0,0,0);
				Vec3f emissive = Vec3f(0,0,0);
				if (SG::Appearance* ap = dynamic_cast<SG::Appearance*>(sh->appearance)) {
					diffuse.set(ap->material->diffuseColor[0], ap->material->diffuseColor[1], ap->material->diffuseColor[2]);
					emissive.set(ap->material->emissiveColor[0], ap->material->emissiveColor[1], ap->material->emissiveColor[2]);
				}

				if (SG::IndexedFaceSet* fc = dynamic_cast<SG::IndexedFaceSet*>(sh->geometry)) {
					int offset = vertices->size();
					for(int k=0;k<fc->coord->point.size();k++) {
						vertex = new ::MyVertex();
						Vec3f position3 = fc->coord->point[k];
						Vec4f position4 = Vec4f(position3[0],position3[1], position3[2],1);
						position4 = matrix_current * position4;
						vertex->position = Vec3f(-position4[0],position4[1],-position4[2]);
						vertices->push_back(vertex);
					}
					 
					for(vector<int>::iterator i=fc->coordIndex.begin();i!=fc->coordIndex.end();++i) {
						if ((*i)==-1) {
							polygon->vertices = nr;
							nr=0;
						} else {
							switch (nr) {
							case 0:
								polygon=new MyPolygon();
								polygons->push_back(polygon);
								polygon->vertex[0]= *i + offset;
								polygon->diffuse = diffuse;
								polygon->emissive = emissive;
								nr++;
								break;
							default:
								polygon->vertex[nr++]= *i + offset;
								break;
							}
						}
					}
				}
			}
		}
		if (SG::Transform* tf=dynamic_cast<SG::Transform*>(group)) {
			matrix_current = matrix_stack.back();
			matrix_stack.pop_back();
		}
	}
}


void load_model(std::string& file_name, 
								std::vector<MyVertex*>& vertices, 
								std::vector<MyPolygon*>& polygons)
{
	X3DSceneGraph::Scene *cornell=0;
	cornell = getModel(file_name.c_str());	
	convertToNewFormat(cornell,&vertices, &polygons);
}
