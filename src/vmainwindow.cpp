/* Voikkoconfig: Configuration tool for Finnish spellchecker Voikko
 * Copyright (C) 2006 Harri Pitkänen <hatapitk@iki.fi>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *********************************************************************************/

#include <QtGui>

#ifdef Q_OS_WIN32
#include <windows.h>
#endif

#include "vmainwindow.h"
#include "utils.h"

VMainWindow::VMainWindow() {
	spellChecker = new SpellChecker();
	dictManager = new DictManager();
	mainTabs = new QTabWidget();
	editorPane = new VEditorPane(spellChecker);
	mainTabs->addTab(editorPane, tr("&Text editor"));
	setupPane = new VSetupPane(spellChecker);
	setupPane->setDictManager(dictManager);
	mainTabs->addTab(setupPane, tr("&Settings"));
	dictPane = new VDictPane();
	dictPane->setDictManager(dictManager);
	mainTabs->addTab(dictPane, tr("&Word lists"));
	
	setCentralWidget(mainTabs);
	connect(mainTabs, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
	
	helpBrowser = new VHelpBrowser(this);
	
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileOpenDictionaryAction = new QAction(tr("&Open dictionary..."), this);
	connect(fileOpenDictionaryAction, SIGNAL(triggered()), this, SLOT(showOpenDictionary()));
	fileMenu->addAction(fileOpenDictionaryAction);
	fileSaveDictionaryAction = new QAction(tr("&Save dictionary"), this);
	connect(fileSaveDictionaryAction, SIGNAL(triggered()), this, SLOT(showSaveDictionary()));
	fileMenu->addAction(fileSaveDictionaryAction);
	fileSaveDictionaryAsAction = new QAction(tr("Save dictionary &as..."), this);
	connect(fileSaveDictionaryAsAction, SIGNAL(triggered()), this, SLOT(showSaveDictionaryAs()));
	fileMenu->addAction(fileSaveDictionaryAsAction);
	fileCloseDictionaryAction = new QAction(tr("&Close dictionary"), this);
	connect(fileCloseDictionaryAction, SIGNAL(triggered()), this, SLOT(closeDictionary()));
	fileMenu->addAction(fileCloseDictionaryAction);
	fileMenu->addSeparator();
	fileQuitAction = new QAction(tr("&Quit"), this);
	connect(fileQuitAction, SIGNAL(triggered()), this, SLOT(quit()));
	fileMenu->addAction(fileQuitAction);
	
	defaultsMenu = menuBar()->addMenu(tr("&Defaults"));
	defaultsSetDefaultAction = new QAction(tr("&Set as user default dictionary"), this);
	connect(defaultsSetDefaultAction, SIGNAL(triggered()), this, SLOT(setDefaultDictionary()));
	defaultsMenu->addAction(defaultsSetDefaultAction);
	defaultsRemoveDefaultAction = new QAction(tr("&Remove user default dictionary"), this);
	connect(defaultsRemoveDefaultAction, SIGNAL(triggered()), this, SLOT(removeDefaultDictionary()));
	defaultsMenu->addAction(defaultsRemoveDefaultAction);
	
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpHelpAction = new QAction(tr("Voikkoconfig &handbook"), this);
	connect(helpHelpAction, SIGNAL(triggered()), helpBrowser, SLOT(show()));
	helpMenu->addAction(helpHelpAction);
	helpAboutAction = new QAction(tr("&About voikkoconfig"), this);
	connect(helpAboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
	helpMenu->addAction(helpAboutAction);
	
	activateSystemSpellchecker();
	resize(400, 500);
}

void VMainWindow::closeEvent(QCloseEvent * event) {
	event->ignore();
	quit();
}

void VMainWindow::quit() {
	if (dictManager->hasConfigurationChanged() &&
	    QMessageBox::question(this, tr("Unsaved changes"),
	       tr("Changes have been made to the configuration, but\n"
	          "they have not been saved. Do you really want to\n"
	          "discard the changes and exit the application?"),
	       QMessageBox::Discard | QMessageBox::Cancel,
	       QMessageBox::Cancel) == QMessageBox::Cancel) return;
	helpBrowser->close();
	QCoreApplication::quit();
}

void VMainWindow::showAbout() {
	QMessageBox::about(this, tr("About voikkoconfig"),
		   tr("Voikkoconfig is a tool for configuring Voikko spellchecking system\n"
		   "Copyright 2006 Harri Pitkanen (hatapitk@iki.fi)\n"
		   "Voikkoconfig is distributed under the GNU General Public License,\n"
		   "version 2 or later."));
}

void VMainWindow::showOpenDictionary() {
	QString dictFileName = QFileDialog::getOpenFileName(
		this, tr("Choose a spelling dictionary"), QString(),
		tr("Spelling dictionary (voikko-fi_FI.pro)"));
	if (!dictFileName.isEmpty()) {
		QString errMsg = dictManager->load(dictFileName);
		if (!errMsg.isEmpty()) {
			errorMsg(errMsg);
			activateSystemSpellchecker();
		}
		else {
			currentDictPath = dictFileName;
			setupPane->updateFromDictManager();
			QString dictDir = QFileInfo(dictFileName).dir().absolutePath();
			QString spError = spellChecker->initialise(dictDir);
			if (!spError.isNull()) {
				errorMsg(tr("Failed to initialise the spellchecker:\n")
				         + spError
				         + tr("\nSpellchecker will not work."));
				editorPane->setEnabled(false);
			}
			else {
				editorPane->setEnabled(true);
				editorPane->setHlType();
			}
			setWindowTitle(QString("Voikkoconfig - ") + currentDictPath);
			defaultsSetDefaultAction->setEnabled(true);
			fileSaveDictionaryAsAction->setEnabled(true);
			fileCloseDictionaryAction->setEnabled(true);
			/* Only enable dictionary settings and saving if selected dictionary
			   seems to have source files available. */ 
			if (QDir(dictDir).exists("voikko-fi_FI.mor")) {
				setupPane->enableOnlyEditorSetup(false);
				dictPane->setEnabled(true);
				fileSaveDictionaryAction->setEnabled(true);
			}
			else {
				setupPane->enableOnlyEditorSetup(true);
				dictPane->setEnabled(false);
				fileSaveDictionaryAction->setEnabled(false);
			}
		}
	}
}

void VMainWindow::showSaveDictionaryAs() {
	QString dictFileName = QFileDialog::getSaveFileName(
		this, tr("Save a spelling dictionary"), QString(),
		tr("Spelling dictionaries (*.pro)"));
	if (!dictFileName.isEmpty()) {
		currentDictPath = dictFileName;
		setWindowTitle(QString("Voikkoconfig - ") + currentDictPath);
		showSaveDictionary();
	}
}

void VMainWindow::showSaveDictionary() {
	if (currentDictPath.isEmpty()) return;
	QString errMsg = dictManager->save(currentDictPath);
	if (!errMsg.isEmpty()) errorMsg(errMsg);
	else {
		if (dictManager->runMalmake(this)) { // Lexicon was succesfully regenerated
			QString dictDir = QFileInfo(currentDictPath).dir().absolutePath();
			spellChecker->initialise(dictDir);
			editorPane->setEnabled(true);
			editorPane->setHlType();
		}
		else editorPane->setEnabled(false);
	}
}

void VMainWindow::closeDictionary() {
	if (dictManager->hasConfigurationChanged() &&
	    QMessageBox::question(this, tr("Unsaved changes"),
	       tr("Changes have been made to the configuration, but\n"
	          "they have not been saved. Do you really want to\n"
	          "discard the changes?"),
	       QMessageBox::Discard | QMessageBox::Cancel,
	       QMessageBox::Cancel) == QMessageBox::Cancel) return;
	activateSystemSpellchecker();
}

void VMainWindow::setDefaultDictionary() {
	QString targetName = QFileInfo(currentDictPath).dir().path();
	#ifdef Q_OS_WIN32
	HKEY regKey;
	if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Voikko", 0, NULL, REG_OPTION_NON_VOLATILE,
	                   KEY_WRITE, NULL, &regKey, NULL)) {
		errorMsg(tr("Failed to create a registry key HKEY_CURRENT_USER\\Software\\Voikko"));
		return;
	}
	QByteArray targetBytes = targetName.toUtf8();
	if (RegSetValueExA(regKey, "DictionaryPath", 0, REG_SZ, (const BYTE *) targetBytes.data(), targetBytes.size())) {
		errorMsg(tr("Failed to set value HKEY_CURRENT_USER\\Software\\Voikko\\DictionaryPath"));
	}
	RegCloseKey(regKey);
	#else
	if (currentDictPath.isNull()) return;
	if (!removeDefaultDictionary()) return;
	QString symlinkName = QDir::homePath().append("/.voikko");
	if (!QFile::link(targetName, symlinkName))
		errorMsg(tr("Failed to create the symbolic link %1").arg(symlinkName));
	#endif
}

bool VMainWindow::removeDefaultDictionary() {
	#ifdef Q_OS_WIN32
	HKEY regKey;
	if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Voikko", 0, NULL, REG_OPTION_NON_VOLATILE,
	                   KEY_WRITE, NULL, &regKey, NULL)) {
		errorMsg(tr("Failed to create a registry key HKEY_CURRENT_USER\\Software\\Voikko"));
		return false;
	}
	if (RegSetValueExA(regKey, "DictionaryPath", 0, REG_SZ, (const BYTE *) "\0", 1)) {
		errorMsg(tr("Failed to reset value HKEY_CURRENT_USER\\Software\\Voikko\\DictionaryPath"));
		RegCloseKey(regKey);
		return false;
	}
	RegCloseKey(regKey);
	return true;
	#else
	QString symlinkName = QDir::homePath().append("/.voikko");
	QFileInfo linkInfo(symlinkName);
	if (linkInfo.exists()) {
		if (linkInfo.isSymLink()) {
			if (currentDictPath.isNull() ||
			    QFileInfo(currentDictPath).dir().path() == symlinkName)
				spellChecker->uninitialise();
			if (!QFile(symlinkName).remove()) {
				errorMsg(tr("Failed to remove symbolic link %1").arg(symlinkName));
				spellChecker->initialise(currentDictPath);
				return false;
			}
			if (QFileInfo(currentDictPath).dir().path() == symlinkName)
				currentDictPath = QString();
			if (currentDictPath.isNull()) spellChecker->initialise();
		}
		else {
			errorMsg(symlinkName + tr(" is not a symbolic link. Please remove it manually."));
			return false;
		}
	}
	return true;
	#endif
}

void VMainWindow::activateSystemSpellchecker() {
	currentDictPath = QString();
	QString error = spellChecker->initialise();
	if (error.isNull()) { // Initialisation succeeded
		editorPane->setEnabled(true);
		setupPane->enableOnlyEditorSetup(true);
	}
	else {
		editorPane->setEnabled(false);
		setupPane->setEnabled(false);
		errorMsg(error);
	}
	dictPane->setEnabled(false);
	fileSaveDictionaryAction->setEnabled(false);
	fileSaveDictionaryAsAction->setEnabled(false);
	fileCloseDictionaryAction->setEnabled(false);
	defaultsSetDefaultAction->setEnabled(false);
	setWindowTitle("Voikkoconfig - default spellchecker");
}

void VMainWindow::currentTabChanged(int newIndex) {
	if (newIndex == 0) {
		if (dictManager->hasConfigurationChanged() &&
		    QMessageBox::question(this, tr("Unsaved changes"),
		       tr("Some changes to the configuration need to be saved\n"
		          "before changes take effect in spell checking.\n"
		          "Do you want to save the changes now?"),
		       QMessageBox::Yes | QMessageBox::No,
		       QMessageBox::Yes) == QMessageBox::Yes)
			showSaveDictionary();
		editorPane->setHlType();
	}
}

VMainWindow::~VMainWindow() {
	delete spellChecker;
}
