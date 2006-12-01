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

#ifndef DICTMANAGER_H
#define DICTMANAGER_H

#include <QStandardItemModel>
#include <QTextStream>

/** Boolean mallex option */
class MallexBOption {
	public:
	/** Constructs a new boolean mallex option
	 * @param opName the name of the option
	 * @param optDescription the description of the option
	 * @param value the value of the option
	 */
	MallexBOption(QString optName, QString optDescription, bool optValue);
	
	/** the name of the option */
	QString name;
	
	/** the description of the option */
	QString description;
	
	/** the value of the option */
	bool value;
};

class DictManager : public QStandardItemModel {
	Q_OBJECT
	public:
	/** Initialise the dictionary manager */
	DictManager();
	
	/** Loads dictionary information from a project file
	 *  @param projectfile name of the file from which the project will be loaded
	 *  @return error message, if loading failed, otherwise a null QString
	 */
	QString load(QString projectfile);
	
	/** Save dictionary information to a project file
	 *  @param projectfile name of the file to be used for saving
	 *  @return error message, if saving failed, otherwise a null QString
	 */
	QString save(QString projectfile);
	
	/** Add a word list to this dictionary
	 *  @param path path to the new word list
	 */
	void addWordList(QString path);
	
	/** Runs malmake on the current project file.
	 *  @param parent parent widget for progress dialog
	 *  @return true, if there were no errors, otherwise false
	 */
	bool runMalmake(QWidget * parent);
	
	/** Get the directory of the current project file
	 *  @return the directory of the current project file
	 */
	QString getProjectDirectory();
	
	/** Get the specified boolean mallex option
	 *  @param optionName name of the mallex option
	 *  @return pointer to the option or null, if no such option exists
	 */
	const MallexBOption * getMallexBOption(QString optionName);
	
	/** Get all boolean mallex options
	 *  @return the list of boolean mallex options
	 */
	QList<MallexBOption> getMallexBOptions();
	
	/** Set the specified boolean mallex option
	 *  @param optionName name of the mallex option
	 *  @param optionValue value of the mallex option
	 *  @return false, if the option name is unknown, otherwise true
	 */
	bool setMallexBOption(QString optionName, bool optionValue);
	
	/** Mark the configuration changed due to external reasons like
	 *  updates to some word lists.
	 */
	void markConfigurationChanged();
	
	/** Has the configuration changed after project loading or saving?
	 *  @return true, if configuration has changed, otherwise false
	 */
	bool hasConfigurationChanged();
	
	/** Get the update source URI for this dictionary
	 *  @return the update source URI, or a null QString if none has been set.
	 */
	QString getUpdateURI();
	
	/** Set the update source URI
	 *  @param value the new update source URI
	 */
	void setUpdateURI(QString value);
	
	private:
	/** Name of the current project file */
	QString projectFileName;
	
	/** Language code of the project file */
	QString languageCode;
	
	/** Language variant or dialect */
	QString languageVariant;
	
	/** Short description of the project file */
	QString description;
	
	/** Authors and other copyright holders */
	QString copyright;
	
	/** License of the project file */
	QString license;
	
	/** URI for word list update index */
	QString updateURI;
	
	/** Lex version string */
	QString lexVersion;
	
	/** Value of the first lex entry in the project file */
	QString mainLexLine;
	
	/** Mallex options */
	QList<MallexBOption> mallexBOptions;
	
	/** Internal configuration data from the project file */
	QString internalConfiguration;
	
	/** Has the configuration changed after project loading or saving? */
	bool configurationChanged;
	
	
	/** Parse an 'info: ...' entry
	 *  @param line text line to parse
	 *  @param name reference to the variable where the name of the entry will be stored
	 *  @param value reference to the variable where the value of the entry will be stored
	 *  @return true, if parsing succeeded, otherwise false
	 */
	bool readInfoToken(QString line, QString &name, QString &value);
	
	/** Parse a '##...' header
	 *  @param line text line to parse
	 *  @param header reference to the variable where name of the header will be stored
	 *  @return true, if parsing succeeded, otherwise false
	 */
	bool readHeader(QString line, QString &header);
	
	/** Parse a general malaga project file item
	 *  @param line text line to parse
	 *  @param name reference to the variable where the name of the item will be stored
	 *  @param value reference to the variable where the value of the item will be stored
	 *  @return true, if parsing succeeded, otherwise false
	 */
	bool readConfigurationItem(QString line, QString &name, QString &value);
	
	/** Parse a 'set switch' statement
	 *  @param line text line to parse
	 *  @param name reference to the variable where the name of the switch will be stored
	 *  @param value reference to the variable where the value of the switch will be stored
	 *  @return true, if parsing succeeded, otherwise false
	 */
	bool readSwitchStatement(QString line, QString &option, QString &value);
	
	/** Check if a line contains interpretable data (not just comments or whitespace)
	 *  @param line text line to check
	 *  @return true, if there is data on this line, otherwise false
	 */
	bool hasData(QString line);
	
	/** Read the data stream to check if the dictionary version is compatible with
	 *  this version of voikkoconfig. The stream will be read until the versions string
	 *  has been parsed.
	 *  @param stream text stream to read
	 *  @return true, if correct version was specified, otherwise false
	 */
	bool checkDictVersion(QTextStream * stream);
	
	/** Read the malaga configuration block from the data stream
	 *  @param stream text stream to read
	 *  @return true, if there were no errors, otherwise false
	 */
	bool readMalagaConfiguration(QTextStream * stream);
	
	/** Read the user configuration block from the data stream
	 *  @param stream text stream to read
	 *  @return true, if there were no errors, otherwise false
	 */
	bool readUserConfiguration(QTextStream * stream);
	
	/** Read the internal configuration block from the data stream
	 *  @param stream text stream to read
	 *  @return true, if there were no errors, otherwise false
	 */
	bool readInternalConfiguration(QTextStream * stream);
	
	/** Writes a boolean switch to the data stream.
	 *  @param stream text stream to write
	 *  @param type type of the switch, should be "mallex" or "malaga"
	 *  @param name name of the switch
	 *  @param value value of the switch
	 */
	void writeBSwitch(QTextStream * stream, QString type, QString name, bool value);
	
	/** Writes the main lex file for this project
	 *  @return true, if there were no errors, otherwise false
	 */
	bool writeMainLexFile();
	
	private slots:
	/** One of the word list items has changed
	 *  @param item the changed item
	 */
	void wordlistItemChanged(QStandardItem * item);
};

#endif
