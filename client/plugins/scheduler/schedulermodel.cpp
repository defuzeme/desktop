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

#include "schedulermodel.hpp"
#include "scheduler.hpp"
#include "eventmodel.hpp"

using namespace Scheduler;

SchedulerModel::SchedulerModel(SchedulerPlugin *scheduler) : scheduler(scheduler), events(0)
{
}

SchedulerModel::~SchedulerModel()
{
	for (int index = 0; index < this->events->count() ; index += 1)
		delete this->events->at(index);
	delete events;
}

SchedulerPlugin	*SchedulerModel::getScheduler() const
{
	return this->scheduler;
}

void	SchedulerModel::loadEvents()
{
	this->getEvents();

	for (int index = 0; index < events->count(); index += 1 )
	{
		EventModel	*event = events->at(index);
		this->scheduler->getWidget()->displayEvent(event);
		emit loadEvent(event);
	}
}

void	SchedulerModel::clearEvents()
{
	for (int index = 0; index < this->events->count(); index +=1 )
		delete this->events->at(index);
}

QList<EventModel*>	*SchedulerModel::getEvents()
{
	if (!events)
	{
		QSqlQuery	query("SELECT id, title, description, day, start, duration FROM events");
		query.exec();
		events		= new QList<EventModel*>();

		while (query.next())
		{
			int		id			= query.value(0).toInt();
			QString	title		= query.value(1).toString();
			QString	description	= query.value(2).toString();
			short	day			= query.value(3).toInt();
			int		start		= query.value(4).toInt();
			int		duration	= query.value(5).toInt();

			EventModel *event	= new EventModel(scheduler, title, description, day, start, duration, id);

			this->events->append(event);
		}
	}
	return events;
}

EventModel*		SchedulerModel::getEvent(int id)
{
	foreach(EventModel* event, *getEvents())
		if (event->getId() == id)
			return event;
	return 0;
}
