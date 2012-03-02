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
	class Parser;
}

#ifndef PARSER_HPP
#define PARSER_HPP

#include <QVariant>
#include <QByteArray>

namespace Network
{
	/** This abstract class handle generic data parsing. child classes must be instancied to process parsing (like JsonParser)
	  *
	  * The parse method generate a QVariant data structure from a byte stream and must be implemented by child classes
	  * The generate method generate a byte stream from a QVariant structure using underlying format implemented by child classes
	  **/

	class Parser
	{
	public:
		virtual QVariant	parse(const QByteArray& data) const = 0;
		virtual QByteArray	serialize(const QVariant& data) const = 0;
	};
}

#include "jsonparser.hpp"

#endif // PARSER_HPP
