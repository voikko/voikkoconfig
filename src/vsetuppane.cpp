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

#include <QGridLayout>
#include <QHashIterator>

#include "vsetuppane.h"
#include "vmainwindow.h"
#include "veditorpane.h"
#include "spellchecker.h"

VSetupPane::VSetupPane(SpellChecker * sc) {
	spellChecker = sc;
	dictSettingsGB = new QGroupBox(tr("D&ictionary settings"));
	dictSettingsLayout = new QVBoxLayout();
	dictSettingsGB->setLayout(dictSettingsLayout);
	
	editorSettings = new QGroupBox(tr("Editor settings"));
	ignoreWordsWithNumbersCB = new QCheckBox(tr("Ignore words containing &numbers"));
	ignoreUppercaseCB = new QCheckBox(tr("Ignore words written in &uppercase"));
	acceptFirstUppercaseCB = new QCheckBox(tr("Allow the first letter of a word to be &captialised"));
	QVBoxLayout * editorSettingsLayout = new QVBoxLayout();
	editorSettingsLayout->addWidget(ignoreWordsWithNumbersCB);
	editorSettingsLayout->addWidget(ignoreUppercaseCB);
	editorSettingsLayout->addWidget(acceptFirstUppercaseCB);
	editorSettings->setLayout(editorSettingsLayout);
	
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(dictSettingsGB, 0, 0, 1, 2, Qt::AlignTop);
	layout->addWidget(editorSettings, 1, 0, 1, 2, Qt::AlignTop);
	
	// Initial widget states and connections
	acceptFirstUppercaseCB->setChecked(true);
	connect(ignoreWordsWithNumbersCB, SIGNAL(stateChanged(int)),
	        this, SLOT(updateEditorSettings()));
	connect(ignoreUppercaseCB, SIGNAL(stateChanged(int)), this, SLOT(updateEditorSettings()));
	connect(acceptFirstUppercaseCB, SIGNAL(stateChanged(int)), this, SLOT(updateEditorSettings()));
	
	setLayout(layout);
	
	dictManager = 0;
}

void VSetupPane::setDictManager(DictManager * newDictManager) {
	dictManager = newDictManager;
	updateFromDictManager();
}

void VSetupPane::enableOnlyEditorSetup(bool onlyEditor) {
	setEnabled(true);
	dictSettingsGB->setEnabled(!onlyEditor);
	editorSettings->setEnabled(true);
}

void VSetupPane::updateDictionaryState() {
	QHashIterator<QString, QCheckBox *> i(dictSettings);
	while (i.hasNext()) {
		i.next();
		dictManager->setMallexBOption(i.key(), i.value()->isChecked());
	}
}

void VSetupPane::updateEditorSettings() {
	spellChecker->setIgnoreWordsWithNumbers(ignoreWordsWithNumbersCB->isChecked());
	spellChecker->setIgnoreUppercase(ignoreUppercaseCB->isChecked());
	spellChecker->setAcceptFirstUppercase(acceptFirstUppercaseCB->isChecked());
}

void VSetupPane::updateFromDictManager() {
	/* Remove old items */
	QHashIterator<QString, QCheckBox *> i(dictSettings);
	while (i.hasNext()) {
		i.next();
		dictSettingsLayout->removeWidget(i.value());
		delete i.value();
	}
	dictSettings.clear();
	
	/* Add new items */
	QList<MallexBOption> mbOptions = dictManager->getMallexBOptions();
	QCheckBox * newCB;
	for (int i = 0; i < mbOptions.size(); i++) {
		newCB = new QCheckBox(mbOptions[i].description);
		dictSettingsLayout->addWidget(newCB);
		newCB->setChecked(mbOptions[i].value);
		connect(newCB, SIGNAL(stateChanged(int)), this, SLOT(updateDictionaryState()));
		dictSettings.insert(mbOptions[i].name, newCB);
	}
}
