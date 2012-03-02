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

#include "listswidget.hpp"
#include "listsdelegate.hpp"
#include "listsplugin.hpp"

using namespace Lists;

ListsWidget::ListsWidget(ListsPlugin *lists, QWidget *parent) :
	ModuleWidget(parent), lists(lists)
{
	setupUi(this);

	QPalette palette;
	palette.setColor(QPalette::Background, QColor::fromRgb(132,130,128));
	palette.setColor(QPalette::Base, QColor::fromRgb(192,192,192));
	palette.setColor(QPalette::AlternateBase, QColor::fromRgb(177,177,177));

	treeView->setListsPlugin(lists);
	treeView->setPalette(palette);
	treeView->setHeaderHidden(true);
	treeView->setItemDelegate(new ListsDelegate);
	treeView->itemDelegate()->setParent(this);
	QObject::connect(treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(toogleItem(QModelIndex)));
}

ListsTreeView *ListsWidget::getTreeViewWidget() const
{
	return treeView;
}

void ListsWidget::toogleItem(const QModelIndex &index)
{
	if (treeView->isExpanded(index))
		treeView->collapse(index);
	else
		treeView->expand(index);
}

void ListsWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}
