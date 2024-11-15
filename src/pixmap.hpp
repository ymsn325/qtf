#pragma once

#include <iostream>

#include "sobj.hpp"

using namespace std;

class Pixmap : public Sobj {
 protected:
  int m_width, m_height;
  bool m_flag_reverse;
  bool m_flag_bw;
  unsigned char *m_data_rgb;
  unsigned char *m_data_alpha;

 public:
  Pixmap(string id, Sobj *parent, int width, int height);
  ~Pixmap();
  int width() { return m_width; }
  int height() { return m_height; }
  unsigned char *data_rgb() { return m_data_rgb; }
  unsigned char *data_alpha() { return m_data_alpha; }
  void draw_background(int color24);
  void put_rgb(int x, int y, int color24) {
    unsigned char *p;
    p = &m_data_rgb[(y * m_width + x) * 3];
    *p++ = (color24 >> 16) & 0xff;
    *p++ = (color24 >> 8) & 0xff;
    *p++ = color24 & 0xff;
    m_data_alpha[y * m_width + x] = 0x80;
  }
};