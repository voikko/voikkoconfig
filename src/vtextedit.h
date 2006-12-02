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

#ifndef VTEXTEDIT_H
#define VTEXTEDIT_H

#include <QTextEdit>
#include <QContextMenuEvent>
#include <QSignalMapper>

#include "spellchecker.h"
#include "vsyntaxhighlighter.h"

/** A spellchecking text editor component */
class VTextEdit : public QTextEdit {
	Q_OBJECT
	public:
	/** Creates a new VTextEdit
	 *  @param sc The spellchecker that will be used to check misspelled words
	 */
	VTextEdit(SpellChecker * sc);
	
	/** Sets the syntax highlight type for this editor
	 *  @param ht The new higlight type
	 */
	void setHighlightType(HighlightType ht);
	
	protected:
	/** Event handler for right mouse clicks on this text editor
	 *  @param event the context menu event
	 */
	void contextMenuEvent(QContextMenuEvent * event);
	
	private:
	/** The associated spell checker */
	SpellChecker * spellChecker;
	
	/** The syntax highlighter for this text editor */
	VSyntaxHighlighter * highlighter;
	
	/** Signal mapper used to add string parameter to the spelling suggestion
	 *  context menu items events.
	 */
	QSignalMapper * selectSuggestionMapper;
	
	/** Contains the misspelled word when user has used right mouse click to
	 *  bring up a context menu containing the spelling suggestions.
	 */
	QTextCursor misspelledSelection;
	
	private slots:
	/** Replace the selected misspelled word with a suggested correction
	 *  @param suggestion the corrected spelling
	 */
	void suggestionSelected(QString suggestion);
};

#endif
