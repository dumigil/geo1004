#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

#include "Point.h"
#include "Rows.h"
#include "VoxelGrid.h"
std::vector<Point> vertices;
std::vector<std::vector<unsigned int>> faces;
float signed_volume(const Point &a, const Point &b, const Point &c, const Point &d) {
  // to do
  return 0;
}

bool intersects(const Point &orig, const Point &dest, const Point &v0, const Point &v1, const Point &v2) {
  // to do
  return 0;
}
bool read_obj(std::string filepath){
    std::ifstream infile(filepath.c_str(),std::ios::in);

    if(!infile){
        std::cerr<<"Input file not found.\n";
        return false;
    }
    std::cout<<"Reading input file "<<filepath<<std::endl;
    std::string line;
    while(std::getline(infile, line)){
        if(line.substr(0,2)=="v "){
            //std::cout<<line<<std::endl;
            std::istringstream v(line.substr(2));
            Point point;
            float x, y, z;
            v>>x,v>>y,v>>z;
            point = Point(x,y,z);
            vertices.push_back(point);
        }

        else if(line.substr(0,2)=="f "){
            std::istringstream f(line.substr(2));
            std::vector<unsigned int> face;

            int a,b,c;
            f>>a,f>>b,f>>c;

            a--;b--;c--;
            face.push_back(a);
            face.push_back(b);
            face.push_back(c);
            faces.push_back(face);
            //std::cout<<"("<<face[0]<<", "<<face[1]<<", "<<face[2]<<")"<<std::endl;
            face.clear();



        }
    }


}


int main(int argc, const char * argv[]) {
  char *file_in = "bag_bk.obj";
  char *path = "../";
  const char *file_out = "vox.obj";
  float voxel_size = 1.0;

  // Read file
  std::ifstream input_file;
  std::string input(std::string(path) + file_in);


  // to do
  read_obj(input.c_str());
  //std::cout<<"../"<<file_in<<std::endl;
  std::cout<<vertices.size()<<" vertices read from .obj"<<std::endl;
  std::cout<<faces.size()<<" faces read from .obj"<<std::endl;

    // Create grid
  Rows rows;
  // to do
  VoxelGrid voxels(rows.x, rows.y, rows.z);
  
  // Voxelise
  for (auto const &triangle: faces) {
    // to do
  }
  
  // Fill model
  // to do
  
  // Write voxels
  // to do
  
  return 0;
}
