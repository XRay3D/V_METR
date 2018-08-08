#QT       += core gui
#greaterThan(QT_MAJOR_VERSION, 4)
#{
#	QT += widgets
#	DEFINES += QT_5
#}

SOURCES += $$PWD/qserialport.cpp

HEADERS  += $$PWD/qserialport.h

RESOURCES += $$PWD/qserialport.qrc

INCLUDEPATH += $$PWD

#CONFIG += object_parallel_to_source

#DEFINES += QT_NO_CAST_FROM_ASCII \
#			QT_NO_CAST_TO_ASCII
