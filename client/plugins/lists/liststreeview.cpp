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

#include "liststreeview.hpp"
#include "listsplugin.hpp"

using namespace Lists;

ListsTreeView::ListsTreeView(QWidget *parent) :
	QTreeView(parent)
{
}

void	ListsTreeView::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("application/x-defuzeme-audiotrack")
			|| event->mimeData()->hasFormat("application/x-defuzeme-artist")
			|| event->mimeData()->hasFormat("application/x-defuzeme-genre")
			|| event->mimeData()->hasFormat("application/x-defuzeme-album"))
		event->accept();
	else
		event->ignore();

}

void	ListsTreeView::dragMoveEvent(QDragMoveEvent *event)
{
	QModelIndex idx = indexAt(event->pos());

	if (!idx.isValid())
	{
		event->ignore();
		return;
	}
	ListsItem *item = static_cast<ListsItem*>(idx.internalPointer());

	foreach (QString format, event->mimeData()->formats())
	{
		if (lists->getModel()->getDropTypesForItem(item).contains(format, Qt::CaseInsensitive))
		{
			event->accept();
			return;
		}
	}
	event->ignore();
}

void ListsTreeView::dropEvent(QDropEvent *event)
{
	lists->getModel()->lastDropIndex = indexAt(event->pos());
	QTreeView::dropEvent(event);
}

void	ListsTreeView::keyReleaseEvent(QKeyEvent *event)
{
	if (event->matches(QKeySequence::Delete) /*|| (event->key() == Qt::Key_Backspace)*/)
	{
		emit deleteElements();
	}
	else
		QWidget::keyReleaseEvent(event);
}

void ListsTreeView::setListsPlugin(ListsPlugin *lists)
{
	this->lists = lists;
}

void ListsTreeView::focusOutEvent(QFocusEvent *event)
{
	Q_UNUSED(event);
	clearSelection();
}
