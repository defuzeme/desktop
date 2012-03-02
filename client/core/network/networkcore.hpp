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
	class NetworkCore;
}

#ifndef NETWORKCORE_HPP
#define NETWORKCORE_HPP

#include <QString>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTcpServer>
#include <QUdpSocket>
#include <QUrl>
#include "core.hpp"
#include "authenticator.hpp"
#include "parser.hpp"
#include "doorman.hpp"

namespace Network
{
	/** This is the network wrapper, It manages every connections and protocols used in the application
	  **/

	class NetworkCore : public QObject, public Core
	{
		Q_OBJECT
	public:
		NetworkCore(QStringList &arguments);
		~NetworkCore();
		void							init(Cores* cores);
		void							aboutToQuit();
		const QString					name();

		void							initRCServer(unsigned short port);					///< Initialize remote control server
		QNetworkAccessManager&			web();												///< Return the HTTP connection pool
		QTcpServer&						remoteControl();									///< Return the remote control tcp server
		QUdpSocket&						autoDiscovery();									///< Return the auto discovery udp server
		QNetworkRequest					apiRequest(const QString& url) const;				///< Factory to generate API requests
		const Parser&					apiParser() const;									///< Return API parser
		QVariantList					discoveryAddresses() const;
		QVariantMap						getRadioInfo() const;

	signals:
		void							newRemoteClient(RemoteSock*);						///< emited on new remote connection
		void							APIAuthenticated();

	public slots:
		void							invalidateWebToken();								///< Call to renew token (when getting 401)

	private slots:
		void							giveCredentials(const QString&);
		void							giveAuthorization(const RemoteEvent&);
		void							discoveryRequest();									///< called when a discovery request is received

	private:
		QNetworkAccessManager			*webManager;										///< This manager will handle all HTTP requests (pooling)
		QTcpServer						*rcServer;											///< This socket will handle remote control (mobile devices)
		QUdpSocket						*adServer;											///< This socket will handle auto discovery requests (mobile devices)
		Authenticator					*authenticator;										///< This class will handle webservice API authentication
		Doorman							*doorman;											///< This class will handle remote control device filtering
		JsonParser						defaultApiParser;									///< This parser will handle all API related data formatting
	};
}

#endif // NETWORKCORE_HPP
