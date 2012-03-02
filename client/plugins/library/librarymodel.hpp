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

#ifndef LIBRARYMODEL_HPP
#define LIBRARYMODEL_HPP

#include <QSqlQuery>
#include "audiotrack.hpp"
#include "treemodel.hpp"

namespace Library
{
	class LibraryPlugin;

	class LibraryModel : public TreeModel
	{
	public:

		enum SortKeys
		{
			INVALID, ALBUM, ARTIST, ALBUM_ARTIST, GENRE, TITLE, TRACK
		};
		enum SortKeysInfoData
		{
			TEXT = 0, FIELD = 1, ORDER_BY = 2, DEFAULT = 3
		};

		LibraryModel(LibraryPlugin *library);
		LibraryPlugin*					getLibrary() const;
		int								getIdByIndex(const QModelIndex &index) const;
		SortKeys						getKindByIndex(const QModelIndex &index) const;
		void							update(int sort = -1, QString search = QString());
		QStringList						mimeTypes() const;
		QMimeData*						mimeData(const QModelIndexList &indexes) const;
		Qt::ItemFlags					flags(const QModelIndex &index) const;
		QVariant						data(const QModelIndex &index, int role) const;

	protected:
		void							setupModelData();

	private:
		bool							isLowestLevelSortKey(SortKeys key);
		void							updateSortKeys(int sort);
		QString							updateSearch();
		QString 						orderByKeys() const;
		void							initKeys();
		void							buildQuery(QSqlQuery *query, int sort, const QString &search);
		void							buildTree(QSqlQuery *query, TreeItem *parent);
		void							clear();

		LibraryPlugin					*library;
		QHash<SortKeys, QStringList>	sortKeysInfo;
		QList<SortKeys>					sortKeys;
		QHash<QModelIndex, int>			idByIndexes;
		QHash<QModelIndex, SortKeys>	kindByIndexes;
		QString							currentSearch;
	};
}

#endif // LIBRARYMODEL_HPP
