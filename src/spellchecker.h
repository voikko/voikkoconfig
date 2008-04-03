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

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QMutex>

/** Spellchecker offering spellchecking services using Voikko */
class SpellChecker : public QObject {
	Q_OBJECT
	public:
	/** Constructs a spellchecker but leaves it in the uninitialised state */
	SpellChecker();
	
	/** Initialise the spellchecker.
	 * @param path Primary dictionary search directory. If path is a null string,
	 *        user (or system) default dictionary will be used.
	 * @return Error message, if the initialisation fails, null string on success.
	 */
	QString initialise(QString path = QString());
	
	/** Uninitialises the spellchecker */
	void uninitialise();
	
	/** Checks if a word is valid or not.
	 *  @param word Word to check
	 *  @return True, if the word is correctly spelled, otherwise false.
	 */
	bool checkWord(QString word);
	
	/** Returns a list of spelling suggestions for a word
	 *  @param word Word to search spelling suggestions for
	 *  @return List of spelling suggestions
	 */
	QStringList suggestions(QString word);
	
	/** Find the next whole word from a string.
	 *  @param string String from which the word will be searched
	 *  @param start Index to start the search from. If a word is found, this
	 *         will be set to its start index.
	 *  @param length If a word is found, lenght will be set to contain its length.
	 *  @return True, if a word is found, otherwise false.
	 */
	bool findNextWord(QString string, int * start, int * length);
	
	/** Defines whether words containing numbers should be ignored during
	 *  the spellchecking.
	 *  @param value Value for option VOIKKO_OPT_IGNORE_NUMBERS
	 */
	void setIgnoreWordsWithNumbers(bool value);
	
	/** Defines whether words written completely in uppercase should be
	 *  automatically accepted.
	 *  @param value Value for option VOIKKO_OPT_IGNORE_UPPERCASE
	 */
	void setIgnoreUppercase(bool value);
	
	/** Defines whether it is allowed to captialise the first character of
	 *  a word when the canonical form is written in lowercase
	 *  @param value Value for option VOIKKO_OPT_ACCEPT_FIRST_UPPERCASE
	 */
	void setAcceptFirstUppercase(bool value);
	
	/** Checks wheter this spellchecker is initialised
	 *  @return True, if spellchecker is initialised, otherwise false
	 */
	bool initialised() { return isInitialised; }
	
	/** Destructor */
	~SpellChecker();
	
	signals:
	/** Spellchecker settings have chenged */
	void settingsChanged();
	
	private:
	/** Libvoikko spellchecker handle */
	int voikkoHandle;
	
	/** Initialisation status of the spellchecker */
	bool isInitialised;
	
	/** Mutex used to serialise the spellchecker operations */
	QMutex voikkoMutex;
	
	/** Is the character a letter as interpreted by Voikko
	 *  @param c Character to check
	 *  @return True, if c is a letter, otherwise false
	 */
	bool isLetter(QChar c);
	
	/** Set the boolean Voikko option
	 *  @param option Option code
	 *  @param value Option value
	 */
	void setVoikkoBoolOption(int option, bool value);
};

#endif
