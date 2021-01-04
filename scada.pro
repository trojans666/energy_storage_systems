TEMPLATE = app

DISTFILES += \
	config.xml 
	
INCLUDEPATH += $$PWD/

HEADERS += \ 
    generic/ctrlnode.h \
    generic/terror.h \
    generic/resalloc.h \
    generic/autohd.h

SOURCES += \ 
    generic/terror.cpp \
    generic/resalloc.cpp

include (xxx/xx.pri)
