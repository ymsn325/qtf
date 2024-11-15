#include <QGridLayout>
#include <QMainWindow>

#include "pixmap.hpp"
#include "sobj.hpp"
#include "standard.hpp"

#define MAINWINDOW_X 60
#define MAINWINDOW_Y 10
#define MAINWINDOW_WIDTH 1750
#define MAINWINDOW_HEIGHT 934
#define TFMAP_HEIGHT 512
#define WAVEMAP_HEIGHT 128

extern int g_mainwindow_init_height;

class MainWindow : public Sobj {
 private:
  QMainWindow *m_toplevel;
  QWidget *m_central_widget;
  QGridLayout *m_top_layout;
  int m_mainwindow_height;
  int m_width, m_height;
  int m_row_cur;
  vector<int> m_height_list;
  vector<QWidget *> m_area_list;

 public:
  MainWindow(string id, int pos_x = MAX_INT, int pos_y = MAX_INT,
             int width = MAX_INT, int height = g_mainwindow_init_height);
  void append_pixmap(Pixmap *parent_pixmap);
};
