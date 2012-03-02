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

#ifndef EDITABLETREEMODEL_HPP
#define EDITABLETREEMODEL_HPP

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class EditableTreeItem;

class EditableTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	EditableTreeModel(QObject *parent = 0);
	~EditableTreeModel();

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant &value,
				 int role = Qt::EditRole);
	bool setHeaderData(int section, Qt::Orientation orientation,
					   const QVariant &value, int role = Qt::EditRole);

	bool insertColumns(int position, int columns,
					   const QModelIndex &parent = QModelIndex());
	bool removeColumns(int position, int columns,
					   const QModelIndex &parent = QModelIndex());
	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());

protected:
	void setupModelData(const QStringList &lines, EditableTreeItem *parent);
	EditableTreeItem *getItem(const QModelIndex &index) const;

	EditableTreeItem *rootItem;
};

#endif // EDITABLETREEMODEL_HPP
