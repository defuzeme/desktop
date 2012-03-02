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
	class WebSocket;
}

#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <QTcpSocket>
#include <QUrl>
#include "webframe.hpp"

namespace Network
{
	/** This class is a C++/Qt implementation of the WebSocket 07 draft specifications
	  * http://tools.ietf.org/html/draft-ietf-hybi-thewebsocketprotocol-07
	  **/

	const QByteArray WebSocketSecret = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	class WebSocket : public QTcpSocket
	{
		Q_OBJECT
	public:
		explicit WebSocket(const QString& uri);
		~WebSocket();
		void		onMessage(const QObject* receiver, const char* method);
		void		onOpen(const QObject* receiver, const char* method);
		void		onClose(const QObject* receiver, const char* method);
		void		send(WebFrame& frame);

	signals:
		void		messageSignal(const QByteArray& data);
		void		openSignal();
		void		closeSignal(const QString& error);

	public slots:
		void		connect();

	private slots:
		void		errorSlot(QAbstractSocket::SocketError error);
		void		handshake();
		void		receive();

	private:
		void		generateToken();
		bool		validToken(const QByteArray& accept);
		bool		parseHandshake();
		bool		parseFrame();

		QUrl		uri;
		QByteArray	buffer;
		WebFrame*	frame;
		QVariantMap	header;
		QByteArray	token;
		bool		handshaking;
	};
}

#endif // WEBSOCKET_HPP
