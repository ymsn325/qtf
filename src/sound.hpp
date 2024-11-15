#pragma once

#include "sobj.hpp"

using namespace std;

extern int g_default_total_ch;
class Sound : public Sobj {
 private:
  double m_min, m_max;
  double m_max_abs;
  double m_ave;
  double m_pow;
  double m_gain;
  double m_gain_dB;
  bool m_flag_param;
  string m_file_name;
  int m_n_samples;
  int m_selected_ch;
  int m_total_ch;
  Sound *m_paremt_sound;
  double m_fs;
  int m_n_bgn, m_n_end;
  int m_in_ch, m_out_ch;
  int m_proc_in_ch, m_proc_out_ch;
  double *m_data_margin;
  double *m_data;

 public:
  Sound(string id);
  Sound(string id, string file_name, double fs = 0.0, int n_bgn = 0,
        int n_end = 0, int selected_ch = 0, int total_ch = g_default_total_ch,
        bool read_data = true);
  double fs() { return m_fs; }
  int n_bgn() { return m_n_bgn; }
  int n_end() { return m_n_end; }
  double *data_margin() { return m_data_margin; }
  double *data() { return m_data; }
  double max_abs() {
    if (!m_flag_param) {
      calculate_param();
    }
    return m_max_abs;
  }
  void set_file_name(string file_name) { m_file_name = file_name; }
  void set_fs(double fs) { m_fs = fs; }
  void set_n_bgn(int n_bgn) { m_n_bgn = n_bgn; }
  void set_n_end(int n_end) { m_n_end = n_end; }
  void set_selected_ch(int selected_ch) { m_selected_ch = selected_ch; }
  void set_total_ch(int total_ch) { m_total_ch = total_ch; }
  void set_data_margin(double *data_margin) { m_data_margin = data_margin; }
  void set_data(double *data) { m_data = data; }
  void set_n_samples(int n_samples) { m_n_samples = n_samples; }
  void set_in_ch(int in_ch) { m_in_ch = in_ch; }
  void read_file();
  void calculate_param();
};

class Sounds : public Sobj {
 private:
  int m_ch;
  vector<Sound *> m_sound_list;

 public:
  Sounds(string id, string file_name, double fs = 0.0, int n_bgn = 0,
         int n_end = 0, int ch = g_default_total_ch);
  vector<Sound *> &sound_list() { return m_sound_list; }
};

void analyze_file_name(string file_name_all, vector<string> &file_name,
                       vector<int> &channel, vector<int> &total_channel,
                       vector<vector<string>> &pixmap_cmd, double &fs,
                       double &fs_file, int &n_samples, int &n_sounds,
                       vector<double> &wavemap_lower,
                       vector<double> &wavemap_upper);
void read_audio_file_header(string file_name, double *fs, int *ch,
                            int *n_samples);
void read_audio_file_data(string file_name, FILE **fp, double *fs, int *ch,
                          int *n_samples, int *bias, int *bitdepth,
                          bool *flag_float);
void audio_read_err(string file_name);
void init_fftw(int n_fft);
void strip_suffix_from_file_name(string &file_name);
void strip_dir_from_file_name(string &file_name);
bool find_string(vector<string> str_list, unsigned int i, string key,
                 int &height, int &rest);