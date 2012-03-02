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

#include "remoteevent.hpp"

using namespace Network;

RemoteEvent::RemoteEvent(RemoteSock *remote, const QByteArray& packet) : remote(remote)
{
	if (packet.size() > 0)
	{
		QVariant	json = remote->eventParser().parse(packet);
		event = json.toMap()["event"].toString();
		uid = json.toMap()["uid"].toUInt();
		replyTo = json.toMap()["replyTo"].toUInt();
		if (json.toMap().contains("data"))
			data = json.toMap()["data"].toMap();
	}
	else
	{
		uid = 0;
		replyTo = 0;
	}
}

RemoteEvent::~RemoteEvent()
{
}

unsigned		RemoteEvent::getUid() const
{
	return uid;
}

bool			RemoteEvent::isReply() const
{
	return (getRequestId() > 0);
}

unsigned		RemoteEvent::getRequestId() const
{
	return replyTo;
}

RemoteSock*		RemoteEvent::getRemote() const
{
	return remote;
}

RemoteEvent*	RemoteEvent::getRequest() const
{
	return remote->events[getRequestId()];
}

const QString&	RemoteEvent::getEvent() const
{
	return event;
}

void			RemoteEvent::send()
{
	remote->sendEvent(this);
	deleteLater();
}

void			RemoteEvent::sendWithReply(const QObject *receiver, const char *method)
{
	connect(this, SIGNAL(receiveReply(const RemoteEvent&)), receiver, method);
	remote->events[getUid()] = this;
	remote->sendEvent(this);
}

void			RemoteEvent::setData(const QVariantMap& data)
{
	this->data = data;
}

QVariant&		RemoteEvent::operator[](const QString& key)
{
	return data[key];
}

QVariant		RemoteEvent::operator[](const QString& key) const
{
	return data[key];
}

QByteArray		RemoteEvent::toStream() const
{
	QByteArray	out;
	QVariantMap	map;

	map["event"] = getEvent();
	map["uid"] = getUid();
	if (isReply())
		map["replyTo"] = getRequestId();
	if (!data.empty())
		map["data"] = data;
	out = remote->eventParser().serialize(map);
		out += '\0';
	return out;
}

RemoteEvent&	RemoteEvent::newReply(const QString& name) const
{
	RemoteEvent	&event = remote->newEvent(name);

	event.replyTo = this->uid;
	return event;
}

void			RemoteEvent::autoReply(AutoReplyType type) const
{
	if (type == OK)
		newReply("ok").send();
	if (type == NO_CHANGES)
		newReply("noChanges").send();
	if (type == ERROR)
		newReply("error").send();
}

