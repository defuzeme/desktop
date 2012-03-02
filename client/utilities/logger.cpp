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

#include "logger.hpp"
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QDebug>

Logger::Logger()
{
	QDir dir;
	dir.mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + gl_MAIN_LOG_DIR);// Create the logs directory if needed.
	logFile = new QFile(logFileName());
	if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
	{
		qDebug() << "Log file is" << QFileInfo(*logFile).absoluteFilePath();
		logFile->write(logHeader());
	}
	else
	{
		qDebug() << "Cannot use Log file" << QFileInfo(*logFile).absoluteFilePath() << ": error" << logFile->error();
	}
}

Logger::~Logger()
{
	logFile->close();
	delete logFile;
}

void Logger::log(const QString &message)
{
	Logger::instance().updateLog(message);
}

void Logger::updateLog(QString msg) const
{
	msg += "\n";
	msg = ". " + msg;
	if (logFile->isOpen())
	{
		logFile->write(msg.toAscii());
		logFile->flush();
	}
}

QByteArray Logger::logHeader() const
{
	QString header(80, '#');
	header += "\n";
	return header.toAscii();
}

QString Logger::logFileName() const
{
	return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + gl_MAIN_LOG_DIR +
			"/" + gl_MAIN_LOG_FILE + "_" +
			QDateTime::currentDateTime().toString("yyyyMMdd") + "." + gl_MAIN_LOG_EXT;
}
