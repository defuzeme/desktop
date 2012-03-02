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
#include "exception.hpp"
#include <QApplication>
#include <QVariant>
#include <QStringList>
#include <QTime>

Exception::Exception()
{
}

Exception::Exception(unsigned code, const QString msg, const char *file, const int line)
{
	this->_msg = msg;
	this->file = file;
	this->line = line;
	this->userCode = code;
	extractCore();
	generateCode();
	Logger::log("Error at " + QTime::currentTime().toString() + ": " + description());
}

Exception::~Exception() throw()
{
}

Exception::Exception(const Exception& src)
{
	_msg = src._msg;
	file = src.file;
	line = src.line;
	userCode = src.userCode;
	module = src.module;
	code = src.code;
}

void		Exception::raise() const
{
	throw *this;
}

Exception*	Exception::clone() const
{
	return new Exception(*this);
}

void		Exception::extractCore() throw()
{
	module = "Base";
	QStringList path = file.split("/");
	for(int i = 0; i < path.size(); ++i)
	{
		if (path[i] == "core" && (i+2) < path.size())
		{
			module = path[i + 1];
			module[0] = module[0].toUpper();
			module += "Core";
		}
		if (path[i] == "plugins" && (i+2) < path.size())
		{
			module = path[i + 1];
			module[0] = module[0].toUpper();
			module += "Plugin";
		}
	}
}

void		Exception::generateCode() throw()
{
	code =	(qHash(module) & 0xFF << 16) |
			(qHash(QFileInfo(file).fileName()) & 0xFF << 8) |
			(userCode & 0xFF);
}

QString		Exception::hexCode() const throw()
{
	return QString("%1").arg(code, 6, 16, QLatin1Char('0')).toUpper();
}

QString		Exception::description() const throw()
{
	return (QString("[%1] %2Exception at %3:%4 -> %5").arg(hexCode(), module, file).arg(line).arg(_msg));
}

QString		Exception::msg() const throw()
{
	return _msg;
}
