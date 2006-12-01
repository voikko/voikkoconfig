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

#ifndef VSYNTAXHIGHLIGHTER_H
#define VSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include "spellchecker.h"

/** Available syntax highlighting types */
enum HighlightType { HLTYPE_TEXT, HLTYPE_HTML, HLTYPE_LATEX };

class VSyntaxHighlighter : public QSyntaxHighlighter {
	Q_OBJECT
	public:
	/** Constructs a new spell checking syntax highlighter
	 *  @param checker the spell checker to use
	 *  @param parent the text editor that will use this highlighter
	 */
	VSyntaxHighlighter(SpellChecker * checker, QTextEdit * parent);
	
	/** Method overridden from QSyntaxHighlighter to update the highlighted parts
	 *  in a changed block
	 *  @param text the text that needs to be rehighlighted
	 */
	void highlightBlock(const QString & text);
	
	/** Sets a new highlighting type
	 *  @param ht the new highlighting type
	 */
	void setHighlightType(HighlightType ht);
	
	private:
	/** the current highlighting type */
	HighlightType type;
	
	/** the spellchecker */
	SpellChecker * spellChecker;
	
	/** text format for the normal text */
	QTextCharFormat defaultFormat;
	
	/** text format for misspelled text */
	QTextCharFormat misspelledFormat;
	
	/** text format for text ignored due to syntax highlighting */
	QTextCharFormat ignoreFormat;
	
	/** Finds the next nontext (to be ignored in spellchecking) part
	 *  according to current syntax highlighting rules.
	 *  @param text string to search from
	 *  @param start index of the position from which the search should be started
	 *  @return the index of the nontext start or -1, if none is found
	 */
	int findNextNontextStart(const QString & text, int start);
	
	/** Fins the end of the current nontex (to be ignored in spellchecking) part
	 *  according to current syntax highlighting rules.
	 *  @param text string to search from
	 *  @param start index of the position from which the search should be started
	 *  @return the index of the nontext end or -1, if none is found
	 */
	int findNextNontextEnd(const QString & text, int start);
	
	/** Highlights the spelling errors from the given text segment, which is assumed
	 *  not to contain anything that should be ignored according to syntax highlighting
	 *  rules.
	 *  @param text string that contains the text to check
	 *  @param start index to start checking from
	 *  @param end index of the end of the segment to check
	 */
	void markErrors(const QString & text, int start, int end);
};

#endif
