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

#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTreeView>
#include <QPushButton>
#include <QLabel>
#include <QStandardItemModel>
#include <QHttp>

#include "lexfile.h"

/** A dialog for retrieving word list updates from a network source */
class UpdateDialog : public QDialog {
	Q_OBJECT
	public:
	/** Constructs a new update dialog
	 *  @param localBasePath base directory for local word list files
	 *  @param parent parent widget for this dialog
	 */
	UpdateDialog(QString localBasePath, QWidget * parent = 0);
	
	/** Destructor */
	~UpdateDialog();
	
	/** Set the update source URI
	 *  @param s the new update source URI
	 */
	void setUpdateURI(QString s) { updateURI->setText(s); }
	
	/** Get the current update source URI
	 *  @return current update source URI
	 */
	QString getUpdateURI() { return updateURI->text(); }
	
	/** Checks if any files have been updated
	 *  @return true, if files have been updated, otherwise false
	 */
	bool getFilesUpdated() { return filesUpdated; }
	
	private slots:
	/** Start the update of available word list information */
	void updateWordLists();
	
	/** Copy the selected word lists to local file */
	void getSelectedLists();
	
	/** Handler for completed http requests
	 *  @param reqId request identifier
	 *  @param error true, if request failed, otherwise false
	 */
	void networkRequestComplete(int reqId, bool error);
	
	/** Set the dialog to only allow aborting the current download */
	void downloadStarted();
	
	/** Set the dialog back to the normal responsive state */
	void downloadsFinished();
	
	private:
	/** Line editor for update source URI */
	QLineEdit * updateURI;
	
	/** List view for available word lists */
	QTreeView * wordTreeView;
	
	/** Get selected lists button */
	QPushButton * getListsButton;
	
	/** Update word lists button */
	QPushButton * updateButton;
	
	/** Close / abort button */
	QPushButton * closeButton;
	
	/** Label for information messages */
	QLabel * infoLabel;
	
	/** Item model for wordListView */
	QStandardItemModel * wordListModel;
	
	/** Http connection */
	QHttp * http;
	
	/** Lex files available from the update source */
	QList<Lexfile *> lexfiles;
	
	/** Id of the http request corresponding to the update of available list information */
	int listRequestId;
	
	/** Base directory for local word list files */
	QString basePath;
	
	/** Has any files been updated? */
	bool filesUpdated;
};


#endif
