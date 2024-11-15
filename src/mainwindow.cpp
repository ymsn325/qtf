#include "mainwindow.hpp"

#include <QGraphicsScene>
#include <QLabel>
#include <QPushButton>
#include <iostream>

#include "pixmap.hpp"
#include "sound.hpp"

using namespace std;

int g_mainwindow_init_x = MAINWINDOW_X;
int g_mainwindow_init_y = MAINWINDOW_Y;
int g_mainwindow_init_width = MAINWINDOW_WIDTH;
int g_mainwindow_init_height = MAINWINDOW_HEIGHT;

MainWindow::MainWindow(string id, int pos_x, int pos_y, int width, int height)
    : Sobj(id) {
  m_mainwindow_height = height;
  m_width = 0;
  m_height = 0;
  m_row_cur = 0;

  m_toplevel = new QMainWindow();
  m_toplevel->setGeometry(pos_x, pos_y, width, height);
  m_toplevel->show();

  m_central_widget = new QWidget(m_toplevel);
  m_top_layout = new QGridLayout(m_central_widget);
  m_top_layout->setSizeConstraint(QLayout::SetFixedSize);
  m_top_layout->setContentsMargins(0, 0, 0, 0);
  m_top_layout->setSpacing(0);
  m_toplevel->setCentralWidget(m_central_widget);
}

void MainWindow::append_pixmap(Pixmap *parent_pixmap) {
  vector<Sound *>::iterator p;
  QPushButton *play_button;
  QImage image;
  QLabel *label;
  QPixmap pixmap;
  if (m_width == 0) {
    m_width = parent_pixmap->width();
  }

  if (m_width != parent_pixmap->width()) {
    cerr << "MainWindow::append_pixmap: width mismatch" << endl;
    exit(0);
  }

  append_parent(parent_pixmap);
  m_height_list.push_back(parent_pixmap->height());
  m_height = parent_pixmap->height();
  label = new QLabel(m_central_widget);
  m_area_list.push_back(label);

  const unsigned char *imageData = parent_pixmap->data_rgb();
  int imageWidth = m_width;
  int imageHeight = m_height;

  int originalWidth = m_width;
  int aligenedWidth = (originalWidth + 3) & ~3;
  unsigned char *alignedImageData =
      new unsigned char[aligenedWidth * imageHeight * 3];
  for (int y = 0; y < imageHeight; y++) {
    memcpy(alignedImageData + y * aligenedWidth * 3,
           imageData + y * originalWidth * 3, originalWidth * 3);
  }

  image = QImage(alignedImageData, aligenedWidth, imageHeight,
                 QImage::Format_RGB888);
  pixmap = QPixmap::fromImage(image);
  label = new QLabel(m_central_widget);
  label->setPixmap(pixmap);
  m_top_layout->addWidget(label, m_row_cur, 0);

  play_button = new QPushButton("PLAY");
  play_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  // m_top_layout->addWidget(view, m_row_cur, 0);
  m_top_layout->addWidget(play_button, m_row_cur, 1);
  m_row_cur++;
}