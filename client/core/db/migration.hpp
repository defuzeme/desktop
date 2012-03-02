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

#ifndef MIGRATION_HPP
#define MIGRATION_HPP

#include <QList>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

namespace DB
{
	class DBCore;

	/** This is the superclass used to define migration to be done on the database.
	  * Create a subclass in the migrations folder, and fill the up and down methods.
	  * Then add your subclass to the global migrations list (migration_list.hpp).
	  * You also need to add your migration to the load list (migration.cpp)
	  *
	  * Migration number is stored and represent the position BETWEEN migrations
	  * It is used as parameter of migrate, migrateFrom and undoFrom methods.
	  *
	  * Migration number: 0 -------------- 1 -------------- 2 -------------- 3 ------
	  * Migrations:       | Settings       | Configurations | ...            | ...
	  **/
	class Migration
	{
	public:
		Migration(const char*);
		virtual ~Migration();
		/**
		  * The up() method is automatically called by the Migration engine to do the migration.
		  * Put inside this method all the database changes you want. return false if you want
		  * to cancel the migration
		  **/
		virtual void	up() = 0;
		/**
		  * The down() method can be called by the Migration engine to undo the migration.
		  * Put inside this method the oposite changes of your up() method. return false if
		  * you want to cancel the migration
		  **/
		virtual void	down() = 0;
		const QString&	name();
		/** This method will run the up() or down() method depending on the argument
		  * it will also wrap the migration into a SQL transaction and throw on errors
		  **/
		bool			run(bool forward = true);

		/** Next function are helper methods to build your migrations.
		  * they make the syntax shorter and allow a better errors handling
		  **/
		bool	exec(const QString sql);
		bool	createTable(const QString name, const QString definition);
		bool	dropTable(const QString name);

	private:
		const QString	_name;
	};

	class MigrationEngine
	{
	public:
		MigrationEngine(DBCore& dbCore);
		~MigrationEngine();
		bool	migrate(int from, int to);	///< Run all migration from the first to the second
		bool	migrate(int to);			///< Run all migration from the actual to the given one
		bool	migrate();					///< Run all migration from the actual to the end
		bool	undo();						///< Undo all migration from the actual to the begining
		int		migrationsCount();			///< Get the number of available migrations
	private:
		QList<Migration*>	migrations;		///< Ordered list of all migrations
		DBCore&				db;
	};
}

#endif // MIGRATION_HPP
