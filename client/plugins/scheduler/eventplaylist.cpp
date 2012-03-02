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

#include "eventplaylist.hpp"

using namespace Scheduler;

EventPlaylist::EventPlaylist(EventWidget *parent) : QListWidget((QWidget*)parent), parent(parent)
{
	this->initialize();

	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setStyleSheet("min-height: 150px; border-top-right-radius: 8px; border-bottom-right-radius: 8px;");

	connect(this->parent->playlistsModel, SIGNAL(playlistContentChanged(int)), SLOT(playlistContentChanged(int)));
	connect(this->parent->playlistsModel, SIGNAL(playlistNameChanged(int, QString)), SLOT(playlistNameChanged(int, QString)));
	connect(this->parent->playlistsModel, SIGNAL(playlistRemoved(int)), SLOT(playlistRemoved(int)));

	return;
}

void	EventPlaylist::initialize()
{
	this->clear();

	this->staticPlaylists	= this->parent->playlistsPlugin->getNormalPlaylists();
	this->dynamicPlaylists	= this->parent->playlistsPlugin->getDynamicPlaylists();

	this->loadStaticsPlaylists();
	this->loadDynamicsPlaylists();
}

// @TODO Need to be sync with playlists changes
void	EventPlaylist::loadStaticsPlaylists()
{
	QMapIterator<int, QString>	it(this->staticPlaylists);

	while (it.hasNext())
	{
		it.next();

		int		id		= it.key();
		QString	name	= it.value();

		this->addItem(new EventPlaylistItem(this, id, name));
	}

	return;
}

// @TODO Need to be sync with playlists changes
void	EventPlaylist::loadDynamicsPlaylists()
{
	QMapIterator<int, QString>	it(this->dynamicPlaylists);

	while (it.hasNext())
	{
		it.next();

		int			id		= it.key();
		QString		name	= it.value();

		this->addItem(new EventPlaylistItem(this, id, name));
	}

	return;
}

void	EventPlaylist::playlistContentChanged(int playlistId)
{
	this->initialize();
}

void	EventPlaylist::playlistNameChanged(int playlistId, QString newName)
{
	for (int index = 0; index < this->count(); index += 1)
		if (((EventPlaylistItem*)this->item(index))->id == playlistId)
		{
			((EventPlaylistItem*)this->item(index))->name = newName;
			((EventPlaylistItem*)this->item(index))->setText(newName);
			return;
		}
}

void	EventPlaylist::playlistRemoved(int playlistId)
{
	for (int index = 0; index < this->count(); index += 1)
		if (((EventPlaylistItem*)this->item(index))->id == playlistId)
		{
			EventPlaylistItem *item = (EventPlaylistItem*)this->item(index);
			delete item;
			continue;
		}

	QSqlQuery query("DELETE FROM events_playlists WHERE playlist_id = :id");

	query.bindValue(":id", playlistId);

	if (!query.exec())
		qDebug() << QString("Can't delete associated playlists : %1").arg(query.lastError().text());
}

QList<int>	EventPlaylist::getSelectedPlaylists()
{
	QList<int>	items = QList<int>();

	for (int index = 0; index < this->selectedItems().count(); index += 1)
	{
		if (this->selectedItems().at(index)->isSelected())
		{
			items.append(((EventPlaylistItem*)this->selectedItems().at(index))->id);
		}
	}

	return	items;
}

QModelIndex	EventPlaylist::indexFromItem(QListWidgetItem *item) const
{
	return	QListWidget::indexFromItem(item);
}
