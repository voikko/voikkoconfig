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

#ifndef VMAINWINDOW_H
#define VMAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

#include "veditorpane.h"
#include "vsetuppane.h"
#include "vdictpane.h"
#include "dictmanager.h"
#include "spellchecker.h"
#include "vhelpbrowser.h"

/** The Voikkoconfig application mani window */
class VMainWindow : public QMainWindow {
	Q_OBJECT
	public:
		VMainWindow();
		~VMainWindow();
	protected:
		/** Handles the window close event for the main window
		 *  @param event the close event
		 */
		void closeEvent(QCloseEvent * event);
	private slots:
		/** Quits the application, asking for confirmation if there are
		 *  unsaved changes in the current vocabulary */
		void quit();
		
		/** Shows information about the application */
		void showAbout();
		
		/** Shows "Open dictionary" dialog */
		void showOpenDictionary();
		
		/** Shows "Save dictionary as" dialog */
		void showSaveDictionaryAs();
		
		/** Saves the current dictionary under its old name */
		void showSaveDictionary();
		
		/** Closes the current dictionary, asking for confirmation if there
		 *  are unsaved changed */
		void closeDictionary();
		
		/** Sets the current dictionary as user default dictionary */
		void setDefaultDictionary();
		
		/** Remove the user default dictionary. Only the link or registry key pointing
		 *  to the dictionary is removed, dictionary files are never removed.
		 *  @return true if the removal succeeds or no dictionary was set, otherwise false.
		 */
		bool removeDefaultDictionary();
		
		/** Opens the default dictionary in read only mode */
		void activateSystemSpellchecker();
		
		/** Handles the tab change event from the main tab widget
		 *  @param newindex index of the tab that was activated
		 */
		void currentTabChanged(int newindex);
	private:
		/* The file menu and actions under it */
		QMenu * fileMenu;
		QAction * fileOpenDictionaryAction;
		QAction * fileSaveDictionaryAsAction;
		QAction * fileSaveDictionaryAction;
		QAction * fileCloseDictionaryAction;
		QAction * fileQuitAction;
		
		/* The default menu and actions under it */
		QMenu * defaultsMenu;
		QAction * defaultsSetDefaultAction;
		QAction * defaultsRemoveDefaultAction;
		
		/* The help menu and actions under it */
		QMenu * helpMenu;
		QAction * helpHelpAction;
		QAction * helpAboutAction;
		
		/* The main widgets */
		QTabWidget * mainTabs;
		VEditorPane * editorPane;
		VSetupPane * setupPane;
		VDictPane * dictPane;
		
		/** The dictionary manager */
		DictManager * dictManager;
		
		/** The spell checker */
		SpellChecker * spellChecker;
		
		/** The help browser dialog */
		VHelpBrowser * helpBrowser;
		
		/** Name of the current Malaga project file */
		QString currentDictPath;
};

#endif

