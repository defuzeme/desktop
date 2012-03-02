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

#include <deque>

namespace Queue {
	class Queueable;

	typedef std::deque<Queueable*>	QueueableDeque;
}

#ifndef QUEUEABLE_HPP
#define QUEUEABLE_HPP

#include <QObject>
#include <QLabel>
#include <QVariantMap>
#include <QDateTime>
#include "container.hpp"
#include "controlswidget.hpp"
#include "eventmodel.hpp"

namespace Queue
{
	class QueueBreak;
	class QueueTrack;

	/** This is the queueable element base class.
	  * It allows any inherited class to be queued in the play queue
	  *
	  * Any queueable element can be linked to a container (album, playlist, search, ...)
	  * allowing to operate easily on his siblings.
	  **/

	class Queueable : public QObject
	{
		friend class PlayQueue;
		Q_OBJECT
	public:
		Queueable(QObject *parent = 0);
		virtual ~Queueable();
		void				setContainer(Container* c);
		bool				hasContainer() const;
		Container*			getContainer() const;
		int					getPosition() const;
		const QVariantMap&	getQueueAttributes() const;
		virtual QVariantMap	getContent(bool forWeb = false) const;
		QVariantMap			getNetData() const;					///< Return the data to send on network
		void				saveQueueAttributes() const;		///< Update attributes of the queueable element in the database
		virtual bool		queueIsFinite() const;				///< This method should return false if the duration of the element is infinite (like scheduled pause or
		virtual int			queueDuration(QDateTime from = QDateTime()) const;///< This method should return the duration of the queued element (in seconds)
		virtual QString		queueType() const = 0;				///< This method should return the child class name
		virtual	int			queueId() const;					///< This method should return the id of the element (if any)
		virtual QString		name() const;						///< This virtual method should return a human-readable string describing the element
		virtual void		setPlayTime(const QDateTime& time);	///< Method called by the play queue to update the play time
		virtual Queueable*	clone() const = 0;					///< Duplicate queueable content
		const QDateTime&	getPlayTime() const;
		QString				textDuration() const;
		void				showControls();
		void				hideControls();
		void				setEvent(int id);
		int					getEvent() const;

		// Shortcut downcasting methods
		bool				isTrack() const;
		bool				isBreak() const;
		QueueTrack*			toQueueTrack();
		QueueBreak*			toQueueBreak();

	signals:
		void				remove(Queueable* elem);

	public slots:
		void				remove();

	protected:
		Container*			container;
		QVariantMap			attributes;
		int					position;
		QDateTime			play_at;

	private:
		static void			remove(int DBposition);				///< Remove from database
		void				queueAt(int position);				///< Save in the database with given position
		void				correctPosition(int position);		///< Correct database position
		bool				controls_active;
		ControlsWidget*		controls;
	};
}

#endif // QUEUEABLE_HPP
