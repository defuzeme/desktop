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
	class PlayQueue;
}

#ifndef PLAYQUEUE_HPP
#define PLAYQUEUE_HPP

#include <QWidget>
#include <QAbstractListModel>
#include <QItemSelectionModel>
#include <deque>
#include "queueable.hpp"
#include "container.hpp"
#include "playqueuewidget.hpp"
#include "staticplugin.hpp"
#include "audiotrack.hpp"
#include "eventmodel.hpp"

namespace Queue
{
	/** This is play queue, it stores all the pending tracks ordered and organized.
	  * It's implemented using a double ended queue (deque) providing O(1) insertions and removal
	  * at begin and end, along with an indexed access.
	  *
	  * Each queuable elements must inherit Queueable class.
	  *
	  * Here is how to use the list from the player:
	  *
	  * \code
	  * Queueable*	elem = play_queue.head(); // Fetch the top element
	  * if (elem->isAudioTrack()) // check if it's an audio track
	  *		path = elem->toAudioTrack->getAttribute("path")
	  * else if (elem->isQueueBreak()) // if it's a break
	  *		pause_playing;
	  *
	  * play_queue.pop() // when playing is over
	  * \endcode
	  **/

	class PlayQueue : public QAbstractListModel, public StaticPlugin
	{
		Q_OBJECT
		friend class PlayQueueDelegate;

	public:
		explicit PlayQueue(QObject *parent = 0);
		~PlayQueue();
		void					aboutToQuit();
		void					init();
		void					add(Queueable*, int position = -1);
		void					add(Container*, int position = -1);
		void					remove(int position);
		void					remove(QList<int> positions);
		void					bulkMove(QList<int> positions, int destination);
		QWidget*				getWidget();
		Queueable*				head() const;								///< Return the currently playing element
		Queueable*				next() const;								///< Return the next element
		Queueable*				nextNext() const;							///< Return the next next (third) element
		void					pop();										///< Pop the head of the queue
		const QueueableDeque&	getQueue() const;
		void					updatePositions();							///< Give all elements their right position
		QList<int>				findByEvent(Scheduler::EventModel *event);	///< Get all elements associated to given event
		Scheduler::SchedulerPlugin*	getSchedulerPlugin() const;
		// Subclassing of QAbstractListModel needs reimplementing this methods:
		int						rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant				data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		Qt::ItemFlags			flags(const QModelIndex &index) const;
		bool					isBeingAlteredALot() const;
		void					emitAltered();
		// Drag
		QStringList				mimeTypes() const;
		QMimeData*				mimeData(const QModelIndexList &indexes) const;
		// Drop
		bool					dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

	signals:
		void					popQueue();
		void					initialized();
		void					altered();
		void					removeQueueElem(Queue::Queueable*);
		void					addQueueElem(Queue::Queueable*);

	public slots:
		void					updateTimes(int shift = 0);					///< Give all elements their play time
		void					removeAll(Library::AudioTrack*);			///< Remove all matching instances from queue
		void					removeSelected();							///< Remove all selected elements
		void					updateFocus(const QModelIndex &current, const QModelIndex &previous);
		void					closeControls();
		void					removeOne(Queueable *elem);
		// Scheduler related events
		void					newEvent(Scheduler::EventModel* event);
		void					updateEvent(Scheduler::EventModel* event);
		void					removeEvent(Scheduler::EventModel* event);


	private:
		void					populateFromDB();							///< Load data from db
		void					shiftDBPositions(int pos, int shift = 1);	///< Move all items above pos by shift in DB
		void					startAlteringALot();
		void					finishAlteringALot();

		QueueableDeque			queue;
		PlayQueueWidget			*widget;
		QItemSelectionModel		*selection;
		Gui::Module				*uiModule;
		bool					altering;
		Scheduler::SchedulerPlugin *scheduler;
	};
}


#endif // PLAYQUEUE_HPP
