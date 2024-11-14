#include "tfmap.hpp"

#include <iostream>

#include "image.hpp"

using namespace std;

TFmap::TFmap(string id, TF *tf, int k_bgn, int k_end, int height, int n_dup,
             double contrast_min, double contrast_max, double limit_lower,
             double limit_upper)
    : Pixmap(id, tf, tf->n_size() * n_dup, height) {
  bool adj_flag = false;

  m_n_bgn = tf->n_bgn();
  m_n_end = tf->n_end();
  m_n_step = tf->n_step();
  m_n_size = tf->n_size();
  m_k_bgn = k_bgn;
  m_k_size = height;
  m_k_step = int(ceil(k_end - k_bgn - 1.0) / (height - 1.0));
  m_n_dup = n_dup;

  while (k_bgn + m_k_step * (height - 1) >= tf->k_end()) {
    m_k_step--;
  }

  while (k_bgn + m_k_step * (height - 1) < tf->k_end()) {
    m_k_step--;
    adj_flag = true;
  }

  if (adj_flag && m_k_step != 0) {
    m_k_step++;
  }

  m_k_end = k_bgn + m_k_step * height;
  if (m_k_step == 0) {
    m_k_step = 1;
    m_k_dup = 2;

    while (k_bgn + m_k_step * ((height + m_k_dup - 1) / m_k_dup - 1) >=
               tf->k_end() ||
           k_bgn + m_k_step * ((height + m_k_dup - 2) / m_k_dup - 2) >= k_end) {
      m_k_dup++;
    }
    m_k_end = k_bgn + m_k_step * ((height + m_k_dup - 1) / m_k_dup);
  } else {
    m_k_dup = 1;
  }

  m_contrast_min = contrast_min;
  m_contrast_max = contrast_max;
  m_limit_lower = limit_lower;
  m_limit_upper = limit_upper;
  m_pow = new double[m_width * m_height];

  modify(m_n_bgn, m_n_end);
}

TFmap::~TFmap() { delete[] m_pow; }

bool TFmap::modify(int n_bgn, int n_end) {
  double lower, upper;
  double x;
  bool zero_flag;
  int pix, pix2;

  if (m_parent_tf->norm_max() != 0.0) {
    upper = m_limit_upper;
    lower = m_limit_lower;
    zero_flag = false;
  } else {
    upper = 0.0;
    lower = 0.0;
    zero_flag = true;
  }

  pix = pix2 = 0;
  if (!zero_flag) {
    if (m_k_dup == 1) {
      for (int k = m_k_end - m_k_step; k >= m_k_bgn; k -= m_k_step) {
        for (int i = 0; i < m_n_size; i++) {
          m_pow[pix] =
              10.0 *
              (log10(norm(m_parent_tf->data()[i][k - m_parent_tf->k_bgn()])) -
               lower) /
              (upper - lower);
          x = (m_pow[pix] - m_contrast_min) / (m_contrast_max - m_contrast_min);
          if (m_flag_reverse) {
            x = 1.0 - x;
          }
          for (int j = 0; j < m_n_dup; j++) {
            if (m_flag_bw) {
              double2rgb_bw(x, m_data_rgb + pix * 3, m_data_rgb + pix * 3 + 1,
                            m_data_rgb + pix * 3 + 2);
            } else {
              double2rgb(x, m_data_rgb + pix * 3, m_data_rgb + pix * 3 + 1,
                         m_data_rgb + pix * 3 + 2);
            }
            pix++;
          }
        }
      }
    } else {
      cerr << "Not implemented yet." << endl;
    }
  } else {
    if (m_k_dup == 1) {
      for (int k = m_k_end - m_k_step; k >= m_k_bgn; k -= m_k_step) {
        for (int i = 0; i < m_n_size * m_n_dup; i++) {
          *(m_data_rgb + pix * 3) = 0;
          *(m_data_rgb + pix * 3 + 1) = 0;
          *(m_data_rgb + pix * 3 + 2) = 0;
          pix++;
        }
      }
    } else {
      cerr << "Not implemented yet." << endl;
    }
  }

  return true;
}
