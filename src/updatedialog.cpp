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

#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QTextStream>

#include "updatedialog.h"
#include "utils.h"

UpdateDialog::UpdateDialog(QString localBasePath, QWidget * parent) : QDialog(parent) {
	basePath = localBasePath;
	filesUpdated = false;
	setWindowTitle(tr("Update word lists"));
	resize(380, 380);
	updateURI = new QLineEdit();
	QLabel * updateURILabel = new QLabel(tr("Update source URL"));
	updateURILabel->setBuddy(updateURI);
	wordTreeView = new QTreeView();
	wordTreeView->setRootIsDecorated(false);
	wordListModel = new QStandardItemModel(this);
	wordTreeView->setModel(wordListModel);
	
	updateButton = new QPushButton(tr("&Update"));
	getListsButton = new QPushButton(tr("&Get selected lists"));
	closeButton = new QPushButton(tr("&Close"));
	
	QHBoxLayout * updateURILayout = new QHBoxLayout();
	updateURILayout->addWidget(updateURI);
	updateURILayout->addWidget(updateButton);
	QWidget * updateURIWidget = new QWidget();
	updateURIWidget->setLayout(updateURILayout);
	
	infoLabel = new QLabel(QString(""));
	
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(updateURILabel, 0, 0, 1, 2);
	layout->addWidget(updateURIWidget, 1, 0, 1, 2, Qt::AlignTop);
	layout->addWidget(new QLabel(tr("Available word lists")), 2, 0, 1, 2);
	layout->addWidget(wordTreeView, 3, 0, 1, 2);
	layout->addWidget(getListsButton, 4, 0, Qt::AlignBottom);
	layout->addWidget(closeButton, 4, 1, Qt::AlignBottom);
	layout->addWidget(infoLabel, 5, 0, 1, 2);
	
	http = new QHttp();
	listRequestId = 0;
	
	// Initial widget states and connections
	connect(updateButton, SIGNAL(clicked()), this, SLOT(updateWordLists()));
	connect(updateURI, SIGNAL(returnPressed()), this, SLOT(updateWordLists()));
	connect(getListsButton, SIGNAL(clicked()), this, SLOT(getSelectedLists()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(networkRequestComplete(int, bool)));
	connect(http, SIGNAL(done(bool)), this, SLOT(downloadsFinished()));
	
	setLayout(layout);
}

void UpdateDialog::updateWordLists() {
	QUrl sourceUrl = QUrl(getUpdateURI(), QUrl::TolerantMode);
	if (!sourceUrl.isValid()) {
		errorMsg(tr("Invalid update source URL"));
		return;
	}
	if (sourceUrl.scheme() != "http") {
		errorMsg(tr("Only http URLs are supported"));
		return;
	}
	
	QString host = sourceUrl.host();
	int port = sourceUrl.port();
	if (port == -1) port = 80;
	QString path = sourceUrl.path();
	
	infoLabel->setText(tr("Downloading list of available updates..."));
	downloadStarted();
	http->setHost(host, port);
	listRequestId = http->get(path);
}

void UpdateDialog::getSelectedLists() {
	QString remoteUrl;
	QString localPath;
	bool hasListsToUpdate = false;
	for (int i = 0; i < wordListModel->rowCount(); i++) {
		if (wordListModel->item(i)->checkState() == Qt::Checked) {
			hasListsToUpdate = true;
			if (!lexfiles[i]->updateLocalFromRemote(http))
				errorMsg(tr("Failed to update %1").arg(lexfiles[i]->description));
			else {
				infoLabel->setText(tr("Downloading %1...").arg(lexfiles[i]->description));
				downloadStarted();
			}
		}
	}
	if (!hasListsToUpdate) accept();
}

void UpdateDialog::networkRequestComplete(int id, bool error) {
	if (error) {
		errorMsg(tr("Network request failed"));
		return;
	}
	if (id == listRequestId) {
		QByteArray data = http->readAll();
		infoLabel->setText(tr("List of available updates downloaded"));
		QTextStream ts(data);
		if (ts.readLine().trimmed() != "[version]") {
			errorMsg(tr("Syntax error: [version] missing"));
			return;
		}
		if (ts.readLine().trimmed() != "1") {
			errorMsg(tr("Lex version 1 expected"));
			return;
		}
		wordListModel->clear();
		QStringList headerList;
		headerList << tr("Description");
		headerList << tr("Date");
		headerList << tr("Local path");
		wordListModel->setHorizontalHeaderLabels(headerList);
		QString line;
		Lexfile * lexfile;
		while (!lexfiles.isEmpty()) {
			lexfile = lexfiles.last();
			lexfiles.removeLast();
			delete lexfile;
		}
		while (!ts.atEnd()) {
			line = ts.readLine().trimmed();
			if (line.isEmpty()) continue;
			if (line.startsWith("#")) continue;
			if (line == "[lex]") {
				lexfile = new Lexfile(basePath);
				lexfile->remotePath = ts.readLine().trimmed();
				lexfile->localPath = ts.readLine().trimmed();
				lexfile->date = ts.readLine().trimmed();
				lexfile->description = ts.readLine().trimmed();
				wordListModel->appendRow(lexfile->getListRow());
				lexfiles << lexfile;
				continue;
			}
			// Anything else is an error
			errorMsg(tr("Syntax error: unexpected data"));
			return;
		}
	}
	else { // Word list download finished
		bool updatePending = false;
		bool updateFinished = false;
		for (int i = 0; i < lexfiles.size(); i++) {
			if (lexfiles[i]->updateRequestCompleted(id, error)) {
				updateFinished = true;
				filesUpdated = true;
				infoLabel->setText(tr("Finished downloading %1").arg(
				                   lexfiles[i]->description));
			}
			if (lexfiles[i]->updatePending()) updatePending = true;
		}
		if (updateFinished && !updatePending) accept();
	}
}

void UpdateDialog::downloadStarted() {
	updateURI->setEnabled(false);
	getListsButton->setEnabled(false);
	wordTreeView->setEnabled(false);
	updateButton->setEnabled(false);
	closeButton->setText(tr("&Abort"));
}
	
void UpdateDialog::downloadsFinished() {
	updateURI->setEnabled(true);
	getListsButton->setEnabled(true);
	wordTreeView->setEnabled(true);
	updateButton->setEnabled(true);
	closeButton->setText(tr("&Close"));
}

UpdateDialog::~UpdateDialog() {
	Lexfile * lexfile;
	while (!lexfiles.isEmpty()) {
		lexfile = lexfiles.last();
		lexfiles.removeLast();
		delete lexfile;
	}
	delete http;
}
