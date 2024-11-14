#include "shape.hpp"

#include <cmath>

#include "standard.hpp"

Shape::Shape(string id, int n, char type, Shapes *parent_shapes)
    : Sobj(id), m_n(n), m_type(type), m_parent_shapes(parent_shapes) {
  m_data = new double[n];
  m_area = 0.0;

  for (int i = 0; i < n; i++) {
    switch (type) {
      case 'm':
        // hamming
        m_data[i] = 0.54 - 0.46 * cos(2.0 * PI * (i + 1.0) / n);
        break;

      case 'r':
        // rectangular
        m_data[i] = 1.0;
        break;

      case 'b':
        // blackman
        m_data[i] = 0.42 - 0.5 * cos(PI * (i + 1.0) / n) +
                    0.08 * cos(2.0 * PI * (i + 1.0) / n);

      case 'f':
        // flat top
        m_data[i] = 1.0 - 1.93 * cos(2.0 * PI * (i + 1.0) / n) +
                    1.29 * cos(4.0 * PI * (i + 1.0) / n) -
                    0.388 * cos(6.0 * PI * (i + 1.0) / n) +
                    0.0322 * cos(8.0 * PI * (i + 1.0) / n);
        break;

      case 'c':
        // cosine
        m_data[i] = sin(0.5 * PI * (i + 1.0) / n);
        break;

      case 'g':
        // gauss
        m_data[i] = exp(-sqr(3.0 * (n - 1.0 - i) / double(n)));
        break;

      case 'n':
      default:
        // hanning
        m_data[i] = 0.5 - 0.5 * cos(PI * (i + 1.0) / n);
        break;
    }

    if (i == n - 1) {
      m_area += 0.5 * m_data[i];
    } else {
      m_area += m_data[i];
    }
  }
}

Shape::~Shape() { delete[] m_data; }

Shapes::Shapes(string id, char type, int n_min, int n_max, int num_shapes)
    : Sobj(id) {
  vector<int> n_list;
  vector<int>::iterator p;
  Shape *shape;
  char tmp[10];

  if (num_shapes == 1) {
    n_list.push_back(n_max);
  } else {
    for (int i = 0; i < num_shapes; i++) {
      n_list.push_back(int(round(
          n_min * pow((double)n_max / (double)n_min, i / (num_shapes - 1.0)))));
    }
  }

  // 重複を取り除く
  for (p = n_list.begin(); p != n_list.end(); p++) {
    if (*p == *(p + 1)) {
      n_list.erase(p);
      p++;
    }
  }

  // 重複を取り除く
  for (p = n_list.begin(); p != n_list.end(); p++) {
    snprintf(tmp, 10, "%04d", *p);
    shape = new Shape(id + "_" + tmp, *p, type, this);
    m_shape_list.push_back(shape);
  }
}
