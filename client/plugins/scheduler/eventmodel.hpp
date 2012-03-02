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

#ifndef EVENTMODEL_HPP
#define EVENTMODEL_HPP

#include <QString>
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QSqlQuery>

#include "guicore.hpp"
#include "scheduler.hpp"
#include "eventplaylistitem.hpp"

namespace Scheduler
{

class SchedulerPlugin;

class EventModel : public QLabel
{
	Q_OBJECT
	friend class EventWidget;

public:
	EventModel(SchedulerPlugin *scheduler, QString title, QString description, short day, int start, int duration, int id = 0);

	int				save();
	void			saveColor(int color);
	void			savePlaylists(QList<int> playlists);

	int				getId() const;				///< Return the event id according to the database

	QString			getTitle() const;			///< Return the title of the event
	QString			getDescription() const;		///< Return the description

	short			getDay() const;				///< Return the day of the event (1-7)
	int				getStartTime() const;		///< Return the event start time (seconds)
	int				getDuration() const;		///< Return event duration (minutes)

	QDateTime		nextInstance() const;		///< Return next event start from now

	const QString&	getColor();					///< Return the color ( ex: "FFAD46" )
	int				getColorID();				///< Return the color id according to the database
	static const QString&	getColorByID(int colorID);	///< Return the color from database by its id ( ex: "FFAD46" )
	QList<int>		getPlaylists();				///< Return an int list of playlists associated to this event

	// Event styles
	void			focusIn();					///< Give the focus on the current event
	void			focusOut();					///< Get the focus out of the event
	void			setStyle();					///< Set the default style for the event + the appropriate color

	void			remove();					///< Delete the event from the database

private:
	SchedulerPlugin	*scheduler;

	int				id;

	QString			title;
	QString			description;

	short			day;
	int				start;
	int				duration;
	int				color;
	static QMap<int, QString>	colors;

public slots:
	void			eventClicked();

signals:
	void			clicked();

protected:
	void			mousePressEvent(QMouseEvent *);

};

}

#endif // EVENTMODEL_HPP
