#include "sobj.hpp"

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
};