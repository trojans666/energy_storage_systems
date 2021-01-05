TEMPLATE = app

QT -= gui

DISTFILES += \
        config.xml

HEADERS += \ 
    src/autohd.h \
    src/ctrlnode.h \
    src/modschedul.h \
    src/module.h \
    src/resalloc.h \
    src/subsys.h \
    src/sys.h \
    src/terror.h


SOURCES += \ 
    src/ctrlnode.cpp \
    src/modschedul.cpp \
    src/module.cpp \
    src/resalloc.cpp \
    src/subsys.cpp \
    src/sys.cpp \
    src/terror.cpp



include (modules/modules.pri)
