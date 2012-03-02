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

#include "eventcolor.hpp"

using namespace Scheduler;

EventColor::EventColor(QWidget *parent, int id, QString color) :
	parent(parent), id(id), color(color)
{
	QColor  *newColor				= new QColor("#" + color);
	QString borderColor				= newColor->lighter(95).name();
	QString backgroundColor			= newColor->lighter(70).name();
	QString borderColorSelected		= newColor->lighter(25).name();
	QString backgroundColorSelected	= newColor->lighter(45).name();

	this->setCheckable(true);
	this->setStyleSheet("\
						QPushButton { border: 1px solid " + borderColor + "; border-radius: 5px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + newColor->name() + ", stop: 1 " + backgroundColor + "); max-width: 20px; min-height: 20px; }\
						QPushButton:checked { border-color: " + borderColorSelected + "; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 " + backgroundColorSelected + ", stop: 1 " + newColor->name() + "); }\
						");

	return;
}

EventColor::~EventColor()
{
}

int		EventColor::getId() const
{
	return this->id;
}

QString	EventColor::getColor() const
{
	return this->color;
}

void EventColor::on_colorPicker_clicked()
{
	if (this->parent)
	{
		int count = ((EventWidget*)this->parent)->colors->count();

		for (int index = 0; index < count; index += 1)
		{
			((EventWidget*)this->parent)->colors->at(index)->setChecked(false);
		}
		this->setChecked(true);
	}

	return;
}
