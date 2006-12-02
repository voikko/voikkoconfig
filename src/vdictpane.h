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

#ifndef VDICTPANE_H
#define VDICTPANE_H

#include <QPushButton>
#include <QListView>

#include "dictmanager.h"

/** Widget to manage word list associated to a dictionary */
class VDictPane : public QWidget {
	Q_OBJECT
	public:
		/** Constructs a new dictionary pane */
		VDictPane();
		
		/** Set a new dictionary manager
		 *  @param newDictManager the new dictionary manager
		 */
		void setDictManager(DictManager * newDictManager);
	public slots:
		/** Add new word list */
		void addWordList();
		
		/** Remove currently selected word list */
		void removeWordList();
		
		/** Invoke word list update dialog */
		void updateWordLists();
	private:
		/** Current associated dictionary manager */
		DictManager * dictManager;
		
		/** List of word lists in the dictionary */
		QListView * dictListView;
		
		/** Button to add a new word list */
		QPushButton * addButton;
		
		/** Button to remove selected word list */
		QPushButton * removeButton; 
		
		/** Button to invoke the word list update dialog */
		QPushButton * updateButton;
};

#endif
