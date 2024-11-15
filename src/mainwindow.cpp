#include "mainwindow.hpp"

int g_mainwindow_init_x = MAINWINDOW_X;
int g_mainwindow_init_y = MAINWINDOW_Y;
int g_mainwindow_init_width = MAINWINDOW_WIDTH;
int g_mainwindow_init_height = MAINWINDOW_HEIGHT;

MainWindow::MainWindow(string id, int pos_x, int pos_y, int width, int height)
    : Sobj(id) {
  m_mainwindow_height = height;
  m_width = 0;
  m_height = 0;
  m_mainwindow = new QMainWindow();
  m_mainwindow->setGeometry(pos_x, pos_y, width, height);
  m_mainwindow->show();
}