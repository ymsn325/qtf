#include <string>
#include <vector>

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