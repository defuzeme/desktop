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
#include "dbcore.hpp"
#include "migration.hpp"
#include "migration_list.hpp"
#include "exception.hpp"

using namespace DB;

Migration::Migration(const char* _name) : _name(_name)
{}

Migration::~Migration()
{}

const QString&	Migration::name()
{
	return (_name);
}

bool	Migration::run(bool forward)
{
	QSqlDatabase db = QSqlDatabase::database();
	try {
		db.transaction();
		if (forward)
		{
			qDebug() << "DB::Migration [" << name() << "] run up";
			up();
		}
		else
		{
			qDebug() << "DB::Migration [" << name() << "] run down";
			down();
		}
		db.commit();
		return true;
	} catch (...) {
		qDebug() << "DB::Migration [" << name() << "] failed: " << db.lastError().text();
		db.rollback();
		throw_exception(0x01, QObject::tr("Can't update your database !"));
		return false;
	}
}

bool	Migration::exec(const QString sql)
{
	return QSqlQuery().exec(sql);
}

bool	Migration::createTable(const QString name, const QString definition)
{
	return exec(QString("create table %1 (%2)").arg(name, definition));
}

bool	Migration::dropTable(const QString name)
{
	return exec(QString("drop table %1").arg(name));
}

MigrationEngine::MigrationEngine(DBCore& dbCore) : db(dbCore)
{
	migrations.push_back(new SettingsMigration("Settings & Initial setup"));
	migrations.push_back(new ConfigurationsMigration("Configurations"));
	migrations.push_back(new SourcesMigration("Audio sources"));
	migrations.push_back(new AudioTracksMigration("Audio tracks & Queue"));
	migrations.push_back(new PlaylistsMigration("Playlists"));
	migrations.push_back(new SchedulerMigration("Scheduler"));
}

MigrationEngine::~MigrationEngine()
{
	while (!migrations.empty())
	{
		delete migrations.back();
		migrations.pop_back();
	}
}

int		MigrationEngine::migrationsCount()
{
	return (migrations.size());
}


bool	MigrationEngine::migrate()
{
	return migrate(migrationsCount());
}

bool	MigrationEngine::undo()
{
	return migrate(0);
}

bool	MigrationEngine::migrate(int to)
{
	return migrate(db.currentMigration(), to);
}

bool	MigrationEngine::migrate(int from, int to)
{
	int i;
	int step = (from > to ? -1 : 1);

	if (from < 0 || from > migrationsCount())
		throw_exception(0x02, QObject::tr("Initial migration outside bounds"));
	Q_ASSERT(to >= 0 && to <= migrationsCount());

	// from = 4; to = 8; => 4..5..6..7..8
	for (i = from; i != to; i += step)
	{
		int	newMig = i + step;
		Migration*	mig = migrations.at(i + (step - 1)/2);
		// run up if step > 0, down otherwise
		if (!mig->run(step > 0))
			break;
		if (newMig > 0)
			db.setSetting("migration", QVariant(newMig));
	}
	if (i != to)
		qDebug() << "DB::MigrationEngine" << abs(to - i) << "later migrations cancelled";
	else
		qDebug() << "DB::MigrationEngine" << abs(to - from) << "migrations run successfully.";
	return (i >= migrationsCount());
}
