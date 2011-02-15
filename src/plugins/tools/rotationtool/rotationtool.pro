# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./src/plugins/tools/rotationtool
# Target is a library:  

INSTALLS += target 
target.path = /plugins/

HEADERS += tweener.h \
           configurator.h \
           settings.h \
           tweenmanager.h

SOURCES += tweener.cpp \
           configurator.cpp \
           settings.cpp \
           tweenmanager.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = rotationtool

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../tools_config.pri)

include(../../../../tupiglobal.pri)
