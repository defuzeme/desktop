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

#include "plugins.hpp"
#include "exception.hpp"
#include "logger.hpp"
#include "playqueue.hpp"
#include "library.hpp"
#include "listsplugin.hpp"
#include "scheduler.hpp"
#include "remotecontrol.hpp"
#include "mainplayer.hpp"
#include "servicesync.hpp"
#include "audiomixerplugin.hpp"
#include "status.hpp"
#include "lastfmplugin.hpp"

Plugins::Plugins(Cores *c) : QObject(NULL), cores(*c)
{
	add("player", new Player::MainPlayer("Player"));
	add("queue", new Queue::PlayQueue);
	add("mixer", new Mixer::AudioMixerPlugin);
	add("library", new Library::LibraryPlugin);
	add("lists", new Lists::ListsPlugin);
	add("remote", new Remote::RemoteControl);
	add("web_service", new WebService::ServiceSync);
	add("status", new Notification::Status);
	add("scheduler", new Scheduler::SchedulerPlugin);
	add("lastfm", new Lastfm::LastfmPlugin);
}

Plugins::~Plugins()
{
	QHash<QString, Plugin*>::iterator	it;
	for (it = plugins.begin(); it != plugins.end(); it++)
	{
		Logger::log(QString("Deleting plugin: %1").arg(it.key()));
		delete it.value();
	}
}

void	Plugins::add(const QString& name, Plugin* plugin)
{
	Q_ASSERT(!plugins.contains(name));
	plugins[name] = plugin;
	order << name;
	plugin->setReferences(cores, *this);
}

Plugin*		Plugins::get(const QString& name)
{
	return plugins.value(name);
}

void	Plugins::init()
{
	foreach (QString name, order)
	{
		QString	msg = QString("Initializing plugin: %1").arg(name);
		Logger::log(msg);
		emit message(msg);
		plugins[name]->init();
	}
}

void	Plugins::aboutToQuit()
{
	foreach (QString name, order)
	{
		Logger::log(QString("Exiting plugin: %1").arg(name));
		plugins[name]->aboutToQuit();
	}
	deleteLater();
}
