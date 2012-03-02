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

#ifndef STARTER_HPP
#define STARTER_HPP

#include <QStringList>
#include <QSplashScreen>
#include "cores.hpp"
#include "plugins.hpp"
#include "exception.hpp"

/** The start-up sequence.
  * It's displaying a splashscreen while loading CORES and static PLUGINS.
  *
  * The sequence:
  * - Set the applications general variables
  * - Display the splashscreen
  * - Create the CORES
  * - Call init() on each
  * - Create the PLUGINS
  * - Call init() on each
  * - Close the splashscreen
  * - Give control to the main window
  *
  *\attention CORES constructors are called with 1 arguments: the command line arguments.
  **/

class Starter : public QObject
{
	Q_OBJECT
public:
	Starter();
	~Starter();
	Cores*			loadCores();						///< Create, init and return all CORES.
	Plugins*		loadPlugins();						///< Create, init and return all PLUGINS.

private slots:
	void			message(const QString& msg) const;	///< Display message on splash screen

private:
	void			initializationError(Exception &e);	///< Show exception details

	Cores			*cores;
	Plugins			*plugins;
	QStringList		arguments;							///< The command line arguments.
	QSplashScreen	*splashScreen;
};

#endif // STARTER_HPP
