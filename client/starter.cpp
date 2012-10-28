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

#include "main.hpp"
#include "starter.hpp"
#include "guicore.hpp"
#include "logger.hpp"
#include <QtCore/QResource>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopServices>

Starter::Starter()
{
	qApp->setApplicationName(gl_APPLICATION_NAME);
	qApp->setApplicationVersion(gl_APPLICATION_VERSION);
	qApp->setOrganizationDomain(gl_ORGANIZATION_DOMAIN);
	qApp->setOrganizationName(gl_ORGANIZATION_NAME);

	Logger::log("Starting at   " + QTime::currentTime().toString());
	Logger::log("Version       " + qApp->applicationVersion());
	Logger::log("Lauching from " + QApplication::applicationDirPath());
	Logger::log("Command line  " + QApplication::arguments().join(" "));
	Logger::log("PID           " + QVariant(QApplication::applicationPid()).toString());

	arguments = qApp->arguments();
	QResource::registerResource("/usr/local/lib/defuze.me/starter.rcc");
	QResource::registerResource("resources/starter.rcc");
	QResource::registerResource("../resources/starter.rcc");
	QResource::registerResource("/Applications/defuze.me.app/Contents/resources/starter.rcc");

	splashScreen = new QSplashScreen(QPixmap(":/images/defuze-me-splash"));
	splashScreen->show();
}

Starter::~Starter()
{
	if (cores)
	{
		splashScreen->finish(cores->gui()->getMainWindow());
		cores->gui()->showAll();
	}
	delete splashScreen;
	QResource::unregisterResource("/usr/lib/defuze.me/starter.rcc");
	QResource::unregisterResource("resources/starter.rcc");
	QResource::unregisterResource("../Resources/starter.rcc");
	QResource::unregisterResource("/Applications/defuze.me.app/Contents/Resources/starter.rcc");
}

Cores* Starter::loadCores()
{
	try
	{
		message("Loading cores");
		cores = new Cores(arguments);
		message("Initializing cores");
		cores->init();
	}
	catch (Exception &e)
	{
		initializationError(e);
		return 0;
	}
	return cores;
}

Plugins* Starter::loadPlugins()
{
	try
	{
		message("Loading plugins");
		plugins = new Plugins(cores);
		message("Initializing plugins");
		connect(plugins, SIGNAL(message(QString)), SLOT(message(QString)));
		plugins->init();
	}
	catch (Exception &e)
	{
		initializationError(e);
		return 0;
	}
	return plugins;
}

void	Starter::initializationError(Exception &e)
{
	qDebug() << "STARTER: " << e.description();
	QMessageBox::critical(NULL, "defuze.me", "An fatal error occurred while initializing defuze.me.<br><i>" +
						  e.msg() + " (error " + e.hexCode() +
						  ")</i><br><br>Get online support about this error <a href='http://defuze.me/support/errors/" +
						  e.hexCode() + "?report=init'>here</a>.");
}

void	Starter::message(const QString& msg) const
{
	splashScreen->showMessage(msg, Qt::AlignCenter | Qt::AlignBottom, Qt::white);
	qApp->processEvents();
}
