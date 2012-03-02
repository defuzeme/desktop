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

#ifndef MAIN_HPP
#define MAIN_HPP

#include <QtCore/QString>
#include <QtCore/QDateTime>

const static QString gl_APPLICATION_NAME = "defuze.me";
const static QString gl_APPLICATION_VERSION = "0.6.0";
const static QString gl_ORGANIZATION_DOMAIN = "defuze.me";
const static QString gl_ORGANIZATION_NAME = "defuze.me";
const static QString gl_VERSION_CODENAME = "beta";

const static QString gl_API_HOSTNAME = "api.defuze.me";
//const static QString gl_API_HOSTNAME = "localhost:3000";
const static QString gl_PUSH_HOSTNAME = "push.defuze.me:8080";
//const static QString gl_PUSH_HOSTNAME = "localhost:8080";

const unsigned short gl_RC_PORT = 3456;
const unsigned short gl_AD_ASK_PORT = 3457;
const unsigned short gl_AD_REPLY_PORT = 3458;

#endif // MAIN_HPP
