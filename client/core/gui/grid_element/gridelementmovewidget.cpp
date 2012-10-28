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

#include "gridelementmovewidget.hpp"
#include "gridelement.hpp"
#include "exception.hpp"
#include "gridwidget.hpp"
#include "tab.hpp"
#include <QMouseEvent>
#include <QDrag>

using namespace Gui;

GridElementMoveWidget::GridElementMoveWidget(QWidget *parent) :
	QLabel(parent)
{
	setAcceptDrops(true);
	gridElement = dynamic_cast<GridElement*>(parent->parentWidget());
	Q_ASSERT(gridElement);
	tab = gridElement->getGridWidget()->getTab();
}

void GridElementMoveWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;
		mimeData->setData("application/x-defuzeme-gridelement", gridElement->getModule()->getUniqId().toUtf8());
		drag->setMimeData(mimeData);
		drag->setPixmap(QPixmap::grabWidget(gridElement).scaledToWidth(qMin(200, gridElement->width()),
																	   Qt::SmoothTransformation));
		tab->showDropLocations(gridElement->getPosition());
		drag->exec();
		tab->cleanGrid();
	}
}
