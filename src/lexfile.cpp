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

#include <QUrl>

#include "lexfile.h"

Lexfile::Lexfile(QString localBasePath) {
	description = "";
	date = "";
	remotePath = "";
	localPath = "";
	updateRequestId = 0;
	basePath = localBasePath;
}

const QList<QStandardItem *> Lexfile::getListRow() {
	QStandardItem * descriptionItem = new QStandardItem(description);
	descriptionItem->setEditable(false);
	descriptionItem->setCheckable(true);
	QStandardItem * dateItem = new QStandardItem(date);
	dateItem->setEditable(false);
	QStandardItem * pathItem = new QStandardItem(localPath);
	pathItem->setEditable(false);
	QList<QStandardItem *> itemList;
	itemList << descriptionItem;
	itemList << dateItem;
	itemList << pathItem;
	return itemList;
}

bool Lexfile::updateLocalFromRemote(QHttp * http) {
	QUrl sourceUrl(remotePath, QUrl::TolerantMode);
	if (!sourceUrl.isValid()) return false;
	if (sourceUrl.scheme() != "http") return false;
	
	QString host = sourceUrl.host();
	int port = sourceUrl.port();
	if (port == -1) port = 80;
	QString path = sourceUrl.path();
	
	localFile = new QFile(basePath + "/" + localPath);
	if (!localFile->open(QIODevice::WriteOnly)) {
		delete localFile;
		localFile = 0;
		return false;
	}
	
	http->setHost(host, port);
	updateRequestId = http->get(path, localFile);
	return true;
}

bool Lexfile::updateRequestCompleted(int reqId, bool error) {
	if (reqId != updateRequestId) return false;
	localFile->close();
	if (error) localFile->remove();
	delete localFile;
	localFile = 0;
	updateRequestId = 0;
	return true;
}

bool Lexfile::updatePending() {
	return (updateRequestId != 0);
}
