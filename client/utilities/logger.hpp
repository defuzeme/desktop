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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "singleton.hpp"
#include <QtCore/QFile>
#include <QDesktopServices>

const static QString gl_MAIN_LOG_DIR = "/logs";														///< Log directory
const static QString gl_MAIN_LOG_FILE = "log";														///< Log file name
const static QString gl_MAIN_LOG_EXT = "txt";														///< Log file extension

/** The tool to write logs
  *
  * The singleton instance is used to write log in a log file.
  * The log file is defined by:
  * - gl_MAIN_LOG_DIR
  * - gl_MAIN_LOG_FILE
  * - "YYYYMMDD"
  * - gl_MAIN_LOG_EXT
  *
  * You have to use the static method log() as follow:
  * \code
  * Logger::log("You message."); // NewLine character will be added.
  * \endcode
  **/

class Logger : public Singleton<Logger>
{
	friend class Singleton<Logger>;
public:
	static void log(const QString &message);														///< Write message into log file

protected:
	Logger();
	~Logger();

private:
	void updateLog(QString msg) const;
	QByteArray logHeader() const;
	QString logFileName() const;
	QFile *logFile;
};

#endif // LOGGER_HPP
