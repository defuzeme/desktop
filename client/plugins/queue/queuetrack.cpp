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

#include "queuetrack.hpp"

using namespace Queue;

QueueTrack::QueueTrack(Library::AudioTrack &track) : track(track)
{
}

QueueTrack::~QueueTrack()
{}

Library::AudioTrack*	QueueTrack::getTrack() const
{
	return &track;
}

Library::AudioTrack*	QueueTrack::getTrack()
{
	return &track;
}

int			QueueTrack::queueDuration(QDateTime) const
{
	return track.getDuration();
}

QString		QueueTrack::queueType() const
{
	return "QueueTrack";
}

int			QueueTrack::queueId() const
{
	return track.getUid();
}

QVariantMap	QueueTrack::getContent(bool	forWeb) const
{
	QVariantMap	content;

	if (forWeb)
		content["uid"] = track.getUid();
	else
		content["id"] = track.getUid();
	content["title"] = track.getTitle();
	content["artist"] = track.getArtist();
	content["album_artist"] = track.getAlbumArtist();
	content["album"] = track.getAlbum();
	content["year"] = track.getYear();
	content["track"] = track.getTrack();
	content["genre"] = track.getGenre();
	content["duration"] = track.getDuration();
	return content;
}

Queueable*	QueueTrack::clone() const
{
	QueueTrack	*instance = new QueueTrack(track);
	instance->attributes = attributes;
	return instance;
}
