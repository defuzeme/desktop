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

#include "authenticator.hpp"
#include "logger.hpp"

using namespace Network;

Authenticator::Authenticator(NetworkCore& net) : network(net), authenticating(false)
{
	setParamsName("authenticator");
}

Authenticator::~Authenticator()
{
}

void	Authenticator::defineParams()
{
}

void	Authenticator::invalidate()
{
	setParameter("token", QVariant());
	getToken();
}

QVariantMap		Authenticator::getRadioInfo() const
{
	return getParameter("radio", QVariant()).toMap();
}

void	Authenticator::receiveResponse()
{
	if (reply->error())
	{
		QVariant data = network.apiParser().parse(reply->readAll());
		QString	error = data.toMap()["error"].toString();
		Logger::log(QString("Authenticator: can't authenticate: %1").arg(reply->errorString()));
		password.clear();
		setParameter("token", QVariant());
		emit failed(error);
	}
	else
	{
		QVariant data = network.apiParser().parse(reply->readAll());
		setParameter("token", data.toMap()["token"].toString());
		setParameter("radio", data.toMap()["radio"].toMap());
		Logger::log(QString("Authenticator: new token received: %1").arg(getParameter("login").toString()));
		commitParameters();
		emit authenticated();
	}
	authenticating = false;
	reply->deleteLater();
}

void	Authenticator::authenticate(const QString& login, const QString& password)
{
	QNetworkRequest	request;
	QVariantMap		data;

	// store login to autofill forms
	setParameter("login", login);
	this->password = password;
	setParameter("token", "pending");

	// build POST data hash
	data["login"] = login;
	data["password"] = password;

	// create request
	request = network.apiRequest("session");

	Logger::log(QString("Authenticator: authenticating user %1").arg(login));

	// send request to web service
	reply = network.web().post(request, network.apiParser().serialize(data));

	// handle events
	connect(reply, SIGNAL(finished()), SLOT(receiveResponse()));
}

void	Authenticator::cancel()
{
	if (getParameter("token", "").toString() == "pending")
		reply->abort();
	setParameter("token", QVariant());
	authenticating = false;
	Logger::log("Authenticator: authentication cancelled by user");
}

const QString	Authenticator::getToken()
{
	if (!hasToken() && 	!authenticating)
	{
		authenticating = true;
		// Try to authenticate if no token is stored
		if (getParameter("login", "").toString().length() == 0 || password.length() == 0)
		{
			// If we need some credentials, ask them to the user
			// Change the token to a fake value not to request credentials several times
			emit needCredentials(getParameter("login", "").toString());
		}
		else
		{
			// If we have cached credentials, just use them
			// Change the token to a fake value not to request token several times
			authenticate(getParameter("login", "").toString(), password);
		}
	}
	// Remember token fetching is asynchronous, so is no token is stored
	// getToken will never return it synchronously
	return getParameter("token", "").toString();
}

bool	Authenticator::hasToken() const
{
	return getParameter("token", "").toString().length() > 0;
}


