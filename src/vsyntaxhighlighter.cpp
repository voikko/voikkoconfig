/* Voikkoconfig: Configuration tool for Finnish spellchecker Voikko
 * Copyright (C) 2006 - 2008 Harri Pitkänen <hatapitk@iki.fi>
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

#include "vsyntaxhighlighter.h"

#include <QBitArray>

VSyntaxHighlighter::VSyntaxHighlighter(SpellChecker * checker, QTextEdit * parent) :
	QSyntaxHighlighter(parent) {
	spellChecker = checker;
	spellingErrorFormat.setForeground(QColor("red"));
	grammarErrorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	grammarErrorFormat.setUnderlineColor(QColor("green"));
	spellingGrammarErrorFormat.merge(spellingErrorFormat);
	spellingGrammarErrorFormat.merge(grammarErrorFormat);
	ignoreFormat.setForeground(QColor("gray"));
	type = HLTYPE_TEXT;
	connect(spellChecker, SIGNAL(settingsChanged()), this, SLOT(rehighlight()));
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
			/* Usually \ marks the start of text to ignore. In a command that contains
			 * ordinary text we also want to mark ending } with LaTeX command colour
			 * for consistency. */
			int cmdstart = text.indexOf("\\", start);
			int cmdendchar = text.indexOf("}", start);
			if (cmdstart != -1 && cmdendchar != -1)
				return (cmdstart < cmdendchar) ? cmdstart : cmdendchar;
			else return (cmdstart > cmdendchar) ? cmdstart : cmdendchar;
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
			if (text[start] == '}') return start;
			if (text.indexOf("\\section", start) == start ||
			    text.indexOf("\\subsection", start) == start)
				return text.indexOf("{", start);
			else return text.indexOf("}", start);
	}
	return -1; // impossible
}

void VSyntaxHighlighter::markErrors(const QString & text, int start, int end) {
	int textLen = text.length();
	QBitArray spellingErrors(textLen);
	QBitArray grammarErrors(textLen);
	int length;
	int first = start;
	
	// Find spelling errors
	while (spellChecker->findNextWord(text, &first, &length)) {
		if (first > end) break;
		if (first + length > end) length = end - first;
		if (!spellChecker->checkWord(text.mid(first, length))) {
			spellingErrors.fill(true, first, first + length);
		}
		first += length;
	}
	
	// Find grammar errors
	int skip = 0;
	while (spellChecker->findNextGrammarError(text, skip++, &first, &length)) {
		if (first > end) break;
		if (first + length > end) length = end - first;
		grammarErrors.fill(true, first, first + length);
	}
	
	// Mark all errors
	for (int i = 0; i < textLen; i++) {
		if (spellingErrors[i]) {
			if (grammarErrors[i]) setFormat(i, 1, spellingGrammarErrorFormat);
			else setFormat(i, 1, spellingErrorFormat);
		}
		else {
			if (grammarErrors[i]) setFormat(i, 1, grammarErrorFormat);
		}
	}
}
