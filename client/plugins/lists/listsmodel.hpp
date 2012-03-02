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

#ifndef LISTSMODEL_HPP
#define LISTSMODEL_HPP

#include "editabletreemodel.hpp"
#include "listsitem.hpp"
#include <QHash>
#include <QItemSelectionModel>

namespace Lists
{
	class ListsPlugin;

	class ListsModel : public EditableTreeModel
	{
		Q_OBJECT
	public:
		enum DropAction {
			INVALID_DROP_ACTION,
			ADD_TO_NORMAL_PLAYLIST,
			ADD_TO_DYNAMIC_PLAYLIST,
			CREATE_NORMAL_PLAYLIST,
			CREATE_DYNAMIC_PLAYLIST
		};

		ListsModel(ListsPlugin *lists);
		~ListsModel();
		void setupModelData();
		ListsItem *getRootItem() const;
		QStringList getDropTypesForItem(ListsItem *item) const;
		DropAction getDropActionsForItem(ListsItem *item, const QString &mimeType = "default") const;

		bool insertRows(int position, int rows, const QModelIndex &parent);
		bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
		QMimeData *mimeData(const QModelIndexList &indexes) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant data(const QModelIndex &index, int role) const;
		ListsItem *getItem(const QModelIndex &index) const;

		ListsItem *normalItem;
		ListsItem *dynamicItem;
		QModelIndex lastDropIndex;

	public slots:
		void dataHaveChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
		void dataDeleted();

	signals:
		void playlistContentChanged(int playlistId);
		void playlistNameChanged(int playlistId, QString newName);
		void playlistRemoved(int playlistId);

	private:
		ListsItem *insertCategoryItem(const QString &title, ListsItem::SubKind subKind, int position = 0);
		ListsItem *insertAddItem(const QString &title, ListsItem::SubKind subKind);
		ListsItem *insertDynamicList(const QMimeData *data, ListsItem *parentItem);
		ListsItem *insertNormalList(const QMimeData *data, ListsItem *parentItem);

		ListsItem *insertIntroItem(ListsItem *parentItem);
		ListsItem *insertSeparatorItem(ListsItem::SubKind subKind, ListsItem *parentItem);
		void insertParametersInDynamicList(const QMimeData *data, ListsItem *parentItem);
		void insertParametersInNormalList(const QMimeData *data, ListsItem *parentItem);
		void removeList(const QModelIndex &listIndex);
		void removeSeparatorFromList(const QModelIndex &separatorIndex);
		void removeParameterFromList(const QModelIndex &parameterIndex);

		void updateDBFromTree(ListsItem *list) const;
		void updateTreeFromDB();
		bool hasAction(ListsItem *item, const QStringList &formats, DropAction action);

		ListsPlugin *lists;
		QHash<ListsItem*, QStringList> dropTypesForItem;
		QHash<ListsItem*, QHash<QString, DropAction> > dropActionsForItem;
	};

}

#endif // LISTSMODEL_HPP
