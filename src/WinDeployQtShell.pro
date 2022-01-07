QT += core widgets

CONFIG += c++17

CONFIG(release, debug|release) {
    CONFIG += optimize_full
}

TARGET = WinDeployQtShell

VERSION = 1.0.0

LITERAL_FOR = for

QMAKE_TARGET_PRODUCT = "WinDeployQtShell"
QMAKE_TARGET_COMPANY = "Lukin Alexey"
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2021 Lukin Alexey"
QMAKE_TARGET_DESCRIPTION = "Shell $$LITERAL_FOR Qt deploy tool"

DEFINES += APP_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\"
DEFINES += APP_COPYRIGHT=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\"
DEFINES += APP_DESCRIPTION=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\"

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    worker.cpp

HEADERS += \
    mainwindow.h \
    worker.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DESTDIR = bin
MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc
UI_DIR = ui

win32-msvc* {
    QMAKE_EXTRA_TARGETS += before_build makefilehook
    makefilehook.target = $(MAKEFILE)
    makefilehook.depends = .beforebuild
    PRE_TARGETDEPS += .beforebuild
    before_build.target = .beforebuild
    before_build.depends = FORCE
    before_build.commands = chcp 1251
}

FORMS += \
    mainwindow.ui
