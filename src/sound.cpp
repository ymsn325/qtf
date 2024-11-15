#include "sound.hpp"

#include <fftw3.h>

#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>

#include "standard.hpp"

using namespace std;

int g_default_total_ch = 1;
static const int g_margin = 6 * 4096;

fftw_complex *g_fftw_in, *g_fftw_out;
fftw_plan g_plan_fftw;

extern string g_program_name;
extern string g_verbose_mode;

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
    read_file();
  }
}

Sounds::Sounds(string id, string file_name, double fs, int n_bgn, int n_end,
               int ch)
    : Sobj(id), m_ch(ch) {
  FILE *fp;
  double fs_dummy;
  int n_samples;
  int bias;
  int bitdepth;
  bool flag_float;
  char ch_name[11];
  Sound *sound;
  int n, m, m2, m3, p;

  read_audio_file_data(file_name, &fp, &fs_dummy, &m_ch, &n_samples, &bias,
                       &bitdepth, &flag_float);

  if (verbose('d')) {
    cerr << "Read file: ";
    if (fs_dummy > 0.0) {
      cerr << "fs = " << fs_dummy << " Hz, ";
    }
    cerr << "bit depth = " << bitdepth << " bit, ";
    if (flag_float) {
      cerr << "(float)" << endl;
    } else {
      cerr << endl;
    }
  }

  for (int c = 0; c < m_ch; c++) {
    sprintf(ch_name, "%d", c);
    sound = new Sound(id + "_ch" + ch_name);
    sound->set_file_name(file_name);
    sound->set_fs(fs);
    sound->set_n_bgn(n_bgn);
    sound->set_n_end(n_end);
    sound->set_selected_ch(c);
    sound->set_total_ch(m_ch);
    sound->set_n_samples(n_samples);
    sound->set_in_ch(c + 1);
    sound->set_data_margin(new double[n_end - n_bgn + 2 * g_margin]);
    sound->set_data(sound->data_margin() + g_margin);
    m_sound_list.push_back(sound);
  }

  for (int i = 0; i < (n_bgn + bias - g_margin) * m_ch; i++) {
    // 先頭の不要な部分を読み飛ばす
    if ((n = getc(fp)) == EOF || (m = getc(fp)) == EOF) {
      cerr << "Error: " << file_name << " is too short." << endl;
      exit(1);
    }

    if (bitdepth > 16) {
      if ((m = getc(fp)) == EOF) {
        break;
      }
    }
    if (bitdepth > 24) {
      if ((m = getc(fp)) == EOF) {
        break;
      }
    }
  }

  p = 0;
  for (int i = 0; i < g_margin - (n_bgn)-bias; i++) {
    for (int c = 0; c < m_ch; c++) {
      m_sound_list[c]->data_margin()[p] = 0.0;
    }
    p++;
  }

  if (flag_float) {
  } else {
    for (int i = max2(n_bgn - g_margin + bias, 0);
         p < n_end - n_bgn + 2 * g_margin && i < n_samples + bias; i++, p++) {
      for (int c = 0; c < m_ch; c++) {
        if (bitdepth > 24) {
          if ((m3 = getc(fp)) == EOF) {
            break;
          }
        } else {
          m3 = 0;
        }
        if (bitdepth > 16) {
          if ((m2 = getc(fp)) == EOF) {
            break;
          }
        } else {
          m2 = 0;
        }
        if ((m = getc(fp)) == EOF) {
          break;
        }
        if ((n = getc(fp)) == EOF) {
          break;
        }
        n = (((((n << 8) | m) << 8) | m2) << 8) | m3;
        m_sound_list[c]->data_margin()[p] = (double)(n >> 16);
      }
    }
  }

  fclose(fp);
}

void Sound::read_file() {
  FILE *fp;
  int ch;
  double fs_dummy;
  int n_samples;
  int bias;
  int bitdepth;
  bool flag_float;
  int n, m, m2, m3;
  int p;

  m_data_margin = new double[m_n_end - m_n_bgn + 2 * g_margin];
  m_data = m_data_margin + g_margin;
  ch = m_total_ch;
  read_audio_file_data(m_file_name, &fp, &fs_dummy, &ch, &n_samples, &bias,
                       &bitdepth, &flag_float);
  if (verbose('d')) {
    cerr << "Read file: ";
    if (fs_dummy > 0.0) {
      cerr << "fs = " << fs_dummy << " Hz, ";
    }
    cerr << "bit depth = " << bitdepth << " bit, ";
    if (flag_float) {
      cerr << "(float)" << endl;
    } else {
      cerr << endl;
    }
  }
  m_total_ch = ch;
  if (n_samples != 0) {
    m_n_samples = n_samples - m_n_bgn - m_n_end;
  } else {
    m_n_samples = m_n_end;
  }

  for (int i = 0; i < (m_n_bgn + bias - g_margin) * ch * bitdepth / 8; i++) {
    // 先頭の不要な部分を読み飛ばす
    if ((n = getc(fp)) == EOF) {
      cerr << "Error: " << m_file_name << " is too short." << endl;
      exit(1);
    }
  }

  if (m_selected_ch >= ch) {
    cerr << "Error: " << m_file_name << " has only " << ch << " channels. ";
    cerr << " But " << m_selected_ch << " channel was specified." << endl;
    exit(1);
  }

  if (m_total_ch > ch) {
    cerr << "Error: " << m_file_name << " has only " << ch << " channels. ";
    cerr << "But " << m_total_ch << " channels were specified." << endl;
    exit(1);
  } else if (ch > m_total_ch) {
    cerr << "Error: " << m_file_name << " has only " << ch << " channels. ";
    cerr << "Read as " << m_total_ch << " channels." << endl;
  }

  p = 0;
  for (int i = 0; i < g_margin - (m_n_bgn)-bias; i++) {
    m_data_margin[p++] = 0.0;
  }

  if (flag_float) {
  } else {
    for (int i = max2(m_n_bgn - g_margin + bias, 0);
         p < m_n_samples + 2 * g_margin && i < n_samples + bias; i++, p++) {
      if (bitdepth > 24) {
        if ((m3 = getc(fp)) == EOF) {
          break;
        }
      } else {
        m3 = 0;
      }
      if (bitdepth > 16) {
        if ((m2 = getc(fp)) == EOF) {
          break;
        }
      } else {
        m2 = 0;
      }
      if ((m = getc(fp)) == EOF) {
        break;
      }
      if ((n = getc(fp)) == EOF) {
        break;
      }
      n = ((((n << 8) | m) << 8) | m2) << 8 | m3;
      m_data_margin[p] = n >> 16;
    }
  }

  for (; p < m_n_end - m_n_bgn + 2 * g_margin; p++) {
    m_data_margin[p] = 0.0;
  }

  fclose(fp);
}

void Sound::calculate_param() {
  if (!m_flag_param) {
    m_max_abs = -MAX_DBL;
    m_min = MAX_DBL;
    m_max = -MAX_DBL;
    m_ave = 0.0;
    m_pow = 0.0;
    for (int i = 0; i < m_n_end - m_n_bgn; i++) {
      if (m_data[i] > m_max) {
        m_max = m_data[i];
      }
      if (m_data[i] < m_min) {
        m_min = m_data[i];
      }
      m_ave += m_data[i];
      m_pow += m_data[i] * m_data[i];
      if (fabs(m_data[i]) > m_max_abs) {
        m_max_abs = fabs(m_data[i]);
      }
    }
    m_ave /= m_n_end - m_n_bgn;
    m_pow /= m_n_end - m_n_bgn;
    m_max_abs = max2(m_max_abs, 1.0);
    m_flag_param = true;
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

    if (fs_file == 0.0) {
      fs_file = fs;
    } else if (fs_file != fs) {
      cerr << "fs: " << fs_file << " was adopted. But fs: " << fs
           << " was detected.\a" << endl;
    }

    if (force_total_ch != 0) {
      n_samples = (n_samples + samples) * ch / force_total_ch - samples;
      ch = force_total_ch;
    }

    total_channel.push_back(ch);
    if (channel.back() == -1) {
      // 指定チャネルがなくて
      if (ch == 1) {
        // ソースが1 chなら
        channel.back() = 0;  // 指定は0 ch(最初のch)とする
        n_sounds++;
      } else {
        // ソースが多chなら
        n_sounds += ch;  // n_soundsは全チャネル分増加。channelは-1のまま
      }
    } else {
      n_sounds++;
    }
    file_num++;
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
  if (*ch == 0) {
    *ch = g_default_total_ch;
  }
  if (*fs == 0.0) {
    *fs = 44100.0;
  }
  if (fp != nullptr) {
    if (*n_samples == 0) {
      file_size = ftell(fp);
      fseek(fp, 0, SEEK_END);
      file_size = ftell(fp) - file_size;
      *n_samples = file_size / ((bitdepth / 8) * *ch) - bias;
    }
    fclose(fp);
  }
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
      *n_samples = buf4 / ((*bitdepth / 8) * *ch);
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

void init_fftw(int n_fft) {
  g_fftw_in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n_fft);
  g_fftw_out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n_fft);
  g_plan_fftw = fftw_plan_dft_1d(n_fft, g_fftw_in, g_fftw_out, FFTW_FORWARD,
                                 FFTW_MEASURE);
}

void strip_suffix_from_file_name(string &file_name) {
  if (file_name.size() > 4 &&
      (file_name.substr(file_name.size() - 4) == ".wav" ||
       file_name.substr(file_name.size() - 4) == ".WAV")) {
    file_name.resize(file_name.size() - 4);
  }
}

void strip_dir_from_file_name(string &file_name) {
  size_t pos;

  if ((pos = file_name.rfind("/")) != string::npos) {
    file_name.substr(pos + 1, file_name.size() - pos - 1);
  }
}

bool find_string(vector<string> str_list, unsigned int i, string key,
                 int &height, int &rest) {
  size_t pos, pos2;

  if (str_list[i].find(key) != string::npos) {
    if ((pos = str_list[i].find(":")) != string::npos) {
      pos2 = str_list[i].rfind(":");
      if (pos + 1 < pos2) {
        // key*:110:abcの形式
        height = string2int(str_list[i].substr(pos + 1, pos2 - pos - 1));
        if (pos2 < str_list[i].size() - 1) {
          rest = string2int(
              str_list[i].substr(pos2 + 1, str_list[i].size() - pos2 - 1));
        }
      } else if (pos + 1 == pos2) {
        // key*::abcの形式
        if (pos2 < str_list[i].size() - 1) {
          rest = string2int(
              str_list[i].substr(pos2 + 1, str_list[i].size() - pos2 - 1));
        }
      } else {
        // key*:110の形式
        if (pos < str_list[i].size() - 1) {
          height = string2int(
              str_list[i].substr(pos + 1, str_list[i].size() - pos - 1));
        }
      }
    }
    return true;
  }
  return false;
}