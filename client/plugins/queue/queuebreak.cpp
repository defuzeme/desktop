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

#include "queuebreak.hpp"

using namespace Queue;

QueueBreak::QueueBreak()
{
}

QueueBreak::QueueBreak(int duration)
{
	setDuration(duration);
}

QueueBreak::QueueBreak(QDateTime end)
{
	setEnd(end);
}

QueueBreak::~QueueBreak()
{
}

int			QueueBreak::duration() const
{
	if (attributes.contains("duration"))
		return attributes["duration"].toInt();
	else
		return -1;
}

QDateTime	QueueBreak::end() const
{
	if (attributes.contains("end"))
		return attributes["end"].toDateTime();
	else
		return QDateTime();
}

void		QueueBreak::setDuration(int duration)
{
	attributes["duration"] = duration;
	saveQueueAttributes();
}

void		QueueBreak::setEnd(QDateTime end)
{
	attributes["end"] = end;
	saveQueueAttributes();
}

QString		QueueBreak::message() const
{
	if (end().isValid())
		return QString(tr("Break until %1").arg(end().toString(tr("dddd hh:mm ap"))));
	else
		return QString(tr("Break during %1").arg(textDuration()));
}

bool		QueueBreak::queueIsFinite() const
{
	return (duration() > 0 || end().isValid());
}

int			QueueBreak::queueDuration(QDateTime from) const
{
	if (end().isValid())
		if (from.secsTo(end()) > 0)
			return from.secsTo(end());
		else
			return 0;
	else
		return duration();
}

QString		QueueBreak::queueType() const
{
	return "QueueBreak";
}

QString		QueueBreak::name() const
{
	return QString("QueueBreak(%1s)").arg(attributes["duration"].toString());
}

Queueable*	QueueBreak::clone() const
{
	QueueBreak	*instance = new QueueBreak();
	instance->attributes = attributes;
	return instance;
}
