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

#include "playerlabel.hpp"
#include <QPaintEvent>

using namespace Player;

PlayerLabel::PlayerLabel(QWidget *parent) :
	QWidget(parent),
	letterPolygonA(4), textPolygonA(4),
	letterPolygonB(4), textPolygonB(4)
{
	right = false;
	letterPolygonA.setPoint(0, 0, 0);
	letterPolygonA.setPoint(1, 30, 0);
	letterPolygonA.setPoint(2, 20, 20);
	letterPolygonA.setPoint(3, 0, 20);
	textPolygonA.setPoint(0, 30, 0);
	textPolygonA.setPoint(1, 100, 0);
	textPolygonA.setPoint(2, 90, 20);
	textPolygonA.setPoint(3, 20, 20);

	letterPolygonB.setPoint(0, 0, 0);
	letterPolygonB.setPoint(1, -30, 0);
	letterPolygonB.setPoint(2, -20, 20);
	letterPolygonB.setPoint(3, 0, 20);
	textPolygonB.setPoint(0, -30, 0);
	textPolygonB.setPoint(1, -100, 0);
	textPolygonB.setPoint(2, -90, 20);
	textPolygonB.setPoint(3, -20, 20);
	text = "hey";
	color = QColor(255, 255, 255, 30);
}

void PlayerLabel::setRight()
{
	right = true;
}

QColor	PlayerLabel::currentColor()
{
	return QColor(255, 0, 128, 80);
}

QColor	PlayerLabel::nextColor()
{
	return QColor(128, 255, 0, 80);
}


void PlayerLabel::setCurrent()
{
	text = "Current";
	color = currentColor();
	update();
}

void PlayerLabel::setNext()
{
	text = "Next";
	color = nextColor();
	update();
}


void PlayerLabel::paintEvent(QPaintEvent*)
{
	painter.begin(this);
	QRect		zone = rect();
	QPolygon	letterZone;
	QPolygon	textZone;
	QPen		p;

	p = painter.pen();
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QBrush(color));
	if (right)
		letterZone = letterPolygonB.translated(zone.topRight());
	else
		letterZone = letterPolygonA.translated(zone.topLeft());
	painter.drawConvexPolygon(letterZone);

	painter.setBrush(QBrush(QColor(0, 0, 0, 60)));
	if (right)
		textZone = textPolygonB.translated(zone.topRight());
	else
		textZone = textPolygonA.translated(zone.topLeft());
	painter.drawConvexPolygon(textZone);
	painter.setPen(QColor(255, 255, 255));
	painter.drawText(textZone.boundingRect(), Qt::AlignCenter, text);
	if (right)
		painter.drawText(letterZone.boundingRect().translated(5, 0), Qt::AlignCenter, "B");
	else
		painter.drawText(letterZone.boundingRect().translated(-5, 0), Qt::AlignCenter, "A");

	painter.end();
}
