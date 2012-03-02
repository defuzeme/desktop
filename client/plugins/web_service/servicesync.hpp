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

namespace WebService {
	class ServiceSync;
}

#ifndef SERVICESYNC_HPP
#define SERVICESYNC_HPP

#include <QLinkedList>
#include <QBuffer>
#include <QMutex>
#include "staticplugin.hpp"
#include "playqueue.hpp"
#include "parameterizable.hpp"
#include "servicerequest.hpp"
#include "playqueue.hpp"
#include "websocket.hpp"

namespace WebService
{
	/** This plugin handles the realtime synchronization with the webservice (defuze.me)
	  * It handles push (http) and pull (websocket) transactions.
	  **/
	class ServiceSync : public Params::Parameterizable, public StaticPlugin
	{
		Q_OBJECT
		friend class ServiceRequest;

	public:
		ServiceSync();
		~ServiceSync();
		void							init();
		bool							hasRequests() const;		///< true if there are pending requests to be sent
		bool							isRequesting() const;		///< true if the client is waiting for the webservice
		ServiceRequest*					newRequest(const QString& verb, const QString& url);	///< prepare a new request

	private slots:
		void							sendNextRequest();			///< Send the next pending request to the webservice
		void							receiveResponse();			///< System slot used by pool management
		// Play queue sync slots
		void							sendPlayQueue();
		void							sendRemoveQueueElem(Queue::Queueable*);
		void							sendAddQueueElem(Queue::Queueable*);
		// WebSockets slots
		void							wsConnect();
		void							onMessage(const QByteArray& data);
		void							onOpen();
		void							onClose(const QString& error);

	private:
		void							wsMove(const QVariantMap& data);
		void							defineParams();

		QLinkedList<ServiceRequest*>	requests;				///< Pending requests pool
		QNetworkReply					*reply;					///< Current request reply
		QBuffer							buffer;					///< Data buffer when receiving multipart messages
		Queue::PlayQueue*				queue;
		Network::WebSocket*				ws;						///< WebSocket implementation
		int								ws_tries, ws_delay;
		QMutex							sync_mutex;				///< Prevent sync loop (server => client => server)
	};
}

#endif // SERVICESYNC_HPP
