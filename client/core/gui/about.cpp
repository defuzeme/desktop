/*******************************************************************************
**  defuze.me - modern radio automation software suite
**  
**  Copyright © 2012
**    Athena Calmettes - Jocelyn De La Rosa - Francois Gaillard
**    Adrien Jarthon - Alexandre Moore - Luc Peres - Arnaud Sellier
**
**  website: http://defuze.me
**  contact: team@defuze.me
**
**  This program is free software: you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**  This software includes code from Nokia (Qt) under the GNU LGPLv2.1
**  This software uses libraries from the FFmpeg project under the GNU LGPLv2.1
**  This software uses libraries from the TagLib project under the GNU LGPLv2.1
**
*******************************************************************************/

#include "about.hpp"
#include "main.hpp"
#include <QResource>
#include <QDebug>

About::About(QWidget *parent) :
	QWidget(parent)
{
	QResource::registerResource("/usr/local/lib/defuze.me/about.rcc");
	QResource::registerResource("resources/about.rcc");
	QResource::registerResource("../Resources/about.rcc");
	QResource::registerResource("/Applications/defuze.me.app/Contents/Resources/about.rcc");
	setupUi(this);
	logo->setPixmap(QPixmap(":/images/defuze-me-small"));
	setStyleSheet("#contents { border-image:url(:/images/defuze-small) 0; }");
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
	setAttribute(Qt::WA_DeleteOnClose, false);
	versionLabel->setText(versionLabel->text().arg(gl_APPLICATION_VERSION, gl_VERSION_CODENAME));
	scrollArea->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
}

About::~About()
{
	QResource::unregisterResource("/usr/lib/defuze.me/about.rcc");
	QResource::unregisterResource("resources/about.rcc");
	QResource::unregisterResource("../resources/about.rcc");
	QResource::unregisterResource("/Applications/defuze.me.app/Contents/resources/about.rcc");
}

void About::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}
