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

#include <QHostAddress>
#include "remotesock.hpp"
#include "logger.hpp"
#include "status.hpp"

using namespace Network;

RemoteSock::RemoteSock(QTcpSocket *sock) : sock(sock)
{
	connect(sock, SIGNAL(disconnected()), SLOT(deleteLater()));
	connect(sock, SIGNAL(readyRead()), SLOT(receive()));
	nextEventId = 1;
}

RemoteSock::~RemoteSock()
{
	sock->deleteLater();
}

QString		RemoteSock::ip() const {
	return sock->peerAddress().toString();
}

int			RemoteSock::port() const {
	return sock->peerPort();
}

const Parser&		RemoteSock::eventParser() const
{
	static JsonParser	parser;
	return parser;
}

RemoteEvent&	RemoteSock::newEvent(const QString& name)
{
	RemoteEvent	*event = new RemoteEvent(this);

	event->uid = nextEventId;
	event->event = name;
	nextEventId++;
	return *event;
}

void			RemoteSock::sendEvent(RemoteEvent *event)
{
	sock->write(event->toStream());
}

const QString&	RemoteSock::getToken() const
{
	return token;
}

const QString&	RemoteSock::getDevice() const
{
	return device;
}

const QString&	RemoteSock::getVersion() const
{
	return version;
}

void			RemoteSock::receive()
{
	int			split;

	buffer += sock->readAll();
	while ((split = buffer.indexOf('\0')) >= 0)
	{
		qDebug() << "received event (" + QString::number(split) + "): " + buffer.left(split);
		RemoteEvent	event(this, buffer.left(split));

		buffer.remove(0, split+1);
		if (event.isReply())
		{
			RemoteEvent	*request = event.getRequest();
			if (request)
			{
				emit request->receiveReply(event);
				events.remove(request->getUid());
				delete request;
			}
			else
			{
				Logger::log(QString("Remote: bad event reply id received: %1").arg(event.getRequestId()));
			}
		}
		else
		{
			emit receiveEvent(event);
		}
	}
}
