#include "sound.hpp"

#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>

#include "standard.hpp"

using namespace std;

int g_default_total_ch = 1;
extern string g_program_name;

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

    // +区切りの前までがファイル名とch。これをfile_name_chに格納
    if ((pos = file_name_all.find("+|+")) != string::npos) {
      file_name_ch = file_name_all.substr(0, pos);
      file_name_all.erase(0, pos + 3);
    } else {
      file_name_ch = file_name_all;
      file_name_all = "";
    }

    // wavemap_lowerの記述があれば、先にそれを処理
    wavemap_lower.push_back(MAX_DBL);

    // wavemap_upperの記述があれば、先にそれを処理
    if (wavemap_lower.back() < 0.0) {
      wavemap_upper.push_back(-wavemap_lower.back());
    } else {
      wavemap_upper.push_back(MAX_DBL);
    }

    // pixmap生成記述があれば、先にそれを処理
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

    // :区切りの前までがファイル名。これらをfile_nameとchannelにpush_back
    if (file_name_ch.rfind(":") == file_name_ch.size() - 1) {
      if ((pos2 = file_name_ch.rfind(":")) == pos) {
        file_name.push_back(file_name_ch.substr(0, pos));
        if ((pos3 = file_name_ch.rfind(".")) != string::npos && pos3 > pos) {
          force_total_ch = stoi(
              file_name_ch.substr(pos3 + 1, file_name_ch.size() - pos3 - 1));
          if (pos3 == pos + 1) {
            // file_name:.2の形式
            ch_cur = 1;
          } else {
            ch_cur = stoi(file_name_ch.substr(pos + 1, pos3 - pos - 1));
          }
        } else {
          ch_cur =
              stoi(file_name_ch.substr(pos + 1, file_name_ch.size() - pos - 1));
        }
        channel.push_back(ch_cur);
      } else {
        // file_name.wav:*.*の形式
        if (file_name_ch.at(file_name_ch.size() - 2) == 's') {
          // filename.wav:3.2sの形式
          ss.str("");
          ss.clear();
          ss << file_name_ch.substr(pos2 + 1, file_name_ch.size() - pos2 - 2);
          ss >> length;
          samples = int(round(length * fs));
          file_name.push_back(int2string(samples) + ":" +
                              file_name_ch.substr(0, pos));
        } else {
          // filename.wav:*:3.2sの形式
          samples = string2int(
              file_name_ch.substr(pos2 + 1, file_name_ch.size() - pos2 - 2));
          file_name.push_back(
              file_name_ch.substr(pos2 + 1, file_name_ch.size() - pos2 - 1) +
              ":" + file_name_ch.substr(0, pos));
        }

        if (pos + 1 == pos2) {
          // file_name::88200の形式
          channel.push_back(-1);
        } else {
          // file_name:1:88200の形式
          if ((pos3 = file_name_ch.find("."), pos) != string::npos &&
              pos3 > pos && pos3 < pos2) {
            if (pos3 == pos + 1) {
              // file_name:.2:*の形式
              ch_cur = -1;
            } else {
              ch_cur = stoi(file_name_ch.substr(pos + 1, pos3 - pos - 1));
            }
          } else {
            ch_cur = stoi(file_name_ch.substr(pos + 1, pos2 - pos - 1));
          }
          channel.push_back(ch_cur);
        }
      }
    } else {
      file_name.push_back(file_name_ch);
      channel.push_back(-1);
    }

    ch = g_default_total_ch;
    fs_file = 0.0;
    read_audio_file_header(file_name.back(), &fs, &ch, &n_samples);
  }
}

void read_audio_file_header(string file_name, double *fs, int *ch,
                            int *n_samples) {
  FILE *fp;
  bool flag_float;
  long file_size;
  int bias;  // file_name:1:44100の44100
  int bitdepth;

  *n_samples = 0;

  read_audio_file_data(file_name, &fp, fs, ch, n_samples, &bias, &bitdepth,
                       &flag_float);
}

void read_audio_file_data(string file_name, FILE **fp, double *fs, int *ch,
                          int *n_samples, int *bias, int *bitdepth,
                          bool *flag_float) {
  int n, m, m2, m3;
  int buf4;
  short buf2;
  int n_bytes;
  int format_id;
  int pos;
  char tag[4];

  *n_samples = 0;
  *bias = 0;

  while ((pos = file_name.find("@$@")) != string::npos) {
    file_name.replace(pos, 3, " ");
  }

  while ((pos = file_name.find("+++")) != string::npos) {
    file_name.replace(pos, 3, " ");
  }

  if ((*fp = fopen(file_name.c_str(), "rb")) == nullptr) {
    cerr << g_program_name << ": " << "Error: file \"" << file_name
         << "\" cannot be opened." << endl;
    exit(0);
  }

  if (!(n = fread(tag, 1, 4, *fp))) {
    audio_read_err(file_name);
  }
  if (strncmp(tag, "RIFF", 4)) {
    audio_read_err(file_name);
  }

  fseek(*fp, 4, SEEK_CUR);  // ファイルサイズを読み飛ばす

  if (!(n = fread(tag, 1, 4, *fp))) {
    audio_read_err(file_name);
  }
  if (strncmp(tag, "WAVE", 4)) {
    audio_read_err(file_name);
  }

  while (true) {
    if (!(n = fread(tag, 1, 4, *fp))) {
      audio_read_err(file_name);
    }

    if (!strncmp(tag, "fmt ", 4)) {
      // この部分のバイト数
      if (!(n = fread(&n_bytes, 4, 1, *fp))) {
        audio_read_err(file_name);
      }

      if (!(n = fread(&buf2, 2, 1, *fp))) {
        audio_read_err(file_name);
      }
      format_id = buf2;
      if (format_id == 3) {
        *flag_float = true;
      }

      if (!(n = fread(&buf2, 2, 1, *fp))) {
        audio_read_err(file_name);
      }
      *ch = buf2;

      if (!(n = fread(&buf4, 4, 1, *fp))) {
        audio_read_err(file_name);
      }
      *fs = buf4;

      fseek(*fp, 6, SEEK_CUR);

      if (!(n = fread(&buf2, 2, 1, *fp))) {
        audio_read_err(file_name);
      }
      *bitdepth = buf2;

      fseek(*fp, n_bytes - 16, SEEK_CUR);
    } else if (!strncmp(tag, "data", 4)) {
      if (!(n = fread(&buf4, 4, 1, *fp))) {
        audio_read_err(file_name);
      }
      *n_samples = buf4;
      break;
    } else {
      if (!(n = fread(&buf4, 4, 1, *fp))) {
        audio_read_err(file_name);
      }
      fseek(*fp, n_bytes, SEEK_CUR);
    }
  }

  if (*n_samples != 0) {
    *n_samples -= *bias;
  }
}

void audio_read_err(string file_name) {
  cerr << g_program_name << ": " << "Error: Input file = \"" << file_name
       << "\"\a" << endl;
  exit(1);
}