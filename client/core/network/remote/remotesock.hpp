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
	class RemoteSock;
}

#ifndef REMOTESOCK_HPP
#define REMOTESOCK_HPP

#include <QObject>
#include <QTcpSocket>
#include "remoteevent.hpp"
#include "parser.hpp"

namespace Network
{
	/** The RemoteSock class represents a remote connection from the mobile application.
	  *
	  * It will handle json events formating & automatic message id attribution.
	  * It will also protect the socket from injections and bad formatted data.
	  **/
	class RemoteSock : public QObject
	{
		friend class Doorman;
		friend class RemoteEvent;

		Q_OBJECT
	public:
		RemoteSock(QTcpSocket *sock);
		~RemoteSock();
		QString			ip() const;
		int				port() const;
		RemoteEvent&	newEvent(const QString& name);		///< Generate a new event linked to this client
		void			sendEvent(RemoteEvent *event);		///< Send a prepared event
		const Parser&	eventParser() const;				///< Return correct event parser
		const QString&	getToken() const;
		const QString&	getDevice() const;
		const QString&	getVersion() const;

	signals:
		void			receiveEvent(const RemoteEvent&);	///< This signal is emited when a new event is received and parsed

	public slots:
		void			receive();							///< This slot is called when data is received

	private:
		QTcpSocket		*sock;								///< Stores the tcp socket
		QString			token, device, version;				///< Stores informations on the device
		QByteArray		buffer;								///< Stores pending data chunks
		unsigned		nextEventId;						///< Autoincremented event id
		QMap<unsigned, RemoteEvent*>	events;				///< Stores all sent events
	};
}

#endif // REMOTESOCK_HPP
