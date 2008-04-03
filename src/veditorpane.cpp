/* Voikkoconfig: Configuration tool for Finnish spellchecker Voikko
 * Copyright (C) 2006 Harri Pitkänen <hatapitk@iki.fi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QGroupBox>

#include "veditorpane.h"
#include "vsyntaxhighlighter.h"

VEditorPane::VEditorPane(SpellChecker * sc) {
	hlText = new QRadioButton(tr("&Plain text"));
	hlHtml = new QRadioButton(tr("Ht&ml"));
	hlLatex = new QRadioButton(tr("&LaTeX"));
	hlText->setChecked(true);
	QGroupBox * hlGBox = new QGroupBox(tr("Text type"));
	QHBoxLayout * hlLayout = new QHBoxLayout();
	hlLayout->addWidget(hlText);
	hlLayout->addWidget(hlHtml);
	hlLayout->addWidget(hlLatex);
	hlGBox->setLayout(hlLayout);
	
	openButton = new QPushButton(tr("&Open text..."));
	saveButton = new QPushButton(tr("S&ave text..."));
	QHBoxLayout * buttonbarLayout = new QHBoxLayout();
	buttonbarLayout->addWidget(openButton);
	buttonbarLayout->addWidget(saveButton);
	buttonbar = new QWidget();
	buttonbar->setLayout(buttonbarLayout);
	
	editor = new VTextEdit(sc);
	editor->setAcceptRichText(false);
	editor->setHighlightType(HLTYPE_TEXT);	
	
	QVBoxLayout * paneLayout = new QVBoxLayout();
	paneLayout->addWidget(editor);
	paneLayout->addWidget(hlGBox);
	paneLayout->addWidget(buttonbar);
	
	// Initial widget states and connections
	connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveFile()));
	connect(hlText, SIGNAL(toggled(bool)), this, SLOT(setHlType()));
	connect(hlHtml, SIGNAL(toggled(bool)), this, SLOT(setHlType()));
	connect(hlLatex, SIGNAL(toggled(bool)), this, SLOT(setHlType()));
	
	setLayout(paneLayout);
}

void VEditorPane::openFile() {
	QString dictFileName = QFileDialog::getOpenFileName(
		this, tr("Open file"), QString(), tr("Text files (*.*)"));
	if (!dictFileName.isEmpty()) {
		QFile file(dictFileName);
		if (file.open(QFile::ReadOnly)) {
			QTextStream inputStream(&file);
			editor->document()->setPlainText(inputStream.readAll());
			file.close();
		}
		else QMessageBox::critical(this, tr("Error"), tr("Reading from file failed."));
	}
}

void VEditorPane::saveFile() {
	QString dictFileName = QFileDialog::getSaveFileName(
		this, tr("Save file"), QString(), tr("Text files (*.*)"));
	if (!dictFileName.isEmpty()) {
		QFile file(dictFileName);
		if (file.open(QFile::WriteOnly | QFile::Truncate)) {
			QTextStream outputStream(&file);
			outputStream << editor->document()->toPlainText();
			file.close();
		}
		else QMessageBox::critical(this, tr("Error"), tr("Writing to file failed."));
	}
}

void VEditorPane::setHlType() {
	HighlightType newHl = HLTYPE_TEXT;
	if (hlHtml->isChecked()) newHl = HLTYPE_HTML;
	else if (hlLatex->isChecked()) newHl = HLTYPE_LATEX;
	editor->setHighlightType(newHl);
}
