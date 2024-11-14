#include "tf.hpp"

#include <iostream>

#include "standard.hpp"

using namespace std;

TF::TF(string id, Sound *sound, Shape *shape, fftw_plan *plan_fftw,
       fftw_complex *fftw_in, fftw_complex *fftw_out, int n_fft, int step_fft,
       int k_bgn, int k_end)
    : Sobj(id, sound, shape) {
  m_parent_sound = sound;
  m_parent_shape = shape;
  m_n_fft = n_fft;
  m_fs = sound->fs();
  m_n_bgn = sound->n_bgn();
  m_n_end = sound->n_end();
  m_n_step = step_fft;
  m_n_size = (sound->n_end() - (sound->n_bgn() - 1)) / step_fft + 1;
  m_plan_fftw = plan_fftw;
  m_fftw_in = fftw_in;
  m_fftw_out = fftw_out;
  m_k_bgn = k_bgn;
  m_k_end = k_end;
  m_k_step = 1;
  m_k_size = k_end - k_bgn;

  m_data = new complex<double> *[m_n_size];
  for (int i = 0; i < m_n_size; i++) {
    m_data[i] = new complex<double>[m_k_end - m_k_bgn];
  }

  modify(m_n_bgn, m_n_end);
}

TF::~TF() {
  for (int i = 0; i < m_n_size; i++) {
    delete[] m_data[i];
  }
  delete[] m_data;
}

bool TF::modify(int n_bgn, int n_end) {
  int n;
  bool flag_modify = false;
  int n_shape_1 = m_parent_shape->n() - 1;
  double *sound_data = m_parent_sound->data();
  double *shape_data = m_parent_shape->data();

  if (n_bgn == 0 && n_end == 0) {
    n_bgn = m_n_bgn;
    n_end = m_n_end;
  }

  if (n_shape_1 * 2 + 1 > m_n_fft) {
    cerr << "Error: n_shape = " << m_parent_shape->n()
         << " is too large for n_fft = " << m_n_fft << endl;
    cerr << "This tf data is not correct" << endl;
    n_shape_1 = m_n_fft / 2;
  }

  for (int i = 0; i < m_n_size; i++) {
    if (m_n_bgn + i * m_n_step + n_shape_1 >= n_bgn &&
        m_n_bgn + i * m_n_step - n_shape_1 < n_end) {
      flag_modify = true;
      m_flag_param = false;
      if (m_parent_shape->type() == 'H' || m_parent_shape->type() == 'h') {
        cerr << "Not implemented yet" << endl;
      } else if (m_parent_shape->type() == 'S' ||
                 m_parent_shape->type() == 's') {
        cerr << "Not implemented yet" << endl;
      } else {
        for (n = 0; n <= n_shape_1; n++) {
          m_fftw_in[n][0] =
              sound_data[i * m_n_step + n] * shape_data[n_shape_1 - n];
          m_fftw_in[n][1] = 0.0;
        }
      }

      for (; n < m_n_fft - n_shape_1; n++) {
        m_fftw_in[n][0] = 0.0;
        m_fftw_in[n][1] = 0.0;
      }

      for (int n2 = 0; n < m_n_fft; n++, n2++) {
        m_fftw_in[n][0] =
            sound_data[i * m_n_step - n_shape_1 + n2] * shape_data[n2];
        m_fftw_in[n][1] = 0.0;
      }

      fftw_execute(*m_plan_fftw);

      for (int k = m_k_bgn; k < m_k_end; k++) {
        m_data[i][k - m_k_bgn] =
            complex<double>(m_fftw_out[k][0], m_fftw_out[k][1]);
      }
    }
  }

  return flag_modify;
}

void TF::calculate_param() {
  double x;
  m_norm_min = MAX_DBL;
  m_norm_max = -MAX_DBL;

  for (int i = 0; i < m_n_size; i++) {
    for (int k = 0; k < m_k_end - m_k_bgn; k++) {
      x = norm(m_data[i][k]);
      if (x > m_norm_max) {
        m_norm_max = x;
        m_norm_max_k = k;
        m_norm_max_n = i;
      }
      if (x < m_norm_min) {
        m_norm_min = x;
      }
    }
  }

  m_flag_param = true;
}