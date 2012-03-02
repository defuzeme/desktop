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

#include "servicesync.hpp"
#include "queuetrack.hpp"
#include "logger.hpp"
#include "status.hpp"
#include <QTimer>

using namespace WebService;

ServiceSync::ServiceSync() : reply(NULL), ws(0), ws_tries(0), ws_delay(1)
{
}

ServiceSync::~ServiceSync()
{
}

void	ServiceSync::init()
{
	queue = plugins->cast<Queue::PlayQueue>("queue");
	connect(queue, SIGNAL(initialized()), SLOT(sendPlayQueue()));
	connect(queue, SIGNAL(addQueueElem(Queue::Queueable*)), SLOT(sendAddQueueElem(Queue::Queueable*)));
	connect(queue, SIGNAL(removeQueueElem(Queue::Queueable*)), SLOT(sendRemoveQueueElem(Queue::Queueable*)));
	connect(queue, SIGNAL(altered()), SLOT(sendPlayQueue()));
	connect(cores->net(), SIGNAL(APIAuthenticated()), SLOT(sendNextRequest()));
	connect(cores->net(), SIGNAL(APIAuthenticated()), SLOT(wsConnect()));
	wsConnect();
}

void	ServiceSync::wsConnect()
{
	int	radio_id = cores->net()->getRadioInfo()["id"].toInt();
	if (radio_id && !ws)
	{
		ws = new Network::WebSocket(QString("ws://%1/push/%2/client").arg(gl_PUSH_HOSTNAME).arg(radio_id));
		ws->onMessage(this, SLOT(onMessage(QByteArray)));
		ws->onOpen(this, SLOT(onOpen()));
		ws->onClose(this, SLOT(onClose(QString)));
		ws->connect();
	}
}

void	ServiceSync::defineParams()
{
}

bool	ServiceSync::hasRequests() const
{
	return (!requests.empty());
}

bool	ServiceSync::isRequesting() const
{
	return (reply != NULL);
}

void	ServiceSync::sendNextRequest()
{
	if (hasRequests() && !isRequesting())
	{
		ServiceRequest	*sRequest = requests.front();
		QNetworkRequest	netRequest = cores->net()->apiRequest(sRequest->url);

		// Notify
		Notification::Status::gMessage(tr("Synchronisation with website"), Notification::SYNC);

		// Store serialized data to a buffer
		buffer.setData(cores->net()->apiParser().serialize(sRequest->data));

		// send request to web service
		reply = cores->net()->web().sendCustomRequest(netRequest, sRequest->verb, &buffer);
		// handle events
		connect(reply, SIGNAL(finished()), SLOT(receiveResponse()));
	}
	else
	{
		//qDebug() << "can't send next request yet (hasRequests = " << hasRequests() << ", isRequesting = " << isRequesting() << ")";
	}
}

void	ServiceSync::sendPlayQueue()
{
	ServiceRequest	*req = newRequest("PUT", "radios/my");
	QVariantMap		radio;
	QVariantList	elems;

	// Ignore changes if queue is being altered
	if (queue->isBeingAlteredALot())
		return;
	// build PUT data hash
	foreach(Queue::Queueable* elem, queue->getQueue())
	{
		QVariantMap	hash;
		hash["position"] = elem->getPosition() + 1;
		hash["kind"] = elem->queueType();
		hash["play_at"] = elem->getPlayTime();
		if (!elem->getQueueAttributes().empty())
			hash["properties"] = elem->getQueueAttributes();
		if (elem->isTrack())
			hash["track_attributes"] = elem->toQueueTrack()->getContent(true);
		elems << hash;
	}
	radio["queue_elems_attributes"] = elems;
	(*req)["radio"] = radio;

	req->enqueue();
}

void	ServiceSync::sendRemoveQueueElem(Queue::Queueable* elem)
{
	int				position = elem->getPosition() + 1;
	ServiceRequest	*req;

	// Ignore changes if queue is being altered
	if (queue->isBeingAlteredALot())
		return;
	if (!sync_mutex.tryLock())
		return;
	sync_mutex.unlock();
	req = newRequest("DELETE", QString("radios/my/queue/%1").arg(position));
	req->enqueue();
}

void	ServiceSync::sendAddQueueElem(Queue::Queueable* elem)
{
	ServiceRequest	*req;
	QVariantMap		queue_elem;

	// Ignore changes if queue is being altered
	if (queue->isBeingAlteredALot())
		return;
	if (!sync_mutex.tryLock())
		return;
	sync_mutex.unlock();
	// build POST data hash
	req = newRequest("POST", "radios/my/queue");
	queue_elem["position"] = elem->getPosition() + 1;
	queue_elem["kind"] = elem->queueType();
	queue_elem["play_at"] = elem->getPlayTime();
	if (!elem->getQueueAttributes().empty())
		queue_elem["properties"] = elem->getQueueAttributes();
	if (elem->isTrack())
		queue_elem["track_attributes"] = elem->toQueueTrack()->getContent(true);

	(*req)["queue_elem"] = queue_elem;
	req->enqueue();
}

void	ServiceSync::receiveResponse()
{
	ServiceRequest*	sRequest = requests.front();

	if (reply->error())
	{
		if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 401)
		{
			//cores->net()->invalidateWebToken();
			reply->deleteLater();
			reply = NULL;
			Notification::Message *msg = plugins->cast<Notification::Status>("status")->message(tr("Bad or expired credentials"), Notification::WARN);
			msg->setAction(tr("Sign in"), cores->net(), SLOT(invalidateWebToken()));
			return;
		}
		QVariant data = cores->net()->apiParser().parse(reply->readAll());
		QString	error = data.toMap()["error"].toString();
		if (error == "")
			error = reply->errorString();
		Logger::log(QString("Web service: error: %1").arg(error));
		Notification::Status::gMessage(tr("Synchronisation failed: %1").arg(error), Notification::ERR);
	}
	else
	{
		QVariant data = cores->net()->apiParser().parse(reply->readAll());
	}
	reply->deleteLater();
	reply = NULL;
	requests.pop_front();
	delete sRequest;
	sendNextRequest();
}

ServiceRequest*	ServiceSync::newRequest(const QString& verb, const QString& url)
{
	ServiceRequest	*request;

	// create request
	request = new ServiceRequest(this, url);
	request->verb = verb.toUtf8();

	return request;
}

void			ServiceSync::onMessage(const QByteArray& data)
{
	QVariantMap		hash = Network::JsonParser().parse(data).toMap();
	QMutexLocker	locker(&sync_mutex);

	if (hash.contains("move"))
		wsMove(hash["move"].toMap());
}

void			ServiceSync::onOpen()
{
	qDebug() << "WebSocket connection OK!";
	ws_tries = 0;
	ws_delay = 1;
}

void			ServiceSync::onClose(const QString& error)
{
	qDebug() << "WebSocket connection error: " << error;
	if (ws_tries < 5)
	{
		ws_tries++;
		qDebug() << "Retry" << ws_tries << "in" << ws_delay << "s";
		QTimer::singleShot(ws_delay * 1000, ws, SLOT(connect()));
		ws_delay *= 2;
	}
}

void			ServiceSync::wsMove(const QVariantMap& data)
{
	int			oldPosition = data["position"].toInt() - 1;
	int			newPosition = data["newPosition"].toInt() - 1;
	Queue::Queueable*	elem = queue->getQueue()[oldPosition];
	Library::AudioTrack	*track = elem->toQueueTrack()->getTrack();


	queue->remove(oldPosition);
	queue->add(new Queue::QueueTrack(*track), newPosition);
	queue->emitAltered();
}
