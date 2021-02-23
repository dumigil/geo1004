#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <iomanip>
namespace fs = std::filesystem;

#include "Point.h"
#include "Rows.h"
#include "VoxelGrid.h"



float signed_volume(const Point &a, const Point &b, const Point &c, const Point &d) {
    Point cross= (b-d).cross(c-d);
    float dot = (a-d).dot(cross);
    //float vol = (dot);
    return dot/6;
}

bool intersects(const Point &orig, const Point &dest, const Point &v0, const Point &v1, const Point &v2) {
    float origin = signed_volume(v0,v1,v2,orig);
    float destination= signed_volume(v0,v1,v2,dest);
    if((origin <0 && destination > 0) || (origin >0 && destination <0)){
        return true;
    }
    else{
        return false;
    }
}

static bool abs_compare(float a, float b)
{
    return (std::abs(a) < std::abs(b));
}


int main(int argc, const char * argv[]) {
  char *file_in = "bag_bk.obj";
  char *path = "../";
  const char *file_out = "vox.obj";
  float voxel_size = 1.0;
  std::vector<Point> vertices;
  std::vector<std::vector<unsigned int>> faces;

  // Read file
  std::ifstream input_file;
  std::string input(std::string(path) + file_in);
  std::ifstream infile(input.c_str(),std::ios::in);

    if(!infile){
        std::cerr<<"Input file not found.\n";
        return false;
    }
    std::cout<<"Reading input file "<<input<<std::endl;
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

  // to do
  //std::cout<<"../"<<file_in<<std::endl;
  std::cout<<vertices.size()<<" vertices read from .obj"<<std::endl;
  std::cout<<faces.size()<<" faces read from .obj"<<std::endl;

  //Compute bbox
  Point min_x = *std::min_element(vertices.begin(), vertices.end(), [](const Point &a, const Point &b){
      return a.x < b.x;
  });
  Point min_y = *std::min_element(vertices.begin(), vertices.end(), [](const Point &a, const Point &b){
        return a.y < b.y;
  });
  Point min_z = *std::min_element(vertices.begin(), vertices.end(), [](const Point &a, const Point &b){
        return a.z < b.z;
  });
  Point max_x = *std::min_element(vertices.begin(), vertices.end(), [](const Point &a, const Point &b){
        return a.x > b.x;
  });
  Point max_y = *std::min_element(vertices.begin(), vertices.end(), [](const Point &a, const Point &b){
        return a.y > b.y;
  });
  Point max_z = *std::min_element(vertices.begin(), vertices.end(), [](const Point &a, const Point &b){
        return a.z > b.z;
  });

  float minX = min_x.x;;
  float minY = min_y.y;
  float minZ = min_z.z;;
  float maxX = max_x.x;;
  float maxY = max_y.y;
  float maxZ = max_z.z;;
  std::cout<<maxX<<" "<<minX<<","<<maxY<<" "<<minY<<","<<maxZ<<" "<<minZ<<std::endl;
  // Create grid
  float xrows = (ceil(maxX) - floor(minX))/voxel_size;
  float yrows = (ceil(maxY) - floor(minY))/voxel_size;
  float zrows = (ceil(maxZ)- floor(minZ))/voxel_size;
  std::cout<<std::setprecision(6)<<xrows<<" "<<yrows<<" "<<zrows<<std::endl;
  Rows rows(xrows, yrows, zrows);
  std::cout<<rows<<std::endl;

    // to do
  VoxelGrid voxels(rows.x, rows.y, rows.z);
  std::cout<<voxels.voxels.size()<<std::endl;


  // Voxelise
  for (auto const &triangle: faces) {
    //std::cout<<vertices[triangle[0]]<<", "<<vertices[triangle[1]]<<", "<<vertices[triangle[2]]<<std::endl;
    Point p1 = Point(vertices[triangle[0]]);
    Point p2 = Point(vertices[triangle[1]]);
    Point p3 = Point(vertices[triangle[2]]);
    //std::cout<<p1<<" "<<p2<<" "<<p3<<std::endl;
      for (int i=0; i<= voxels.max_x; i++){
          for (int j=0; j<=voxels.max_y; j++){
              for(int h=0; h<=voxels.max_z; h++){
                  float x_coord = float(i) + minX;
                  float y_coord = float(j) + minY;
                  float z_coord = float(h) + minZ;
                  //std::cout<<x_coord<<" "<<y_coord<<" "<<z_coord<<std::endl;
                  Point voxel_center=Point(x_coord-0.5*voxel_size,y_coord-0.5*voxel_size, z_coord-0.5*voxel_size);
                  Point x_max_face = Point(x_coord,y_coord-0.5*voxel_size,z_coord-0.5*voxel_size);
                  Point x_min_face = Point(x_coord-voxel_size,y_coord-0.5*voxel_size,z_coord-0.5*voxel_size);
                  Point y_max_face = Point(x_coord-0.5*voxel_size,y_coord,z_coord-0.5*voxel_size);
                  Point y_min_face = Point(x_coord-0.5*voxel_size,y_coord-voxel_size,z_coord-0.5*voxel_size);
                  Point z_max_face = Point(x_coord-0.5*voxel_size,y_coord-0.5*voxel_size,z_coord);
                  Point z_min_face = Point(x_coord-0.5*voxel_size,y_coord-0.5*voxel_size,z_coord-voxel_size);

                  //std::cout<<x_min_face<<voxel_center<<x_max_face<<std::endl;
                  //std::cout<<y_min_face<<voxel_center<<y_max_face<<std::endl;
                  //std::cout<<z_min_face<<voxel_center<<z_max_face<<std::endl;
                  //std::cout<<std::setprecision(10)<<signed_volume(p1,p2,p3,x_max_face)<<" versus "<<signed_volume(p1,p2,p3,x_min_face)<<std::endl;
                  //std::cout<<intersects(x_min_face,x_max_face,p1,p2,p3)<<std::endl;
                  if(intersects(x_min_face,x_max_face,p1,p2,p3) || intersects(y_min_face,y_max_face,p1,p2,p3) || intersects(z_min_face,z_max_face,p1,p2,p3)){
                      std::cout<<"Intersection"<<std::endl;
                  }
              }
          }
      }
  }
  
  // Fill model
  // to do
  
  // Write voxels
  // to do
  
  return 0;
}
