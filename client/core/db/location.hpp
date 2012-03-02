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

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

namespace DB
{
	/** This class is used to store informations about one of the database file.
	  * We will have one instance per replication path.
	  **/

	class Location
	{
	public:
		Location(const QString path);
		QString		filePath() const;
		QDir		dirPath() const;
		bool		exists() const;
		QDateTime	lastModified() const;		///< Return file last modification date
		int			age() const;				///< Return number of seconds since last modification
		bool&		open();						///< Return if this location is the current database
		void		reload();
		void		mkPath() const;				///< Create all folders contained in the path

	private:
		QFileInfo	infos;
		bool		_open;
	};
}

#endif // LOCATION_HPP
