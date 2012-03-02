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

#include "listsmodel.hpp"
#include "listsplugin.hpp"
#include "editabletreeitem.hpp"
#include "audiotrack.hpp"
#include "QVariantMap"

using namespace Lists;

ListsModel::ListsModel(ListsPlugin *lists) : EditableTreeModel(), lists(lists)
{
	rootItem = new ListsItem("One", QPixmap(), this);
	setupModelData();
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataHaveChanged(QModelIndex,QModelIndex)));
	connect(lists->getWidget()->getTreeViewWidget(), SIGNAL(deleteElements()), this, SLOT(dataDeleted()));
}

ListsModel::~ListsModel()
{
}

void ListsModel::dataHaveChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	Q_UNUSED(bottomRight)

	if (!topLeft.isValid())
		return;
	updateDBFromTree(getItem(topLeft));
	emit playlistNameChanged(getItem(topLeft)->getId(), getItem(topLeft)->data(0).toString());
}

void ListsModel::dataDeleted()
{
	foreach(const QModelIndex &index, lists->getWidget()->getTreeViewWidget()->selectionModel()->selectedIndexes())
	{
		if (!index.isValid())
			return;
		ListsItem *item = getItem(index);
		if (item->isOfKind(ListsItem::PLAYLIST))
			removeList(index);
		else if (item->isOfKind(ListsItem::SEPARATOR))
			removeSeparatorFromList(index);
		else if (item->isOfKind(ListsItem::PARAMETER))
			removeParameterFromList(index);
	}
}

void ListsModel::removeList(const QModelIndex &listIndex)
{
	ListsItem *item = getItem(listIndex);
	int id = item->getId();
	if (!item->isOfKind(ListsItem::PLAYLIST))
		return;
	removeRows(listIndex.row(), 1, listIndex.parent());
	QSqlQuery query;
	query.prepare("DELETE FROM playlists WHERE id = :id");
	query.bindValue("id", item->getId());
	query.exec();
	emit playlistRemoved(id);
}

void ListsModel::removeSeparatorFromList(const QModelIndex &separatorIndex)
{
	ListsItem *item = getItem(separatorIndex);
	if (!item->isOfKind(ListsItem::SEPARATOR))
		return;
	if (item->parent()->childCount() == 1)
		removeList(separatorIndex.parent());
	else
	{
		int removedId = getItem(separatorIndex.parent())->getId();
		ListsItem *parentItem = getItem(separatorIndex.parent());
		removeRows(separatorIndex.row(), 1, separatorIndex.parent());
		updateDBFromTree(parentItem);
		emit playlistContentChanged(removedId);
	}
}

void ListsModel::removeParameterFromList(const QModelIndex &parameterIndex)
{
	ListsItem *item = getItem(parameterIndex);
	if (!item->isOfKind(ListsItem::PARAMETER))
		return;
	if (item->parent()->childCount() == 1)
		if (item->isOfKind(ListsItem::PARAMETER, ListsItem::TRACK))
			removeList(parameterIndex.parent());
		else
			removeSeparatorFromList(parameterIndex.parent());
	else
	{
		if (item->isOfKind(ListsItem::PARAMETER, ListsItem::TRACK))
		{
			int removedId = getItem(parameterIndex.parent())->getId();
			ListsItem *parentItem = getItem(parameterIndex.parent());
			removeRows(parameterIndex.row(), 1, parameterIndex.parent());
			updateDBFromTree(parentItem);
			emit playlistContentChanged(removedId);
		}
		else
		{
			int removedId = getItem(parameterIndex.parent().parent())->getId();
			ListsItem *parentItem = getItem(parameterIndex.parent().parent());
			removeRows(parameterIndex.row(), 1, parameterIndex.parent());
			updateDBFromTree(parentItem);
			emit playlistContentChanged(removedId);
		}
	}
}

QVariant ListsModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::ToolTipRole)
	{
		if (getItem(index)->isOfKind(ListsItem::PLAYLIST))
			return QVariant("Start typing to edit");
		else
			return QVariant();
	}
	else
		return EditableTreeModel::data(index, role);
}

ListsItem *ListsModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		ListsItem *item = static_cast<ListsItem*>(index.internalPointer());
		if (item) return item;
	}
	return getRootItem();
}

Qt::ItemFlags ListsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	Qt::ItemFlags itemFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	ListsItem *item = getItem(index);
	if (item->isOfKind(ListsItem::PLAYLIST))
	{
		itemFlags |= Qt::ItemIsEditable;
		itemFlags |= Qt::ItemIsDragEnabled;
	}
	return itemFlags;
}

QMimeData	*ListsModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray playlists, audioTracks;

	QDataStream sPlaylists(&playlists, QIODevice::WriteOnly);
	QDataStream sAudioTracks(&audioTracks, QIODevice::WriteOnly);
	foreach (const QModelIndex &index, indexes)
	{
		if (index.isValid() && getItem(index)->isOfKind(ListsItem::PLAYLIST))
		{
			ListsItem *item = getItem(index);
			sPlaylists << item->getId();
			foreach (int id, ListsPlugin::getTracksForPlaylist(item->getId()))
				sAudioTracks << id;
		}
	}
	mimeData->setData("application/x-defuzeme-playlist", playlists);
	mimeData->setData("application/x-defuzeme-audiotrack", audioTracks);
	return mimeData;
}

bool ListsModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	ListsItem *parentItem = static_cast<ListsItem*>(getItem(parent));
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, rootItem->columnCount());
	endInsertRows();

	return success;
}

void ListsModel::setupModelData()
{
	dynamicItem = insertCategoryItem("Dynamic Playlists", ListsItem::DYNAMIC_PLAYLIST, 0);
	normalItem = insertCategoryItem("Normal Playlists", ListsItem::NORMAL_PLAYLIST, 0);

	insertAddItem("Drop tracks here to create a playlist", ListsItem::ADD_NORMAL);
	insertAddItem("Drop artists, albums or genres here", ListsItem::ADD_DYNAMIC);

	updateTreeFromDB();
}

bool ListsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(action)
	Q_UNUSED(row)
	Q_UNUSED(column)

	ListsItem *item = getItem(lastDropIndex);
	ListsItem *parentItem = getItem(parent);

	if (item->isOfKind(ListsItem::ADD, ListsItem::ADD_DYNAMIC))
	{
		if (hasAction(item, data->formats(), CREATE_DYNAMIC_PLAYLIST))
			insertDynamicList(data, parentItem);
	}
	else if (item->isOfKind(ListsItem::PLAYLIST, ListsItem::NORMAL_PLAYLIST))
	{
		if (hasAction(item, data->formats(), ADD_TO_NORMAL_PLAYLIST))
			insertParametersInNormalList(data, item);
	}
	else if (item->isOfKind(ListsItem::PLAYLIST, ListsItem::DYNAMIC_PLAYLIST))
	{
		if (hasAction(item, data->formats(), ADD_TO_DYNAMIC_PLAYLIST))
			insertParametersInDynamicList(data, item);
	}
	else if (item->isOfKind(ListsItem::SEPARATOR))
	{
		if (hasAction(item, data->formats(), ADD_TO_DYNAMIC_PLAYLIST))
			insertParametersInDynamicList(data, parentItem);
	}
	else if (item->isOfKind(ListsItem::ADD, ListsItem::ADD_NORMAL))
	{
		if (hasAction(item, data->formats(), CREATE_NORMAL_PLAYLIST))
			insertNormalList(data, parentItem);
	}
	else
		return false;
	return true;
}

bool ListsModel::hasAction(ListsItem *item, const QStringList &formats, ListsModel::DropAction action)
{
	DropAction defaultAction = getDropActionsForItem(item);

	if (defaultAction != INVALID_DROP_ACTION)
		if (defaultAction == action)
			return true;
	foreach (QString format, formats)
		if (getDropActionsForItem(item, format) == action)
			return true;
	return false;
}

ListsItem *ListsModel::insertCategoryItem(const QString &title, ListsItem::SubKind subKind, int position)
{
	if (!getRootItem()->insertChildren(position, 1, 1))
		return 0;
	ListsItem *item = static_cast<ListsItem*>(rootItem->child(position));
	item->setKind(ListsItem::CATEGORY);
	item->setSubKind(subKind);
	item->setData(0, title);
	return item;
}

ListsItem *ListsModel::insertAddItem(const QString &title, ListsItem::SubKind subKind)
{
	ListsItem *parentItem;

	if (subKind == ListsItem::ADD_NORMAL)
		parentItem = normalItem;
	else
		parentItem = dynamicItem;
	if (!parentItem->insertChildren(0, 1, 1))
		return 0;
	ListsItem *item = static_cast<ListsItem*>(parentItem->child(0));
	item->setKind(ListsItem::ADD);
	item->setSubKind(subKind);
	if (subKind == ListsItem::ADD_NORMAL)
	{
		dropTypesForItem[item] << "application/x-defuzeme-audiotrack";
		dropActionsForItem[item]["default"] = ListsModel::CREATE_NORMAL_PLAYLIST;
	}
	else
	{
		dropTypesForItem[item] << "application/x-defuzeme-genre";
		dropTypesForItem[item] << "application/x-defuzeme-album";
		dropTypesForItem[item] << "application/x-defuzeme-artist";
		dropActionsForItem[item]["default"] = ListsModel::CREATE_DYNAMIC_PLAYLIST;
	}
	item->setData(0, title);
	return item;
}

ListsItem *ListsModel::insertNormalList(const QMimeData *data, ListsItem *parentItem)
{
	int position = parentItem->childCount();
	insertRows(position, 1, parentItem->index);
	ListsItem *item = parentItem->child(position);
	item->setKind(ListsItem::PLAYLIST);
	item->setSubKind(ListsItem::NORMAL_PLAYLIST);
	dropTypesForItem[item] << "application/x-defuzeme-audiotrack";
	dropActionsForItem[item]["default"] = ListsModel::ADD_TO_NORMAL_PLAYLIST;
	item->setData(0, "New Playlist");
	item->index = index(position, 0, parentItem->index);

	QSqlQuery query;
	query.prepare("INSERT INTO playlists(name, is_dynamic) VALUES (:name, :is_dynamic)");
	query.bindValue(":name", "New playlist");
	query.bindValue(":is_dynamic", false);
	query.exec();
	item->setId(query.lastInsertId().toInt());

	insertParametersInNormalList(data, item);
	lists->getWidget()->getTreeViewWidget()->setExpanded(item->index, true);
	lists->getWidget()->getTreeViewWidget()->setCurrentIndex(item->index);
	lists->getWidget()->getTreeViewWidget()->edit(item->index);
	return item;
}

void ListsModel::insertParametersInNormalList(const QMimeData *data, ListsItem *parentItem)
{
	QByteArray	encodedData;
	ListsItem::SubKind subKind = ListsItem::INVALID_SUBKIND;
	if (data->hasFormat("application/x-defuzeme-audiotrack"))
	{
		encodedData = data->data("application/x-defuzeme-audiotrack");
		subKind = ListsItem::TRACK;
	}
	if (subKind == ListsItem::INVALID_SUBKIND)
		return;
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QList<int> ids;
	while (!stream.atEnd())
	{
		int id;
		stream >> id;
		ids << id;
	}
	////////////////////////////////////
	QList<int> existingTitles;
	for (int i = 0 ; i < parentItem->childCount() ; ++i)
		existingTitles << parentItem->child(i)->getId();
	int p = parentItem->childCount();
	for (int i = 0 ; i < ids.size() ; ++i)
	{
		Library::AudioTrack *audioTrack = Library::AudioTrack::getTrack(ids.at(i));
		if (!audioTrack)
			continue;
		QString title = audioTrack->getTitle();
		if (existingTitles.contains(ids.at(i)))
			continue;
		insertRows(p + i, 1, parentItem->index);
		ListsItem *paramItem = parentItem->child(p + i);
		paramItem->setId(ids.at(i));
		paramItem->setKind(ListsItem::PARAMETER);
		paramItem->setSubKind(subKind);
		paramItem->setIcon(audioTrack->getAlbumArtAtSize(22));
		paramItem->setData(0, title);
	}
	updateDBFromTree(parentItem);
	emit playlistContentChanged(parentItem->getId());
}

ListsItem *ListsModel::insertDynamicList(const QMimeData *data, ListsItem *parentItem)
{
	int position = parentItem->childCount();
	if (!insertRows(position, 1, parentItem->index))
		return 0;
	ListsItem *item = static_cast<ListsItem*>(parentItem->child(position));
	item->setKind(ListsItem::PLAYLIST);
	item->setSubKind(ListsItem::DYNAMIC_PLAYLIST);
	dropTypesForItem[item] << "application/x-defuzeme-genre";
	dropTypesForItem[item] << "application/x-defuzeme-album";
	dropTypesForItem[item] << "application/x-defuzeme-artist";
	dropActionsForItem[item]["default"] = ListsModel::ADD_TO_DYNAMIC_PLAYLIST;
	item->setData(0, "New playlist");
	item->index = index(position, 0, parentItem->index);

	QSqlQuery query;
	query.prepare("INSERT INTO playlists(name, is_dynamic) VALUES (:name, :is_dynamic)");
	query.bindValue(":name", "New playlist");
	query.bindValue(":is_dynamic", true);
	query.exec();
	item->setId(query.lastInsertId().toInt());

	ListsItem *introItem = insertIntroItem(item);

	insertParametersInDynamicList(data, introItem);

	lists->getWidget()->getTreeViewWidget()->setExpanded(item->index, true);
	lists->getWidget()->getTreeViewWidget()->setExpanded(introItem->index, true);
	lists->getWidget()->getTreeViewWidget()->setCurrentIndex(item->index);
	lists->getWidget()->getTreeViewWidget()->edit(item->index);

	return item;
}

ListsItem *ListsModel::insertIntroItem(ListsItem *parentItem)
{
	insertRows(0, 1, parentItem->index);
	ListsItem *introItem = static_cast<ListsItem*>(parentItem->child(0));
	introItem->setKind(ListsItem::SEPARATOR);
	introItem->setSubKind(ListsItem::INTRO);
	dropTypesForItem[introItem] << "application/x-defuzeme-genre";
	dropTypesForItem[introItem] << "application/x-defuzeme-album";
	dropTypesForItem[introItem] << "application/x-defuzeme-artist";
	dropActionsForItem[introItem]["default"] = ListsModel::ADD_TO_DYNAMIC_PLAYLIST;
	introItem->setData(0, "Songs");
	introItem->index = index(0, 0, parentItem->index);
	return introItem;
}

ListsItem *ListsModel::insertSeparatorItem(ListsItem::SubKind subKind, ListsItem *parentItem)
{
	int position = parentItem->childCount();
	insertRows(position, 1, parentItem->index);
	ListsItem *sepItem = static_cast<ListsItem*>(parentItem->child(position));
	sepItem->setKind(ListsItem::SEPARATOR);
	sepItem->setSubKind(subKind);
	dropTypesForItem[sepItem] << "application/x-defuzeme-genre";
	dropTypesForItem[sepItem] << "application/x-defuzeme-album";
	dropTypesForItem[sepItem] << "application/x-defuzeme-artist";
	dropActionsForItem[sepItem]["default"] = ListsModel::ADD_TO_DYNAMIC_PLAYLIST;
	sepItem->setData(0, "And");
	sepItem->index = index(position, 0, parentItem->index);
	return sepItem;
}

void ListsModel::insertParametersInDynamicList(const QMimeData *data, ListsItem *parentItem)
{
	QByteArray	encodedData;
	ListsItem::SubKind subKind = ListsItem::INVALID_SUBKIND;
	if (data->hasFormat("application/x-defuzeme-genre"))
	{
		encodedData = data->data("application/x-defuzeme-genre");
		subKind = ListsItem::GENRE;
	}
	else if (data->hasFormat("application/x-defuzeme-album"))
	{
		encodedData = data->data("application/x-defuzeme-album");
		subKind = ListsItem::ALBUM;
	}
	else if (data->hasFormat("application/x-defuzeme-artist"))
	{
		encodedData = data->data("application/x-defuzeme-artist");
		subKind = ListsItem::ARTIST;
	};
	if (subKind == ListsItem::INVALID_SUBKIND)
		return;
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QList<int> ids;
	while (!stream.atEnd())
	{
		int id;
		stream >> id;
		ids << id;
	}
	////////////////////////////////////
	ListsItem *separatorItem;
	ListsItem *listItem;

	if (parentItem->isOfKind(ListsItem::SEPARATOR))
	{
		separatorItem = parentItem;
		listItem = static_cast<ListsItem*>(parentItem->parent());
	}
	else if (parentItem->isOfKind(ListsItem::PLAYLIST, ListsItem::DYNAMIC_PLAYLIST))
	{
		listItem = parentItem;
		if (parentItem->childCount() == 0)
			separatorItem = insertIntroItem(parentItem);
		else
		{
			bool existingSeparator = false;
			for (int i = 0 ; i < parentItem->childCount() ; ++i)
			{
				separatorItem = parentItem->child(i);
				if (separatorItem->childCount() && separatorItem->child(0)->getSubKind() == subKind)
				{
					existingSeparator = true;
					break;
				}
			}
			if (!existingSeparator)
			{
				separatorItem = insertSeparatorItem(ListsItem::AND, parentItem);
				lists->getWidget()->getTreeViewWidget()->setExpanded(separatorItem->index, true);
			}
		}
	}
	else
		return;
	////////////////////////////////////
	QStringList existingTitles;
	for (int i = 0 ; i < separatorItem->childCount() ; ++i)
		existingTitles << separatorItem->child(i)->data(0).toString();
	int p = separatorItem->childCount();
	for (int i = 0 ; i < ids.size() ; ++i)
	{
		Library::AudioTrack *audioTrack = Library::AudioTrack::getTrack(ids.at(i));
		if (!audioTrack)
			continue;
		QString title;
		if (subKind == ListsItem::ARTIST)
			title = audioTrack->getAlbumArtist();
		else if (subKind == ListsItem::ALBUM)
			title = audioTrack->getAlbum();
		else if (subKind == ListsItem::GENRE)
			title = audioTrack->getGenre();
		if (existingTitles.contains(title, Qt::CaseInsensitive))
			continue;
		insertRows(p + i, 1, separatorItem->index);
		ListsItem *paramItem = static_cast<ListsItem*>(separatorItem->child(p + i));
		paramItem->setId(ids.at(i));
		paramItem->setKind(ListsItem::PARAMETER);
		paramItem->setSubKind(subKind);
		paramItem->setData(0, title);
	}
	updateDBFromTree(listItem);
	emit playlistContentChanged(listItem->getId());
}

void ListsModel::updateDBFromTree(ListsItem *list) const
{
	if (!list->isOfKind(ListsItem::PLAYLIST))
		return;
	if (list->isOfKind(ListsItem::PLAYLIST, ListsItem::NORMAL_PLAYLIST))
	{
		QList<QVariant> paramsList;
		for (int i = 0 ; i < list->childCount() ; ++i)
			paramsList << QVariant(list->child(i)->getId());
		QSqlQuery query;
		query.prepare("UPDATE playlists SET name = :name, definition = :definition WHERE id = :id");
		query.bindValue(":definition", Network::JsonParser().serialize(QVariant(paramsList)));
		query.bindValue(":name", list->data(0).toString());
		query.bindValue(":id", list->getId());
		query.exec();

	}
	else if (list->isOfKind(ListsItem::PLAYLIST, ListsItem::DYNAMIC_PLAYLIST))
	{
		QVariantMap map;
		QList<QVariant> separatorsList;
		map["id"] = list->getId();
		map["title"] = list->data(0);
		map["is_dynamic"] = true;
		for (int i = 0 ;  i < list->childCount() ; ++i)
		{
			ListsItem *separator = list->child(i);
			if (!separator->isOfKind(ListsItem::SEPARATOR))
				continue;

			QMap<QString, QList<QVariant> > subMap;
			for (int j = 0 ; j < separator->childCount() ; ++j)
			{
				ListsItem *parameter = separator->child(j);
				QString subKindStr;
				if (parameter->getSubKind() == ListsItem::GENRE)
					subKindStr = "genres";
				else if (parameter->getSubKind() == ListsItem::ALBUM)
					subKindStr = "albums";
				else if (parameter->getSubKind() == ListsItem::ARTIST)
					subKindStr = "artists";
				else
					continue;
				subMap[subKindStr] << QVariant(parameter->data(0));
			}
			QVariantMap variantSubMap;
			QMapIterator<QString, QList<QVariant> > it(subMap);
			while (it.hasNext()) {
				it.next();
				variantSubMap[it.key()] = QVariant(it.value());
			}
			separatorsList << variantSubMap;
		}
		map["separators"] = separatorsList;
		QSqlQuery query;
		query.prepare("UPDATE playlists SET name = :name, definition = :definition WHERE id = :id");
		query.bindValue(":definition", Network::JsonParser().serialize(QVariant(map)));
		query.bindValue(":name", map["title"]);
		query.bindValue(":id", list->getId());
		query.exec();
	}
}

void ListsModel::updateTreeFromDB()
{
	QSqlQuery normalQuery;
	normalQuery.prepare("SELECT id, name, definition FROM playlists WHERE is_dynamic = 'false'");
	normalQuery.exec();
	while (normalQuery.next())
	{
		int id = normalQuery.value(0).toInt();
		QString name = normalQuery.value(1).toString();
		QList<QVariant> trackList = Network::JsonParser().parse(normalQuery.value(2).toByteArray()).toList();

		int p = normalItem->childCount();
		normalItem->insertChildren(p, 1, 1);
		ListsItem *item = normalItem->child(p);
		item->setKind(ListsItem::PLAYLIST);
		item->setSubKind(ListsItem::NORMAL_PLAYLIST);
		dropTypesForItem[item] << "application/x-defuzeme-audiotrack";
		dropActionsForItem[item]["default"] = ListsModel::ADD_TO_NORMAL_PLAYLIST;
		item->setId(id);
		item->setData(0, name);

		foreach (QVariant trackId, trackList)
		{
			Library::AudioTrack *audioTrack = Library::AudioTrack::getTrack(trackId.toInt());
			if (!audioTrack)
				continue;
			int position = item->childCount();
			item->insertChildren(position, 1, 1);
			ListsItem *trackItem = item->child(position);
			trackItem->setKind(ListsItem::PARAMETER);
			trackItem->setSubKind(ListsItem::TRACK);
			trackItem->setId(trackId.toInt());
			trackItem->setIcon(audioTrack->getAlbumArtAtSize(22));
			trackItem->setData(0, audioTrack->getTitle());
		}

	}

	QSqlQuery query;
	query.prepare("SELECT id, definition FROM playlists WHERE is_dynamic = 'true'");
	query.exec();
	while (query.next())
	{
		int id = query.value(0).toInt();
		QVariantMap data = Network::JsonParser().parse(query.value(1).toByteArray()).toMap();
		if (data["id"] != id)
			continue;
		int p = dynamicItem->childCount();
		dynamicItem->insertChildren(p, 1, 1);
		ListsItem *item = dynamicItem->child(p);
		item->setKind(ListsItem::PLAYLIST);
		item->setSubKind(ListsItem::DYNAMIC_PLAYLIST);
		dropTypesForItem[item] << "application/x-defuzeme-genre";
		dropTypesForItem[item] << "application/x-defuzeme-album";
		dropTypesForItem[item] << "application/x-defuzeme-artist";
		dropActionsForItem[item]["default"] = ListsModel::ADD_TO_DYNAMIC_PLAYLIST;
		item->setId(id);
		item->setData(0, data["title"]);

		QList<QVariant> separators = data["separators"].toList();
		for (int i = 0 ; i < separators.size() ; ++i)
		{
			int position = item->childCount();
			item->insertChildren(position, 1, 1);
			ListsItem *sepItem = item->child(position);
			sepItem->setKind(ListsItem::SEPARATOR);
			dropTypesForItem[sepItem] << "application/x-defuzeme-genre";
			dropTypesForItem[sepItem] << "application/x-defuzeme-album";
			dropTypesForItem[sepItem] << "application/x-defuzeme-artist";
			dropActionsForItem[sepItem]["default"] = ListsModel::ADD_TO_DYNAMIC_PLAYLIST;
			if (i == 0)
			{
				sepItem->setSubKind(ListsItem::INTRO);
				sepItem->setData(0, "Songs");
			}
			else
			{
				sepItem->setSubKind(ListsItem::AND);
				sepItem->setData(0, "And");
			}

			QMap<QString, QVariant> paramsMap = separators.at(i).toMap();
			if (!paramsMap.size())
				continue;
			QString subKindStr = paramsMap.keys().first();
			QList<QVariant> paramsList = paramsMap[subKindStr].toList();
			ListsItem::SubKind subKind;
			if (subKindStr == "artists")
				subKind = ListsItem::ARTIST;
			else if (subKindStr == "albums")
				subKind = ListsItem::ALBUM;
			else if (subKindStr == "genres")
				subKind = ListsItem::GENRE;
			else
				continue;
			foreach (QVariant param, paramsList)
			{
				int pos = sepItem->childCount();
				sepItem->insertChildren(pos, 1, 1);
				ListsItem *paramItem = sepItem->child(pos);
				paramItem->setKind(ListsItem::PARAMETER);
				paramItem->setSubKind(subKind);
				paramItem->setData(0, param.toString());
			}
		}
	}
}

ListsItem *ListsModel::getRootItem() const
{
	return static_cast<ListsItem*>(rootItem);
}

QStringList ListsModel::getDropTypesForItem(ListsItem *item) const
{
	return dropTypesForItem[item];
}

ListsModel::DropAction ListsModel::getDropActionsForItem(ListsItem *item, const QString &mimeType) const
{
	if (dropActionsForItem[item].contains(mimeType))
		return dropActionsForItem[item][mimeType];
	else
		return ListsModel::INVALID_DROP_ACTION;
}
