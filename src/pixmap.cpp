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