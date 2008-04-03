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

#include <QMenu>
#include <QTextDocument>

#include "vtextedit.h"

VTextEdit::VTextEdit(SpellChecker * sc): QTextEdit() {
	spellChecker = sc;
	document()->setDefaultFont(QFont("Courier", 12));
	
	highlighter = new VSyntaxHighlighter(sc, this);
	
	selectSuggestionMapper = new QSignalMapper(this);
	connect(selectSuggestionMapper, SIGNAL(mapped(QString)),
	        this, SLOT(suggestionSelected(QString)));
}

void VTextEdit::setHighlightType(HighlightType ht) {
	highlighter->setHighlightType(ht);
}

void VTextEdit::contextMenuEvent(QContextMenuEvent * event) {
	QTextCursor cursor = cursorForPosition(event->pos());
	QTextBlock block = cursor.block();
	QString blockText = block.text();
	int clickPosition = cursor.position() - block.position();
	
	int wstart = 0;
	int wlength = 0;
	QString misspelled = QString();
	while (spellChecker->findNextWord(blockText, &wstart, &wlength)) {
		if (wstart > clickPosition) break;
		if (wstart + wlength >= clickPosition) {
			if (!spellChecker->checkWord(blockText.mid(wstart, wlength)))
				misspelled = blockText.mid(wstart, wlength);
			break;
		}
		wstart += wlength;
	}
	
	QMenu * menu;
	if (misspelled.isEmpty()) {
		menu = createStandardContextMenu();
	}
	else {
		menu = new QMenu();
		QAction * suggAction;
		QStringList suggestions = spellChecker->suggestions(misspelled);
		for (int i = 0; i < suggestions.size(); i++) {
			suggAction = new QAction(suggestions[i], menu);
			connect(suggAction, SIGNAL(triggered()), selectSuggestionMapper, SLOT(map()));
			selectSuggestionMapper->setMapping(suggAction, suggestions[i]);
			menu->addAction(suggAction);
		}
		misspelledSelection = cursor;
		misspelledSelection.setPosition(block.position() + wstart, QTextCursor::MoveAnchor);
		misspelledSelection.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, wlength);
	}
	
	menu->exec(event->globalPos());
	delete menu;
}

void VTextEdit::suggestionSelected(QString suggestion) {
	misspelledSelection.insertText(suggestion);
}
