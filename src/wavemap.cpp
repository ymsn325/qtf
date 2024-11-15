#include "wavemap.hpp"

#include <cmath>
#include <iostream>

using namespace std;

Wavemap::Wavemap(string id, Sound *sound, int width, int height, int color_bg,
                 int color_ax, int color_tick, int color_wave)
    : Pixmap(id, sound, width, height) {
  m_lower = 0.0;
  m_upper = 0.0;
  m_color_bg = color_bg;
  m_color_ax = color_ax;
  m_color_tick = color_tick;
  m_color_wave = color_wave;
  parent_sound = sound;
  parent_sounds = NULL;
  modify(parent_sound->n_bgn(), parent_sound->n_end());
}

bool Wavemap::modify(int n_bgn, int n_end) {
  int color_bg_final, color_ax_final, color_tick_final, color_wave_final;
  double lower_new, upper_new;
  double original_n_samples;
  int y, y1, y2, n1, n2;
  double min, max;

  if (m_flag_reverse) {
    color_bg_final = m_color_wave;
    color_ax_final = LIGHT_BLUE;
    color_tick_final = m_color_tick;
    color_wave_final = m_color_bg;
  } else {
    color_bg_final = m_color_bg;
    color_ax_final = m_color_ax;
    color_tick_final = m_color_tick;
    color_wave_final = m_color_wave;
  }

  if (m_flag_bw) {
    cerr << "Not implemented yet" << endl;
  }

  upper_new = parent_sound->max_abs() * WAVEMAP_PLOT_RATIO;
  lower_new = -upper_new;

  if (lower_new == upper_new) {
    lower_new -= 1.0;
    upper_new += 1.0;
  }

  m_upper = upper_new;
  m_lower = lower_new;

  original_n_samples = parent_sound->n_end() - parent_sound->n_bgn();

  draw_background(color_bg_final);

  y = m_height - int(round((-m_lower / (m_upper - m_lower)) * (m_height - 1))) -
      1;
  if (y >= 0 && y < m_height) {
    for (int x = 0; x < m_width; x++) {
      put_rgb(x, y, color_ax_final);
    }
  }

  for (int x = 0; x < m_width; x++) {
    n1 = int((original_n_samples * x) / m_width);
    n2 = int((original_n_samples * (x + 1)) / m_width);
    if (n2 == int(original_n_samples)) {
      n2--;
    }
    min = MAX_DBL;
    max = -MAX_DBL;
    for (int n = n1; n <= n2; n++) {
      if (parent_sound->data()[n] < min) {
        min = parent_sound->data()[n];
      }
      if (parent_sound->data()[n] > max) {
        max = parent_sound->data()[n];
      }
    }
    y1 = min2(m_height - 1,
              m_height -
                  int(round(((min - m_lower) / (m_upper - m_lower)) *
                            (m_height - 1))) -
                  1);
    y2 = max2(0, m_height -
                     int(round(((max - m_lower) / (m_upper - m_lower)) *
                               (m_height - 1))) -
                     1);
    for (int y = y2; y <= y1; y++) {
      put_rgb(x, y, color_wave_final);
    }
  }

  return true;
}
