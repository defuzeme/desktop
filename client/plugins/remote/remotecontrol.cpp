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

#include "remotecontrol.hpp"
#include "queuetrack.hpp"

using namespace Remote;

RemoteControl::RemoteControl()
{
}

RemoteControl::~RemoteControl()
{
}

void	RemoteControl::init()
{
	play_queue = plugins->cast<Queue::PlayQueue>("queue");
	player = plugins->cast<Player::MainPlayer>("player");
	connect(cores->net(), SIGNAL(newRemoteClient(RemoteSock*)), SLOT(newRemoteClient(RemoteSock*)));
	connect(play_queue, SIGNAL(popQueue()), SLOT(popQueue()));
	connect(play_queue, SIGNAL(removeQueueElem(Queue::Queueable*)), SLOT(removeQueueElem(Queue::Queueable*)));
	connect(play_queue, SIGNAL(addQueueElem(Queue::Queueable*)), SLOT(addQueueElem(Queue::Queueable*)));
	connect(player, SIGNAL(played()), SLOT(playStatusChanged()));
	connect(player, SIGNAL(paused()), SLOT(playStatusChanged()));
	connect(player, SIGNAL(stopped()), SLOT(playStatusChanged()));
}

void		RemoteControl::newRemoteClient(RemoteSock* sock)
{
	clients.insert(sock);
	connect(sock, SIGNAL(receiveEvent(const RemoteEvent&)), SLOT(receiveEvent(const RemoteEvent&)));
	connect(sock, SIGNAL(destroyed()), SLOT(removeRemoteClient()));
	sendPlayQueue(sock);
	sendPlayState(sock);
}

void		RemoteControl::removeRemoteClient()
{
	clients.remove(static_cast<RemoteSock*>(sender()));
}

void		RemoteControl::sendToAll(const QString& event, const QVariantMap& data)
{
	QSet<RemoteSock*>::const_iterator	it;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		RemoteSock	*client = (*it);
		RemoteEvent	&evt = client->newEvent(event);

		evt.setData(data);
		evt.send();
	}
}

void		RemoteControl::receiveEvent(const RemoteEvent &packet)
{
	if (packet.getEvent() == "removeQueueElem")
		removeQueueElem(packet);
	else if (packet.getEvent() == "addQueueElem")
		addQueueElem(packet);
	else if (packet.getEvent() == "moveQueueElem")
		moveQueueElem(packet);
	else if (packet.getEvent() == "play")
		play(packet);
	else if (packet.getEvent() == "pause")
		pause(packet);
	else if (packet.getEvent() == "stop")
		stop(packet);
	else if (packet.getEvent() == "next")
		next(packet);
	else
	{
		qDebug() << "Unknow event received: " << packet.getEvent();
	}
}

void		RemoteControl::popQueue()
{
	sendToAll("popQueue");
}

void		RemoteControl::removeQueueElem(Queue::Queueable* elem)
{
	sendToAll("removeQueueElem", elem->getNetData());
}

void		RemoteControl::removeQueueElem(const RemoteEvent &packet)
{
	unsigned int		position = packet["position"].toInt();

	if (position >= play_queue->getQueue().size())
		return packet.autoReply(RemoteEvent::ERROR);
	play_queue->remove(position);
	play_queue->emitAltered();
	packet.autoReply(RemoteEvent::OK);
}

void		RemoteControl::addQueueElem(Queue::Queueable* elem)
{
	sendToAll("newQueueElem", elem->getNetData());
}

void		RemoteControl::addQueueElem(const RemoteEvent &packet)
{
	unsigned int		position = packet["position"].toInt();
	int					track_id = packet["content"].toMap()["id"].toInt();
	Library::AudioTrack	*track = Library::AudioTrack::getTrack(track_id);
	if (!track)
		return packet.autoReply(RemoteEvent::ERROR);
	if (position >= play_queue->getQueue().size())
		return packet.autoReply(RemoteEvent::ERROR);

	play_queue->add(new Queue::QueueTrack(*track), position);
	play_queue->emitAltered();
	packet.autoReply(RemoteEvent::OK);
}

void		RemoteControl::moveQueueElem(const RemoteEvent &packet)
{
	unsigned int		oldPosition = packet["oldPosition"].toInt();
	unsigned int		newPosition = packet["position"].toInt();
	if (oldPosition >= play_queue->getQueue().size())
		return packet.autoReply(RemoteEvent::ERROR);
	Queue::Queueable*	elem = play_queue->getQueue()[oldPosition];
	Library::AudioTrack	*track = elem->toQueueTrack()->getTrack();
	if (!track)
		return packet.autoReply(RemoteEvent::ERROR);
	if (newPosition >= play_queue->getQueue().size())
		return packet.autoReply(RemoteEvent::ERROR);

	if (newPosition == oldPosition)
		return packet.autoReply(RemoteEvent::NO_CHANGES);

	play_queue->remove(oldPosition);
	play_queue->add(new Queue::QueueTrack(*track), newPosition);
	play_queue->emitAltered();
	packet.autoReply(RemoteEvent::OK);
}

void		RemoteControl::play(const RemoteEvent &packet)
{
	if (player->play())
		packet.autoReply(RemoteEvent::OK);
	else
		packet.autoReply(RemoteEvent::NO_CHANGES);
}

void		RemoteControl::pause(const RemoteEvent &packet)
{
	if (player->pause())
		packet.autoReply(RemoteEvent::OK);
	else
		packet.autoReply(RemoteEvent::NO_CHANGES);
}

void		RemoteControl::stop(const RemoteEvent &packet)
{
	if (player->stop())
		packet.autoReply(RemoteEvent::OK);
	else
		packet.autoReply(RemoteEvent::NO_CHANGES);
}

void		RemoteControl::next(const RemoteEvent &packet)
{
	if (player->crossfadeNext())
		packet.autoReply(RemoteEvent::OK);
	else
		packet.autoReply(RemoteEvent::NO_CHANGES);
}

void		RemoteControl::playStatusChanged()
{
	QString		state = player->isPlaying() ? "play" : "pause";
	sendToAll(state);
}

void		RemoteControl::sendPlayQueue(RemoteSock* client)
{
	Queue::QueueableDeque::const_iterator	it;
	for (it = play_queue->getQueue().begin(); it != play_queue->getQueue().end(); it++)
	{
		Queue::Queueable	*elem = (*it);
		RemoteEvent			&evt = client->newEvent("newQueueElem");

		evt.setData(elem->getNetData());
		evt.send();
	}
}

void		RemoteControl::sendPlayState(RemoteSock* client)
{
	QString		state = player->isPlaying() ? "play" : "pause";
	RemoteEvent	&evt = client->newEvent(state);
	evt.send();
}
