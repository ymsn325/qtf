#include "sound.hpp"

#include "standard.hpp"

int g_default_total_ch = 1;

Sound::Sound(string id) : Sobj(id) {
  m_min = MAX_DBL;
  m_max = -MAX_DBL;
  m_max_abs = -MAX_DBL;
  m_ave = -MAX_DBL;
  m_pow = -MAX_DBL;
  m_gain = 1.0;
  m_gain_dB = 0.0;
  m_flag_param = false;
  m_file_name = "";
  m_n_samples = 0;
  m_selected_ch = 0;
  m_total_ch = g_default_total_ch;
  m_paremt_sound = nullptr;
  m_fs = 0.0;
  m_n_bgn = 0;
  m_n_end = 0;
  m_in_ch = 0;
  m_out_ch = 0;
  m_proc_in_ch = 0;
  m_proc_out_ch = 0;
  m_data_margin = nullptr;
  m_data = nullptr;
}

Sound::Sound(string id, string file_name, double fs, int n_bgn, int n_end,
             int selected_ch, int total_ch, bool read_data)
    : Sobj(id),
      m_file_name(file_name),
      m_fs(fs),
      m_n_bgn(n_bgn),
      m_n_end(n_end),
      m_selected_ch(selected_ch),
      m_total_ch(total_ch) {
  m_min = MAX_DBL;
  m_max = -MAX_DBL;
  m_max_abs = -MAX_DBL;
  m_ave = -MAX_DBL;
  m_pow = -MAX_DBL;
  m_gain = 1.0;
  m_gain_dB = 0.0;
  m_flag_param = false;
  m_paremt_sound = nullptr;
  m_in_ch = 0;
  m_out_ch = 0;
  m_proc_in_ch = 0;
  m_proc_out_ch = 0;
  m_data_margin = nullptr;
  m_data = nullptr;

  if (read_data) {
    // Read data from file
  }
}

void analyze_file_name(string file_name_all, vector<string> &file_name,
                       vector<int> &channel, vector<int> &total_channel,
                       vector<vector<string>> &pixmap_cmd, double &fs,
                       double &fs_file, int &n_samples, int &n_sounds,
                       vector<double> &wavemap_lower,
                       vector<double> &wavemap_upper) {
  stringstream ss;
  string file_name_ch;
  size_t pos, pos2, pos3;
  int ch_cur, ch, samples, file_num, force_total_ch;
  double length;

  n_sounds = 0;
  file_num = 0;

  while (file_name_all.size() != 0) {
    force_total_ch = 0;
    samples = 0;

    if ((pos = file_name_all.find("+|+")) != string::npos) {
      file_name_ch = file_name_all.substr(0, pos);
      file_name_all.erase(0, pos + 3);
    } else {
      file_name_ch = file_name_all;
      file_name_all = "";
    }

    wavemap_lower.push_back(MAX_DBL);

    if (wavemap_lower.back() < 0.0) {
      wavemap_upper.push_back(-wavemap_lower.back());
    } else {
      wavemap_upper.push_back(MAX_DBL);
    }

    pixmap_cmd.resize(file_num + 1);
    if ((pos = file_name_ch.find(":")) != string::npos &&
        (pos2 = file_name_ch.rfind("_")) != string::npos && pos < pos2) {
      do {
        pixmap_cmd[file_num].push_back(
            file_name_ch.substr(pos2 + 1, file_name_ch.size() - pos2));
        file_name_ch.resize(pos2);
      } while ((pos2 = file_name_ch.rfind("_")) != string::npos && pos < pos2);
    } else {
      pixmap_cmd[file_num].push_back("t");
      pixmap_cmd[file_num].push_back("w");
    }

    if (file_name_ch.rfind(":") == file_name_ch.size() - 1) {
      file_name_ch.resize(file_name_ch.size() - 1);
    }
  }
}