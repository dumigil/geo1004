#include <iostream>
#include <list>
#include "DCEL.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <map>
// forward declarations; these functions are given below main()
void DemoDCEL();
void printDCEL(DCEL & D);
Vertex findOrigin(DCEL &D);
float signedVolume( Vertex* &a,  Vertex* &b,  Vertex* &c,  Vertex* &d);
void flipFace(const Face* face);

//global maps used
std::map<Vertex*,unsigned int> invMap;
std::map<std::pair<int,int>, HalfEdge*> emap;
std::map<unsigned int, Vertex*> map;

//Point struct used from hw1 because these vertices are annoying me so much.
struct Point {
    float x, y, z;

    Point() {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }

    Point(const float &x, const float &y, const float &z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    const Point operator-(const Point &other) const {
        return Point(x-other.x, y-other.y, z-other.z);
    }


    float dot(const Point &other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    const Point cross(const Point &other) const {
        return Point(y * other.z - z * other.y, -(x * other.z - z * other.x), x * other.y - y * other.x);
    }
};



/* 
  Example functions that you could implement. But you are 
  free to organise/modify the code however you want.
  After each function you should have a DCEL without invalid elements!
*/
// 1.
void importOBJ(DCEL & D,const char *input) {
  std::vector<std::vector<unsigned int>> faces;

  std::ifstream infile(input, std::ios::in);
  if(!infile){
      std::cerr<<"Input file not found\n";
      return;
  }
  int objIndex = 0;
  std::cout<<"--- Reading input file "<< input <<" ---\n";
  std::string line;
  while(std::getline(infile, line)) {
      if (line.substr(0, 2) == "v ") {

          std::istringstream v(line.substr(2));
            float x, y, z;
            v >> x, v >> y, v >> z;
            Vertex * vert = D.createVertex(x, y, z);
            map[objIndex]=vert;
            invMap[vert]=objIndex;
            objIndex++;

      }
      else if (line.substr(0, 2) == "f ") {
          std::istringstream f(line.substr(2));
          std::vector<unsigned int> face;
          int a, b, c;
          f >> a, f >> b, f >> c;

          a--;b--;c--;
          Vertex* v0=map[a];
          Vertex* v1=map[b];
          Vertex* v2=map[c];
          HalfEdge* e0= D.createHalfEdge();
          HalfEdge* e1= D.createHalfEdge();
          HalfEdge* e2= D.createHalfEdge();
          Face*  f0=D.createFace();

          std::pair<int,int>twin;
          e0->origin = v0;
          e0->destination = v1;
          e0->next = e1;
          e0->prev = e2;
          e0->incidentFace = f0;
          if(v0<=v1){
              {twin.first=a;twin.second=b;}
          }else{
              {twin.first=b;twin.second=a;}
          }if(emap.count(twin)==0){
              emap[twin]=e0;
          }else{
              HalfEdge* etwin=emap[twin];
              e0->twin=etwin;
              emap[twin]->twin=e0;
          }
          e1->origin = v1;
          e1->destination = v2;
          e1->next = e2;
          e1->prev = e0;
          e1->incidentFace = f0;
          if(v1<=v2){
              {twin.first=b;twin.second=c;}
          }else{
              {twin.first=c;twin.second=b;}
          }if(emap.count(twin)==0){
              emap[twin]=e1;
          }else{
              HalfEdge* etwin=emap[twin];
              e1->twin=etwin;
              emap[twin]->twin=e1;
          }
          e2->origin = v2;
          e2->destination = v0;
          e2->next = e0;
          e2->prev = e1;
          e2->incidentFace = f0;
          if(v2<=v0){
              {twin.first=c;twin.second=a;}
          }else{
              {twin.first=a;twin.second=c;}
          }if(emap.count(twin)==0){
              emap[twin]=e2;
          }else{
              HalfEdge* etwin=emap[twin];
              e2->twin=etwin;
              emap[twin]->twin=e2;
          }
          f0->exteriorEdge = e0;


      }
  }
  std::cout<<"--- "<<map.size()<<" vertices read from "<<input<<" ---\n";
  //printDCEL(D);


}
// 2.
void groupTriangles(DCEL & D) {
    std::stack<Face*> faceStack;
    std::list<Face*> inMesh;
    std::list<Face*> checkList;

    for(const auto &face:D.faces()){
        if(std::find(checkList.begin(), checkList.end(), face.get())!= checkList.end()){
            continue;
        }
        else{
            faceStack.push(face.get());
        }
        inMesh.clear();
        while(faceStack.empty()== false){
            Face* f = faceStack.top();
            faceStack.pop();
            std::vector<HalfEdge*> faceEdges = {f->exteriorEdge, f->exteriorEdge->next, f->exteriorEdge->prev};
            for(const auto &currEdge: faceEdges){
                if(std::find(checkList.begin(), checkList.end(), currEdge->twin->incidentFace) != checkList.end()){
                    continue;
                }
                else {
                    faceStack.push(currEdge->twin->incidentFace);
                    checkList.push_back(currEdge->twin->incidentFace);
                    inMesh.push_back(currEdge->twin->incidentFace);
                }
            }

        }
        std::cout<<inMesh.size()<<std::endl;
        D.infiniteFace()->holes.push_back(face->exteriorEdge);
    }
    //printDCEL(D);


}
// 3.
void orientMeshes(DCEL & D) {
    std::list<Face*> correctList;
    std::list<Face*> checkList;
    std::stack<Face*> s;
    Vertex org = findOrigin(D);
    Vertex *origin=&org;
    Face* face = D.faces().begin()->get();
    for(const auto &meshes: D.infiniteFace()->holes){
        Face* f0 = meshes->incidentFace;
        float vol = signedVolume(f0->exteriorEdge->origin, f0->exteriorEdge->next->origin, f0->exteriorEdge->prev->origin, origin);
        correctList.clear();
        if(vol > 0){
            checkList.push_back(f0);
            s.push(f0);
        }
        else{
            flipFace(f0);
            checkList.push_back(f0);
            s.push(f0);
        }
        for(const auto &f: D.faces()) {
            while (s.empty() == false) {
                Face *f_curr = s.top();
                s.pop();
                std::vector<HalfEdge *> faceEdges = {f_curr->exteriorEdge, f_curr->exteriorEdge->next,
                                                     f_curr->exteriorEdge->prev};
                for (const auto &e: faceEdges) {
                    if (std::find(checkList.begin(), checkList.end(), f) != checkList.end()) {
                        continue;
                    } else {
                        if (e->origin != e->twin->destination || e->destination != e->twin->origin) {
                            flipFace(e->twin->incidentFace);
                            s.push(e->twin->incidentFace);
                            correctList.push_back(e->twin->incidentFace);
                            checkList.push_back(e->twin->incidentFace);
                        }
                    }
                }

            }
        }







    }
    printDCEL(D);

}
// 4.
void mergeCoPlanarFaces(DCEL & D) {
  // to do
}
// 5.
void exportCityJSON(DCEL & D, const char *file_out) {
    auto startWrite = std::chrono::high_resolution_clock::now();

    std::ofstream outfile(file_out, std::ofstream::out);
    outfile<<"{\n";
    outfile<<"  \"type\": \"CityJSON\",\n";
    outfile<<"  \"version\": \"1.0\",\n";
    outfile<<"  \"CityObjects\": {\n";
    outfile<<"      \"id-1\": {\n";
    outfile<<"          \"type\": \"Building\",\n";
    outfile<<"          \"geometry\": [{\n";
    outfile<<"              \"type\": \"MultiSurface\",\n";
    outfile<<"              \"lod\": 1,\n";
    outfile<<"              \"boundaries\": [\n";
    std::string delim;



    for(auto &each: D.faces()){

        HalfEdge* ef= each->exteriorEdge;
        Vertex* curr = ef->origin;
        Vertex* next = ef->next->origin;
        std::string comma;

        outfile << "                  " << delim << "[[";
        HalfEdge* e = each->exteriorEdge;
        const HalfEdge* e_start = e;
        do {
            outfile <<comma<< invMap[e->origin];
            e = e->next;
            comma=", ";
        } while ( e_start!=e) ;
        outfile<<"]]\n ";
        delim = ", ";
    }
    outfile<<"               ]\n";
    outfile<<"          }]\n";

    outfile<<"          }\n";
    outfile<<"  },\n";
    outfile<<"  \"vertices\": [\n";

    delim="";
    for(auto &all: D.vertices()){
        outfile<<std::setprecision(6)<<"    "<<delim<<"[ "<<all->x<<", "<<all->y<<", "<<all->z<<"]\n";
        delim=",";
    }
    outfile<<"  ]\n";
    outfile<<"}\n";



    outfile.close();
    auto stopWrite = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedWrite = stopWrite-startWrite;
    std::cout<<"--- File written to "<< file_out<< " in " << elapsedWrite.count() << " seconds ---"<<std::endl;
}

Vertex findOrigin(DCEL &D){
    Vertex* minx=D.vertices().begin()->get();
    Vertex* miny=D.vertices().begin()->get();
    Vertex* minz=D.vertices().begin()->get();
    for(const auto &v: D.vertices()){
        if(v->x<minx->x){
            minx=v.get();
        }
        if(v->y<miny->y){
            miny=v.get();
        }
        if(v->z<minz->z){
            minz=v.get();
        }

    }
    return Vertex(minx->x-1,miny->y-1,minz->y-1);
}

float normalVector(HalfEdge* &e1, HalfEdge* &e2, HalfEdge* &e3){
    Vertex* a=e1->origin;
    Vertex* b=e2->origin;
    Vertex* c=e3->origin;
    Vertex edge1=Vertex(a->x-b->x,a->y-b->y,a->z-a->z);
    Vertex edge2=Vertex(b->x-c->x,b->y-c->y,b->z-c->z);
    //std::cout<<crossProduct(edge1,edge2)<<std::endl;
    //std::cout<<crossProduct(edge2,edge1)<<std::endl;
    std::cout<<" \n";
}

void flipFace(const Face* face){
    int e0_o =invMap[face->exteriorEdge->origin];
    int e0_d =invMap[face->exteriorEdge->destination];
    int e1_o =invMap[face->exteriorEdge->next->origin];
    int e1_d =invMap[face->exteriorEdge->next->destination];
    int e2_o =invMap[face->exteriorEdge->prev->origin];
    int e2_d =invMap[face->exteriorEdge->prev->destination];
    face->exteriorEdge->origin=map[e0_d];
    face->exteriorEdge->destination=map[e0_o];
    face->exteriorEdge->next->origin=map[e1_d];
    face->exteriorEdge->next->destination=map[e1_o];
    face->exteriorEdge->prev->origin=map[e2_d];
    face->exteriorEdge->prev->destination=map[e2_o];
}

float signedVolume( Vertex* &a,  Vertex* &b,  Vertex* &c,  Vertex* &d){
    Point aP = Point(a->x, a->y, a->z);
    Point bP = Point(b->x, b->y, b->z);
    Point cP = Point(c->x, c->y, c->z);
    Point dP = Point(d->x, d->y, d->z);
    Point cross= (bP-dP).cross(cP-dP);
    float dot = (aP-dP).dot(cross);
    return dot/6;
}

int main(int argc, const char * argv[]) {
  const char * file_in = "../../isolated_cubes.obj";
  const char *file_out = "../bk.json";
  // Demonstrate how to use the DCEL to get you started (see function implementation below)
  // you can remove this from the final code
  //DemoDCEL();

  // create an empty DCEL
  DCEL D;
  importOBJ(D, file_in);
  // 1. read the triangle soup from the OBJ input file and convert it to the DCEL,
  
  // 2. group the triangles into meshes,
  groupTriangles(D);

  // 3. determine the correct orientation for each mesh and ensure all its triangles
  //    are consistent with this correct orientation (ie. all the triangle normals 
  //    are pointing outwards).
  orientMeshes(D);
  // 4. merge adjacent triangles that are co-planar into larger polygonal faces.
  
  // 5. write the meshes with their faces to a valid CityJSON output file.
  exportCityJSON(D,file_out);
  return 0;
}


void printDCEL(DCEL & D) {

  // Quick check if there is an invalid element
  auto element = D.findInValid();
  if ( element == nullptr ) {
    // Beware that a 'valid' DCEL here only means there are no dangling links and no elimated elements.
    // There could still be problems like links that point to the wrong element.
    std::cout << "DCEL is valid\n";
  } else {
    std::cout << "DCEL is NOT valid ---> ";
    std::cout << *element << "\n";
  }
/*
  // iterate all elements of the DCEL and print the info for each element
  const auto & vertices = D.vertices();
  const auto & halfEdges = D.halfEdges();
  const auto & faces = D.faces();
  std::cout << "DCEL has:\n";
  std::cout << " " << vertices.size() << " vertices:\n";
  for ( const auto & v : vertices ) {
    std::cout << "  * " << *v << "\n";
  }
  std::cout << " " << halfEdges.size() << " half-edges:\n";
  for ( const auto & e : halfEdges ) {
    std::cout << "  * " << *e << "\n";
  }
  std::cout << " " << faces.size() << " faces:\n";
  for ( const auto & f : faces ) {
    std::cout << "  * " << *f << "\n";
  }
*/
}


void DemoDCEL() {

  std::cout << "/// STEP 1 Creating empty DCEL...\n";
  DCEL D;
  printDCEL(D);

  /*

  v2 (0,1,0)
   o
   |\
   | \
   |  \
   o---o v1 (1,0,0)
  v0
  (0,0,0)

  We will construct the DCEL of a single triangle 
  in the plane z=0 (as shown above).

  This will require:
    3 vertices
    6 halfedges (2 for each edge)
    1 face

  */
  std::cout << "\n/// STEP 2 Adding triangle vertices...\n";
  Vertex* v0 = D.createVertex(0,0,0);
  Vertex* v1 = D.createVertex(1,0,0);
  Vertex* v2 = D.createVertex(0,1,0);
  printDCEL(D);

  std::cout << "\n/// STEP 3 Adding triangle half-edges...\n";
  HalfEdge* e0 = D.createHalfEdge();
  HalfEdge* e1 = D.createHalfEdge();
  HalfEdge* e2 = D.createHalfEdge();
  HalfEdge* e3 = D.createHalfEdge();
  HalfEdge* e4 = D.createHalfEdge();
  HalfEdge* e5 = D.createHalfEdge();
  printDCEL(D);

  std::cout << "\n/// STEP 4 Adding triangle face...\n";
  Face* f0 = D.createFace();
  printDCEL(D);

  std::cout << "\n/// STEP 5 Setting links...\n";
  e0->origin = v0;
  e0->destination = v1;
  e0->twin = e3;
  e0->next = e1;
  e0->prev = e2;
  e0->incidentFace = f0;

  e3->origin = v1;
  e3->destination = v0;
  e3->twin = e0;
  e3->next = e5;
  e3->prev = e4;

  /* 
  If a half-edge is incident to 'open space' (ie not an actual face with an exterior boundary), 
  we use the infiniteFace which is predifined in the DCEL class
  */
  e3->incidentFace = D.infiniteFace();

  e1->origin = v1;
  e1->destination = v2;
  e1->twin = e4;
  e1->next = e2;
  e1->prev = e0;
  e1->incidentFace = f0;

  e4->origin = v2;
  e4->destination = v1;
  e4->twin = e1;
  e4->next = e3;
  e4->prev = e5;
  e4->incidentFace = D.infiniteFace();

  e2->origin = v2;
  e2->destination = v0;
  e2->twin = e5;
  e2->next = e0;
  e2->prev = e1;
  e2->incidentFace = f0;

  e5->origin = v0;
  e5->destination = v2;
  e5->twin = e2;
  e5->next = e4;
  e5->prev = e3;
  e5->incidentFace = D.infiniteFace();

  f0->exteriorEdge = e0;
  printDCEL(D);


  std::cout << "\n/// STEP 6 Traversing exterior vertices of f0...\n";
  /* 
  if all is well in the DCEL, following a chain of half-edges (ie keep going to e.next)
  should lead us back the the half-edge where we started.
  */
  HalfEdge* e = f0->exteriorEdge;
  const HalfEdge* e_start = e;
  do {
    std::cout << " -> " << *e->origin << "\n";
    e = e->next;
  } while ( e_start!=e) ; // we stop the loop when e_start==e (ie. we are back where we started)
  
  
  std::cout << "\n/// STEP 7 eliminating v0...\n";
  v0->eliminate();
  printDCEL(D);
  
  /* 
  We just eliminated v0. At the same time we know there are elements that still 
  pointers to v0 (ie the edges e0, e2, e3, e5). This means we can NOT call D.cleanup()!
  If you do this anyways, the program may crash. 
  
  Eg. if you uncomment the following there could be a crash/stall of the program.
  */
  // D.cleanup(); // this will remove v0 from memory (because we just eliminated v0 and the cleanup() function simply removes all the eliminated elements)
  // std::cout << *v0 << "\n"; // we try to access that memory, but v0 is gone -> undefined behaviour 
  // std::cout << *e0->origin << "\n"; // this equivalent to the previous line (both point to the same memory address)


  std::cout << "\n/// STEP 8 eliminating all the remaining DCEL elements\n";
  for ( const auto & v : D.vertices() ) {
    v->eliminate();
  }
  for ( const auto & e : D.halfEdges() ) {
    e->eliminate();
  }
  for ( const auto & f : D.faces() ) {
    f->eliminate();
  }
  printDCEL(D);

  std::cout << "\n/// STEP 9 cleaning up the DCEL\n";
  D.cleanup();
  printDCEL(D);

}