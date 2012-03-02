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

#include "editabletreemodel.hpp"
#include "editabletreeitem.hpp"
#include <QtGui>

EditableTreeModel::EditableTreeModel(QObject *parent)
	: QAbstractItemModel(parent)
{
//	QVector<QVariant> rootData;
//	rootData << "One";

//	rootItem = new EditableTreeItem(rootData);
	//setupModelData(data.split(QString("\n")), rootItem);
}

EditableTreeModel::~EditableTreeModel()
{
	delete rootItem;
}

int EditableTreeModel::columnCount(const QModelIndex & /* parent */) const
{
	return rootItem->columnCount();
}

QVariant EditableTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	EditableTreeItem *item = getItem(index);

	return item->data(index.column());
}

Qt::ItemFlags EditableTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

EditableTreeItem *EditableTreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		EditableTreeItem *item = static_cast<EditableTreeItem*>(index.internalPointer());
		if (item) return item;
	}
	return rootItem;
}

QVariant EditableTreeModel::headerData(int section, Qt::Orientation orientation,
							   int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QModelIndex EditableTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	EditableTreeItem *parentItem = getItem(parent);

	EditableTreeItem *childItem = parentItem->child(row);
	if (childItem)
	{
		QModelIndex idx = createIndex(row, column, childItem);
		childItem->index = idx;
		return idx;
	}
	else
		return QModelIndex();
}

bool EditableTreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginInsertColumns(parent, position, position + columns - 1);
	success = rootItem->insertColumns(position, columns);
	endInsertColumns();

	return success;
}

bool EditableTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	EditableTreeItem *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, rootItem->columnCount());
	endInsertRows();

	return success;
}

QModelIndex EditableTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	EditableTreeItem *childItem = getItem(index);
	EditableTreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	QModelIndex idx = createIndex(parentItem->childNumber(), 0, parentItem);
	parentItem->index = idx;
	return idx;
}

bool EditableTreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginRemoveColumns(parent, position, position + columns - 1);
	success = rootItem->removeColumns(position, columns);
	endRemoveColumns();

	if (rootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return success;
}

bool EditableTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	EditableTreeItem *parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

int EditableTreeModel::rowCount(const QModelIndex &parent) const
{
	EditableTreeItem *parentItem = getItem(parent);

	return parentItem->childCount();
}

bool EditableTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	EditableTreeItem *item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result)
		emit dataChanged(index, index);

	return result;
}

bool EditableTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
	if (role != Qt::EditRole || orientation != Qt::Horizontal)
		return false;

	bool result = rootItem->setData(section, value);

	if (result)
		emit headerDataChanged(orientation, section, section);

	return result;
}

void EditableTreeModel::setupModelData(const QStringList &lines, EditableTreeItem *parent)
{
	QList<EditableTreeItem*> parents;
	QList<int> indentations;
	parents << parent;
	indentations << 0;

	int number = 0;

	while (number < lines.count()) {
		int position = 0;
		while (position < lines[number].length()) {
			if (lines[number].mid(position, 1) != " ")
				break;
			position++;
		}

		QString lineData = lines[number].mid(position).trimmed();

		if (!lineData.isEmpty()) {
			// Read the column data from the rest of the line.
			QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
			QVector<QVariant> columnData;
			for (int column = 0; column < columnStrings.count(); ++column)
				columnData << columnStrings[column];

			if (position > indentations.last()) {
				// The last child of the current parent is now the new parent
				// unless the current parent has no children.

				if (parents.last()->childCount() > 0) {
					parents << parents.last()->child(parents.last()->childCount()-1);
					indentations << position;
				}
			} else {
				while (position < indentations.last() && parents.count() > 0) {
					parents.pop_back();
					indentations.pop_back();
				}
			}

			// Append a new item to the current parent's list of children.
			EditableTreeItem *parent = parents.last();
			parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
			for (int column = 0; column < columnData.size(); ++column)
				parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
		}

		number++;
	}
}
