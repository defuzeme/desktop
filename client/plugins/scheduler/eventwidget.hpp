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

#ifndef EVENTWIDGET_HPP
#define EVENTWIDGET_HPP

#include <QLabel>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QTreeView>

#include "guicore.hpp"
#include "ui_eventwidget.h"
#include "scheduler.hpp"
#include "eventmodel.hpp"
#include "eventplaylist.hpp"

namespace Scheduler
{

class EventColor;
class EventModel;
class EventPlaylist;
class SchedulerPlugin;

class EventWidget : public Gui::ModuleWidget, private Ui::EventWidget
{
	Q_OBJECT
public:
	explicit EventWidget(SchedulerPlugin *scheduler, QWidget *parent = 0);

	void						clear();
	void						setUpdate(EventModel *updateEvent);

	QList<EventColor*>			*colors;
	SchedulerPlugin				*scheduler;

	EventPlaylist				*playlistsWidget;

	Lists::ListsModel			*playlistsModel;
	Lists::ListsPlugin			*playlistsPlugin;

private:
	bool						check();
	void						initialize();
	void						updateEvent();

signals:
	void						newEvent(Scheduler::EventModel*);
	void						updateEvent(Scheduler::EventModel*);
	void						removeEvent(Scheduler::EventModel*);

public slots:
	void						on_submitEventButton_clicked();
	void						on_deleteEventButton_clicked();

protected:
	EventModel					*event;
};
}

#endif // EVENTWIDGET_HPP
