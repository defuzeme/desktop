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

#include <iostream>
#include <QDir>
#include "dbcore.hpp"
#include "exception.hpp"
#include "parser.hpp"
#include "status.hpp"

using namespace DB;

DBCore::DBCore(QStringList &)
{
	sync = false;
	timer.setInterval(30*60*1000);	// Backup every 30 min
	connect(&timer, SIGNAL(timeout()), SLOT(rewrite()));
}

DBCore::~DBCore()
{
	while (!locations.empty())
	{
		delete locations.back();
		locations.pop_back();
	}
}

void	DBCore::defineParams()
{
	// Default locations
	QVariantList list;
	list.push_back(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/database.db");
	list.push_back(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + "/defuze.me/backup/database.db");
	setParameter("locations", list);
}

void	DBCore::loadLocations()
{
	QVariantList list = getParameter("locations").toList();
	for (int i = 0; i < list.size(); i++)
		locations.push_back(new Location(list.at(i).toString()));
	currentLocation = NULL;
}

void	DBCore::saveLocations()
{
	QVariantList list;
	for (int i = 0; i < locations.size(); i++)
		list.push_back(locations[i]->filePath());
	setParameter("locations", list);
	commitParameters();
}

void DBCore::init(Cores *c)
{
	cores = c;

	setParamsName("db");
	setParamsBackEnd(Params::Parameterizable::SETTINGS);
	registerToParamsCore(cores->params());
	loadLocations();
	// Find the better location
	Location* original = locations.first();
	Location* newer = locations.first();

	for(int i = 1; i < locations.size(); ++i)
		if (locations.at(i)->lastModified() > newer->lastModified())
			newer = locations.at(i);

	if (newer != original && openLocation(newer))
	{
		Notification::Status::gMessage(tr("You're running on the backup database because it looks newer, if everything looks ok, click 'rewrite' to update your main database"), Notification::WARN, this, "rewriteMsg");
	}
	else if (!openLocation(original))
	{
		Notification::Status::gMessage(tr("You're running on the backup database because the main one could not be opened"), Notification::WARN);

		// try to load backups
		bool	backupOk = false;
		for(int i = 1; i < locations.size(); ++i)
		{
			if (openLocation(locations.at(i)))
			{
				backupOk = true;
				break;
			}
		}
		if (!backupOk && !openLocation(original))
			throw_exception(0x01, tr("No database found, no backup available, can't create a new one, please check your persmissions!"));
	}
	else
	{
		sync = true;
		timer.start();
	}

	MigrationEngine	mEngine(*this);
	mEngine.migrate();

	if (getParameter("reset", false) == true)
	{
		// qDebug() << "DB reset requested";
		// Undo all migrations
		mEngine.undo();
		// Migrate database to new schema
		mEngine.migrate();
	}
	// self-test
	//test();
}

void DBCore::aboutToQuit()
{
	connection.close();
	rewrite();
	currentLocation = NULL;
}

bool	DBCore::openLocation(Location* loc)
{
	// qDebug() << "DB::Core opening database:" << loc->filePath();
	loc->mkPath();
	connection = QSqlDatabase::addDatabase("QSQLITE");
	connection.setDatabaseName(loc->filePath());
	connection.setUserName("defuze.me");
	if (!connection.open())
	{
		std::cerr << connection.lastError().text().toStdString() << std::endl;
		return false;
	}
	loc->open() = true;
	currentLocation = loc;
	return true;
}

QVariant	DBCore::setting(const QString& key) const
{
	QSqlQuery query;
	QVariant value;
	query.prepare("SELECT * FROM settings WHERE key = :key LIMIT 1");
	query.bindValue(":key", key);
	query.exec();
	int fieldNo = query.record().indexOf("value");
	if (query.next())
		value = query.value(fieldNo);
	return (Network::JsonParser().parse(value.toByteArray()));
}

QHash<QString, QVariant> DBCore::settingsStartingWith(const QString &key) const
{
	QHash<QString, QVariant> result;
	QSqlQuery query;
	QVariant value;
	QString fullKey;
	query.prepare("SELECT * FROM settings WHERE key LIKE :key");
	query.bindValue(":key", key + "%");
	query.exec();
	int keyFieldNo = query.record().indexOf("key");
	int valueFieldNo = query.record().indexOf("value");
	while (query.next())
	{
		fullKey = query.value(keyFieldNo).toString();
		value = query.value(valueFieldNo);
		result[fullKey] = Network::JsonParser().parse(value.toByteArray());
	}
	return (result);
}

bool	DBCore::setSetting(const QString& key, const QVariant value) const
{
	QSqlQuery query;
	query.prepare("INSERT OR REPLACE INTO settings (key, value) VALUES (:key, :value)");
	query.bindValue(":key", key);
	query.bindValue(":value", Network::JsonParser().serialize(value));
	if (!query.exec())
		throw_exception(0x02, tr("Can't store setting value: %1").arg(query.lastError().databaseText()));
	return true;
}

int		DBCore::currentMigration() const
{
	QVariant	value = setting("migration");
	if (value.isNull())
		return 0;
	else
		return value.toInt();
}

void		DBCore::rewrite()
{
	if (!sync)
		return;
	for(int i = 0; i < locations.size(); ++i)
	{
		Location* loc = locations.at(i);
		if (!loc->open())
		{
			//qDebug() << "DB::Core backuping database to" << loc->filePath() << "(age=" << loc->age() << ")";
			loc->mkPath();
			QFile::remove(loc->filePath());
			QFile	database(currentLocation->filePath());
			if (!database.copy(loc->filePath()))
			{
				Notification::Status::gMessage(tr("Can't backup to %1 (%2)").arg(loc->filePath()).arg(database.errorString()), Notification::WARN);
			}
			loc->reload();
		}
	}
}

void		DBCore::forceRewrite()
{
	sync = true;
	timer.start();
	message->disableAction();
	rewrite();
}

void		DBCore::rewriteMsg(Notification::Message* msg)
{
	message = msg;
	msg->setAction(tr("Rewrite"), this, SLOT(forceRewrite()));
}

void		DBCore::test()
{
	// migration tests
	MigrationEngine	mEngine(*this);
	mEngine.undo();
	mEngine.migrate();
	mEngine.undo();
	mEngine.migrate();

	// settings tests
	setSetting("test-foo", 42);
	Q_ASSERT_X(setting("test-foo") == 42, "DBCore::test", "Integer setting error");
	setSetting("test-foo", 40.42);
	Q_ASSERT_X(setting("test-foo") == 40.42, "DBCore::test", "Double setting error");
	setSetting("test-foo", "Hello");
	Q_ASSERT_X(setting("test-foo") == "Hello", "DBCore::test", "String setting error");
	QVariantList list;
	list.append(42);
	list.append(40.42);
	list.append("Hello");
	setSetting("test-foo", list);
	Q_ASSERT_X(setting("test-foo") == list, "DBCore::test", "Array setting error");
	QVariantMap map;
	map["int"] = 42;
	map["double"] = 40.42;
	map["string"] = "Hello";
	setSetting("test-foo", map);
	Q_ASSERT_X(setting("test-foo") == map, "DBCore::test", "Map setting error");
	Q_ASSERT_X(setting("test-empty") == QVariant(), "DBCore::test", "Empty setting error");
}
