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

#include "lastfmwidget.hpp"
#include "lastfmplugin.hpp"
#include <QGraphicsObject>
#include <QDebug>
#include <QResource>
#include <QDeclarativeEngine>
#include <QDesktopServices>
#include <QDeclarativeContext>

using namespace Lastfm;

LastfmWidget::LastfmWidget(Player::MainPlayer *player, Library::LibraryPlugin *library, QWidget *parent) :
	ModuleWidget(parent), player(player), library(library)
{
	setupUi(this);
	declarativeView = new QDeclarativeView;
	declarativeView->engine()->setOfflineStoragePath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
	qDebug() << "Offline storage path: " << declarativeView->engine()->offlineStoragePath();
	declarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
	declarativeView->rootContext()->setContextProperty("mainPlayer", player);
	declarativeView->rootContext()->setContextProperty("library", library);
//	declarativeView->setSource(QUrl::fromLocalFile("../client/plugins/lastfm/LastfmView.qml"));
	QResource::registerResource("/usr/local/lib/defuze.me/lastfm.rcc");
	QResource::registerResource("resources/lastfm.rcc");
	QResource::registerResource("../resources/lastfm.rcc");
	QResource::registerResource("/Applications/defuze.me.app/Contents/resources/lastfm.rcc");
	declarativeView->setSource(QUrl("qrc:/qml/lastfm/LastfmView.qml"));
	verticalLayout->addWidget(declarativeView);
}

void LastfmWidget::changeEvent(QEvent *e)
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
