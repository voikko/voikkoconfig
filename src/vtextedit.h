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
	void contextMenuEvent(QContextMenuEvent * event);
	private:
	SpellChecker * spellChecker;
	VSyntaxHighlighter * highlighter;
	QSignalMapper * selectSuggestionMapper;
	QTextCursor misspelledSelection;
	private slots:
	void suggestionSelected(QString suggestion);
};

#endif
