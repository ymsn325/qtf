QT += core gui widgets

TARGET = bin/qtf

CONFIG += debug

QMAKE_CXXFLAGS_DEBUG += -O0

SOURCES += \
  src/main.cpp \
  src/mainwindow.cpp \
  src/sobj.cpp \
  src/sound.cpp \
  src/shape.cpp \
  src/tf.cpp \
  src/pixmap.cpp \
  src/tfmap.cpp \
  src/wavemap.cpp \
  src/alsa_out.cpp \
  src/image.cpp

HEADERS += \
  src/mainwindow.hpp \
  src/arg_srv.hpp \
  src/sobj.hpp \
  src/sound.hpp \
  src/shape.hpp \
  src/tf.hpp \
  src/pixmap.hpp \
  src/tfmap.hpp \
  src/wavemap.hpp \
  src/alsa_out.hpp \
  src/image.hpp \
  src/standard.hpp

LIBS += \
  -lasound \
  -lfftw3 \
  -lpng
