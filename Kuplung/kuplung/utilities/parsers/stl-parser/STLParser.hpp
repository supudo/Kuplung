//
//  STLParser.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef STLParser_hpp
#define STLParser_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"

class STLParser {
public:
    ~STLParser();
    void init(std::function<void(float)> doProgress);
    std::vector<MeshModel> parse(FBEntity file);
    void destroy();

    std::vector<float> vertices, normals;

private:
    std::function<void(float)> funcProgress;
};

#endif /* STLParser_hpp */

/*
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define DEG_TO_RAD(x) (x * 0.0174532925199f)
using namespace std;

struct v3 {
    float x, y, z;
    v3(float _x=0, float _y=0, float _z=0) : x(_x), y(_y), z(_z) {}
    float dotproduct(const v3 &v) const { return x*v.x + y*v.y + z*v.z; }
    void transform(const glm::mat4 &mat) { glm::vec4 v = glm::vec4(x, y, z, 1.0f), vt; vt = mat*v; x=vt.x; y=vt.y; z=vt.z;}
    v3& operator-=(const v3 &pt) { x-=pt.x; y-=pt.y; z-=pt.z; return *this;    }
    v3 operator-(const v3 &pt) { return v3(x-pt.x, y-pt.y, z-pt.z); }
    v3 operator+(const v3 &pt) { return v3(x+pt.x, y+pt.y, z+pt.z); }
    v3 operator/(float a) { return v3(x/a, y/a, z/a); }
    v3 operator*(float a) { return v3(x*a, y*a, z*a); }
    float normalize() const { return sqrt(x*x+y*y+z*z); }
};
v3 operator-(const v3 &a, const v3 &b) {return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
v3 operator+(const v3 &a, const v3 &b) {return v3(a.x+b.x, a.y+b.y, a.z+b.z); }

struct LineSegment {
    LineSegment(v3 p0=v3(), v3 p1=v3()) { v[0]=p0; v[1]=p1; }
    v3 v[2];
};

class Plane {
public:
    Plane() : mDistance(0) {}
    float distance() const { return mDistance; }
    float distanceToPoint(const v3 &vertex) const {    return vertex.dotproduct(mNormal) – mDistance; }
    void setNormal(v3 normal) { mNormal = normal; }
    void setDistance(float distance) { mDistance = distance; }
protected:
    v3        mNormal;    // normalized Normal-Vector of the plane
    float   mDistance;  // shortest distance from plane to Origin
};
struct Triangle    {
    Triangle(v3 n, v3 v0, v3 v1, v3 v2) : normal(n) { v[0]=v0; v[1]=v1; v[2]=v2; }
    Triangle& operator-=(const v3 &pt) { v[0]-=pt; v[1]-=pt; v[2]-=pt; return *this;}
    void transform(const glm::mat4 &mat) { v[0].transform(mat); v[1].transform(mat); v[2].transform(mat);}
    // @return -1 = all triangle is on plane back side
    //          0 = plane intersects the triangle
    //          1 = all triangle is on plane front side
    //         -2 = error in function
    int intersectPlane(const Plane &plane, LineSegment &ls) const {
        // a triangle has 3 vertices that construct 3 line segments
        size_t cntFront=0, cntBack=0;
        for (size_t j=0; j<3; ++j) {
            float distance = plane.distanceToPoint(v[j]);
            if (distance<0) ++cntBack;
            else ++cntFront;
        }
        if (3 == cntBack) {
            return -1;
        }
        else if (3 == cntFront) {
            return 1;
        }
        size_t lines[] = {0,1,1,2,2,0}; // CCW Triangle
        std::vector<v3> intersectPoints;
        for (size_t i=0; i<3; ++i) {
            const v3 &a = v[lines[i*2+0]];
            const v3 &b = v[lines[i*2+1]];
            const float da = plane.distanceToPoint(a);
            const float db = plane.distanceToPoint(b);
            if (da*db<0) {
                const float s = da/(da-db); // intersection factor (between 0 and 1)
                v3 bMinusa = b-a;
                intersectPoints.push_back(a+bMinusa*s);
            }
            else if (0==da) { // plane falls exactly on one of the three Triangle vertices
                if (intersectPoints.size()<2)
                    intersectPoints.push_back(a);
            }
            else if (0==db) { // plane falls exactly on one of the three Triangle vertices
                if (intersectPoints.size()<2)
                    intersectPoints.push_back(b);
            }
        }
        if (2==intersectPoints.size()) {
            // Output the intersecting line segment object
            ls.v[0]=intersectPoints[0];
            ls.v[1]=intersectPoints[1];
            return 0;
        }
        return -2;
    }
    v3 v[3], normal;
};
class TriangleMesh {
public:
    TriangleMesh() : bottomLeftVertex(999999,999999,999999), upperRightVertex(-999999,-999999,-999999) {}
    size_t size() const { return mesh.size(); }
    // move 3D Model coordinates to be center around COG(0,0,0)
    void normalize() {
        v3 halfBbox = (upperRightVertex – bottomLeftVertex)/2.0f;
        v3 start = bottomLeftVertex + halfBbox;
        for (size_t i=0; i<mesh.size(); ++i) {
            Triangle &triangle = mesh[i];
            triangle -= start;
        }
        bottomLeftVertex = halfBbox*-1.0f;
        upperRightVertex = halfBbox;
    }
    void push_back(const Triangle &t) {
        mesh.push_back(t);
        for (size_t i=0; i<3; ++i) {
            if (t.v[i].x < bottomLeftVertex.x) bottomLeftVertex.x = t.v[i].x;
            if (t.v[i].y < bottomLeftVertex.y) bottomLeftVertex.y = t.v[i].y;
            if (t.v[i].z < bottomLeftVertex.z) bottomLeftVertex.z = t.v[i].z;
            if (t.v[i].x > upperRightVertex.x) upperRightVertex.x = t.v[i].x;
            if (t.v[i].y > upperRightVertex.y) upperRightVertex.y = t.v[i].y;
            if (t.v[i].z > upperRightVertex.z) upperRightVertex.z = t.v[i].z;
        }
    }
    v3 meshAABBSize() const {
        return v3(upperRightVertex.x-bottomLeftVertex.x, upperRightVertex.y-bottomLeftVertex.y, upperRightVertex.z-bottomLeftVertex.z);
    }
    std::vector<Triangle>& getMesh() { return mesh; }
    const std::vector<Triangle>& getMesh() const { return mesh; }
    v3 getBottomLeftVertex() const { return bottomLeftVertex; }
    v3 getUpperRightVertex() const { return upperRightVertex; }
    // Mesh COG point should be at (0,0,0)
    int transform(const glm::mat4 &mat) {
        for (size_t i=0; i<mesh.size(); ++i) {
            Triangle &triangle = mesh[i];
            triangle.transform(mat);
        }
        return 0;
    }
protected:
    std::vector<Triangle> mesh;
    v3 bottomLeftVertex, upperRightVertex;
};

// read the given STL file name (ascii or binary is set using ‘isBinaryFormat’)
// and generate a Triangle Mesh object in output parameter ‘mesh’
int stlToMeshInMemory(const char *stlFile, TriangleMesh *mesh, bool isBinaryFormat) {
    if (!isBinaryFormat) {
        ifstream in(stlFile);
        if (!in.good()) return 1;
        char title[80];
        std::string s0, s1;
        float n0, n1, n2, f0, f1, f2, f3, f4, f5, f6, f7, f8;
        in.read(title, 80);
        while (!in.eof()) {
            in >> s0;                                // facet || endsolid
            if (s0==”facet”) {
                in >> s1 >> n0 >> n1 >> n2;            // normal x y z
                in >> s0 >> s1;                        // outer loop
                in >> s0 >> f0 >> f1 >> f2;         // vertex x y z
                in >> s0 >> f3 >> f4 >> f5;         // vertex x y z
                in >> s0 >> f6 >> f7 >> f8;         // vertex x y z
                in >> s0;                            // endloop
                in >> s0;                            // endfacet
                // Generate a new Triangle with Normal as 3 Vertices
                Triangle t(v3(n0, n1, n2), v3(f0, f1, f2), v3(f3, f4, f5), v3(f6, f7, f8));
                mesh->push_back(t);
            }
            else if (s0==”endsolid”) {
                break;
            }
        }
        in.close();
    }
    else {
        FILE *f = fopen(stlFile, “rb”);
        if (!f) return 1;
        char title[80];
        int nFaces;
        fread(title, 80, 1, f);
        fread((void*)&nFaces, 4, 1, f);
        float v[12]; // normal=3, vertices=3*3 = 12
        unsigned short uint16;
        // Every Face is 50 Bytes: Normal(3*float), Vertices(9*float), 2 Bytes Spacer
        for (size_t i=0; i<nFaces; ++i) {
            for (size_t j=0; j<12; ++j) {
                fread((void*)&v[j], sizeof(float), 1, f);
            }
            fread((void*)&uint16, sizeof(unsigned short), 1, f); // spacer between successive faces
            Triangle t(v3(v[0], v[1], v[2]), v3(v[3], v[4], v[5]), v3(v[6], v[7], v[8]), v3(v[9], v[10], v[11]));
            mesh->push_back(t);
        }
        fclose(f);
    }
    mesh->normalize();
    return 0;
}

// Take an input Triangle Mesh ‘mesh’ and fill the output
// parameter ‘slicesWithLineSegments’ with line segments for
// each slice
int triMeshSlicer(
                  const TriangleMesh                        *mesh,        // the const input mesh
                  std::vector<std::vector<LineSegment>>    &slicesWithLineSegments,
                  // the result slices
                  const float                                sliceSize) {// slice size in 3D Model digital units
    Plane plane;                                        // The intersection plane
    plane.setNormal(v3(0, 0, 1));                        // normal does not change during slicing
    const v3 aabb = mesh->meshAABBSize();                // as the model for it’s 3D axis-aligned bounding-box
    const size_t nSlices = 1+(int)(aabb.z/sliceSize);    // compute number of output slices
    const std::vector<Triangle> &m = mesh->getMesh();    // get a const handle to the input mesh
    const float z0 = mesh->getBottomLeftVertex().z;        // find the minimal z coordinate of the model (z0)
    for (size_t i=0; i<nSlices; ++i) {                    // start generating slices
        std::vector<LineSegment> linesegs;                // the linesegs vector for each slice
        plane.setDistance(z0+(float)i*sliceSize);        // position the plane according to slice index
        for (size_t t=0; t<m.size(); ++t) {                // iterate all mesh triangles
            const Triangle &triangle = m[t];            // get a const handle to a triangle
            LineSegment ls;
            if (0==triangle.intersectPlane(plane, ls)) {// the plane does intersect the triangle
                linesegs.push_back(ls);                    // push a new Line Segment object to this slice
            }
        }
        slicesWithLineSegments.push_back(linesegs);        // push this vector to the slices vector
    }
    return 0;
}

// GIV is a free viewer: http://giv.sourceforge.net/giv/
// output a single GIV ascii file with the slices, layed out in a matrix form
int    exportSingleGIVFormat(
                             const std::vector<std::vector<LineSegment>> &slicesWithLineSegments,
                             const v3                                    &aabbSize) {
    char filename[256];
    FILE *f=NULL;
    float dx=0, dy=0;
    f=fopen(“out.marks”, “w”);
    if (!f)    return 1;
    const size_t nSlices = slicesWithLineSegments.size();
    const size_t slicePerRow = (size_t)sqrt((float)nSlices);
    for (size_t i=0; i<nSlices; ++i) {
        const std::vector<LineSegment> &lss = slicesWithLineSegments[i];
        dx = (float)(i%slicePerRow)*(aabbSize.x*1.05f);
        dy = (float)(i/slicePerRow)*(aabbSize.y*1.05f);
        for (size_t j=0; j<lss.size(); ++j) {
            fprintf(f, “\n\n$line”);
            fprintf(f, “\n$color blue”);
            fprintf(f, “\n%f %f”, dx+lss[j].v[0].x, dy+lss[j].v[0].y);
            fprintf(f, “\n%f %f”, dx+lss[j].v[1].x, dy+lss[j].v[1].y);
        }
    }
    fclose(f);
    return 0;
}

// GIV is a free viewer: http://giv.sourceforge.net/giv/
// output a single GIV ascii file with the slices, layed out in 3D form
int    exportSingleGIVFormat3D(
                               const std::vector<std::vector<LineSegment>> &slicesWithLineSegments,
                               const v3                                    &aabbSize) {
    // Generate a 3D View using OpenGL Math Library
    glm::detail::tvec3<float> eulerAngles(0, 0.0f, 45.0f);
    glm::detail::tquat<float> quaternion = glm::detail::tquat<float>(DEG_TO_RAD(eulerAngles)); // This glm func wants values as radians
    float angle = glm::angle(quaternion);
    glm::detail::tvec3<float> axis = glm::axis(quaternion);

    glm::mat4 View        = glm::rotate(glm::mat4(1.0), angle, axis);
    glm::mat4 Projection  = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);
    glm::mat4 Model       = glm::lookAt(
                                        glm::vec3(1, 1, 1),    // Eye point (where am I?)
                                        glm::vec3(0, 0, 0),    // Look at Center point (What am I looking at?)
                                        glm::vec3(0, 1, 0));// UP vector of the camera (Where is my UP vector?)
    glm::mat4 MVP = Projection * View * Model;

    // Start Output
    char filename[256];
    FILE *f=NULL;
    v3 p0, p1;
    f=fopen(“out3D.marks”, “w”);
    if (!f)    return 1;
    const size_t nSlices = slicesWithLineSegments.size();
    const size_t slicePerRow = (size_t)sqrt((float)nSlices);
    for (size_t i=0; i<nSlices; ++i) {
        const std::vector<LineSegment> &lss = slicesWithLineSegments[i];
        for (size_t j=0; j<lss.size(); ++j) {
            p0=lss[j].v[0];
            p1=lss[j].v[1];
            p0.transform(MVP);
            p1.transform(MVP);
            fprintf(f, “\n\n$line”);
            fprintf(f, “\n$color blue”);
            fprintf(f, “\n%f %f”, p0.x, p0.y);
            fprintf(f, “\n%f %f”, p1.x, p1.y);
        }
    }
    fclose(f);
    return 0;
}

int main(int argc, char **argv) {
    float    sliceSize = 1.0f;
    char    modelFileName[1024];
    bool    isBinaryFormat = true;
    glm::detail::tvec3<float> eulerAngles(0,0,0);

    for (int i=1; i<argc;) {
        if (0==strcmp(argv[i], “-slicesize”)) {
            sliceSize = atof(argv[i+1]);
            i+=2;
        }
        else if (0==strcmp(argv[i], “-model”)) {
            strcpy(modelFileName, argv[i+1]);
            i+=2;
        }
        else if (0==strcmp(argv[i], “-ascii”)) {
            isBinaryFormat = false;
            i+=1;
        }
        else if (0==strcmp(argv[i], “-eulerangles”)) {
            eulerAngles.x = atof(argv[i+1]);
            eulerAngles.y = atof(argv[i+2]);
            eulerAngles.z = atof(argv[i+3]);
            i+=4;
        }
        else {
            ++i;
        }
    }
    // Parse the file and generate a TriangleMesh
    TriangleMesh mesh;
    if (stlToMeshInMemory(modelFileName, &mesh, isBinaryFormat)!=0)
        return 1;
    fprintf(stderr, “Mesh has %d triangles\n”, mesh.size());

    // Optional Model Rotation Around COG Point using GLM Library
    glm::mat4 mat = glm::mat4(1.0f);
    glm::detail::tquat<float> quaternion = glm::detail::tquat<float>(DEG_TO_RAD(eulerAngles)); // This glm func wants values as radians
    float angle = glm::angle(quaternion);
    glm::detail::tvec3<float> axis = glm::axis(quaternion);
    mat = glm::rotate(mat, angle, axis);
    mesh.transform(mat);

    // Generate Slices
    std::vector<std::vector<LineSegment>> slicesWithLineSegments;
    triMeshSlicer(&mesh, slicesWithLineSegments, sliceSize);
    // Output in 2D Matrix form
    exportSingleGIVFormat(slicesWithLineSegments, mesh.meshAABBSize());
    // Output in 3D Layered Form
    exportSingleGIVFormat3D(slicesWithLineSegments, mesh.meshAABBSize());
    return 0;
}
*/
