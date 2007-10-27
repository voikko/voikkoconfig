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
		emit settingsChanged();
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
	emit settingsChanged();
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
	wchar_t * buffer;
	wchar_t * startp;
	int blen;
	size_t tokenlen;
	enum voikko_token_type tokenType;
	
	if (startind < 0 || startind >= slen) return false;
	if (!isInitialised) return false;
	
	buffer = (wchar_t *) malloc(4 * (slen - startind) + 1);
	if (!buffer) return false;
	
	blen = string.mid(startind).toWCharArray(buffer);
	startp = buffer;
	*length = 0;
	voikkoMutex.lock();
	while (true) {
		tokenType = voikko_next_token_ucs4(startp, blen, &tokenlen);
		switch (tokenType) {
			case TOKEN_NONE:
				voikkoMutex.unlock();
				free(buffer);
				return false;
			case TOKEN_WORD:
				voikkoMutex.unlock();
				(*length) += tokenlen;
				free(buffer);
				return true;
			case TOKEN_PUNCTUATION:
			case TOKEN_WHITESPACE:
				blen -= tokenlen;
				startp += tokenlen;
				(*start) += tokenlen;
				(*length) += tokenlen;
		}
	}
}

bool SpellChecker::isLetter(QChar c) {
	if (c.isLetter()) return true; // Ordinary Unicode letter
	else return false;
}

SpellChecker::~SpellChecker() {
	uninitialise();
}
