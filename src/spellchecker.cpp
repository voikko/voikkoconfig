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

#include <libvoikko/voikko.h>

#include "spellchecker.h"

SpellChecker::SpellChecker() {
	isInitialised = false;
}

QString SpellChecker::initialise(QString path) {
	if (isInitialised) uninitialise();
	const char * voikko_error;
	voikkoMutex.lock();
	if (path.isNull()) voikko_error = voikko_init(&voikkoHandle, "fi_FI", 0);
	else voikko_error = voikko_init_with_path(&voikkoHandle, "fi_FI", 0, qPrintable(path));
	voikkoMutex.unlock();
	if (voikko_error) {
		isInitialised = false;
		return QString::fromUtf8(voikko_error);
	}
	else {
		voikkoMutex.lock();
		voikko_set_bool_option(voikkoHandle, VOIKKO_OPT_IGNORE_DOT, 1);
		voikkoMutex.unlock();
		isInitialised = true;
		return QString();
	}
}

void SpellChecker::uninitialise() {
	if (isInitialised) {
		voikkoMutex.lock();
		voikko_terminate(voikkoHandle);
		isInitialised = false;
		voikkoMutex.unlock();
	}
}

void SpellChecker::setVoikkoBoolOption(int option, bool value) {
	if (!isInitialised) return;
	voikkoMutex.lock();
	if (value) voikko_set_bool_option(voikkoHandle, option, 1);
	else voikko_set_bool_option(voikkoHandle, option, 0);
	voikkoMutex.unlock();
}

void SpellChecker::setIgnoreWordsWithNumbers(bool value) {
	setVoikkoBoolOption(VOIKKO_OPT_IGNORE_NUMBERS, value);
}

void SpellChecker::setIgnoreUppercase(bool value) {
	setVoikkoBoolOption(VOIKKO_OPT_IGNORE_UPPERCASE, value);
}

void SpellChecker::setAcceptFirstUppercase(bool value) {
	setVoikkoBoolOption(VOIKKO_OPT_ACCEPT_FIRST_UPPERCASE, value);
}

bool SpellChecker::checkWord(QString word) {
	int spellresult;
	if (!isInitialised) return false;
	voikkoMutex.lock();
	spellresult = voikko_spell_cstr(voikkoHandle, word.toUtf8().data());
	voikkoMutex.unlock();
	if (spellresult == VOIKKO_SPELL_OK) return true;
	else return false;
}

QStringList SpellChecker::suggestions(QString word) {
	char ** suggestions;
	QStringList suggestionList;
	if (!isInitialised) return suggestionList;
	voikkoMutex.lock();
	suggestions = voikko_suggest_cstr(voikkoHandle, word.toUtf8().data());
	voikkoMutex.unlock();
	if (!suggestions) return suggestionList;
	for (char ** suggPtr = suggestions; *suggPtr; suggPtr++) {
		suggestionList << QString::fromUtf8(*suggPtr);
		free(*suggPtr);
	}
	free(suggestions);
	return suggestionList;
}

bool SpellChecker::findNextWord(QString string, int * start, int * length) {
	int slen = string.length();
	int startind = *start;
	if (startind < 0 || startind >= slen) return false;
	
	// Find the start of the word
	while (startind < slen) {
		if (isLetter(string[startind])) break;
		if (string[startind] == '-' && startind + 1 < slen &&
		    isLetter(string[startind + 1])) break;
		if (string[startind].isNumber() && startind + 1 < slen &&
		    isLetter(string[startind])) break;
		startind++;
	}
	if (startind == slen) return false;
	
	// Find the end of the word
	int endind = startind + 1;
	while (endind < slen) {
		if (isLetter(string[endind]) || string[endind].isNumber()) {
			endind++;
			continue;
		}
		if (string[endind] == '-' ||
		    string[endind] == QChar(0x10, 0x20) /* HYPHEN */ ||
		    string[endind] == QChar(0x11, 0x20) /* NON-BREAKING HYPHEN */) {
			if (endind + 1 == slen || string[endind + 1].isSpace()) {
				endind++;
				break;
			}
			if (isLetter(string[endind + 1]) || string[endind + 1].isNumber()) {
				endind++;
				continue;
			}
			break;
		}
		if (string[endind] == '.') {
			if (endind + 1 == slen || string[endind + 1].isSpace()) {
				endind++;
				break;
			}
			break;
		}
		if (string[endind] == ':' || string[endind] == '\'' ||
		    string[endind] == QChar(0x19, 0x20) /* RIGHT SINGLE QUOTATION MARK */) {
			if (endind + 1 < slen && isLetter(string[endind + 1])) {
				endind++;
				continue;
			}
		}
		break;
	}
	
	*start = startind;
	*length = endind - startind;
	return true;
}

bool SpellChecker::isLetter(QChar c) {
	if (c.isLetter()) return true; // Ordinary Unicode letter
	else return false;
}

SpellChecker::~SpellChecker() {
	uninitialise();
}
