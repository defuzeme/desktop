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

#include "cores.hpp"
#include "starter.hpp"
#include "audiocore.hpp"
#include "audiodecoder.hpp"
#include "defuzemeapplication.hpp"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	DefuzemeApplication app(argc, argv);

	QTranslator translator;
	translator.load("defuzeme_" + QLocale::system().name(), "translations");
	app.installTranslator(&translator);

	Starter *starter = new Starter();

	Cores *cores = starter->loadCores();
	Plugins *plugins = starter->loadPlugins();
	delete starter;

	if (!cores || !plugins)
		return 1;

	QObject::connect(&app, SIGNAL(aboutToQuit()), plugins, SLOT(aboutToQuit()));
	QObject::connect(&app, SIGNAL(aboutToQuit()), cores, SLOT(aboutToQuit()));

	int out = app.exec();
	return out;
}
