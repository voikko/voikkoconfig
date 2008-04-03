# Voikkoconfig: Configuration tool for Finnish spellchecker Voikko
# Copyright (C) 2006 Harri Pitkänen <hatapitk@iki.fi>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#########################################################################

CONFIG += qt

# Debug configuration does not work on Windows, use release configuration instead
CONFIG += release
# CONFIG += debug

QT += network

TARGET = voikkoconfig

SOURCES += main.cpp
HEADERS += vmainwindow.h
SOURCES += vmainwindow.cpp
HEADERS += veditorpane.h
SOURCES += veditorpane.cpp
HEADERS += vsetuppane.h
SOURCES += vsetuppane.cpp
HEADERS += vdictpane.h
SOURCES += vdictpane.cpp
HEADERS += spellchecker.h
SOURCES += spellchecker.cpp
HEADERS += dictmanager.h
SOURCES += dictmanager.cpp
HEADERS += utils.h
SOURCES += utils.cpp
HEADERS += vsyntaxhighlighter.h
SOURCES += vsyntaxhighlighter.cpp
HEADERS += vtextedit.h
SOURCES += vtextedit.cpp
HEADERS += updatedialog.h
SOURCES += updatedialog.cpp
HEADERS += lexfile.h
SOURCES += lexfile.cpp
HEADERS += vhelpbrowser.h
SOURCES += vhelpbrowser.cpp

unix:LIBS += -lvoikko
win32:INCLUDEPATH += c:/msys/1.0/inst/include
win32:LIBS += c:/msys/1.0/inst/bin/libvoikko-1.dll

TRANSLATIONS += voikkoconfig_fi.ts
RESOURCES = voikkoconfig.qrc
RC_FILE = voikkoconfig.rc
