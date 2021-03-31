#include <iostream>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "map"
#include "stack"
#include "Points.h"
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;
#include "json.hpp"
using json = nlohmann::json;
std::map<int,Point> vmap;
int count=0;
std::vector<Point> vertices;
std::vector<std::vector<unsigned int>> triangles;
float maxZ = -INFINITY; float minZ = INFINITY; float maxX = -INFINITY;
float minX = INFINITY; float maxY = -INFINITY; float minY = INFINITY;
void importOBJ(const char *file_in) {
    std::ifstream infile(file_in);
    std::string line= "";
    while (std::getline(infile, line)) {
        std::stringstream ss;
        ss.clear();
        ss.str(line);
        std::string prefix = "";
        ss >> prefix;
        if (prefix == "v") {
            count++;
            float x, y, z;
            ss >> x >> y >> z;
            Point vertex={x,y,z};
            vmap[count]=vertex;
            vertices.push_back(vertex);
            if (z<minZ){minZ = z;}
            if (z>maxZ){maxZ = z;}
            if (x<minX){minX = x;}
            if (x>maxX){maxX = x;}
            if (y<minY){minY = y;}
            if (y>maxY){maxY = y;}
        }
        else if (prefix == "f") {
            unsigned int v1,v2,v3;
            ss >> v1 >> v2 >> v3;
            std::vector<unsigned int> temp = {v1,v2,v3};
            triangles.push_back(temp);
        }
        }
        std::cout << vertices.size() << std::endl;
        std::cout << triangles.size() << std::endl;
}
void exportCityJSON(const char *file_out) {
    std::ofstream outfile(file_out);
    outfile <<"{\n";
    outfile <<"  \"type\": \"CityJSON\",\n";
    outfile <<"  \"version\": \"1.0\",\n";
    outfile <<"  \"CityObjects\": {\n";
    outfile << "       \"MyTerrain\": {\n";
    outfile << "          \"type\": \"TINRelief\",\n";
    outfile << "          \"geographicalExtent\": [ "<<minX<<", "<<minY<<", "<<minZ<<", "<<maxX<<", "<<maxY<<", "<<maxZ<<" ],\n";
    outfile << "          \"geometry\": [{\n";
    outfile << "              \"type\": \"CompositeSurface\",\n";
    outfile << "              \"lod\": 2,\n";
    outfile << "              \"boundaries\": [\n";
    std::string del="";
    for (auto const &t:triangles) {
        outfile << del << "[[";
        outfile << t[0] - 1 << ", " << t[1] - 1 << ", " << t[2] - 1 << "]]";
        del = ", ";
    }
    outfile<<"\n";
    outfile<<"          ]\n";
    outfile<<"      }]\n";
    outfile<<"      }\n";
    outfile<<"    },"<<"\n";
    //now the vertices
//    outfile << "   },\n";
    outfile << "\"vertices\": [\n";
    std::string d="";
    for (auto const &v:vertices){
        outfile<<"      "<<d<<"[ "<<v.x<<", "<<v.y<<", "<<v.z<<"]\n";
        d=", ";
    }
    //close
    outfile<<"   ]\n";
    outfile<<"}\n";
    outfile.close();
}

struct Point2d{
    float x, y;

    Point2d() {
        x = 0.0;
        y = 0.0;
    }
    Point2d(const float &x, const float &y) {
        this->x = x;
        this->y = y;
    }
    float &operator[](const int &coordinate) {
        if (coordinate == 0) return x;
        else if (coordinate == 1) return y;
        else assert(false);
    }

    float operator[](const int &coordinate) const {
        if (coordinate == 0) return x;
        else if (coordinate == 1) return y;
        else assert(false);
    }
};
std::ostream& operator<<(std::ostream& os, const Point2d& p) {
    os << "(" << p.x << ", " << p.y <<")";
    return os;
}




void importGeoJSON(const std::string json_in, const std::string json_out){
    std::vector<Point> vertices;
    int i=0;
    int k=1;
    int ii=0;
    int kk=1;
    std::ifstream infile(json_in, std::ios::in);
    std::ofstream outfile(json_out, std::ios::out);
    std::string delim2="";
    outfile<<"{\n";
    outfile<<"  \"type\": \"CityJSON\",\n";
    outfile<<"  \"version\": \"1.0\",\n";
    outfile<<"  \"metadata\": {\n";
    outfile<<"      \"referenceSystem\": \"urn:ogc:def:crs:EPSG::7415\"\n";
    outfile<<"                },\n";

    outfile<<"  \"CityObjects\": {\n";
    if(!infile){
        std::cerr<<"Input file not found\n";
        return;
    }
    json j;infile >>j;
    auto features = j["features"];
    for(auto &all: features){
        std::vector<int> base;
        std::vector<int> roof;
        std::vector<std::vector<int>> walls;
        std::vector<std::vector<int>> base_rings;
        std::vector<std::vector<int>> roof_rings;

        auto geom = all["geometry"]["coordinates"];
        float height = all["properties"]["_elevation_max"];
        auto id = all["properties"]["identificatie"];
        auto year = all["properties"]["bouwjaar"];
        auto storeys = ceil(height/3);
        std::vector<std::vector<Point>> rings;
        for(auto &xy: geom){
            std::vector<Point> base_ring;
            for(const auto &p: xy){

                if(height != 0){
                    Point p_base = Point(p[0],p[1],p[2]);
                    base_ring.push_back(p_base);
                }
            }
            rings.push_back(base_ring);
        }
        for(auto ring: rings){
            std::vector<int> base_ring;
            std::vector<int> roof_ring;
            for(auto point: ring){
                Point pt_base = Point(point.x, point.y, point.z);
                base_ring.push_back(ii);
                ii+=2;
                vertices.push_back(pt_base);
                Point pt_roof = Point(point.x, point.y, height);
                roof_ring.push_back(kk);
                kk+=2;
                vertices.push_back(pt_roof);

            }
            base_rings.push_back(base_ring);
            roof_rings.push_back(roof_ring);
        }

        /*
        for(auto &xy: geom){
            for(const auto &p: xy){
                if(height != 0){
                    Point p_base = Point(p[0],p[1],0);
                    base.push_back(i);
                    i+=2;
                    vertices.push_back(p_base);

                    Point p_roof = Point(p[0], p[1], height);
                    roof.push_back(k);
                    k+=2;
                    vertices.push_back(p_roof);
                }
            }
        }
         */
        if(!base_rings.empty()){
            for(int a=0; a<base_rings.size();a++){
                for(int x=1; x<base_rings[a].size();x++){
                    std::vector<int> wall;
                    wall.push_back(base_rings[a][x-1]);
                    wall.push_back(base_rings[a][x]);
                    wall.push_back(roof_rings[a][x]);
                    wall.push_back(roof_rings[a][x-1]);
                    walls.push_back(wall);
                }
            }
        }

        if(base_rings[0].size()!=0) {
            /*
            for (int b = 1; b < base.size(); b++) {
                std::vector<int> wall;
                wall.push_back(base[b-1]);
                wall.push_back(base[b]);
                wall.push_back(roof[b]);
                wall.push_back(roof[b-1]);
                walls.push_back(wall);
            }*/


            outfile << "      " << delim2 << id << ": {\n";
            outfile << "          \"type\": \"Building\",\n";
            outfile << "          \"attributes\": {\n";
            outfile << "              \"yearOfConstruction\": " << year << " ,\n";
            outfile << "              \"measuredHeight\": " << height << " ,\n";
            outfile << "              \"storeysAboveGround\": " << storeys << "\n";
            outfile << "          },\n";
            outfile << "          \"geometry\": [{\n";
            outfile << "              \"type\": \"Solid\",\n";
            outfile << "              \"lod\": 1.2,\n";
            outfile << "              \"boundaries\": [[\n";
            std::string delim;
            std::string comma;
            outfile << delim << "[";
            /*
            for(auto all:base_rings){
                std::reverse(all.begin(), all.end());
            }
             */
            std::string delim3=" ";

            for(auto base:base_rings) {
                std::reverse(base.begin(), base.end());

                outfile << "                  " << delim3 << "[";
                std::string comma3=" ";

                for (auto &v: base) {
                    outfile << comma3 << v;
                    comma3 = ", ";
                }
                delim3=", ";
                outfile << "]\n ";

            }

            outfile << "]\n ";
            outfile << ", ";

            for (auto &w: walls) {
                comma = "";
                delim = "";

                outfile << "                  " << delim << "[[";

                for (auto &wall: w) {
                    outfile << comma << wall;
                    comma = ", ";
                }

                outfile << "]]\n ";
                outfile << ", ";
            }

            comma = "";
            delim = "";

            delim3=" ";
            outfile<<"[";



            for(auto roof:roof_rings) {
                outfile << "                  " << delim3 << "[";
                std::string comma3=" ";

                for (auto &v: roof) {
                    outfile << comma3 << v;
                    comma3 = ", ";
                }
                delim3=", ";
                outfile << "]\n ";

            }
            outfile<<"]]";

            outfile << "               ],\n";
            outfile << "              \"semantics\": {\n";
            outfile << "                \"surfaces\": [\n";
            outfile << "                            {\n";
            outfile << "                    \"type\": \"GroundSurface\"\n";
            outfile << "                            },\n";
            outfile << "                            {\n";
            outfile << "                    \"type\": \"WallSurface\"\n";
            outfile << "                            },\n";
            outfile << "                            {\n";
            outfile << "                    \"type\": \"RoofSurface\"\n";
            outfile << "                            }\n";
            outfile << "                ]," << "\n";
            outfile << "                \"values\": [[0, ";
            comma = "";
            for (auto &n: walls) {
                outfile << comma << 1;
                comma = ", ";
            }
            outfile << ", 2]] \n";
            outfile << "                }" << "\n";

            outfile << "          }]\n";

            outfile << "          }" << "\n";
            delim2 = ", ";
        }

    }
    outfile<<"  },\n";
    outfile<<"  \"vertices\": [\n";
    std::string delim;
    delim="";
    for(auto &e: vertices){
        outfile<<std::setprecision(6)<<"    "<<delim<<"[ "<<e.x<<", "<<e.y<<", "<<e.z<<"]\n";
        delim=",";
    }
    outfile<<"  ]\n";
    outfile<<"}\n";
    outfile.close();
    //std::cout<<features<<std::endl;

}

int main(int argc, const char * argv[]) {
    const char *file_in = "C:\\Users\\theoj\\Desktop\\TIN\\LAS.obj";
    const char *file_out = "C:\\Users\\theoj\\Desktop\\TIN\\Tin.json";
    std::string json_out = "../buildings.json";
    std::string json_path = JSON_ELEV_PATH;
    fs::path working_dir = fs::path(json_path).parent_path();
    fs::current_path(working_dir);
    //importOBJ(file_in);
    importGeoJSON(json_path, json_out);
    //exportCityJSON(file_out);
    return 0;
}