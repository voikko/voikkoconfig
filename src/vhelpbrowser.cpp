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

#include <QUrl>
#include <QVBoxLayout>

#include "vhelpbrowser.h"

VHelpBrowser::VHelpBrowser(QWidget * parent) : QDialog(parent) {
	browser = new QTextBrowser(this);
	browser->setSource(QUrl("qrc:/doc/fi/index.html"));
	QVBoxLayout * layout = new QVBoxLayout();
	layout->addWidget(browser);
	setLayout(layout);
	setWindowTitle(tr("Voikkoconfig - handbook"));
	resize(500, 500);
}
