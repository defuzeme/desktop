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

#include "listsitem.hpp"
#include "listsmodel.hpp"
#include <QDebug>

using namespace Lists;

ListsItem::ListsItem(const QString &title, QPixmap icon, ListsModel *model, EditableTreeItem *parent)
	: EditableTreeItem(title, parent), icon(icon), model(model)
{
	id = 0;
}

bool ListsItem::insertChildren(int position, int count, int columns)
{
	Q_UNUSED(columns)

	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		ListsItem *item = new ListsItem("", QPixmap(), model, this);
		childItems.insert(position, item);
	}

	return true;
}

ListsItem *ListsItem::child(int number)
{
	return static_cast<ListsItem*>(childItems.value(number));
}

ListsItem::Kind ListsItem::getKind() const
{
	return kind;
}

ListsItem::SubKind ListsItem::getSubKind() const
{
	return subKind;
}

void ListsItem::setKind(ListsItem::Kind kind)
{
	this->kind = kind;
}

void ListsItem::setSubKind(ListsItem::SubKind subKind)
{
	this->subKind = subKind;
}

ListsModel *ListsItem::getModel() const
{
	return model;
}

bool ListsItem::isOfKind(Kind _kind)
{
	if (kind == _kind)
		return true;
	return false;
}

bool ListsItem::isOfKind(Kind _kind, SubKind _subKind)
{
	if (kind == _kind && subKind == _subKind)
		return true;
	return false;
}

unsigned int ListsItem::getId() const
{
	return id;
}

void ListsItem::setId(unsigned int id)
{
	this->id = id;
}

void ListsItem::setIcon(QPixmap icon)
{
	this->icon = icon;
}

const QPixmap &ListsItem::getIcon() const
{
	return icon;
}
