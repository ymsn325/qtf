#include "pixmap.hpp"
#include "sound.hpp"
#include "standard.hpp"

#define WAVEMAP_PLOT_RATIO 1.02

class Wavemap : public Pixmap {
 private:
  double m_lower, m_upper;
  int m_color_bg, m_color_ax, m_color_tick, m_color_wave;
  Sound *parent_sound;
  Sounds *parent_sounds;

 public:
  Wavemap(string id, Sound *sound, int widht, int height,
          int color_bg = MAX_INT, int color_ax = MAX_INT,
          int color_tick = MAX_INT, int color_wave = MAX_INT);
  bool modify(int n_bgn, int n_end);
};