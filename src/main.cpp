#include <png.h>

#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <string>

#include "alsa_out.hpp"
#include "arg_srv.hpp"
#include "mainwindow.hpp"
#include "shape.hpp"
#include "sound.hpp"
#include "standard.hpp"
#include "tf.hpp"
#include "tfmap.hpp"
#include "wavemap.hpp"

using namespace std;

static const string DEFAULT_FILE_NAME =
    "/home/sy/audio/ichimoji_PF02_0501_033.wav";

extern int g_mainwindow_init_x;
extern int g_mainwindow_init_y;
extern int g_mainwindow_init_width;
extern int g_mainwindow_init_height;
extern string g_alsa_dev_default;
extern string g_alsa_dev;
extern int g_alsa_fragment_size;
extern int g_alsa_latency_default;
extern int g_alsa_latency;
extern int g_alsa_period;
extern fftw_complex *g_fftw_in, *g_fftw_out;
extern fftw_plan g_plan_fftw;

// デバッグ用の関数を追加
void printImageData(const unsigned char *data, int width, int height) {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int index = (y * width + x) * 3;  // RGBなので3倍
      std::cerr << "(" << (int)data[index] << ", " << (int)data[index + 1]
                << ", " << (int)data[index + 2] << ") ";
    }
    std::cerr << std::endl;
  }
}

int main(int argc, char **argv) {
  string file_name_all;
  vector<string> file_name;
  vector<int> channel, total_channel;
  vector<vector<string>> pixmap_cmd;
  int default_output_bitdepth;
  int n_samples;
  int n_sounds;
  double f_bgn, f_end, f_bgn_init, f_end_init, f_end_default = 44100.0;
  int n_bgn, n_end, k_bgn, k_end, k_bgn_init, k_end_init;
  vector<double> wavemap_lower, wavemap_upper;
  double fs, fs_file;
  char shape_type;
  int n_shape_min, n_shape_max, n_shape_num, init_shape;
  int n_fft, step_fft;
  double tfmap_limit_lower, tfmap_limit_upper;
  int mainwindow_init_x_default, mainwindow_init_y_default;
  int mainwindow_init_width_default, mainwindow_init_height_default;
  int tfmap_height, wavemap_height;
  int width;
  string file_id, files_id, ch_id;
  bool tf_construct_flag;
  int height, rest;
  vector<Sounds *> sounds;
  vector<Sound *> sound_list;
  Shapes *shapes;
  vector<TF *> tf_list;
  vector<TFmap *> tfmap_list;
  vector<Wavemap *> wavemap_list;

  QApplication app(argc, argv);

  ArgSrv arg_srv(argc, argv);
  arg_srv.set(&file_name_all, "file_name_all", "ファイル名", DEFAULT_FILE_NAME);
  arg_srv.range("1 256");
  arg_srv.set(&default_output_bitdepth, "default_output_bitdepth",
              "出力ビット深度", 16);
  fs_file = -1.0;
  arg_srv.range("");
  arg_srv.set(&fs, "fs", "サンプリング周波数", 44100.0);
  analyze_file_name(file_name_all, file_name, channel, total_channel,
                    pixmap_cmd, fs, fs_file, n_samples, n_sounds, wavemap_lower,
                    wavemap_upper);
  arg_srv.range("");
  arg_srv.set(&n_bgn, "n_bgn", "開始サンプル番号", 0);
  arg_srv.set(&n_end, "n_end", "終了サンプル番号", 0);
  arg_srv.range("1000.0 192000.0");
  if (fs_file != -1.0) {
    f_end_default = fs_file / 2.0;
  }
  arg_srv.range("");
  arg_srv.set(&f_bgn, "f_bgn", "取り扱い最低周波数", 0.0);
  arg_srv.set(&f_end, "f_end", "取り扱い最高周波数", f_end_default);
  arg_srv.set(&f_bgn_init, "f_bgn_init", "起動時の最低周波数", f_bgn);
  arg_srv.set(&f_end_init, "f_bgn_init", "起動時の最高周波数", f_end);
  arg_srv.enumerate("n m r b f c g t h H s S");
  arg_srv.set(&shape_type, "shape_type", "窓タイプ", 'g');
  arg_srv.enumerate("");
  arg_srv.range("1 4096");
  arg_srv.set(&n_shape_min, "n_shape_min", "最小の窓点数", 31);
  arg_srv.set(&n_shape_max, "n_shape_max", "最大の窓点数", 1023);
  arg_srv.range("1 16");
  arg_srv.set(&n_shape_num, "n_shape_num", "窓関数の種類数", 16);
  arg_srv.range("0 15");
  arg_srv.set(&init_shape, "init_shape", "何番の窓", 15);
  arg_srv.range("1 4096");
  arg_srv.set(&step_fft, "step_fft", "FFTシフト点数", 25);
  arg_srv.set(&n_fft, "n_fft", "FFT点数", 2048);
  arg_srv.range("-200.0 540.0");
  arg_srv.set(&tfmap_limit_lower, "tfmap_limit_lower",
              "tfmapでこのdB値を黒にする。lower = upperなら自動", 0.0);
  arg_srv.set(&tfmap_limit_upper, "tfmap_limit_lower",
              "tfmapでこのdB値を黒にする。lower = upperなら自動", 0.0);

  arg_srv.range("0 5120");
  mainwindow_init_x_default = MAINWINDOW_X;
  arg_srv.set(&g_mainwindow_init_x, "x", "起動時のx座標",
              mainwindow_init_x_default);
  arg_srv.range("0 10001");
  mainwindow_init_y_default = MAINWINDOW_Y;
  arg_srv.set(&g_mainwindow_init_y, "y", "起動時のy座標",
              mainwindow_init_y_default);
  arg_srv.range("2 10001");
  mainwindow_init_width_default = MAINWINDOW_WIDTH;
  arg_srv.set(&g_mainwindow_init_width, "w", "起動時の横方向の幅",
              mainwindow_init_width_default);
  arg_srv.range("0 10001");
  mainwindow_init_height_default = MAINWINDOW_HEIGHT;
  arg_srv.set(&g_mainwindow_init_height, "h", "起動時の縦方向の高さ",
              mainwindow_init_height_default);
  arg_srv.range("2 10001");
  arg_srv.set(&tfmap_height, "tfmap_height", "デフォルトのtfmapの高さ",
              TFMAP_HEIGHT);
  arg_srv.range("2 10001");
  arg_srv.set(&wavemap_height, "wavemap_height", "デフォルトのwavemapの高さ",
              WAVEMAP_HEIGHT);

  g_alsa_dev_default = ALSA_DEV_DEFAULT;
  arg_srv.range("");
  arg_srv.set(&g_alsa_dev, "alsa_dev", "ALSAのデバイス名", g_alsa_dev_default);
  g_alsa_latency_default = ALSA_LATENCY_DEFAULT;
  arg_srv.range("1 44100");
  arg_srv.set(&g_alsa_latency, "alsa_latency", "ALSAのレイテンシ",
              g_alsa_latency_default);
  arg_srv.range("10 22050");
  arg_srv.set(&g_alsa_fragment_size, "alsa_fragment_size",
              "ALSAのフラグメントサイズ", ALSA_FRAGMENT_SIZE);
  arg_srv.range("1 1000");
  arg_srv.set(&g_alsa_period, "alsa_period", "ALSAのコールバック間隔 [ms]",
              ALSA_PERIOD);

  arg_srv.finish();

  if (g_mainwindow_init_height == 0) {
    g_mainwindow_init_height = MAINWINDOW_HEIGHT;
  }

  if (n_shape_num - 1 < init_shape) {
    init_shape = n_shape_num - 1;
  }

  if (init_shape < 0) {
    init_shape = 0;
  }

  if (n_shape_min == n_shape_max) {
    n_shape_num = 1;
    init_shape = 0;
  }

  if (n_bgn > n_end) {
    cerr << "Negative data length. (n_bgn > n_end)" << endl;
    exit(1);
  }

  if (fs_file != -1.0 && fs_file != fs) {
    cerr << "fs was changed from " << fs << " Hz to " << fs_file << " Hz."
         << endl;
    fs = fs_file;
  }

  if (n_bgn == n_end) {
    n_bgn = 0;
    n_end = n_bgn + n_samples;
  }

  step_fft = int(ceil((n_end - n_bgn) / double(g_mainwindow_init_width)));
  k_bgn = int(round(f_bgn / (fs / n_fft)));
  if (k_bgn < 0) {
    k_bgn = 0;
  }
  k_end = int(round(f_end / (fs / n_fft)));
  if (k_end > n_fft / 2) {
    k_end = n_fft / 2;
  }
  k_bgn_init = int(round(f_bgn_init / (fs / n_fft)));
  if (k_bgn_init < k_bgn) {
    k_bgn_init = k_bgn;
  }
  k_end_init = int(round(f_end_init / (fs / n_fft)));
  if (k_end_init > k_end) {
    k_end_init = k_end;
  }

  width = g_mainwindow_init_width;
  if (width == 0) {
    width = (n_end - n_bgn - 1) / step_fft + 1;
  }

  init_fftw(n_fft);

  sounds.resize(file_name.size());
  sound_list.resize(n_sounds);

  shapes =
      new Shapes("shapes", shape_type, n_shape_min, n_shape_max, n_shape_num);

  for (int i = 0, j = 0; i < file_name.size(); i++) {
    file_id = file_name[i];
    strip_suffix_from_file_name(file_id);
    strip_dir_from_file_name(file_id);
    if (i != 0) {
      file_id += ", ";
    }
    files_id += file_id;
    sounds[i] = new Sounds("sounds_" + file_id, file_name[i], fs, n_bgn, n_end,
                           total_channel[i]);
    for (int k = 0; k < total_channel[i]; k++) {
      if (total_channel[i] == 2) {
        if (k == 0) {
          ch_id = ":L";
        } else {
          ch_id = ":R";
        }
      } else {
        ch_id = ":" + int2string(k);
      }
      sound_list[j] = sounds[i]->sound_list()[k];
      sound_list[j]->set_id("sound_" + file_id + ch_id);
      tf_construct_flag = false;

      for (int j2 = 0; j2 < pixmap_cmd[i].size(); j2++) {
        height = tfmap_height;
        rest = MAX_INT;
        if (find_string(pixmap_cmd[i], j2, "t", height, rest)) {
          if (!tf_construct_flag) {
            tf_list.push_back(new TF("tf_" + file_id + ch_id, sound_list[j],
                                     shapes->shape_list()[init_shape],
                                     &g_plan_fftw, g_fftw_in, g_fftw_out, n_fft,
                                     step_fft, k_bgn, k_end));
            tf_construct_flag = true;
          }

          tfmap_list.push_back(new TFmap(
              "tfmap_" + file_id + ch_id, tf_list.back(), k_bgn_init,
              k_end_init, height, 1, CONTRAST_MIN_DEFAULT, CONTRAST_MAX_DEFAULT,
              tfmap_limit_lower, tfmap_limit_upper));
        }
      }

      for (int j2 = 0; j2 < pixmap_cmd[i].size(); j2++) {
        height = wavemap_height;
        rest = 0;
        if (find_string(pixmap_cmd[i], j2, "w", height, rest)) {
          wavemap_list.push_back(
              new Wavemap("wavemap_" + file_id + ch_id, sound_list[j], width,
                          height, DARK_BLUE, LIGHT_BLUE, LIGHT_BLUE, WHITE));
        }
      }
    }
    j++;
  }

  // QMainWindow window;
  // const unsigned char *imageData = tfmap_list.back()->data_rgb();
  // int imageWidth = tfmap_list.back()->width();
  // int imageHeight = tfmap_list.back()->height();

  // int originalWidth = tfmap_list.back()->width();
  // int aligenedWidth = (originalWidth + 3) & ~3;
  // unsigned char *alignedImageData =
  //     new unsigned char[aligenedWidth * imageHeight * 3];
  // for (int y = 0; y < imageHeight; y++) {
  //   memcpy(alignedImageData + y * aligenedWidth * 3,
  //          imageData + y * originalWidth * 3, originalWidth * 3);
  // }
  // QImage image(alignedImageData, aligenedWidth, imageHeight,
  //              QImage::Format_RGB888);
  // QPixmap pixmap = QPixmap::fromImage(image);
  // QGraphicsScene scene;
  // scene.addPixmap(pixmap);
  // QGraphicsView view(&scene, &window);
  // view.setFixedSize(imageWidth, imageHeight);
  // view.show();

  // window.show();

  MainWindow window("TF: ");
  return app.exec();
}