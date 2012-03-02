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

#include "editabletreeitem.hpp"
#include <QStringList>

EditableTreeItem::EditableTreeItem(const QVector<QVariant> &data, EditableTreeItem *parent)
{
	parentItem = parent;
	itemData = data;
}

EditableTreeItem::EditableTreeItem(const QString &title, EditableTreeItem *parent)
{
	parentItem = parent;
	QVector<QVariant> data;
	data << title;
	itemData = data;
}

EditableTreeItem::~EditableTreeItem()
{
	qDeleteAll(childItems);
}

EditableTreeItem *EditableTreeItem::child(int number)
{
	return childItems.value(number);
}

int EditableTreeItem::childCount() const
{
	return childItems.count();
}

int EditableTreeItem::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<EditableTreeItem*>(this));

	return 0;
}

int EditableTreeItem::columnCount() const
{
	return itemData.count();
}

QVariant EditableTreeItem::data(int column) const
{
	return itemData.value(column);
}

bool EditableTreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		EditableTreeItem *item = new EditableTreeItem(data, this);
		childItems.insert(position, item);
	}

	return true;
}

bool EditableTreeItem::insertColumns(int position, int columns)
{
	if (position < 0 || position > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.insert(position, QVariant());

	foreach (EditableTreeItem *child, childItems)
		child->insertColumns(position, columns);

	return true;
}

EditableTreeItem *EditableTreeItem::parent()
{
	return parentItem;
}

bool EditableTreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

bool EditableTreeItem::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.remove(position);

	foreach (EditableTreeItem *child, childItems)
		child->removeColumns(position, columns);

	return true;
}

bool EditableTreeItem::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= itemData.size())
		return false;

	itemData[column] = value;
	return true;
}
