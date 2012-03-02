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
	class Authenticator;
}

#ifndef AUTHENTICATOR_HPP
#define AUTHENTICATOR_HPP

#include <QString>
#include "networkcore.hpp"
#include "dbcore.hpp"
#include "main.hpp"
#include "parameterizable.hpp"

namespace Network
{
	/** This class is used by the NetworkCore to handle and maintaing authentication informations for the webservice API
	  *
	  * You should not instantiate it, the networkCore contain one.
	  * The networkCore will automatically call this class to fill API auth informations
	  * If the authenticator need credentials, it will emit needCredentials event so the receiver can fills required credentials and call back authenticate()
	  **/

	class Authenticator : public Params::Parameterizable
	{
		Q_OBJECT
	public:
		Authenticator(NetworkCore& net);
		~Authenticator();
		const QString	getToken();
		bool			hasToken() const;
		QVariantMap		getRadioInfo() const;

	public slots:
		void			authenticate(const QString& login, const QString& password);
		void			cancel();										///< Cancel the authentication request and clear the token
		void			invalidate();									///< Destroy token and query a new one

	private slots:
		void			receiveResponse();

	signals:
		void			failed(const QString& message);					///< Emited if the server rejected the authentication
		void			authenticated();								///< Emited after the authenticator sucessfully received a new token
		void			needCredentials(const QString& login);			///< Emited when the authenticator need credentials

	private:
		void			defineParams();

		NetworkCore&	network;										///< Keep a reference on the network core to send requests
		QString			password;										///< Keep the password cached no to ask it every time to the user
		QNetworkReply	*reply;
		bool			authenticating;
	};
}

#endif // AUTHENTICATOR_HPP
