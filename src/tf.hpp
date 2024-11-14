#pragma once

#include <fftw3.h>

#include <complex>

#include "shape.hpp"
#include "sobj.hpp"
#include "sound.hpp"

class TF : public Sobj {
 private:
  bool m_flag_param;
  double m_norm_min, m_norm_max;
  int m_norm_max_k, m_norm_max_n;
  string file_name;
  Sound *m_parent_sound;
  Shape *m_parent_shape;
  double m_fs;
  int m_k_bgn, m_k_end, m_k_step, m_k_size;
  int m_n_bgn, m_n_end, m_n_step, m_n_size;
  int m_n_fft;
  fftw_complex *m_fftw_in, *m_fftw_out;
  fftw_plan *m_plan_fftw;
  complex<double> **m_data;

 public:
  TF(string id, Sound *sound, Shape *shape, fftw_plan *plan_fftw,
     fftw_complex *fftw_in, fftw_complex *fftw_out, int n_fft, int step_fft,
     int k_bgn, int k_end);
  ~TF();
  bool modify(int n_bgn, int n_end);
  void calculate_param();
  double norm_min() { return m_norm_min; }
  double norm_max() { return m_norm_max; }
  int norm_max_k() { return m_norm_max_k; }
  int norm_max_n() { return m_norm_max_n; }
  int n_bgn() { return m_n_bgn; }
  int n_end() { return m_n_end; }
  int n_step() { return m_n_step; }
  int n_size() { return m_n_size; }
  int k_bgn() { return m_k_bgn; }
  int k_end() { return m_k_end; }
  complex<double> **data() { return m_data; }
};