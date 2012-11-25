TEMPLATE = lib
VERSION = 0.1.0

CONFIG += qt warn_on release
QT += core

DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = obj

INCLUDEPATH += src/

INCLUDEPATH += /usr/local/cuda/include  # Default NVIDIA CUDA SDK include path
INCLUDEPATH += /opt/AMDAPP/include      # Default AMD APP SDK include path

LIBS += -lOpenCL

QMAKE_CXXFLAGS_RELEASE = -std=c++11 -march=native -O3 -fPIC

SOURCES += \
    src/util/singleton.cpp \

HEADERS += \
    src/util/singleton.h \