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

namespace Network {
	class JsonParser;
}

#ifndef JSONPARSER_HPP
#define JSONPARSER_HPP

#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QString>
#include "parser.hpp"

namespace Network
{
	/** This class implement Json parsing and generation using QScriptEngine
	  **/

	class JsonParser : public Parser
	{
	public:
		JsonParser();

		QVariant	parse(const QByteArray& data) const;
		QByteArray	serialize(const QVariant& data) const;

		static void	test();																	///< Perform some unit tests
	private:
		QVariant	partialParse(const QScriptValue& value) const;
		QByteArray	partialSerialize(const QVariant& value, const short spaces) const;

		QByteArray	join( const QList<QByteArray>& list, const QByteArray& sep) const;		///< This method allows to join QByteArray together
		QString		sanitizeString(const QString& input) const;								///< This method provide json string escaping
		QByteArray	indent(const short spaces) const;										///< This method return a blank QByteArray to indent easily
	};
}

#endif // JSONPARSER_HPP
