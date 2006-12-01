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

#ifndef LEXFILE_H
#define LEXFILE_H

#include <QStandardItem>
#include <QString>
#include <QHttp>
#include <QFile>

class Lexfile {
	public:
	/** Constructs a new lex file object
	 *  @param localBasePath base directory for local word list files
	 */
	Lexfile(QString localBasePath);
	const QList<QStandardItem *> getListRow();
	
	/** Tries to update the local file with the contents of the remote file.
	 *  @return true, if update succeeds, otherwise false.
	 */
	bool updateLocalFromRemote(QHttp * http);
	
	/** Free resources after the file download was finished.
	 * @param reqId id of the completed request
	 * @param error true, if there was an error during file download
	 * @return true, if the completed request belonged to this object and cleanup succeeded,
	 *         otherwise false.
	 */
	bool updateRequestCompleted(int reqId, bool error);
	
	/** Is there a pending network update request for this object?
	 *  @return true, if update is pending, otherwise false
	 */
	bool updatePending();
	
	/** Base directory for local word list files */
	QString basePath;
	
	QString description;
	QString date;
	QString remotePath;
	QString localPath;
	int updateRequestId;
	
	private:
	QFile * localFile;
};

#endif

