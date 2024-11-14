#include "pixmap.hpp"

Pixmap::Pixmap(string id, Sobj *parent, int width, int height)
    : Sobj(id, parent), m_width(width), m_height(height) {
  m_data_rgb = new unsigned char[width * height * 3];
  m_data_alpha = new unsigned char[width * height];
}

Pixmap::~Pixmap() {
  delete[] m_data_rgb;
  delete[] m_data_alpha;
}