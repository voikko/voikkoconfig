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
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>

#include "vdictpane.h"
#include "updatedialog.h"

VDictPane::VDictPane() {
	// Create dictionary list
	dictListView = new QListView();
	
	addButton = new QPushButton(tr("&Add"));
	removeButton = new QPushButton(tr("&Remove"));
	updateButton = new QPushButton(tr("&Update"));
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(dictListView, 0, 0, 1, 3, Qt::AlignTop);
	layout->addWidget(addButton, 1, 0, Qt::AlignBottom);
	layout->addWidget(removeButton, 1, 1, Qt::AlignBottom);
	layout->addWidget(updateButton, 1, 2, Qt::AlignBottom);
	
	// Initial widget states and connections
	connect(addButton, SIGNAL(clicked()), this, SLOT(addWordList()));
	connect(removeButton, SIGNAL(clicked()), this, SLOT(removeWordList()));
	connect(updateButton, SIGNAL(clicked()), this, SLOT(updateWordLists()));
	addButton->setEnabled(true);
	removeButton->setEnabled(true);
	updateButton->setEnabled(true);
	
	setLayout(layout);
	
	dictManager = 0;
}

void VDictPane::setDictManager(DictManager * newDictManager) {
	if (dictManager != 0) {disconnect(dictManager, SIGNAL(itemChanged(QStandardItem*)),
		                  this, SLOT(updateChangedState()));}
	dictManager = newDictManager;
	dictListView->setModel(newDictManager);
}

void VDictPane::addWordList() {
	QString wlFileName = QFileDialog::getOpenFileName(
		this, tr("Choose a word list"), QString(),
		tr("Voikko word list (*.lex)"));
	if (!wlFileName.isEmpty()) {
		dictManager->addWordList(wlFileName);
	}
}

void VDictPane::removeWordList() {
	QModelIndex current = dictListView->currentIndex();
	dictManager->removeRow(current.row());
}

void VDictPane::updateWordLists() {
	if (!dictManager) return;
	UpdateDialog * ud = new UpdateDialog(dictManager->getProjectDirectory(), this);
	ud->setUpdateURI(dictManager->getUpdateURI());
	ud->exec();
	if (ud->result() == QDialog::Accepted) {
		dictManager->setUpdateURI(ud->getUpdateURI());
		if (ud->getFilesUpdated()) dictManager->markConfigurationChanged();
	}
	delete ud;
}
