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

#include "eventmodel.hpp"

using namespace Scheduler;

QMap<int, QString>	EventModel::colors;

EventModel::EventModel(SchedulerPlugin *scheduler, QString title, QString description, short day, int start, int duration, int id) :
	scheduler(scheduler), id(id), title(title), description(description), day(day), start(start), duration(duration), color(-1)
{
	this->setText(title);
	this->setToolTip(this->description);

	// Various style properties
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	this->setAlignment(Qt::AlignCenter);
	this->setWordWrap(true);
	this->setStyle();

	this->connect(this, SIGNAL(clicked()), this, SLOT(eventClicked()));

	return;
}

int	EventModel::save()
{
	QSqlQuery query;

	if ( ! id)
	{
		query.prepare("INSERT INTO events (title, description, day, start, duration) "
					  "VALUES (:title, :description, :day, :start, :duration)");
	}
	else
	{
		query.prepare("UPDATE events "
					  "SET title = :title, description = :description, day = :day, start = :start, duration = :duration "
					  "WHERE id = :id");
	}

	query.bindValue(":id", id);
	query.bindValue(":title", title);
	query.bindValue(":description", description);
	query.bindValue(":day", day);
	query.bindValue(":start", start);
	query.bindValue(":duration", duration);

	if (!query.exec())
	{
		qDebug() << QString("Can't save event: %1").arg(query.lastError().text());
		return 0;
	}

	if ( ! id)
		return this->id = query.lastInsertId().toInt();
	else
		return id;
}

void	EventModel::saveColor(int color)
{
	QSqlQuery query("INSERT OR REPLACE INTO colors_events"
					"(color_id, event_id) VALUES (:color, :event)");

	query.bindValue(":color", color);
	query.bindValue(":event", id);

	if (query.exec())
		this->color = color;
	else
		qDebug() << QString("Can't save color : %1").arg(query.lastError().text());
}

void	EventModel::savePlaylists(QList<int> playlists)
{
	QSqlQuery query("DELETE FROM events_playlists "
					"WHERE event_id = :id");

	query.bindValue(":id", this->id);

	if ( ! query.exec())
		qDebug() << QString("Can't delete playlists associations : %1").arg(query.lastError().text());

	query.clear();

	for (int index = 0; index < playlists.count(); index += 1)
	{
		query.prepare("INSERT INTO events_playlists (event_id, playlist_id) "
					  "VALUES (:event, :playlist)");

		query.bindValue(":event", this->id);
		query.bindValue(":playlist", playlists.at(index));

		if (!query.exec())
			qDebug() << QString("Can't save playlists associations : %1").arg(query.lastError().text());

		query.clear();
	}

	return;
}

int	EventModel::getId() const
{
	return (this->id) ? this->id : 0 ;
}

QString	EventModel::getTitle() const
{
	return this->title;
}

QString	EventModel::getDescription() const
{
	return this->description;
}

short	EventModel::getDay() const
{
	return this->day;
}

int	EventModel::getStartTime() const
{
	return this->start;
}

int	EventModel::getDuration() const
{
	return this->duration;
}

QDateTime	EventModel::nextInstance() const
{
	QDate	date = QDate::currentDate();
	QTime	curTime = QTime::currentTime();
	QTime	time(start / 60, start % 60);

	// Search for the good day
	if (date.dayOfWeek() == day + 1 && time <= curTime)
		date = date.addDays(7);
	while (date.dayOfWeek() != day + 1)
		date = date.addDays(1);

	return QDateTime(date, time);
}

const QString&	EventModel::getColor()
{
	return	getColorByID(getColorID());
}

int	EventModel::getColorID()
{
	if (color >= 0)
		return color;
	QSqlQuery query("SELECT color_id FROM colors_events WHERE event_id = :id");

	query.bindValue(":id", id);

	if (!query.exec())
		qDebug() << QString("Can't select : %1").arg(query.lastError().text());
	else if (query.next())
		return query.value(0).toInt();

	return 0;
}

const QString& EventModel::getColorByID(int colorID)
{
	if (!colors.contains(colorID))
	{
		QSqlQuery query("SELECT color FROM colors WHERE id = :id");

		query.bindValue(":id", colorID);

		colors[colorID] = "000000";
		if (!query.exec())
			qDebug() << QString("Can't select color : %1").arg(query.lastError().text());
		else if (query.next())
			colors[colorID] = query.value(0).toString();
	}
	return colors[colorID];
}

QList<int> EventModel::getPlaylists()
{
	QList<int>	items;

	QSqlQuery	query("SELECT playlist_id FROM events_playlists WHERE event_id = :id");

	query.bindValue(":id", this->id);

	if (!query.exec())
		qDebug() << QString("Can't select : %1").arg(query.lastError().text());
	else
	{
		while (query.next())
			items.append(query.value(0).toInt());
	}

	return items;
}

void	EventModel::remove()
{
	QSqlQuery query("DELETE FROM events_playlists WHERE event_id = :p_id;");

	query.bindValue(":event", this->id);

	if (!query.exec())
		qDebug() << QString("Can't delete event or dependencies : %1").arg(query.lastError().text());

	query.prepare("DELETE FROM colors_events WHERE event_id = :c_id;");

	query.bindValue(":event", this->id);

	if (!query.exec())
		qDebug() << QString("Can't delete event or dependencies : %1").arg(query.lastError().text());

	query.prepare("DELETE FROM events WHERE id = :event;");

	query.bindValue(":event", this->id);

	if (!query.exec())
		qDebug() << QString("Can't delete event or dependencies : %1").arg(query.lastError().text());

	return;
}

void	EventModel::eventClicked()
{
	scheduler->getEventWidget()->setUpdate(this);

	this->focusIn();
}

void	EventModel::focusIn()
{
	QString stylesheet	= this->styleSheet();

	this->setStyleSheet(stylesheet + "background-image: url(:/scheduler/bg-selected);");
}

void	EventModel::focusOut()
{
	this->setStyle();
}

void	EventModel::setStyle()
{
	QString	color((this->getColor() != NULL) ? this->getColor() : "FFAD46");
	QColor	newColor			= QColor("#" + color);
	QString	newBackgroundColor	= newColor.lighter(70).name();

	this->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #" + color + ", stop: 1 " + newBackgroundColor + "); color: #333; border-radius: 3px; margin: 1px; text-align: center;");
}

void	EventModel::mousePressEvent(QMouseEvent *)
{
	this->focusIn();
	emit clicked();
}
