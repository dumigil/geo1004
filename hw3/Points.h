#ifndef Point_h
#define Point_h

struct Point {
  double x, y, z;
  
  Point() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  }
  
  Point(const double &x, const double &y, const double &z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  

  
  const Point operator+(const Point &other) const {
    return Point(x+other.x, y+other.y, z+other.z);
  }
  
  const Point operator-(const Point &other) const {
    return Point(x-other.x, y-other.y, z-other.z);
  }
  
  const Point operator*(const double &other) const {
    return Point(x*other, y*other, z*other);
  }
  
  const Point operator/(const double &other) const {
    return Point(x/other, y/other, z/other);
  }
  
  double dot(const Point &other) const {
    return x*other.x + y*other.y + z*other.z;
  }
  
  const Point cross(const Point &other) const {
    return Point(y*other.z-z*other.y, -(x*other.z-z*other.x), x*other.y-y*other.x);
  }
};



#endif /* Point_h */
