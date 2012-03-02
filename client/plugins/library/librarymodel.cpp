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

#include "librarymodel.hpp"
#include "library.hpp"
#include "exception.hpp"
#include "libraryitem.hpp"
#include <QSqlRecord>
#include <QStack>
#include <QDebug>

using namespace Library;

LibraryModel::LibraryModel(LibraryPlugin *library) : TreeModel(), library(library)
{
	currentSearch = "";
	initKeys();
}

void LibraryModel::initKeys()
{
	sortKeysInfo[TRACK]  << "Track"  << "title"  << "track"  << "No title info";
	sortKeysInfo[TITLE]  << "Title"  << "title"  << "title"  << "No title info";
	sortKeysInfo[ALBUM]  << "Album"  << "album"  << "album"  << "Unknown Album";
	sortKeysInfo[ARTIST] << "Artist" << "artist" << "artist" << "Unknown Artist";
	sortKeysInfo[GENRE]  << "Genre"  << "genre"  << "genre"  << "Unknown Genre";
	sortKeysInfo[ALBUM_ARTIST] << "Artist" << "album_artist" << "album_artist" << "Unknown Artist";
	sortKeys << ALBUM_ARTIST << ALBUM << TRACK;
}

void LibraryModel::setupModelData()
{
	update();
}

void LibraryModel::update(int sort, QString search)
{
	QSqlQuery query;

	buildQuery(&query, sort, search);
	if (!query.exec())
		throw_exception(0x01, tr("Can't get AudioTracks from database: %1").arg(query.lastError().text()));
	clear();
	buildTree(&query, rootItem);
}

void LibraryModel::clear()
{
	beginRemoveRows(QModelIndex(), 0, rootItem->childCount());
	delete rootItem;
	endRemoveRows();
	QList<QVariant> rootData;
	rootData << "1";
	rootItem = new TreeItem(rootData);
}

void LibraryModel::buildQuery(QSqlQuery *query, int sort, const QString &search)
{
	QString searchWhere = "";

	if (!search.isNull())
		currentSearch = search;
	if (sort >= 0)
		updateSortKeys(sort);
	if (!(currentSearch.isNull() || currentSearch.isEmpty()))
		searchWhere = updateSearch();
	query->prepare("SELECT id, album_artist, artist, album, title, duration, genre "
				   "FROM audio_tracks " + searchWhere + "ORDER BY " + orderByKeys() + ";");
	if (!(searchWhere.isNull() || searchWhere.isEmpty()))
		for (int i = 0 ; i < sortKeys.size() ; ++i)
			query->bindValue(":search" + QVariant(i).toString(), "%" + currentSearch + "%");
}

void LibraryModel::buildTree(QSqlQuery *query, TreeItem *parent)
{
	QStringList fields;
	QStack<TreeItem*> itemsStack;

	for (int i = 0 ; i < sortKeys.size() ; ++i)
		fields << QString();
	while (query->next())
	{
		foreach (SortKeys key , sortKeys)
		{
			QString str = query->value(query->record().indexOf(sortKeysInfo[key][FIELD])).toString();
			if (str.isEmpty())
				str = sortKeysInfo[key][DEFAULT];
			if (str.compare(fields[sortKeys.indexOf(key)], Qt::CaseInsensitive) || isLowestLevelSortKey(key))
			{
				for (int i = sortKeys.indexOf(key) + 1 ; i < sortKeys.size() ; ++i)
					fields[i] = QString();
				while (itemsStack.size() > sortKeys.indexOf(key))
					itemsStack.pop();
				fields[sortKeys.indexOf(key)] = str;
				if (itemsStack.isEmpty())
					parent = rootItem;
				else
					parent = itemsStack.top();
				LibraryItem *elem = new LibraryItem(str, query->value(0).toInt(), key, parent);
				itemsStack.push(elem);
				parent->appendChild(elem);
				elem->setIndex(createIndex(elem->row(), 0, elem));
				if (!(currentSearch.isNull() || currentSearch.isEmpty()))
					if (str.contains(currentSearch, Qt::CaseInsensitive))
					{
						library->getWidget()->getTreeViewWidget()->setExpanded(elem->getIndex().parent(), true);
						library->getWidget()->getTreeViewWidget()->setExpanded(elem->getIndex().parent().parent(), true);
						//setElementAsSearchResult
					}
				//if (isLowestLevelSortKey(key))
				idByIndexes[elem->getIndex()] = query->value(0).toInt();
				kindByIndexes[elem->getIndex()] = key;
			}
		}
	}
}

QString LibraryModel::orderByKeys() const
{
	QStringList orderByKeys;

	foreach (SortKeys key, sortKeys)
		orderByKeys << sortKeysInfo[key][ORDER_BY];
	return orderByKeys.join(", ");
}

bool LibraryModel::isLowestLevelSortKey(SortKeys key)
{
	return (sortKeys.indexOf(key) == (sortKeys.size() - 1));
}

void LibraryModel::updateSortKeys(int sort)
{
	sortKeys.clear();
	if (sort == 1)
		sortKeys << ALBUM_ARTIST << TITLE;
	else if (sort == 2)
		sortKeys << GENRE << ARTIST << TITLE;
	else if (sort == 3)
		sortKeys << GENRE << TITLE;
	else
		sortKeys << ALBUM_ARTIST << ALBUM << TRACK;

}

QString LibraryModel::updateSearch()
{
	QString like = " LIKE :search";
	QStringList fields;
	int i = 0;

	foreach (SortKeys key, sortKeys)
	{
		fields << sortKeysInfo[key][FIELD] + like + QVariant(i).toString() + " ";
		++i;
	}
	return QString("WHERE (") + fields.join(" OR ") + QString(") ");
}

QStringList LibraryModel::mimeTypes() const
{
	QStringList types;
	types << "application/x-defuzeme-audiotrack";
	return types;
}

QMimeData *LibraryModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndex index;

	if (indexes.length() >= 1)
		index = indexes.first();
	//foreach (const QModelIndex &index, indexes) {
	if (index.isValid()) {
		stream << getIdByIndex(index);
	}
	//}
	LibraryModel::SortKeys key = kindByIndexes[index];
	if (key == LibraryModel::TRACK || key == LibraryModel::TITLE)
		mimeData->setData("application/x-defuzeme-audiotrack", encodedData);
	else if (key == LibraryModel::ALBUM_ARTIST || key == LibraryModel::ARTIST)
		mimeData->setData("application/x-defuzeme-artist", encodedData);
	else if (key == LibraryModel::ALBUM)
		mimeData->setData("application/x-defuzeme-album", encodedData);
	else if (key == LibraryModel::GENRE)
		mimeData->setData("application/x-defuzeme-genre", encodedData);
	else
		mimeData->setData("application/x-defuzeme-invalid", encodedData);
	return mimeData;
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	LibraryItem *item = static_cast<LibraryItem*>(index.internalPointer());

	if (role == Qt::ToolTipRole)
		return item->data(index.column());

	return qVariantFromValue((void*)item);
}

int LibraryModel::getIdByIndex(const QModelIndex &index) const
{
	if (idByIndexes.contains(index))
		return idByIndexes[index];
	else
		return 0;
}

LibraryModel::SortKeys LibraryModel::getKindByIndex(const QModelIndex &index) const
{
	if (kindByIndexes.contains(index))
		return kindByIndexes[index];
	else
		return LibraryModel::INVALID;
}

LibraryPlugin *LibraryModel::getLibrary() const
{
	return library;
}
