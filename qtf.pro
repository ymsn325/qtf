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
  src/alsa_out.cpp

HEADERS += \
  src/mainwindow.hpp \
  src/arg_srv.hpp \
  src/sobj.hpp \
  src/sound.hpp \
  src/shape.hpp \
  src/tf.hpp \
  src/alsa_out.hpp

LIBS += \
  -lasound \
  -lfftw3
