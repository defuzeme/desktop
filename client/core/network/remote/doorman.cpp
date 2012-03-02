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

#include <QDebug>
#include "doorman.hpp"
#include "remotesock.hpp"
#include "remoteevent.hpp"
#include "logger.hpp"
#include "status.hpp"

using namespace Network;

Doorman::Doorman(QTcpServer *rcServer) : rcServer(rcServer)
{
	connect(rcServer, SIGNAL(newConnection()), SLOT(newConnection()));
	setParamsName("doorman");
}

void	Doorman::loadKnownDevices()
{
	knownDevices = getParameter("known-devices", QVariantMap()).toMap();
}

void	Doorman::storeKnownDevices()
{
	setParameter("known-devices", knownDevices);
	commitParameters();
}

void	Doorman::defineParams()
{
}

void	Doorman::newConnection()
{
	QTcpSocket	*socket;

	while ((socket = rcServer->nextPendingConnection()))
	{
		RemoteSock	*client = new RemoteSock(socket);
		RemoteEvent	&event = client->newEvent("authenticationRequest");

		Logger::log(QString("Remote: connection received from %1:%2").arg(client->ip()).arg(client->port()));
		event.sendWithReply(this, SLOT(authentication(const RemoteEvent&)));
	}
}

void	Doorman::authentication(const RemoteEvent& event)
{
	if (event.getEvent() == "authentication")
	{
		QString token = event["token"].toString();
		event.remote->token = token;
		event.remote->device = event["deviceName"].toString();
		event.remote->version = event["appVersion"].toString();
		Logger::log(QString("Remote: authentication token received from %2: %1").arg(token, event.getRemote()->ip()));
		// Check for already autorized (or not) devices
		loadKnownDevices();
		if (knownDevices.contains(token))
		{
			const QVariantMap &info = knownDevices[token].toMap();
			if (info.contains("accepted"))
			{
				answered(event.remote, event.uid, info["accepted"].toBool(), true);
				Notification::Status::gMessage(tr("Remote device %1 has been %2").arg(
												   event.remote->device, (info["accepted"].toBool() ? tr("accepted") : tr("rejected"))), Notification::DEVICE);
			}
			else
				emit needAuthorization(event);
		}
		else
		{
			Notification::Status::gMessage(tr("Remote device %1 is requesting remote control").arg(event.remote->device), Notification::DEVICE);
			emit needAuthorization(event);
		}
	}
	else
	{
		Logger::log(QString("Remote: bad authentication reply: %1").arg(event.getEvent()));
	}
}

void	Doorman::answered(RemoteSock* remote, unsigned msgid, bool accepted, bool permanent)
{
	if (accepted)
	{
		RemoteEvent	&reply = remote->newEvent("authenticated");
		reply.replyTo = msgid;
		Logger::log(QString("Remote: user accepted client %1, permanent = %2").arg(remote->ip()).arg(permanent));
		reply.send();
		emit authenticatedRemote(remote);
	}
	else
	{
		Logger::log(QString("Remote: user rejected client %1, permanent = %2").arg(remote->ip()).arg(permanent));
		RemoteEvent	&reply = remote->newEvent("authenticationFailed");
		reply.replyTo = msgid;
		reply["msg"] = "Sorry, you've been rejected by the client software";
		reply.send();
	}
	if (permanent)
	{
		// Store choice to database
		QVariantMap	info;
		info["accepted"] = accepted;
		info["last-ip"] = remote->ip();
		info["device-name"] = remote->getDevice();
		info["app-version"] = remote->getVersion();
		Logger::log(QString("Remote: store choice to database for %1").arg(remote->getToken()));
		knownDevices[remote->getToken()] = info;
		storeKnownDevices();
	}
}

