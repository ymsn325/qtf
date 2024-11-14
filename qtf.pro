QT += core gui widgets

TARGET = bin/qtf

CONFIG += debug

QMAKE_CXXFLAGS_DEBUG += -O0

SOURCES += \
  src/main.cpp \
  src/sobj.cpp \
  src/sound.cpp

HEADERS += \
  src/arg_srv.hpp
  src/sobj.hpp
  src/sound.hpp
