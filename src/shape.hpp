#pragma once

#include "sobj.hpp"

class Shapes;
class Shape : public Sobj {
 private:
  Shapes *m_parent_shapes;
  int m_n;
  char m_type;
  double m_area;
  double *m_data;

 public:
  Shape(string id, int n, char type, Shapes *parent_shapes = nullptr);
  ~Shape();
  int n() { return m_n; }
  char type() { return m_type; }
  double area() { return m_area; }
  double *data() { return m_data; }
};

class Shapes : public Sobj {
 private:
  vector<Shape *> m_shape_list;

 public:
  Shapes(string id, char type, int n_min, int n_max, int num_shapes);
  vector<Shape *> &shape_list() { return m_shape_list; }
};