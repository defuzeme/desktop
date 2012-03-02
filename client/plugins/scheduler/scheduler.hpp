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

#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#define	SCALE	4

#include <QColor>
#include <QList>
#include <QSqlQuery>
#include <QTableView>

#include "staticplugin.hpp"
#include "lists/listsplugin.hpp"
#include "lists/listsmodel.hpp"

#include "eventcolor.hpp"
#include "eventmodel.hpp"
#include "eventplaylist.hpp"
#include "eventplaylistitem.hpp"
#include "eventwidget.hpp"

#include "schedulerwidget.hpp"
#include "schedulermodel.hpp"

namespace Scheduler
{

	class EventModel;
	class EventWidget;
	class SchedulerModel;
	class SchedulerWidget;

	class SchedulerPlugin : public QObject, public StaticPlugin
	{
		Q_OBJECT
	public:
		SchedulerPlugin();
		~SchedulerPlugin();
		Plugins					*getPlugins();
		void					init();
		void					aboutToQuit();
		SchedulerModel			*getModel() const;
		SchedulerWidget			*getWidget() const;
		EventWidget				*getEventWidget() const;
		EventModel*				getEvent(int id);
		void					addScheduler(QString name);
		void					loadScheduler();

	signals:
		void					newEvent(Scheduler::EventModel* event);
		void					updateEvent(Scheduler::EventModel* event);
		void					removeEvent(Scheduler::EventModel* event);
		void					loadEvent(Scheduler::EventModel* event);

	private:
		SchedulerModel			*schedulerModel;
		EventWidget				*eventWidget;
		SchedulerWidget			*schedulerWidget;

	};

}

#endif // SCHEDULER_HPP
