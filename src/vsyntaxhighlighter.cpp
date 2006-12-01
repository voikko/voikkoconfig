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

#include "vsyntaxhighlighter.h"

VSyntaxHighlighter::VSyntaxHighlighter(SpellChecker * checker, QTextEdit * parent) :
	QSyntaxHighlighter(parent) {
	spellChecker = checker;
	misspelledFormat.setForeground(QColor("red"));
	ignoreFormat.setForeground(QColor("gray"));
	type = HLTYPE_TEXT;
}

void VSyntaxHighlighter::highlightBlock(const QString & text) {
	int textS = 0;
	int textLen = text.length();
	int nontextS;
	int nontextE;
	while (true) {
		nontextS = findNextNontextStart(text, textS);
		if (nontextS != -1) {
			markErrors(text, textS, nontextS);
			nontextE = findNextNontextEnd(text, nontextS);
			if (nontextE == -1) {
				setFormat(nontextS, textLen - nontextS, ignoreFormat);
				return;
			}
			else {
				setFormat(nontextS, nontextE + 1 - nontextS, ignoreFormat);
				textS = nontextE + 1;
				if (textS >= textLen) return;
			}
		}
		else {
			markErrors(text, textS, textLen);
			return;
		}
	}
}

void VSyntaxHighlighter::setHighlightType(HighlightType ht) {
	type = ht;
	rehighlight();
}

int VSyntaxHighlighter::findNextNontextStart(const QString & text, int start) {
	switch (type) {
		case HLTYPE_TEXT:
			return -1;
		case HLTYPE_HTML:
			return text.indexOf("<", start);
		case HLTYPE_LATEX:
			return text.indexOf("\\", start);
	}
	return -1; // impossible
}

int VSyntaxHighlighter::findNextNontextEnd(const QString & text, int start) {
	switch (type) {
		case HLTYPE_TEXT:
			return -1;
		case HLTYPE_HTML:
			return text.indexOf(">", start);
		case HLTYPE_LATEX:
			return text.indexOf("}", start);
	}
	return -1; // impossible
}

void VSyntaxHighlighter::markErrors(const QString & text, int start, int end) {
	int length;
	while (spellChecker->findNextWord(text, &start, &length)) {
		if (start > end) return;
		if (start + length > end) length = end - start;
		if (!spellChecker->checkWord(text.mid(start, length))) {
			setFormat(start, length, misspelledFormat);
		}
		start += length;
	}
}
