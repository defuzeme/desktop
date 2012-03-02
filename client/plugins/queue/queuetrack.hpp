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

namespace Queue {
	class QueueTrack;
}

#ifndef QUEUETRACK_HPP
#define QUEUETRACK_HPP

#include <QVariantMap>
#include "queueable.hpp"
#include "library/audiotrack.hpp"

namespace Queue {

	/** This class inherits Queueable an represent a track in the play queue.
	  *
	  * It is used to associate an AudioTrack with a position and a widget in the PlayQueue
	  **/

	class QueueTrack : public Queueable
	{
		Q_OBJECT
	public:
		QueueTrack(Library::AudioTrack& track);
		~QueueTrack();
		Library::AudioTrack*	getTrack() const;
		Library::AudioTrack*	getTrack();
		int						queueDuration(QDateTime from) const;		///< virtual method returning element's duration tu calculate queue length
		QVariantMap				getContent(bool	forWeb = false) const;		///< virtual method returning track's information for mobile devices
		QString					queueType() const;
		int						queueId() const;
		Queueable*				clone() const;

	signals:

	private:
		Library::AudioTrack		&track;
	};
}
#endif // QUEUETRACK_HPP
