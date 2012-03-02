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

#include "migration_list.hpp"

using namespace DB;

void	SchedulerMigration::up()
{
	createTable("events", "id INTEGER PRIMARY KEY, title VARCHAR(255), description VARCHAR(255), day INTEGER, start INTEGER, duration INTEGER");
	exec("CREATE UNIQUE INDEX event_id ON events(id)");

	createTable("colors", "id INTEGER PRIMARY KEY, color VARCHAR(6)");
	exec("CREATE UNIQUE INDEX color_id ON colors(id)");

	createTable("colors_events", "event_id INTEGER PRIMARY KEY, color_id INTEGER");
	exec("CREATE UNIQUE INDEX color_event_id ON colors_events(event_id)");

	createTable("events_playlists", "id INTEGER PRIMARY KEY, event_id INTEGER, playlist_id INTEGER");
	exec("CREATE UNIQUE INDEX event_playlist_id ON events_playlists(id)");

	exec("insert into colors values('1', 'FFAD46')");
	exec("insert into colors values('2', 'A4BDFC')");
	exec("insert into colors values('3', '7AE7BF')");
	exec("insert into colors values('4', 'DBADFF')");
	exec("insert into colors values('5', 'FF887C')");
	exec("insert into colors values('6', 'FBD75B')");
	exec("insert into colors values('7', 'FFB878')");
	exec("insert into colors values('8', '46D6DB')");
	exec("insert into colors values('9', 'E1E1E1')");
}

void    SchedulerMigration::down()
{
	dropTable("colors");
	dropTable("events");
	dropTable("colors_events");
	dropTable("events_playlists");
}
