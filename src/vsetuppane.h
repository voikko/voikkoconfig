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

#ifndef VSETUPPANE_H
#define VSETUPPANE_H

#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHash>

#include "dictmanager.h"
#include "spellchecker.h"

class VSetupPane : public QWidget {
	Q_OBJECT
	public:
		/** Constructs a new setup pane
		 *  @param sc the spell checker that will be controlled by this pane
		 */
		VSetupPane(SpellChecker * sc);
		
		/** Set a new dictionary manager
		 *  @param newDictManager the new dictionary manager
		 */
		void setDictManager(DictManager * newDictManager);
		
		/** Partially enables the available settings
		 *  @param onlyEditor if true, enables only editor settings, if false,
		 *         all settings will be enabled
		 */
		void enableOnlyEditorSetup(bool onlyEditor);
	public slots:
		/** Update configuration choices from dictionary manager */
		void updateFromDictManager();
	private slots:
		/** Updates the dictionary manager to use the settings
		 *  specified in this pane.
		 */
		void updateDictionaryState();
		
		/** Updates the editor to use the settings specified in this pane */
		void updateEditorSettings();
	private:
		/** The spell checker */
		SpellChecker * spellChecker;
		
		/** The dictionary manager */
		DictManager * dictManager;
		
		/** The group box containing the dictinary settings */
		QGroupBox * dictSettingsGB;
		
		/** The layout containing the dictionary settings */
		QVBoxLayout * dictSettingsLayout;
		
		/** The checkboxes for dictionary settings referenced by their names */
		QHash<QString, QCheckBox *> dictSettings;
		
		/* The editor settings group box and the check boxes inside it */
		QGroupBox * editorSettings;
		QCheckBox * ignoreWordsWithNumbersCB;
		QCheckBox * ignoreUppercaseCB;
		QCheckBox * acceptFirstUppercaseCB;
};

#endif
