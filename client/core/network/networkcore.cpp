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

#include <QHostInfo>
#include "guicore.hpp"
#include "networkcore.hpp"
#include "exception.hpp"
#include "parser.hpp"
#include "logger.hpp"
#include "loginview.hpp"
#include "authview.hpp"
#include "status.hpp"

using namespace Network;

NetworkCore::NetworkCore(QStringList &)
{
	webManager = new QNetworkAccessManager();
	rcServer = new QTcpServer();
	adServer = new QUdpSocket();
	authenticator = NULL;
	doorman = NULL;
}

NetworkCore::~NetworkCore()
{
	delete rcServer;
	delete webManager;
	delete adServer;
}

void	NetworkCore::init(Cores *c)
{
	JsonParser::test();

	cores = c;

	// Web service
	authenticator = new Authenticator(*this);
	authenticator->setParamsBackEnd(cores->db());
	authenticator->registerToParamsCore(cores->params());
	connect(authenticator, SIGNAL(needCredentials(const QString&)), SLOT(giveCredentials(const QString&)));
	connect(authenticator, SIGNAL(authenticated()), SIGNAL(APIAuthenticated()));
	authenticator->getToken();

	// Remote control
	initRCServer(gl_RC_PORT);

	// Auto discovery
	adServer->bind(gl_AD_ASK_PORT, QUdpSocket::ShareAddress);
	connect(adServer, SIGNAL(readyRead()), this, SLOT(discoveryRequest()));
}

void	NetworkCore::aboutToQuit()
{
	if (authenticator)
		delete authenticator;
	if (doorman)
		delete doorman;
}

void	NetworkCore::initRCServer(unsigned short port)
{
	if (!rcServer->listen(QHostAddress::Any, port))
	{
		Logger::log(QString("Network: RCServer could not listen port %1").arg(port));
		Notification::Status::gMessage(tr("Could not listen TCP port %1, launched twice or firewall?").arg(port), Notification::ERR);
	}

	if (!doorman)
	{
		doorman = new Doorman(rcServer);
		doorman->setParamsBackEnd(cores->db());
		doorman->registerToParamsCore(cores->params());
		connect(doorman, SIGNAL(authenticatedRemote(RemoteSock*)), SIGNAL(newRemoteClient(RemoteSock*)));
		connect(doorman, SIGNAL(needAuthorization(const RemoteEvent&)), SLOT(giveAuthorization(const RemoteEvent&)));
	}
}


void	NetworkCore::discoveryRequest()
{
	while (adServer->hasPendingDatagrams())
	{
		QByteArray		datagram;
		QHostAddress	sender;
		quint16			senderPort;

		datagram.resize(adServer->pendingDatagramSize());
		adServer->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
		if (datagram == "\"defuze.me:discovery\"")
		{
			Logger::log("Network: new discovery request from " + sender.toString());
			QUdpSocket().writeDatagram(JsonParser().serialize(discoveryAddresses()), QHostAddress::Broadcast, gl_AD_REPLY_PORT);
		}
		else
		{
			Logger::log("Network: bad udp request from " + sender.toString());
		}
	}
}

QVariantList	NetworkCore::discoveryAddresses() const
{
	QVariantList	list;
	QString			host = QHostInfo::localHostName();
	QVariantMap		address;

	address["host"] = host;
	address["port"] = gl_RC_PORT;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	for (int i = 0; i < ipAddressesList.size(); ++i)
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i) != QHostAddress::LocalHostIPv6)
		{
			address["ip"] = ipAddressesList.at(i).toString();
			address["protocol"] = (ipAddressesList.at(i).protocol() ? "IPv6" : "IPv4");
			list << address;
		}
	if (list.isEmpty())
	{
		address["ip"] = QHostAddress(QHostAddress::LocalHost).toString();
		list << address;
	}
	return list;
}


void	NetworkCore::invalidateWebToken()
{
	authenticator->invalidate();
}

QVariantMap		NetworkCore::getRadioInfo() const
{
	if (authenticator)
		return authenticator->getRadioInfo();
	else
		return QVariantMap();
}

void	NetworkCore::giveCredentials(const QString& login)
{
	LoginView *lv = new LoginView(login);

	// This slot is called when the authenticator ask for credentials,
	// it displays a window.
	authenticator->connect(lv, SIGNAL(authenticate(QString, QString)), SLOT(authenticate(QString, QString)));
	authenticator->connect(lv, SIGNAL(cancelled()), SLOT(cancel()));
	lv->connect(authenticator, SIGNAL(failed(QString)), SLOT(login_failed(QString)));
	lv->connect(authenticator, SIGNAL(authenticated()), SLOT(login_sucess()));
	cores->gui()->popup(lv);
}

void	NetworkCore::giveAuthorization(const RemoteEvent& event)
{
	AuthView *av = new AuthView(event);

	// This slot is called when the doorman ask for authorization,
	// it displays a window.
	doorman->connect(av, SIGNAL(answered(RemoteSock*, unsigned, bool, bool)), SLOT(answered(RemoteSock*, unsigned, bool, bool)));
	cores->gui()->popup(av);
}

const QString NetworkCore::name()
{
	return "Network";
}

QNetworkAccessManager&	NetworkCore::web()
{
	return *webManager;
}

QTcpServer&				NetworkCore::remoteControl()
{
	return *rcServer;
}

QUdpSocket&				NetworkCore::autoDiscovery()
{
	return *adServer;
}

// This is our default API parser, all parsing & serialization must be done using this method
// This way we can easily switch our entire application API format.
const Parser&			NetworkCore::apiParser() const
{
	return defaultApiParser;
}

QNetworkRequest			NetworkCore::apiRequest(const QString& url) const
{
	QNetworkRequest	request;
	QUrl			fullUrl("http://" + gl_API_HOSTNAME + "/" + url);

	request.setUrl(fullUrl);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	request.setRawHeader("Accept", "application/json");
	request.setRawHeader("Connection", "Close");
	request.setRawHeader("User-Agent", QString("%1 client %2").arg(gl_APPLICATION_NAME, gl_APPLICATION_VERSION).toUtf8());
	request.setRawHeader("Client-Version", gl_APPLICATION_VERSION.toUtf8());
	request.setRawHeader("API-Token", authenticator->getToken().toUtf8());

	Logger::log(QString("Network: new API request on %1").arg(fullUrl.toString()));

	return request;
}
