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

namespace DB {
	class DBCore;
}

#ifndef DBCORE_HPP
#define DBCORE_HPP

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <QStringList>
#include <QTimer>
#include <QDesktopServices>
#include "core.hpp"
#include "migration.hpp"
#include "location.hpp"
#include "parameterizable.hpp"
#include "message.hpp"

namespace DB
{

	/** This is the database access wrapper, It manage database and backups locations.
	  **/

	class DBCore : public Params::Parameterizable, public Core
	{
		Q_OBJECT

	public:
		DBCore(QStringList &arguments);
		~DBCore();
		void						init(Cores* cores);
		void						aboutToQuit();
		bool						openLocation(Location* loc);									///< Try to open the database at loc
		QVariant					setting(const QString& key) const;								///< Get the value of a setting
		QHash< QString, QVariant>	settingsStartingWith(const QString& key) const;					///< Get the settings where the key start LIKE 'key'
		bool						setSetting(const QString& key, const QVariant value) const;		///< Set the value of a setting
		int							currentMigration() const;										///< Get the current migration number
		void						test();															///< Some tests

	public slots:
		void						rewriteMsg(Notification::Message* msg);							///< Called when the rewrite message appears to add custom actions
		void						rewrite();														///< Replicate current database to other places
		void						forceRewrite();

	private:
		void						defineParams();
		void						loadLocations();												///< Load databases locations from settings
		void						saveLocations();												///< save databases locations to settings

		QList<Location*>			locations;														///< Store all locations where the database will be replicated
		QSqlDatabase				connection;
		Location*					currentLocation;
		bool						sync;
		QTimer						timer;
		Notification::Message*		message;
	};
}

#endif // DBCORE_HPP
