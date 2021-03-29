#include <iostream>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "map"
#include "stack"
#include "Points.h"
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

void importGeoJSON(const std::string json_in){
    std::ifstream infile(json_in, std::ios::in);

    if(!infile){
        std::cerr<<"Input file not found\n";
        return;
    }
    json j;infile >>j;
    auto features = j["features"];
    for(auto &all: features){
        std::vector<Point> base;
        std::vector<Point> roof;
        auto geom = all["geometry"]["coordinates"];
        auto height = all["properties"]["_elevation_max"];
        auto id = all["properties"]["identificatie"];
        auto year = all["properties"]["bouwjaar"];
        for(auto &xy: geom){
            for(auto &p: xy){
                if(height != 0){
                    //std::cout<<p[0]<<" and "<<p[1]<<std::endl;
                    Point p_base = Point(p[0],p[1],0);
                    Point p_roof = Point(p[0], p[1], height);
                    base.push_back(p_base);
                    roof.push_back(p_roof);
                }
            }

        }
        std::cout<<base.size()<<std::endl;


    }
    //std::cout<<features<<std::endl;

}

int main(int argc, const char * argv[]) {
    const char *file_in = "C:\\Users\\theoj\\Desktop\\TIN\\LAS.obj";
    const char *file_out = "C:\\Users\\theoj\\Desktop\\TIN\\Tin.json";
    std::string json_path = JSON_ELEV_PATH;
    fs::path working_dir = fs::path(json_path).parent_path();
    fs::current_path(working_dir);
    //importOBJ(file_in);
    importGeoJSON(json_path);
    //exportCityJSON(file_out);
    return 0;
}