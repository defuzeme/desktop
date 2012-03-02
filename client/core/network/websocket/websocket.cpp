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

#include "websocket.hpp"
#include "logger.hpp"
#include <QHttpResponseHeader>
#include <QCryptographicHash>
#include <QApplication>

using namespace Network;

WebSocket::WebSocket(const QString& uri) : uri(uri), frame(0)
{
	handshaking = false;
	QObject::connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(errorSlot(QAbstractSocket::SocketError)));
	QObject::connect(this, SIGNAL(connected()), SLOT(handshake()));
	QObject::connect(this, SIGNAL(readyRead()), SLOT(receive()));
}

WebSocket::~WebSocket()
{
}

void		WebSocket::generateToken()
{
	token.resize(16);
	for(int i = 0; i < 16; i++)
		token[i] = qrand() % 0xFF;
}

bool		WebSocket::validToken(const QByteArray& accept)
{
	QByteArray hash = QCryptographicHash::hash(token.toBase64() + WebSocketSecret, QCryptographicHash::Sha1);
	return (accept == hash.toBase64());
}

void		WebSocket::connect()
{
	generateToken();
	if (isOpen())
	{
		disconnectFromHost();
		readAll();
	}
	buffer.clear();
	connectToHost(uri.host(), uri.port());
	Logger::log(QString("WebSocket: connecting to %1:%2").arg(uri.host()).arg(uri.port()));
}

void		WebSocket::send(WebFrame& frame)
{
	while (frame.remainingFrames() > 0)
	{
		QByteArray	data = frame.encoded();
		write(data);
		qApp->processEvents();
	}
}

void		WebSocket::onMessage(const QObject* receiver, const char* method)
{
	QObject::connect(this, SIGNAL(messageSignal(QByteArray)), receiver, method);
}

void		WebSocket::onOpen(const QObject* receiver, const char* method)
{
	QObject::connect(this, SIGNAL(openSignal()), receiver, method);
}

void		WebSocket::onClose(const QObject* receiver, const char* method)
{
	QObject::connect(this, SIGNAL(closeSignal(QString)), receiver, method);
}

void		WebSocket::errorSlot(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error);
	Logger::log(QString("WebSocket: %1").arg(errorString()));
	emit closeSignal(errorString());
}

void		WebSocket::handshake()
{
	handshaking = true;
	QString	path = uri.path();
	if (uri.encodedQuery().size() > 0)
		path += "?" + QString::fromUtf8(uri.encodedQuery());
	write(QString("GET %1 HTTP/1.1\r\n").arg(path).toUtf8());
	write(QString("Host: %1\r\n").arg(uri.host()).toUtf8());
	write("Upgrade: websocket\r\n");
	write("Connection: Upgrade\r\n");
	write(QString("Sec-WebSocket-Key: %1\r\n").arg(QString::fromUtf8(token.toBase64())).toUtf8());
	write("Sec-WebSocket-Protocol: chat\r\n");
	write("Sec-WebSocket-Version: 8\r\n");
	write("\r\n");
}

bool	WebSocket::parseHandshake()
{
	if (!canReadLine())
		return false;

	QByteArray	data = readLine();
	if (data == "\r\n")	// Header end
	{
		QHttpResponseHeader	header(QString::fromUtf8(buffer));
		if (header.statusCode() == 101 &&
			header.value("Upgrade") == "websocket" &&
			validToken(header.value("Sec-WebSocket-Accept").toUtf8()))
		{
			handshaking = false;
			Logger::log("WebSocket: connection ok");
			buffer.clear();
			emit openSignal();
		}
		else
		{
			Logger::log(QString("WebSocket: invalid handshake"));
			emit closeSignal(tr("Invalid handshake"));
			disconnectFromHost();
			return false;
		}
	}
	else
		buffer += data;
	return true;
}

bool	WebSocket::parseFrame()
{
	if (frame)
		frame->append(this);
	else
	{
		frame = WebFrame::fromStream(this);
		if (!frame)
		{
			Logger::log("WebSocket: invalid framing data");
			emit closeSignal(tr("Invalid framing data"));
			disconnectFromHost();
			return false;
		}
	}
	if (frame && frame->complete())
	{
		emit messageSignal(frame->content());
		delete frame;
		frame = 0;
	}
	return true;
}

void		WebSocket::receive()
{
	while (bytesAvailable())
	{
		if (handshaking)
		{
			if (!parseHandshake())
				return;
		}
		else
		{
			if (!parseFrame())
				return;
		}
	}
}
