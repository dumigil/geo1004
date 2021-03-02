#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <chrono>

#include "Point.h"
#include "Rows.h"
#include "VoxelGrid.h"



float signed_volume(const Point &a, const Point &b, const Point &c, const Point &d) {
    Point cross= (b-d).cross(c-d);
    float dot = (a-d).dot(cross);
    return dot/6;
}

bool intersects(const Point &orig, const Point &dest, const Point &v0, const Point &v1, const Point &v2) {
    float origin = signed_volume(v0,v1,v2,orig);
    float destination= signed_volume(v0,v1,v2,dest);
    if((origin <0 && destination > 0) || (origin >0 && destination <0)){
        float v0v1_tet = signed_volume(v0,orig, dest,v1);
        float v0v2_tet = signed_volume(v0,orig, dest,v2);
        float v1v0_tet = signed_volume(v1, orig, dest, v0);
        float v1v2_tet = signed_volume(v1, orig, dest, v2);
        float v2v1_tet = signed_volume(v2, orig, dest, v1);
        float v2v0_tet = signed_volume(v2, orig, dest, v0);
        if(v0v1_tet*v0v2_tet < 0 && v1v0_tet * v1v2_tet < 0 && v2v1_tet * v2v0_tet < 0) {
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}


int main(int argc, const char * argv[]) {
  char *file_in = "bag_bk.obj";
  char *path = "../";
  const char *file_out = "../bk_voxelised.obj";
  float voxel_size =1.0;
  float voxelVolume= voxel_size * voxel_size * voxel_size;
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<Point> vertices;
  std::vector<std::vector<unsigned int>> faces;

  // Read file
  std::ifstream input_file;
  std::string input(std::string(path) + file_in);
  std::ifstream infile(input.c_str(),std::ios::in);

    if(!infile){
        std::cerr<<"Input file not found.\n";
        return 1;
    }
    std::cout<<"--- Reading input file "<<input<<" ---"<<std::endl;
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
  std::cout<<"--- "<<vertices.size()<<" vertices read from"<< input<<" ---"<<std::endl;
  std::cout<<"--- "<<faces.size()<<" faces read from "<<input<<" ---"<<std::endl;

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

  float minX = min_x.x;
  float minY = min_y.y;
  float minZ = min_z.z;
  float maxX = max_x.x;
  float maxY = max_y.y;
  float maxZ = max_z.z;
  //std::cout<<maxX<<" "<<minX<<", "<<maxY<<" "<<minY<<", "<<maxZ<<" "<<minZ<<std::endl;
  // Create grid
  float xrows = ceil((ceil(maxX) - floor(minX))/voxel_size);
  float yrows = ceil((ceil(maxY) - floor(minY))/voxel_size);
  float zrows = ceil((ceil(maxZ)- floor(minZ))/voxel_size);
  //std::cout<<std::setprecision(6)<<xrows<<" "<<yrows<<" "<<zrows<<std::endl;
  Rows rows(xrows, yrows, zrows);
  //std::cout<<rows<<std::endl;

    // to do
  VoxelGrid voxels(rows.x, rows.y, rows.z);
  std::cout<<"--- A grid with "<<voxels.voxels.size()<<" voxels has been initialised ---"<<std::endl;
  int boundaryCells;


  // Voxelise
  for (auto const &triangle: faces) {
    Point p1 = Point(vertices[triangle[0]]);
    Point p2 = Point(vertices[triangle[1]]);
    Point p3 = Point(vertices[triangle[2]]);
    std::vector<Point> bbox = {p1, p2, p3};
    Point tMinX = *std::min_element(bbox.begin(), bbox.end(), [](const Point &a, const Point &b) {
        return a.x < b.x;
    });
    Point tMinY = *std::min_element(bbox.begin(), bbox.end(), [](const Point &a, const Point &b) {
        return a.y < b.y;
    });
    Point tMinZ = *std::min_element(bbox.begin(), bbox.end(), [](const Point &a, const Point &b) {
        return a.z < b.z;
    });
    Point tMaxX = *std::min_element(bbox.begin(), bbox.end(), [](const Point &a, const Point &b) {
        return a.x > b.x;
    });
    Point tMaxY = *std::min_element(bbox.begin(), bbox.end(), [](const Point &a, const Point &b) {
        return a.y > b.y;
    });
    Point tMaxZ = *std::min_element(bbox.begin(), bbox.end(), [](const Point &a, const Point &b) {
        return a.z > b.z;
    });
    int tRowXmin = floor(tMinX.x-minX)/voxel_size;
    int tRowYmin = floor(tMinY.y-minY)/voxel_size;
    int tRowZmin = floor(tMinZ.z-minZ)/voxel_size;
    int tRowXmax = ceil(tMaxX.x-minX)/voxel_size;
    int tRowYmax = ceil(tMaxY.y-minY)/voxel_size;
    int tRowZmax = ceil(tMaxZ.z-minZ)/voxel_size;
      //std::cout<<p1<<" "<<p2<<" "<<p3<<std::endl;
      for (int i=tRowXmin; i< tRowXmax; i++){
          for (int j=tRowYmin; j<tRowYmax; j++){
              for(int h=tRowZmin; h<tRowZmax; h++){
                  float x_coord = float(i*voxel_size) + minX;
                  float y_coord = float(j*voxel_size) + minY;
                  float z_coord = float(h*voxel_size) + minZ;
                  Point voxel_center=Point(x_coord+0.5*voxel_size,y_coord+0.5*voxel_size, z_coord+0.5*voxel_size);
                  Point x_max_face = Point(voxel_center.x+0.5*voxel_size,voxel_center.y,voxel_center.z);
                  Point x_min_face = Point(voxel_center.x-0.5*voxel_size,voxel_center.y,voxel_center.z);
                  Point y_max_face = Point(voxel_center.x,voxel_center.y+0.5*voxel_size,voxel_center.z);
                  Point y_min_face = Point(voxel_center.x,voxel_center.y-0.5*voxel_size,voxel_center.z);
                  Point z_max_face = Point(voxel_center.x,voxel_center.y,voxel_center.z+0.5*voxel_size);
                  Point z_min_face = Point(voxel_center.x,voxel_center.y,voxel_center.z-0.5*voxel_size);


                  if(intersects(x_min_face, x_max_face, p1, p2, p3) || intersects(y_min_face, y_max_face, p1, p2, p3) || intersects(z_min_face, z_max_face, p1, p2, p3)){
                      //std::cout<<"Intersection"<<std::endl;
                      voxels(i,j,h) = 1;
                      boundaryCells ++;
                  }
                  else{
                      continue;
                  }
              }
          }
      }
  }
  
  // Fill model
  // to do
    int exteriorCells;

    for (int i=0; i< voxels.max_x; i++) {
        for (int j=0; j< voxels.max_y; j++) {
            for (int k = voxels.max_z - 1; k >= 0; k--) {
                if(voxels(i,j,k) != 1){
                    voxels(i,j,k) = 2;
                    exteriorCells ++;
                }
                else{
                    break;
                }

            }
        }
    }
    int interiorCells =  voxels.voxels.size() - exteriorCells - boundaryCells;
    float volume = (boundaryCells * 0.5*voxelVolume) + (interiorCells*voxelVolume);
    std::cout<<"--- The volume of this model is " <<std::setprecision(10)<< volume << " m^3 ---"<<std::endl;

  // Write voxels
  // to do
  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = stop - start;
  std::cout<<"--- Voxelisation performed in " << elapsed.count() << " seconds ---"<<std::endl;
  auto startWrite = std::chrono::high_resolution_clock::now();
  std::ofstream outfile(file_out, std::ofstream::out);
  int vertexCount = 0;
  int b = 0;

    for (int i=0; i< voxels.max_x; i++) {
        for (int j = 0; j < voxels.max_y; j++) {
            for (int h = 0; h < voxels.max_z; h++) {
                float x_coord = float(i*voxel_size) + minX;
                float y_coord = float(j*voxel_size) + minY;
                float z_coord = float(h*voxel_size) + minZ;
                float scale = 0.8;
                float renderSize = 0.5*scale*voxel_size;
                Point voxel_center=Point(x_coord+0.5*voxel_size,y_coord+0.5*voxel_size, z_coord+0.5*voxel_size);
                float x = voxel_center.x;
                float y = voxel_center.y;
                float z = voxel_center.z;

                if(voxels(i,j,h) != 2) {
                    outfile <<std::setprecision(8) << "v "<<x-renderSize<<" "<<y-renderSize<<" "<<z-renderSize<<std::endl;
                    outfile <<std::setprecision(8) << "v "<<x-renderSize<<" "<<y+renderSize<<" "<<z-renderSize<<std::endl;
                    outfile <<std::setprecision(8) << "v "<<x+renderSize<<" "<<y-renderSize<<" "<<z-renderSize<<std::endl;
                    outfile <<std::setprecision(8) << "v "<<x+renderSize<<" "<<y+renderSize<<" "<<z-renderSize<<std::endl;

                    outfile <<std::setprecision(8) << "v "<<x-renderSize<<" "<<y-renderSize<<" "<<z+renderSize<<std::endl;
                    outfile <<std::setprecision(8) << "v "<<x-renderSize<<" "<<y+renderSize<<" "<<z+renderSize<<std::endl;
                    outfile <<std::setprecision(8) << "v "<<x+renderSize<<" "<<y-renderSize<<" "<<z+renderSize<<std::endl;
                    outfile <<std::setprecision(8) << "v "<<x+renderSize<<" "<<y+renderSize<<" "<<z+renderSize<<std::endl;
                    vertexCount += 8;
                    outfile <<"f "<<b+1<<" "<<b+2<<" "<<b+3<<std::endl;
                    outfile <<"f "<<b+2<<" "<<b+3<<" "<<b+4<<std::endl;
                    outfile <<"f "<<b+1<<" "<<b+2<<" "<<b+6<<std::endl;
                    outfile <<"f "<<b+1<<" "<<b+5<<" "<<b+6<<std::endl;
                    outfile <<"f "<<b+1<<" "<<b+5<<" "<<b+7<<std::endl;
                    outfile <<"f "<<b+1<<" "<<b+3<<" "<<b+7<<std::endl;
                    outfile <<"f "<<b+4<<" "<<b+3<<" "<<b+7<<std::endl;
                    outfile <<"f "<<b+4<<" "<<b+8<<" "<<b+7<<std::endl;
                    outfile <<"f "<<b+4<<" "<<b+2<<" "<<b+6<<std::endl;
                    outfile <<"f "<<b+4<<" "<<b+8<<" "<<b+6<<std::endl;
                    outfile <<"f "<<b+8<<" "<<b+7<<" "<<b+6<<std::endl;
                    outfile <<"f "<<b+5<<" "<<b+7<<" "<<b+6<<std::endl;
                    b+=8;
                }
            }
        }
    }

    outfile.close();
    auto stopWrite = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedWrite = stopWrite-startWrite;
    std::cout<<"--- File written to "<< file_out<< " in " << elapsedWrite.count() << " seconds ---"<<std::endl;




  return 0;
}
