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
	class RemoteEvent;
}

#ifndef REMOTEEVENT_HPP
#define REMOTEEVENT_HPP

#include <QObject>
#include <QVariantMap>
#include "remotesock.hpp"

namespace Network
{
	/** The RemotEvent class define an event occuring between the mobile app and the client
	  *
	  * To send an event you should follow this pattern:
	  * \code
	  * // Create an event associated to the client
	  * RemoteEvent	*event = client->newEvent("authenticationRequest");
	  * // Fill any data field you want
	  * (*event)["msg"] = "All right sir!";
	  * // Send it to the client
	  * event->send();
	  * // You MUST not delete the event by hand, the remote sock will do it when it's done.
	  * \endcode
	  *
	  * To send an event waiting for a reply you should send it like this:
	  * \code
	  * // This will link the given callback to the first received reply
	  * event->sendWithReply(this, SLOT(replyReceived(const RemoteEvent&)));
	  * \endcode
	  *
	  **/

	class RemoteEvent : public QObject
	{
		Q_OBJECT
		friend class	RemoteSock;
		friend class	Doorman;

	public:
		enum AutoReplyType {
			OK,
			NO_CHANGES,
			ERROR
		};

		unsigned		getUid() const;
		bool			isReply() const;
		unsigned		getRequestId() const;
		RemoteEvent*	getRequest() const;
		const QString&	getEvent() const;
		RemoteSock*		getRemote() const;
		void			setData(const QVariantMap& data);
		QVariant&		operator[](const QString& key);
		QVariant		operator[](const QString& key) const;
		void			send();											///< send the event to the saved remote
		void			sendWithReply(const QObject *, const char *);	///< send the event and connect the reply callback
		QByteArray		toStream() const;								///< Turn the event into a formated json stream
		RemoteEvent&	newReply(const QString& name) const;			///< Create a reply event
		void			autoReply(AutoReplyType type) const;

	signals:
		void			receiveReply(const RemoteEvent&);				///< Signal called when a reply to this event is received

	public slots:

	private:
		RemoteEvent(RemoteSock *remote, const QByteArray& packet = ""); ///< Private constructors, only instanciable by friend classes
		~RemoteEvent();

		QString			event;											///< Event name (like "play")
		unsigned		uid;											///< Event uid, set by the RemoteSock class at instantiation
		unsigned		replyTo;										///< The event we reply to,
		QVariantMap		data;
		RemoteSock		*remote;										///< The remote connection linked to this event
	};
}

#endif // REMOTEEVENT_HPP
