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

#include "queueable.hpp"
#include "jsonparser.hpp"
#include "exception.hpp"
#include "audiotrack.hpp"
#include "queuebreak.hpp"
#include "queuetrack.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QTextCodec>

using namespace Queue;

Queueable::Queueable(QObject *parent) :	QObject(parent), container(NULL),
										controls_active(false), controls(0)
{
	position = -1;
}

Queueable::~Queueable()
{
}

void		Queueable::setContainer(Container* c)
{
	container = c;
}

bool		Queueable::hasContainer() const
{
	return(container != NULL);
}

Container*	Queueable::getContainer() const
{
	return container;
}

int			Queueable::getPosition() const
{
	return position;
}

const QVariantMap&	Queueable::getQueueAttributes() const
{
	return attributes;
}

QVariantMap	Queueable::getContent(bool forWeb) const
{
	Q_UNUSED(forWeb);
	return QVariantMap();
}

bool		Queueable::queueIsFinite() const
{
	return true;
}

int		Queueable::queueDuration(QDateTime) const
{
	return 1;
}

void	Queueable::setPlayTime(const QDateTime& time)
{
	play_at = time;
}

const QDateTime&	Queueable::getPlayTime() const
{
	return play_at;
}

int		Queueable::queueId() const
{
	return 0;
}

QVariantMap		Queueable::getNetData() const
{
	QVariantMap	evt;

	evt["position"] = getPosition();
	evt["type"] = queueType();
	if (!getQueueAttributes().empty())
		evt["attributes"] = getQueueAttributes();
	if (!getContent().empty())
		evt["content"] = getContent();
	return evt;
}

void		Queueable::queueAt(int position)
{
	QSqlQuery query;
	query.prepare("INSERT INTO queue(position, queueable_type, queueable_id, attributes) "
				  "VALUES(:position, :type, :id, :attributes)");
	query.bindValue(":position", position);
	query.bindValue(":type", queueType());
	query.bindValue(":id", queueId());
	query.bindValue(":attributes", Network::JsonParser().serialize(attributes));
	if (!query.exec())
		throw_exception(0x01, tr("Can't insert queueable: %1").arg(query.lastError().databaseText()));
	this->position = position;
}

void		Queueable::saveQueueAttributes() const
{
	QSqlQuery query;

	if (position < 0)
		return;
	query.prepare("UPDATE queue SET attributes = :attributes WHERE position = :position");
	query.bindValue(":position", position);
	query.bindValue(":attributes", Network::JsonParser().serialize(attributes));
	if (!query.exec())
		throw_exception(0x02, tr("Can't update queueable: %1").arg(query.lastError().databaseText()));
}

void		Queueable::remove(int DBposition)
{
	QSqlQuery rmquery;
	rmquery.prepare("DELETE FROM queue WHERE position = :position");
	rmquery.bindValue(":position", DBposition);
	rmquery.exec();
	if (!rmquery.exec())
		throw_exception(0x03, tr("Can't remove queue element: %1").arg(rmquery.lastError().databaseText()));
}

void		Queueable::correctPosition(int DBposition)
{
	QSqlQuery query;
	query.prepare("UPDATE queue SET position = :newPosition WHERE position = :position");
	query.bindValue(":newPosition", position);
	query.bindValue(":position", DBposition);
	if (!query.exec())
		throw_exception(0x04, tr("Can't update queueable: %1").arg(query.lastError().databaseText()));
}

QString		Queueable::name() const
{
	// This generic method should be overriden to give more details
	QString	out = QString("Queueable<%1>").arg((qlonglong)this);
	if (hasContainer())
		out += " in: " + container->name();
	return out;
}

QString		Queueable::textDuration() const
{
	if (queueIsFinite())
	{
		int seconds = queueDuration() % 60;
		int minutes = (queueDuration() - seconds) / 60;
		return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
	}
	else
	{
		return "--:--";
	}
}

bool		Queueable::isTrack() const
{
	return queueType() == "QueueTrack";
}

bool		Queueable::isBreak() const
{
	return queueType() == "QueueBreak";
}

QueueTrack*		Queueable::toQueueTrack()
{
	return static_cast<QueueTrack*>(this);
}

QueueBreak*		Queueable::toQueueBreak()
{
	return static_cast<QueueBreak*>(this);
}

void			Queueable::remove()
{
	emit remove(this);
}

void			Queueable::showControls()
{
	/*
	if (controls_active)
		return;
	if (!controls)
	{
		controls = new ControlsWidget(this);
	}
	controls_active = true;
	*/
}

void			Queueable::hideControls()
{
	/*
	if (!controls_active)
		return;
	controls->dissapear();
	controls = 0;
	controls_active = false;
	*/
}

void			Queueable::setEvent(int id)
{
	attributes["event"] = id;
	saveQueueAttributes();
}

int				Queueable::getEvent() const
{
	if (attributes.contains("event"))
		return attributes["event"].toInt();
	else
		return -1;
}
