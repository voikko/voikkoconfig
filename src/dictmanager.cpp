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

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

#include "dictmanager.h"
#include "utils.h"

MallexBOption::MallexBOption(QString optName, QString optDescription, bool optValue) {
	name = optName;
	description = optDescription;
	value = optValue;
}

DictManager::DictManager() {
	/* Set the default values for a new dictionary */
	languageCode = QString("fi_FI");
	languageVariant = QString("standard");
	description = QString("(no description)");
	copyright = QString("(no copyright information)");
	license = QString("(no license information)");
	mainLexLine = QString("voikko-fi_FI.lex suomi.lex");
	updateURI = QString();
	lexVersion = QString("");
	mallexBOptions << MallexBOption("vanhahkot_muodot",
		tr("Accept old style inflections"), true);
	mallexBOptions << MallexBOption("vanhat_muodot",
		tr("Accept old inflections"), false);
	internalConfiguration = QString("");
	configurationChanged = false;
	connect(this, SIGNAL(itemChanged(QStandardItem*)),
	        this, SLOT(wordlistItemChanged(QStandardItem*)));
}

QString DictManager::load(QString projectfile) {
	QFile file(projectfile);
	QString currentLine;
	QString error;
	QString name;
	QString value;
	if (file.open(QFile::ReadOnly)) {
		projectFileName = projectfile;
		QTextStream inputStream(&file);
		inputStream.setCodec("UTF-8");
		if (!checkDictVersion(&inputStream)) {
			file.close();
			return tr("Project file failed version check");
		}
		if (!readMalagaConfiguration(&inputStream)) {
			file.close();
			return tr("Error reading malaga configuration from project file");
		}
		if (!readUserConfiguration(&inputStream)) {
			file.close();
			return tr("Error reading user configuration from project file");
		}
		if (!readInternalConfiguration(&inputStream)) {
			file.close();
			return tr("Error reading internal configuration from project file");
		}
		file.close();
		configurationChanged = false;
		return QString();
	}
	else return tr("Failed to open the specified file for reading");
}

QString DictManager::save(QString projectfile) {
	QFile file(projectfile);
	if (file.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream outputStream(&file);
		outputStream.setCodec("UTF-8");
		
		outputStream << "info: Voikko-Dictionary-Format: 1\n\n";
		
		outputStream << "## Begin-Malaga-Configuration\n";
		outputStream << "info: Language-Code: " << languageCode << "\n";
		outputStream << "info: Language-Variant: " << languageVariant << "\n";
		outputStream << "info: Description: " << description << "\n";
		outputStream << "info: Copyright: " << copyright << "\n";
		outputStream << "info: License: " << license << "\n";
		if (!updateURI.isNull()) outputStream << "info: Update-URI: " << updateURI << "\n";
		outputStream << "info: Lex-Version: " << lexVersion << "\n";
		outputStream << "## End-Malaga-Configuration\n\n";
		
		outputStream << "## Begin-User-Configuration\n";
		outputStream << "lex: " << mainLexLine << "\n";
		for (int i = 0; i < rowCount(); i++) {
			outputStream << "lex: ";
			if (item(i)->checkState() == Qt::Unchecked) outputStream << "#";
			outputStream << item(i)->text() << "\n";
		}
		QList<MallexBOption> mbOptions = getMallexBOptions();
		for (int i = 0; i < mbOptions.size(); i++)
			writeBSwitch(&outputStream, "mallex", mbOptions[i].name,
			             mbOptions[i].value);
		outputStream << "## End-User-Configuration\n\n";
		
		outputStream << "## Begin-Internal-Configuration\n";
		outputStream << internalConfiguration;
		outputStream << "## End-Internal-Configuration\n\n";
		
		file.close();
		
		if (!writeMainLexFile()) return tr("Failed to write the main lex file");
		
		projectFileName = projectfile;
		configurationChanged = false;
		return QString();
	}
	else return tr("Failed to open the specified file for writing");
}

bool DictManager::readInfoToken(QString line, QString &name, QString &value) {
	QString str = line.trimmed();
	if (!str.startsWith("info:")) return false;
	str = line.mid(5).trimmed();
	int nameEnd = str.indexOf(':');
	if (nameEnd == -1) return false;
	name = str.left(nameEnd).trimmed();
	if (nameEnd == str.length() - 1) value = QString("");
	else value = str.mid(nameEnd + 1).trimmed();
	return true;
}

bool DictManager::readConfigurationItem(QString line, QString &name, QString &value) {
	QString str = line.trimmed();
	int nameEnd = str.indexOf(':');
	if (nameEnd == -1) return false;
	name = str.left(nameEnd).trimmed();
	if (nameEnd == str.length() - 1) value = QString("");
	else value = str.mid(nameEnd + 1).trimmed();
	return true;
}

bool DictManager::readSwitchStatement(QString line, QString &option, QString &value) {
	QString str = line.trimmed();
	if (!str.startsWith("set switch ")) return false;
	str = line.mid(11).trimmed();
	int nameEnd = str.indexOf(' ');
	if (nameEnd == -1) return false;
	option = str.left(nameEnd).trimmed();
	if (nameEnd == str.length() - 1) value = QString("");
	else value = str.mid(nameEnd + 1).trimmed();
	return true;
}

bool DictManager::readHeader(QString line, QString &header) {
	QString str = line.trimmed();
	if (!str.startsWith("##")) return false;
	header = str.mid(2).trimmed();
	return true;
}

bool DictManager::hasData(QString line) {
	if (line.isEmpty()) return false;
	if (line[0] != '#') return true;
	if (line.length() == 1) return false;
	if (line[0] == '#') return true;
	return false;
}

bool DictManager::checkDictVersion(QTextStream * stream) {
	QString line;
	QString name;
	QString value;
	while (!stream->atEnd()) {
		line = stream->readLine();
		if (hasData(line)) {
			if (readInfoToken(line, name, value)) {
				if (name == "Voikko-Dictionary-Format" && value == "1")
					return true;
				else return false;
			}
			else return false;
		}
	}
	return false;
}

bool DictManager::readMalagaConfiguration(QTextStream * stream) {
	QString line;
	QString header;
	QString name;
	QString value;
	bool startHeaderRead = false;
	while (!stream->atEnd()) {
		line = stream->readLine();
		if (hasData(line)) {
			if (readHeader(line, header)) {
				if (header == "Begin-Malaga-Configuration") {
					startHeaderRead = true;
					break;
				}
				else return false;
			}
			else return false;
		}
	}
	if (!startHeaderRead) return false;
	while (!stream->atEnd()) {
		line = stream->readLine();
		if (hasData(line)) {
			if (readInfoToken(line, name, value)) {
				if (name == "Language-Code") languageCode = value;
				else if (name == "Language-Variant") languageVariant = value;
				else if (name == "Description") description = value;
				else if (name == "Copyright") copyright = value;
				else if (name == "License") license = value;
				else if (name == "Update-URI") updateURI = value;
				else if (name == "Lex-Version") lexVersion = value;
				else return false;
			}
			else if (readHeader(line, header)) {
				if (header == "End-Malaga-Configuration") return true;
				else return false;
			}
			else return false;
		}
	}
	return false;
}

bool DictManager::readUserConfiguration(QTextStream * stream) {
	QString line;
	QString header;
	QString name;
	QString value;
	QString optname;
	QString optvalue;
	bool optBoolValue;
	
	/* Remove old items from dictionary list */
	clear();
	mainLexLine = QString();
	
	bool startHeaderRead = false;
	while (!stream->atEnd()) {
		line = stream->readLine();
		if (hasData(line)) {
			if (readHeader(line, header)) {
				if (header == "Begin-User-Configuration") {
					startHeaderRead = true;
					break;
				}
				else return false;
			}
			else return false;
		}
	}
	if (!startHeaderRead) return false;
	while (!stream->atEnd()) {
		line = stream->readLine();
		if (hasData(line)) {
			if (readConfigurationItem(line, name, value)) {
				if (name == "lex") {
					if (mainLexLine.isEmpty()) mainLexLine = value;
					else addWordList(value);
				}
				else if (name == "mallex") {
					if (readSwitchStatement(value, optname, optvalue)) {
						if (optvalue == "yes")
							optBoolValue = true;
						else if (optvalue == "no")
							optBoolValue = false;
						else return false;
						if (!setMallexBOption(optname, optBoolValue))
							return false;
					}
					else return false;
				}
				else return false;
			}
			else if (readHeader(line, header)) {
				if (header == "End-User-Configuration") return true;
				else return false;
			}
			else return false;
		}
	}
	return false;
}

bool DictManager::readInternalConfiguration(QTextStream * stream) {
	QString line;
	QString header;
	bool startHeaderRead = false;
	internalConfiguration = QString("");
	while (!stream->atEnd()) {
		line = stream->readLine();
		if (hasData(line)) {
			if (readHeader(line, header)) {
				if (header == "Begin-Internal-Configuration") {
					startHeaderRead = true;
					break;
				}
				else return false;
			}
			else return false;
		}
	}
	if (!startHeaderRead) return false;
	while (!stream->atEnd()) {
		line = stream->readLine();
		if (readHeader(line, header)) {
			if (header == "End-Internal-Configuration") return true;
			else return false;
		}
		internalConfiguration.append(line).append("\n");
	}
	return false;
}

void DictManager::writeBSwitch(QTextStream * stream, QString type, QString name, bool value) {
	*stream << type << ": set switch " << name;
	if (value) *stream << " yes\n";
	else *stream << " no\n";
}

void DictManager::addWordList(QString path) {
	bool isActive = true;
	if (path.startsWith("#")) {
		path = path.mid(1);
		isActive = false;
	}
	QString relativePath = QFileInfo(projectFileName).dir().relativeFilePath(path);
	QStandardItem * item = new QStandardItem(relativePath);
	item->setCheckable(true);
	if (isActive) item->setCheckState(Qt::Checked);
	else item->setCheckState(Qt::Unchecked);
	appendRow(item);
	configurationChanged = true;
}

bool DictManager::writeMainLexFile() {
	int lexNameEnd = mainLexLine.indexOf(' ');
	if (lexNameEnd == -1) return false;
	QString mainFile = mainLexLine.left(lexNameEnd).trimmed();
	QString internalFile = mainLexLine.mid(lexNameEnd + 1).trimmed();
	QString absMainFile = QFileInfo(projectFileName).dir().filePath(mainFile);
	QFile file(absMainFile);
	if (file.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream outputStream(&file);
		outputStream.setCodec("UTF-8");
		outputStream << "include \"" << internalFile << "\";\n\n";
		for (int i = 0; i < rowCount(); i++) {
			if (item(i)->checkState() == Qt::Checked)
				outputStream << "include \"" << item(i)->text() << "\";\n";
		}
		file.close();
		return true;
	}
	else return false;
}

bool DictManager::runMalmake(QWidget * parent) {
	QMessageBox * mbox = new QMessageBox(parent);
	mbox->setText(tr("Running malmake to regenerate the lexicon, please wait..."));
	mbox->setStandardButtons(QMessageBox::NoButton);
	QProcess * malmake = new QProcess();
	malmake->setWorkingDirectory(QFileInfo(projectFileName).dir().absolutePath());
	QStringList params;
	params << projectFileName;
	connect(malmake, SIGNAL(finished(int, QProcess::ExitStatus)), mbox, SLOT(accept()));
	malmake->start(QString("malmake"), params);
	mbox->exec();
	if (malmake->exitCode() == 0) {
		delete malmake;
		return true;
	}
	else {
		QString errorOutput(malmake->readAllStandardError());
		// Display no more than 500 last characters of the output
		if (errorOutput.length() > 500) errorOutput = QString("...\n").append(errorOutput.right(500));
		delete malmake;
		errorMsg(tr("Running malmake failed:\n").append(errorOutput));
		return false;
	}
}

QString DictManager::getProjectDirectory() {
	return QFileInfo(projectFileName).dir().path();
}

const MallexBOption * DictManager::getMallexBOption(QString optionName) {
	for (int i = 0; i < mallexBOptions.size(); i++)
		if (mallexBOptions[i].name == optionName) return &mallexBOptions[i];
	return 0;
}

QList<MallexBOption> DictManager::getMallexBOptions() {
	return mallexBOptions;
}

bool DictManager::setMallexBOption(QString optionName, bool optionValue) {
	for (int i = 0; i < mallexBOptions.size(); i++) {
		if (mallexBOptions[i].name == optionName) {
			mallexBOptions[i].value = optionValue;
			configurationChanged = true;
			return true;
		}
	}
	return false;
}

void DictManager::markConfigurationChanged() {
	configurationChanged = true;
}

bool DictManager::hasConfigurationChanged() {
	return configurationChanged;
}

QString DictManager::getUpdateURI() {
	return updateURI;
}

void DictManager::setUpdateURI(QString value) {
	updateURI = value;
}

void DictManager::wordlistItemChanged(QStandardItem * item) {
	configurationChanged = true;
	item = item; // Silence the warning about unused parameter
}
