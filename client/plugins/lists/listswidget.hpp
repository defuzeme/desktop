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

#ifndef LISTSWIDGET_HPP
#define LISTSWIDGET_HPP

#include "ui_listswidget.h"
#include "guicore.hpp"


namespace Lists
{
	class ListsPlugin;

	class ListsWidget : public Gui::ModuleWidget, private Ui::ListsWidget
	{
		Q_OBJECT

	public:
		explicit	ListsWidget(ListsPlugin *lists, QWidget *parent = 0);
		ListsTreeView	*getTreeViewWidget() const;

	public slots:
		void		toogleItem(const QModelIndex &index);

	protected:
		void		changeEvent(QEvent *e);
		ListsPlugin *lists;
	};

}

#endif // LISTSWIDGET_HPP
