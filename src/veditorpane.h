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

#ifndef VEDITORPANE_H
#define VEDITORPANE_H

#include <QPushButton>
#include <QRadioButton>

#include "spellchecker.h"
#include "vtextedit.h"

/** Widget containing a simple spellchecking text editor and related controls */
class VEditorPane : public QWidget {
	Q_OBJECT
	public:
		/** Constructs a new editor pane
		 *  @param sc the spell checker that will be used to check the spelling
		 *         in the editor
		 */
		VEditorPane(SpellChecker * sc);
	public slots:
		/** Opens a text file in the editor */
		void openFile();
		
		/** Saves the text in the editor to a file */
		void saveFile();
		
		/** Sets the editor highlight type to match the currently selected type */
		void setHlType();
	private:
		/** The text editor */
		VTextEdit * editor;
		
		/** The widget containing the open and save buttons */
		QWidget * buttonbar;
		
		/** The button for opening a text file */
		QPushButton * openButton;
		
		/** The button for saving current text to a file */
		QPushButton * saveButton;
		
		/** Plain text mode radio button */
		QRadioButton * hlText;
		
		/** Html mode radio button */
		QRadioButton * hlHtml;
		
		/** LaTeX mode radio button */
		QRadioButton * hlLatex;
};

#endif
