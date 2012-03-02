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

namespace Remote {
	class RemoteControl;
}

#ifndef REMOTECONTROL_HPP
#define REMOTECONTROL_HPP

#include <QObject>
#include <QSet>
#include "staticplugin.hpp"
#include "networkcore.hpp"
#include "playqueue.hpp"
#include "mainplayer.hpp"

using namespace Network;

namespace Remote
{
	/** This class handle remote control through mobile devices.
	  * Connection & Authentication is handled by the NetworkCore
	  **/

	class RemoteControl : public QObject, public StaticPlugin
	{
		Q_OBJECT
	public:
		explicit RemoteControl();
		virtual ~RemoteControl();
		void				init();								///< Automatically called after plugin referencing
		void				sendToAll(const QString& event, const QVariantMap& data = QVariantMap());

	signals:

	public slots:
		void				newRemoteClient(RemoteSock*);		///< Slot called by doorman when a new client authenticated
		void				removeRemoteClient();				///< Slot called when a client disconnected
		void				receiveEvent(const RemoteEvent&);	///< Slot called when a packet is received from the mobile device

		void				popQueue();
		void				removeQueueElem(Queue::Queueable* elem);
		void				addQueueElem(Queue::Queueable* elem);
		void				removeQueueElem(const RemoteEvent &packet);
		void				addQueueElem(const RemoteEvent &packet);
		void				moveQueueElem(const RemoteEvent &packet);
		void				play(const RemoteEvent &packet);
		void				pause(const RemoteEvent &packet);
		void				stop(const RemoteEvent &packet);
		void				next(const RemoteEvent &packet);
		void				playStatusChanged();

	private:
		void				sendPlayQueue(RemoteSock* client);	///< Send all the current playing queue
		void				sendPlayState(RemoteSock* client);	///< Send the current playing status

		QSet<RemoteSock*>	clients;
		Queue::PlayQueue*	play_queue;
		Player::MainPlayer*	player;
	};
}

#endif // REMOTECONTROL_HPP
