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

#include "playqueue.hpp"
#include "audiotrack.hpp"
#include "queuebreak.hpp"
#include "queuetrack.hpp"
#include "playqueuewidget.hpp"
#include "jsonparser.hpp"
#include "exception.hpp"
#include "scheduler.hpp"
#include "listsplugin.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <algorithm>

using namespace Queue;

PlayQueue::PlayQueue(QObject *parent) :
	QAbstractListModel(parent), altering(false)
{
	widget = new PlayQueueWidget(this);
	connect(widget, SIGNAL(deleteElements()), SLOT(removeSelected()));
	connect(widget, SIGNAL(clearListFocus()), SLOT(closeControls()));
	uiModule = Gui::ModuleFactory::create("PlayQueue", QPoint(1, 1), widget);
	uiModule->setSizePolicy(QSizePolicy::MinimumExpanding);
	uiModule->submitForDisplay();
	populateFromDB();
	updateTimes();
	selection = widget->selectionModel();
//	connect(widget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(updateSelection(QItemSelection,QItemSelection)));
//	connect(widget->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(updateFocus(QModelIndex, QModelIndex)));
	QTimer	*updateTimer = new QTimer(this);
	updateTimer->start(10000);
	connect(updateTimer, SIGNAL(timeout()), SLOT(updateTimes()));
	connect(updateTimer, SIGNAL(timeout()), widget, SLOT(update()));
}

PlayQueue::~PlayQueue()
{
	delete widget;
	while (!queue.empty())
	{
		Queueable	*elem = queue.back();
		delete elem;
		queue.pop_back();
	}
}

void	PlayQueue::aboutToQuit()
{
	QueueableDeque::iterator	it;
	for (it = queue.begin(); it != queue.end(); it++)
	{
		Queueable	*elem = (*it);
		elem->saveQueueAttributes();
	}
}

void PlayQueue::init()
{
	scheduler = plugins->cast<Scheduler::SchedulerPlugin>("scheduler");

	connect(scheduler, SIGNAL(newEvent(Scheduler::EventModel*)), SLOT(newEvent(Scheduler::EventModel*)));
	connect(scheduler, SIGNAL(updateEvent(Scheduler::EventModel*)), SLOT(updateEvent(Scheduler::EventModel*)));
	connect(scheduler, SIGNAL(removeEvent(Scheduler::EventModel*)), SLOT(removeEvent(Scheduler::EventModel*)));
	connect(scheduler, SIGNAL(loadEvent(Scheduler::EventModel*)), SLOT(newEvent(Scheduler::EventModel*)));
	emit initialized();
}

Scheduler::SchedulerPlugin*	PlayQueue::getSchedulerPlugin() const
{
	return scheduler;
}

const QueueableDeque&	PlayQueue::getQueue() const
{
	return queue;
}

void		PlayQueue::populateFromDB()
{
	QSqlQuery query("SELECT position, queueable_type, queueable_id, attributes FROM queue ORDER BY position ASC");
	if (!query.exec())
		throw_exception(0x01, tr("Can't load play queue: %1").arg(query.lastError().databaseText()));
	int position = 0;
	while (query.next()) {
		Queueable	*elem = NULL;
		int			DBposition = query.value(0).toInt();
		int			id = query.value(2).toInt();
		QString		type = query.value(1).toString();

		// TODO: Validate position errors
		if (type == "QueueTrack")
		{
			Library::AudioTrack	*track = Library::AudioTrack::getTrack(id);
			if (track)
			{
				elem = new QueueTrack(*track);
				connect(track, SIGNAL(removed(Library::AudioTrack*)), SLOT(removeAll(Library::AudioTrack*)));
			}
		}
		else if (type == "QueueBreak")
			elem = new QueueBreak();
		else
			qDebug() << "Unknow queue elem type: " << type;

		if (elem)
		{
			elem->attributes = Network::JsonParser().parse(query.value(3).toByteArray()).toMap();
			elem->position = position;
			if (DBposition != position)
				elem->correctPosition(DBposition);
			connect(elem, SIGNAL(remove(Queueable*)), SLOT(removeOne(Queueable*)));
			beginInsertRows(QModelIndex(), queue.size(), queue.size());
			queue.push_back(elem);
			endInsertRows();
			position++;
		}
		else
		{
			Queueable::remove(DBposition);
		}
	}
//	QueueBreak	*b = new QueueBreak;
//	b->setEnd(QDateTime::currentDateTime().addSecs(6600));
//	add(b, 5);
//	for(int i = 6; i < 11; i++)
//		queue[i]->setEvent(4);
}

Queueable*	PlayQueue::head() const
{
	if (queue.size() > 0)
		return queue[0];
	else
		return NULL;
}

Queueable*	PlayQueue::next() const
{
	if (queue.size() > 1)
		return queue[1];
	else
		return NULL;
}

Queueable*	PlayQueue::nextNext() const
{
	if (queue.size() > 2)
		return queue[2];
	else
		return NULL;
}

void		PlayQueue::pop()
{
	QSqlQuery	query;
	query.exec("BEGIN");
	remove(0);
	query.exec("END");
	emit popQueue();
	emitAltered();
}

void		PlayQueue::shiftDBPositions(int pos, int shift)
{
	QSqlQuery	query;
	bool		failed = false;
	//query.exec("BEGIN");
	//QString		sql = "";
	query.prepare("UPDATE queue SET position = position + :shift WHERE position = :position");
	if (shift > 0)
	{
		for (int p = queue.size() - 1; p >= pos; p--)
//			sql += QString("UPDATE queue SET position = position + %1 WHERE position = %2; ").arg(shift).arg(p);
		{
			query.bindValue(":position", p);
			query.bindValue(":shift", shift);
			failed |= !query.exec();
		}
	}
	else if (shift < 0)
	{
		for (unsigned p = pos; p < queue.size(); p++)
//			sql += QString("UPDATE queue SET position = position + %1 WHERE position = %2; ").arg(shift).arg(p);
		{
			query.bindValue(":position", p);
			query.bindValue(":shift", shift);
			failed |= !query.exec();
		}
	}
	//sql += "";
	//query.exec("COMMIT");
	if (failed)
		throw_exception(0x02, tr("Can't shift queue position: %1").arg(query.lastError().databaseText()));
}

int			PlayQueue::rowCount(const QModelIndex &) const
{
	return queue.size();
}

void		PlayQueue::updateFocus(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(current);
	Q_UNUSED(previous);
	/*
	if (current.isValid())
		queue[current.row()]->showControls();
	if (previous.isValid())
		queue[previous.row()]->hideControls();*/
}

void		PlayQueue::closeControls()
{
	/*
	if (widget->selectionModel()->currentIndex().isValid())
		queue[widget->selectionModel()->currentIndex().row()]->hideControls();*/
}

QVariant	PlayQueue::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole)
		return qVariantFromValue((void*)queue[index.row()]);
	else
		return QVariant();
}

Qt::ItemFlags	PlayQueue::flags(const QModelIndex &) const
{
	return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
}

void		PlayQueue::updateTimes(int shift)
{
	QDateTime					t = QDateTime::currentDateTime();
	QueueableDeque::iterator	it;
	bool						finite = true;

	t = t.addSecs(-shift);
	for (it = queue.begin(); it != queue.end(); it++)
	{
		Queueable	*elem = (*it);
		if (finite)
		{
			elem->setPlayTime(t);
			if (elem->queueIsFinite())
			{
				t = t.addSecs(elem->queueDuration(t));
			}
			else
				finite = false;
		}
		else
			elem->setPlayTime(QDateTime());
	}
}

void		PlayQueue::updatePositions()
{
	for (unsigned pos = 0; pos < queue.size(); pos++)
		queue[pos]->position = pos;
}

QWidget*	PlayQueue::getWidget()
{
	return widget;
}

void		PlayQueue::add(Queueable *elem, int position)
{
	if (position == -1)
	{
		beginInsertRows(QModelIndex(), queue.size(), queue.size());
		elem->queueAt(queue.size());
		queue.push_back(elem);
	}
	else
	{
		beginInsertRows(QModelIndex(), position, position);
		shiftDBPositions(position, +1);
		elem->queueAt(position);
		queue.insert(queue.begin() + position, elem);
		updatePositions();
	}
	if (elem->isTrack())
	{
		QueueTrack *track = static_cast<QueueTrack*>(elem);
		connect(track->getTrack(), SIGNAL(removed(Library::AudioTrack*)), SLOT(removeAll(Library::AudioTrack*)));
	}
	connect(elem, SIGNAL(remove(Queueable*)), SLOT(removeOne(Queueable*)));
	endInsertRows();
	updateTimes();
	emit addQueueElem(elem);
}

void		PlayQueue::add(Container *elem, int position)
{
	Q_ASSERT_X(false, "PlayQueue::add(Container *elem, int position)", "not implemented yet");
	if (position == -1)
		queue.insert(queue.end(), elem->getChildren().begin(), elem->getChildren().end());
	else
		queue.insert(queue.begin() + position, elem->getChildren().begin(), elem->getChildren().end());
}

void		PlayQueue::remove(int position)
{
	Queueable	*elem;
	QSqlQuery	query;

	if (position >= (int)queue.size() || position < 0)
		return;
	query.prepare("DELETE FROM queue WHERE position = :position");
	query.bindValue(":position", position);
	if (!query.exec())
		throw_exception(0x04, tr("Can't remove queueable: %1").arg(query.lastError().databaseText()));
	beginRemoveRows(QModelIndex(), position, position);
	shiftDBPositions(position, -1);
	elem = queue.at(position);
	queue.erase(queue.begin() + position);
	updatePositions();
	emit removeQueueElem(elem);
	delete elem;
	updateTimes();
	endRemoveRows();
}

void		PlayQueue::remove(QList<int> positions)
{
	QSqlQuery	query;
	query.exec("BEGIN");
	qSort(positions);
	for (int i = positions.size() - 1; i >= 0; --i)
		remove(positions[i]);
	query.exec("COMMIT");
}

void		PlayQueue::removeAll(Library::AudioTrack* track)
{
	QList<int>	positions;
	QueueableDeque::iterator	it;
	for (it = queue.begin(); it != queue.end(); it++)
	{
		Queueable	*elem = *it;
		if (elem->isTrack() && elem->toQueueTrack()->getTrack() == track)
			positions << elem->toQueueTrack()->getPosition();
	}
	remove(positions);
}

void		PlayQueue::removeSelected()
{
	QList<int>	positions;
	foreach(const QModelIndex &index, selection->selectedIndexes())
		positions << index.row();
	startAlteringALot();
	remove(positions);
	finishAlteringALot();
}

void		PlayQueue::removeOne(Queueable *elem)
{
	if (queue[elem->getPosition()] == elem)
	{
		remove(elem->getPosition());
		emitAltered();
	}
	else
		qDebug() << "bad remove position";
}

void		PlayQueue::bulkMove(QList<int> positions, int destination)
{
	qSort(positions);
	QSqlQuery	query;

	startAlteringALot();
	query.exec("BEGIN");
	// Get element's references
	QList<Queueable*>	queueables;
	foreach(int pos, positions)
		if (queue[pos])
		{
			qDebug() << "move " << queue[pos];
			queueables << queue[pos];
		}

	// Clone and insert
	int shift = 0;
	foreach(Queueable* queueable, queueables)
	{
		add(queueable->clone(), destination + shift);
		if (destination >= 0)
			shift++;
	}

	// Remove
	foreach(Queueable* queueable, queueables)
		removeOne(queueable);
	query.exec("COMMIT");
	finishAlteringALot();
}

void		PlayQueue::emitAltered()
{
	emit altered();
}

QStringList		PlayQueue::mimeTypes() const
{
	QStringList types;
	types << "application/x-defuzeme-audiotrack";
	return types;
}

QMimeData	*PlayQueue::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray queueables, audioTracks;

	// Generate list of queueable positions (for local moves)
	QDataStream sQueueables(&queueables, QIODevice::WriteOnly);
	// Generate list of audiotrack id (for other widgets like playlist)
	QDataStream sAudioTracks(&audioTracks, QIODevice::WriteOnly);
	foreach (const QModelIndex &index, indexes)
	{
		if (index.isValid() && queue[index.row()])
		{
			sQueueables << index.row();
			if (queue[index.row()]->isTrack())
				sAudioTracks << queue[index.row()]->toQueueTrack()->getTrack()->getUid();
		}
	}
	mimeData->setData("application/x-defuzeme-queueable", queueables);
	mimeData->setData("application/x-defuzeme-audiotrack", audioTracks);
	return mimeData;
}


bool PlayQueue::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(action);
	Q_UNUSED(column);
	Q_UNUSED(parent);

	QByteArray	encodedData;
	bool		isQueueable = data->hasFormat("application/x-defuzeme-queueable");

	if (isQueueable)
		encodedData = data->data("application/x-defuzeme-queueable");
	else if (data->hasFormat("application/x-defuzeme-audiotrack"))
		encodedData = data->data("application/x-defuzeme-audiotrack");
	else
		return false;

	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QList<int> ids;

	while (!stream.atEnd()) {
		int id;
		stream >> id;
		ids << id;
	}

	if (isQueueable)
		bulkMove(ids, row);
	else
	{
		// When multiple tracks are dropped, increment position in order not to inverse them
		QSqlQuery	query;
		query.exec("BEGIN");
		int shift = 0;
		foreach(int id, ids)
		{
			Library::AudioTrack	*track = Library::AudioTrack::getTrack(id);
			if (track)
			{
				add(new Queue::QueueTrack(*track), row + shift);
				if (row >= 0)
					shift++;
			}
		}
		query.exec("COMMIT");
		emitAltered();
	}

	return true;
}

bool	PlayQueue::isBeingAlteredALot() const
{
	return altering;
}

void	PlayQueue::startAlteringALot()
{
	if (isBeingAlteredALot())
		qDebug() << "Warning: recursive bulk altering on queue list!";
	else
		altering = true;
}

void	PlayQueue::finishAlteringALot()
{
	if (isBeingAlteredALot())
	{
		altering = false;
		emitAltered();
	}
	else
		qDebug() << "Warning: finishing bulk altering not started on queue list!";
}

QList<int>	PlayQueue::findByEvent(Scheduler::EventModel *event)
{
	QList<int>	elems;
	for(unsigned i = 0; i < queue.size(); i++)
		if (queue[i]->getEvent() == event->getId())
			elems << i;
	return elems;
}

void	PlayQueue::newEvent(Scheduler::EventModel* event)
{
	QSqlQuery	query;
	qDebug() << "newEvent" << event->getTitle();

	// Check existing items
	if (!findByEvent(event).empty())
		return;

	// Find starting position
	QDateTime	start = event->nextInstance();
	unsigned	position = 0;
	while (position < queue.size() && queue[position]->getPlayTime() < start)
		position++;
	if (position > 0 && queue[position - 1]->isBreak())
		position--;
	// position is now just after event start
	// optionnally test if it's better to take the previous

	// Fetch tracks from playlists
	QList<int>	playlists = event->getPlaylists();
	QList<int>	tracks;
	foreach (int playlist, playlists)
		tracks.append(Lists::ListsPlugin::getTracksForPlaylist(playlist));

	if (tracks.empty())
		return;

	startAlteringALot();
	query.exec("BEGIN");
	// Insert break if needed
	if (position >= queue.size() || queue[position]->getPlayTime() < start)
	{
		// best position is end of queue, insert break to fill
		add(new QueueBreak(start), position);
		position++;
	}

	// Random order
	std::random_shuffle(tracks.begin(), tracks.end());

	// Insert into queue
	int		duration = 0;
	int		t = 0;
	while (duration < event->getDuration() * 60)
	{
		Queueable	*track = new QueueTrack(*Library::AudioTrack::getTrack(tracks[t]));
		duration += track->queueDuration();
		track->setEvent(event->getId());
		add(track, position);
		position++;
		// Cycle through tracks
		t++;
		if (t >= tracks.size())
			t = 0;
	}
	query.exec("COMMIT");
	finishAlteringALot();
}

void	PlayQueue::updateEvent(Scheduler::EventModel* event)
{
	qDebug() << "updateEvent" << event->getTitle();

	removeEvent(event);
	newEvent(event);
}

void	PlayQueue::removeEvent(Scheduler::EventModel* event)
{
	qDebug() << "removeEvent" << event->getTitle();

	// Find event elements
	QList<int> elems = findByEvent(event);
	if (elems.empty())
		return;

	// Add optional break
	if (elems.first() > 0 && queue[elems.first() - 1]->isBreak())
		elems << elems.first() - 1;

	startAlteringALot();
	remove(elems);
	finishAlteringALot();
}
