#include <QApplication>
#include <QMainWindow>
#include <string>

#include "arg_srv.hpp"
#include "sound.hpp"

using namespace std;

static const string DEFAULT_FILE_NAME =
    "/home/sy/audio/ichimoji_PF02_0501_033.wav";

int main(int argc, char **argv) {
  string file_name_all;
  vector<string> file_name;
  vector<int> channel, total_channel;
  vector<vector<string>> pixmap_cmd;
  int default_output_bitdepth;
  int n_samples;
  int n_sounds;
  vector<double> wavemap_lower, wavemap_upper;
  double fs, fs_file;

  QApplication app(argc, argv);

  vector<Sound *> sound_list;
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
  arg_srv.finish();

  sound_list.push_back(
      new Sound("sound", DEFAULT_FILE_NAME, 44100.0, 0, 0, 0, 1, true));

  QMainWindow window;
  window.show();
  return app.exec();
}