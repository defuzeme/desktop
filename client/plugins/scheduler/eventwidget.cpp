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

#include "eventwidget.hpp"

using namespace Scheduler;

EventWidget::EventWidget(SchedulerPlugin *scheduler, QWidget *parent) :
	ModuleWidget(parent), scheduler(scheduler)
{
	// @TODO Need to be fixed, need to restart application when new playlist(s) are added
	this->playlistsPlugin	= scheduler->getPlugins()->cast<Lists::ListsPlugin>("lists");
	this->playlistsModel	= this->playlistsPlugin->getModel();

	setupUi(this);

	this->event = false;

	this->initialize();
}

void	EventWidget::setUpdate(EventModel *updateEvent)
{
	this->clear();

	this->event = updateEvent;

	this->title->setText(updateEvent->getTitle());
	this->description->setText(updateEvent->getDescription());
	this->day->setCurrentIndex(updateEvent->getDay());
	this->start->setTime(QTime().addSecs(updateEvent->getStartTime() * 60));
	this->duration->setTime(QTime().addSecs(updateEvent->getDuration() * 60));

	int colorID = (updateEvent->getColorID() != 0) ? updateEvent->getColorID() : 1 ;
	this->colors->at(colorID - 1)->setChecked(true);

	QList<int> items = updateEvent->getPlaylists();

	for (int index = 0; index < this->playlistsWidget->count(); index += 1)
	{
		int widgetItemID	= ((EventPlaylistItem*)this->playlistsWidget->item(index))->id;

		if (items.contains(widgetItemID))
			((EventPlaylistItem*)this->playlistsWidget->item(index))->setSelected(true);
	}

	this->submitEventButton->setText(tr("Update"));
	this->deleteEventButton->setEnabled(true);
}

void	EventWidget::initialize()
{
	this->colors	= new QList<EventColor*>();

	QSqlQuery	query("SELECT id, color FROM colors");

	if (! query.exec())
		qDebug() << QString("Can't get colors : %1").arg(query.lastError().text());
	else
	{
		while (query.next())
		{
			EventColor *color = new EventColor(this, query.value(0).toInt(), query.value(1).toString());
			connect(color, SIGNAL(clicked()), color, SLOT(on_colorPicker_clicked()));
			this->colorLayout->addWidget(color, 0, this->colorLayout->columnCount(), 1, 1);
			this->colors->append(color);
		}
	}

	QStringList	days;
	days << tr("Monday") << tr("Tuesday") << tr("Wednesday") << tr("Thursday") << tr("Friday") << tr("Saturday") << tr("Sunday");

	this->day->addItems(days);

	this->playlistsWidget	= new EventPlaylist(this);
	this->playlistsLayout->addWidget(this->playlistsWidget, 0, 0);

}

void	EventWidget::clear()
{
	this->title->clear();
	this->description->clear();
	this->day->setCurrentIndex(0);
	this->start->setTime(QTime(0, 0, 0, 0));
	this->duration->setTime(QTime(0, 0, 0, 0));

	for (int index = 0; index < this->colors->count(); index += 1)
		this->colors->at(index)->setChecked(false);

	for (int index = 0; index < this->playlistsWidget->count(); index += 1)
		((EventPlaylistItem*)this->playlistsWidget->item(index))->setSelected(false);

	this->submitEventButton->setText(tr("Add event"));
	this->deleteEventButton->setEnabled(false);

	if (this->event)
	{
		this->event->focusOut();
		this->event = false;
	}
}

bool	EventWidget::check()
{
	if (this->title->text().length() <= 0)
		return false;
	for (int index = 0; index < this->colors->count(); index += 1)
	{
		if (this->colors->at(index)->isChecked())
			return true;
	}
	return false;
}

void	EventWidget::on_submitEventButton_clicked()
{
	if ( ! this->check())
		return;

	bool update			= false;
	int startTime		= this->start->time().hour() * 60 + this->start->time().minute();
	int durationTime	= this->duration->time().hour() * 60 + this->duration->time().minute();

	if (this->event)
	{
		event->title = title->text();
		event->description = description->text();
		event->day = day->currentIndex();
		event->start = startTime;
		event->duration = durationTime;
		update = true;
	}
	else
	{
		this->event	= new EventModel(scheduler, title->text(), description->text(),
									 day->currentIndex(), startTime, durationTime);
		this->scheduler->getModel()->getEvents()->append(event);
	}

	event->save();
	int colorId	= this->colors->at(0)->getId();

	for (int index = 0; index < this->colors->count(); index += 1)
	{
		if (this->colors->at(index)->isChecked())
		{
			colorId		= this->colors->at(index)->getId();
			continue;
		}
	}

	event->saveColor(colorId);
	event->savePlaylists(this->playlistsWidget->getSelectedPlaylists());
	this->scheduler->getWidget()->displayEvent(event);

	if (update)
		emit updateEvent(event);
	else
		emit newEvent(event);


	this->clear();
}

void	EventWidget::on_deleteEventButton_clicked()
{
	emit removeEvent(this->event);

	this->event->remove();
	delete this->event;
	this->event = false;
	this->clear();
}
