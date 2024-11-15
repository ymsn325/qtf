#include "pixmap.hpp"

extern bool g_reverse_mode;
extern bool g_bw_mode;

Pixmap::Pixmap(string id, Sobj *parent, int width, int height)
    : Sobj(id, parent), m_width(width), m_height(height) {
  m_flag_reverse = g_reverse_mode;
  m_flag_bw = g_bw_mode;
  m_data_rgb = new unsigned char[width * height * 3];
  m_data_alpha = new unsigned char[width * height];
}

Pixmap::~Pixmap() {
  delete[] m_data_rgb;
  delete[] m_data_alpha;
}

void Pixmap::draw_background(int color24) {
  char r, g, b;

  r = (color24 >> 16) & 0xff;
  g = (color24 >> 8) & 0xff;
  b = color24 & 0xff;

  for (int i = 0; i < m_width * m_height * 3;) {
    m_data_rgb[i++] = r;
    m_data_rgb[i++] = g;
    m_data_rgb[i++] = b;
  }

  for (int i = 0; i < m_width * m_height; i++) {
    m_data_alpha[i] = 0;
  }
}
