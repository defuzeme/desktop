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

#include "droplocation.hpp"
#include "modulefactory.hpp"
#include "gridelement.hpp"
#include "tab.hpp"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>

using namespace Gui;

DropLocation::DropLocation(Tab *tab, QPoint targetPosition, bool newColumn, QWidget *parent) :
	QWidget(parent), tab(tab), targetPosition(targetPosition), newColumn(newColumn)
{
	setupUi(this);
	setAcceptDrops(true);
}

void DropLocation::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-defuzeme-gridelement"))
	{
		setStyleSheet("background-color:rgb(230, 255, 108);border-radius:2px;");
		event->accept();
	}
	else
		event->ignore();
}

void DropLocation::dragLeaveEvent(QDragLeaveEvent *event)
{
	setStyleSheet("background-color:rgba(200,200,200, 128);border-radius:2px;");
	event->accept();
}

void DropLocation::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-defuzeme-gridelement"))
	{
		QString moduleId = QString::fromUtf8(event->mimeData()->data("application/x-defuzeme-gridelement"));
		Module* module = ModuleFactory::getModule(moduleId);
		GridElement *gridElement = tab->getGridElementByModule(module);
		tab->insertGridElement(gridElement, targetPosition, newColumn);
		event->accept();
		delete this;
	}
}

void DropLocation::changeEvent(QEvent *e)
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
