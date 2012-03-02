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
	class QueueBreak;
}

#ifndef QUEUEBREAK_HPP
#define QUEUEBREAK_HPP

#include "queueable.hpp"
#include <QDateTime>

namespace Queue {

	/** This class inherits Queueable an represent a break in the play queue.
	  *
	  * It is used to pause the player at a determined time
	  **/

	class QueueBreak : public Queueable
	{
		Q_OBJECT
	public:
		QueueBreak();
		QueueBreak(int duration);
		QueueBreak(QDateTime end);
		~QueueBreak();
		int			duration() const;
		QDateTime	end() const;
		void		setDuration(int duration);
		void		setEnd(QDateTime end);
		QString		message() const;
		bool		queueIsFinite() const;
		int			queueDuration(QDateTime from) const;
		QString		queueType() const;
		QString		name() const;
		Queueable*	clone() const;
	signals:

	public slots:
	private:
	};
}

#endif // QUEUEBREAK_HPP
