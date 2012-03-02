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

#include "scheduler.hpp"

using namespace Scheduler;

SchedulerPlugin::SchedulerPlugin()
{
}

SchedulerPlugin::~SchedulerPlugin()
{
}

Plugins *SchedulerPlugin::getPlugins()
{
	return this->plugins;
}

void	SchedulerPlugin::init()
{
	eventWidget		= new EventWidget(this);
	schedulerWidget	= new SchedulerWidget(this);
	schedulerModel	= new SchedulerModel(this);

	connect(eventWidget, SIGNAL(newEvent(Scheduler::EventModel*)), SIGNAL(newEvent(Scheduler::EventModel*)));
	connect(eventWidget, SIGNAL(updateEvent(Scheduler::EventModel*)), SIGNAL(updateEvent(Scheduler::EventModel*)));
	connect(eventWidget, SIGNAL(removeEvent(Scheduler::EventModel*)), SIGNAL(removeEvent(Scheduler::EventModel*)));
	connect(schedulerModel, SIGNAL(loadEvent(Scheduler::EventModel*)), SIGNAL(loadEvent(Scheduler::EventModel*)));

	loadScheduler();
	Gui::Module *eventModule = Gui::ModuleFactory::create("Event Manager", QPoint(0, 1), eventWidget, 1);
	Gui::Module *schedulerModule = Gui::ModuleFactory::create("Scheduler", QPoint(1, 0), schedulerWidget, 1);
	eventModule->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	eventModule->submitForDisplay();
	schedulerModule->submitForDisplay();
}

EventModel*	SchedulerPlugin::getEvent(int id)
{
	return schedulerModel->getEvent(id);
}

void	SchedulerPlugin::aboutToQuit()
{
}

SchedulerModel  *SchedulerPlugin::getModel() const
{
	return schedulerModel;
}

SchedulerWidget *SchedulerPlugin::getWidget() const
{
	return schedulerWidget;
}

EventWidget *SchedulerPlugin::getEventWidget() const
{
	return eventWidget;
}

void	SchedulerPlugin::loadScheduler()
{
	this->schedulerModel->loadEvents();
}
