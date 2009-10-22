// Mesher.cpp - simple scene mesher for radiosity
// khn, sep, 2001

#include "Mesher.h"

using namespace std;
using namespace CGLA;


class TriangleArea {
public:

	float Area(Vec3f* u) const {

		Vec3f u1 = u[0];
		Vec3f u2 = u[1];
		Vec3f u3 = u[2];

		Vec3f A = u2-u1;
		Vec3f B = u2-u3;
		Vec3f C = u3-u1;	
		
		double a = A.length();
		double b = B.length();
		double c = C.length();
		double s=(a+b+c)/2;
		double r= sqrt(s*(s-a)*(s-b)*(s-c));

		return (float)r;

	}

};


class MyPolygonSplitter {		// splits an n-gon into triangles

	Vec3f tri[3]; // triangle data;
	int offset;
	Vec3f* poly;
	int verts;

public: 

	
	void Split(Vec3f* _poly, int polyverts) {
		poly = _poly;
		verts = polyverts;
		offset=1;

	}

	Vec3f* NextTriangle(void) {

		if (offset+1>=verts) return NULL;
			
		tri[0] = poly[0];
		tri[1] = poly[offset];
		tri[2] = poly[offset+1];
		offset+=1;

		return tri;

	}

};


class SutherlandHodgmanClipper {

	void Output(Vec3f newMyVertex, int *outLength, Vec3f *vertOut);
	bool Inside(Vec3f testvertex, Vec3f clS, Vec3f clE);
	Vec3f Intersect(Vec3f first, Vec3f second, Vec3f clS, Vec3f clE);

public:

	void ClipToLine( Vec3f *vertIn, Vec3f *vertOut, int inLength, int *outLength, Vec3f clS, Vec3f clE);
	void ClipToMyPolygon( Vec3f *pvertIn, Vec3f *vertOut, int inLength, int *outLength, Vec3f* poly, int polylength);

};

void SutherlandHodgmanClipper::ClipToMyPolygon( Vec3f *pvertIn, Vec3f *vertOut, int inLength, int *outLength, Vec3f* poly, int polylength) {
	
	int k;
	int olength=0;
	Vec3f vertIn[10];
		
	for (k=0;k<inLength;k++) vertIn[k] = pvertIn[k];	
	
	for(k=0; k<polylength; k++)	{	
		
		ClipToLine(vertIn, vertOut, inLength, &olength,	poly[k%polylength],poly[(k+1)%polylength]);

		for(int y=0; y<olength; y++) vertIn[y] = vertOut[y];
		inLength = olength;

	}

	*outLength = olength;
}


void SutherlandHodgmanClipper::ClipToLine(	Vec3f *vertIn, Vec3f *vertOut,
			    int inLength, int *outLength, Vec3f clS, Vec3f clE)
{
	Vec3f s,p,i;
	int j;

	*outLength = 0 ;  
	s = vertIn[inLength -1];
	for(j=0; j<inLength; j++)
	{
		p=vertIn[j];
		if(Inside(p,clS,clE))
		{
			if(Inside(s, clS, clE))
			{
				Output(p, outLength, vertOut);
			}
			else
			{
				i = Intersect(s,p, clS, clE);
				Output(i, outLength, vertOut);
				Output(p, outLength, vertOut);
			}
		}
		else
		{
			if(Inside(s, clS, clE))
			{
				i = Intersect(s,p, clS, clE);
				Output(i, outLength, vertOut);
			}
		}
		s = p;
	}
}	

void SutherlandHodgmanClipper::Output(Vec3f newMyVertex, int *outLength, Vec3f *vertOut)
{
	vertOut[*outLength] = newMyVertex;
	(*outLength)++;
}

bool SutherlandHodgmanClipper::Inside(Vec3f testvertex, Vec3f clS, Vec3f clE)
{
	Vec3f temp = clE-clS;
	Vec3f normal(temp[1], -temp[0], 0);
	Vec3f dir = testvertex-clS;

	if(dot(normal,dir)>0) return false;
	else return true;
}

Vec3f SutherlandHodgmanClipper::Intersect(Vec3f first, Vec3f second, Vec3f clS, Vec3f clE)
{
	float t;
	Vec3f temp = clE-clS;
	Vec3f normal(temp[1], -temp[0], 0);
	Vec3f dir = first-clS;
	Vec3f D = second - first;

	t = -(dot(normal,dir)/dot(normal,D));
	Vec3f result = D*t + first;
	return result;
}



namespace {
	void calcMyPolygonAreaAndCenter(Vec3f* poly, int polyverts, float &area, Vec3f &center) {

		area = 0;
		center = Vec3f(0,0,0);	
		
		if (polyverts) {	// calc polygon area and polygon center-point

			MyPolygonSplitter psplit;
			TriangleArea triarea;
		
			psplit.Split(poly, polyverts);
			Vec3f* tri;							
			int tris = 0;							
			Vec3f base = poly[0];							
			
			while ((tri=psplit.NextTriangle())!=NULL) {
				Vec3f mycenter = (tri[0]+tri[1]+tri[2])*(1.0f/3.0f)-base;																
				float myarea = triarea.Area(tri);								
				area+=myarea;
				center += myarea * mycenter;
				tris++;
			}

			if (area!=0) center *= 1.0f/area;
			center = base + center;

		}

	}
}



namespace Mesher {
	void mesh(vector<MyVertex*> inv, vector<MyPolygon*> inp, vector<MyVertex*> &outv, vector<MyPolygon*> &outp, float _size, float lightsize) {
		
		for (int i=0;i<inp.size();i++) {

			float size = _size;
			
			int vertices = inp[i]->vertices;
					
			// setup basis

			Vec3f opoint[4];

			{
				for (int j=0;j<vertices;j++) {
					opoint[j] = inv[inp[i]->vertex[j]]->position;
				}
			}		
			
			Vec3f basisu = opoint[1] - opoint[0];
			Vec3f basisv = opoint[vertices-1] - opoint[0];

			basisu.normalize();
			basisv.normalize();
			
			Vec3f basisw = cross(basisu, basisv);
			Vec3f newbasisv = cross(basisw, basisu);

			Vec3f normal = basisw;
			normal.normalize();
				
			Mat4x4f mat = identity_Mat4x4f();
			
			identity_Mat4x4f();

			{ for (int q=0;q<3;q++) {	mat[q][0] = basisu[q]; } }
			{ for (int q=0;q<3;q++) {	mat[q][1] = newbasisv[q]; } }
			{ for (int q=0;q<3;q++) {	mat[q][2] = basisw[q]; } }

			Mat4x4f imat = invert(mat);

			Vec3f point[4];
			
			// transform to 2D and find AABB
			
			float minx, miny, maxx, maxy;
			minx = miny = +1e+30f;
			maxx = maxy = -1e+30f;
				
			{
				for (int j=0;j<vertices;j++) {
					
					point[j] = imat.mul_3D_point(opoint[j]);
				
					if (point[j][0] < minx ) minx = point[j][0];
					if (point[j][0] > maxx ) maxx = point[j][0];

					if (point[j][1] < miny ) miny = point[j][1];
					if (point[j][1] > maxy ) maxy = point[j][1];
					
				}
			}

			// calculate imaginary "texture coordinates"
			
			int inverts = vertices;
			int outverts=0;
			
			int ymin, xmin, xmax, ymax;

			int max_subdivx = 10;
			int max_subdivy = 10;
			
			Vec3f crapx = opoint[1] - opoint[0];
			Vec3f crapy = opoint[vertices-1] - opoint[0];		
			
			if (0 != inp[i]->emissive.length()) {
				size = lightsize;
			}
			
			max_subdivx = (int)( crapy.length() / size +0.5f);
			max_subdivy = (int)( crapx.length() / size +0.5f);

			xmin = ymin = 0;
			xmax = max_subdivx;		
			ymax = max_subdivy;		
			
			Vec3f unclipped[10];  // unclipped polygon
			Vec3f clipped[10];

			float scalex = (1.0f/(maxx-minx));
			float scaley = (1.0f/(maxy-miny));
			
			{
				for (int j=0;j<vertices;j++) {				
					unclipped[j][0] = (point[j][0] - minx) * scalex;
					unclipped[j][1] = (point[j][1] - miny) * scaley;
					unclipped[j][2] = point[j][2] ;
					
				}
			}		
									
			// adjust bounding box (just to be sure every element is accounted for )
			ymin--; if (ymin<0) ymin = 0;
			xmin--; if (xmin<0) xmin = 0;
			xmax++; if (xmax>max_subdivx) xmax = max_subdivx; if (xmax<=0) xmax=1;
			ymax++; if (ymax>max_subdivy) ymax = max_subdivy; if (ymax<=0) ymax=1;
						
			Vec3f emissive = inp[i]->emissive;
			Vec3f diffuse  = inp[i]->diffuse;
			
			{
				
				MyPolygon* p;

				int vertexoffset = outv.size()-1; 
				if (vertexoffset<0) vertexoffset=0;
				
				for (int i=ymin;i<ymax;i++) {
					for (int j=xmin;j<xmax;j++) {					
				
							Vec3f texel[10];  // texel boundary
							Vec3f* pin = texel;

							float deltax = 1  *  1.0f/(float)max_subdivy;
							float deltay = 1  *  1.0f/(float)max_subdivx;
							float xx= 1  *  (float)i/(float)max_subdivy;
							float yy= 1  *  (float)j/(float)max_subdivx;						

							*pin++ = Vec3f(xx, yy, unclipped[0][2]);
							*pin++ = Vec3f(xx+deltax,yy, unclipped[0][2]);
							*pin++ = Vec3f(xx+deltax,yy+deltay, unclipped[0][2]);
							*pin++ = Vec3f(xx,yy+deltay, unclipped[0][2]);
							
							SutherlandHodgmanClipper clipper;
							clipper.ClipToMyPolygon(unclipped, clipped, inverts, &outverts, texel, 4);
							
							// create polygon & vertices 

							p = NULL;
							int verts = outverts;
							Vec3f* vpoly = clipped;
							int first=1;							
							MyPolygonSplitter psplit;	
							
							if (outverts) while (true) {		

								if (outverts>4) {	// larger polygons
																		
									Vec3f A = clipped[0] - clipped[2];
									Vec3f B = clipped[1] - clipped[3];									

									float a = A.length();
									float b = B.length();

									cout << a << "," << b << endl;

									if (a>=b-b && a<=b+b && outverts==4) {  // check if square quad 
										first = -1;
									} else {			// split non-square quad into triangles									
										if (first) psplit.Split(clipped, outverts);
										first=0;
										vpoly = psplit.NextTriangle();
										verts = 3;										
										if (!vpoly) break;
									}

								} else first = -1;	
								
								p = new MyPolygon();
								p->diffuse = Vec3f(-1,-1,-1);

								for (int q=0;q<verts;q++) {
									
									vpoly[q][0] = (((vpoly[q][0]/scalex)) + minx);
									vpoly[q][1] = (((vpoly[q][1]/scaley)) + miny);
									vpoly[q] = mat.mul_3D_point(vpoly[q]);	// transform back to 3D
									
									#define EQ(a,b) (a>=b-0.001f && a<=b+0.001f)
																	
									int found = -1;
									for(int i=vertexoffset;i<outv.size();i++) {
										if (EQ(outv[i]->position[0],vpoly[q][0]) && 
											EQ(outv[i]->position[1],vpoly[q][1]) && 
											EQ(outv[i]->position[2],vpoly[q][2]) ) {
											found = i;
											break;
										}
									}

									if (-1!=found) {
										p->vertex[q] = found;
									} else {
										MyVertex* v = new MyVertex();
										v->position = vpoly[q];
										p->vertex[q] = outv.size();
										outv.push_back(v);
									}

								}
								
								
								Vec3f center;
								float area = 0;				
																
								calcMyPolygonAreaAndCenter(vpoly, verts, area, center);

								if (area>0.001f) outp.push_back(p);
								else p=NULL;

								if (p) {
	
									p->center     = center;
									p->area       = area;
									p->normal     = normal;
									p->rad        = emissive;
									p->diffuse    = diffuse;
									p->unshot_rad = p->rad;
									p->color	  = diffuse;
									p->vertices   = verts;

								}

								if (-1==first) break;
										
															
							} // end while (true)

						} // end for (int j=xmin;j<xmax;j++) 

					}	// end for (int i=ymin;i<ymax;i++) 
					
				}

		} // end for (int i=0;i<inp.size();i++)

	}
}


