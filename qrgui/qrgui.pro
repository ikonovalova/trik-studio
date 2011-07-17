QT		+=	svg xml
CONFIG += rpath_libdirs
macx {
	CONFIG -= app_bundle
}

RESOURCES	=	qrgui.qrc
SOURCES		=	main.cpp

TRANSLATIONS = qrgui_ru.ts

# workaround для http://bugreports.qt.nokia.com/browse/QTBUG-8110
# как только поправят, можно будет юзать QMAKE_LFLAGS_RPATH
!macx {
	QMAKE_LFLAGS="-Wl,-O1,-rpath,$(PWD)"
}

OBJECTS_DIR = .obj
UI_DIR = .ui
MOC_DIR = .moc
RCC_DIR = .moc

if (equals(QMAKE_CXX, "g++") : !macx) {
	QMAKE_LFLAGS += -Wl,-E
}

# Путь до библиотеки с АПИ. Где-нибудь она найдётся...Path to the API library
LIBS += -Ldebug -lqrrepo -Lrelease -lqrrepo -L. -lqrrepo -lqrmc

CONFIG(debug, debug|release):LIBS  += -lqextserialportd
else:LIBS  += -lqextserialport

unix:DEFINES   = _TTY_POSIX_
win32:DEFINES  = _TTY_WIN_

# Graphical elements
include (umllib/umllib.pri)

# Dialogs
include (dialogs/dialogs.pri)

# Main window
include (mainwindow/mainwindow.pri)

# View
include (view/view.pri)

# Stuff used in all places
include (kernel/kernel.pri)

# "Встроенные" генераторы
include (generators/generators.pri)

# "Встроенные" средства реверс-инжиниринга
include (parsers/parsers.pri)

# .pri заведён, чтобы структура папок более круто показывалась в креаторе.
# Содержит код, общий для генератора редакторов и основной части.
include (../utils/utils.pri)

# Код, скачанный из интернета.
include (thirdparty/thirdparty.pri)

# Управление плагинами. Plugin managment
include (pluginManager/pluginManager.pri)

# Graphical and logical models
include (models/models.pri)

# Interfaces for plugins, used by qrxc and qrmc.
include (editorPluginInterface/editorPluginInterface.pri)

# Interfaces for tool plugins, used in handcoded tools.
include (toolPluginInterface/toolPluginInterface.pri)

# Interpreters: visual debugger and robots
include (interpreters/interpreters.pri)
