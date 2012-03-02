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

#include "tab.hpp"
#include "module.hpp"
#include "droplocation.hpp"
#include <QDebug>

using namespace Gui;

Tab::Tab(const QString &name) : name(name)
{
	grid = new GridWidget(this);
}

Tab::~Tab()
{
	delete grid;
}

QString Tab::getName() const
{
	return name;
}

GridWidget* Tab::getGrid() const
{
	return grid;
}

int Tab::columnCount() const
{
	return getGrid()->getColumnsLayout()->count();
}

void Tab::addModule(Module *module)
{
	if (module->getColumn() == -1)
		module->setPosition(QPoint(columnCount(), module->getRow()));
	if (module->getColumn() >= columnCount())
	{
		int columns = columnCount();
		while (columns <= module->getColumn())
		{
			getGrid()->getColumnsLayout()->addLayout(new QVBoxLayout);
			++columns;
		}
	}
	GridElement *elem = new GridElement(module->title, getGrid());
	elem->setModule(module);
	elements[module] = elem;
	int row = 0;
	while (row < elem->getRow() &&
		   row < grid->getColumn(elem->getColumn())->count() &&
		   row >= static_cast<GridElement*>(grid->getColumn(elem->getColumn())->itemAt(row)->widget())->getRow())
		row++;
	grid->getColumn(module->getColumn())->insertWidget(row, elem);
}

void Tab::cleanGrid(QWidget *targetDropLocation)
{
	QList<QWidget*> toRemove;
	for (int i = 0 ; i < columnCount() ; ++i)
	{
		for (int j = 0 ; j < grid->getColumn(i)->count() ; ++j)
		{
			if (!dynamic_cast<GridElement*>(grid->getColumn(i)->itemAt(j)->widget()))
			{
				if (grid->getColumn(i)->itemAt(j)->widget() != targetDropLocation)
					toRemove << grid->getColumn(i)->itemAt(j)->widget();
				else
					grid->getColumn(i)->takeAt(j);
			}
		}

	}
	while (!toRemove.isEmpty())
		delete toRemove.takeFirst();
	int offset = 0;
	for (int i = 0 ; i < columnCount() ; ++i)
	{
		int column = i - offset;

		if (!grid->getColumn(column)->count())
		{
			delete grid->getColumnsLayout()->itemAt(column)->layout();
			offset += 1;
		}
		else
		{
			for (int j = 0 ; j < grid->getColumn(column)->count() ; ++j)
			{
				GridElement *elem = static_cast<GridElement*>(grid->getColumn(column)->itemAt(j)->widget());
				elem->setPosition(QPoint(column, j));
			}
		}
	}
//	grid->layout()->setContentsMargins(9,9,9,9);
//	grid->layout()->setSpacing(6);
}

void Tab::showDropLocations(QPoint sourcePosition)
{
	cleanGrid();
//	grid->layout()->setContentsMargins(0, 0, 0, 0);
//	grid->layout()->setSpacing(0);
	int offset = 0;
	int orientation = 0;
	int initialColumnCount = columnCount();
	for (int i = 0 ; i < initialColumnCount ; ++i)
	{
		int column = i + offset;
		showColumnDropLocations(column, orientation, offset, sourcePosition);
	}
}

void Tab::showRowDropLocations(int column, int initialColumn, QPoint &sourcePosition)
{
	int initialRowCount = grid->getColumn(column)->count();
	if (initialColumn == sourcePosition.x())
	{
		int offset = 0;
		int orientation = 0;
		for (int i = 0 ; i < initialRowCount ; ++i)
		{
			int row = i + offset;
			if (row - offset == sourcePosition.y())
				orientation = -1;
			if (orientation >= 0)
			{
				showDropLocation(QPoint(column, row + orientation), false);
				offset += 1;
			}
			else
				orientation = 1;
		}
	}
	else
	{
		int offset = 0;
		for (int i = 0 ; i < initialRowCount ; ++i)
		{
			int row = i + offset;
			if (row == 0)
			{
				showDropLocation(QPoint(column, row), false);
				showDropLocation(QPoint(column, row + 2), false);
				offset += 2;
			}
			else
			{
				showDropLocation(QPoint(column, row + 1), false);
				offset += 1;
			}
		}
	}
}

void Tab::showColumnDropLocations(int & column, int &orientation, int &offset, QPoint &sourcePosition)
{
	if (column - offset == sourcePosition.x())
		orientation = -1;
	if (orientation >= 0)
	{
		showDropLocation(QPoint(column + orientation, 0), true);
		showRowDropLocations(column + 1 - orientation, column - offset, sourcePosition);
		++offset;
	}
	else if (grid->getColumn(column)->count() > 1)
	{
		showDropLocation(QPoint(column, 0), true);
		showRowDropLocations(column + 1, column - offset, sourcePosition);
		showDropLocation(QPoint(column + 2, 0), true);
		offset += 2;
		orientation = 1;
	}
	else
		orientation = 1;
}

void Tab::showDropLocation(QPoint targetPosition, bool newColumn)
{
	if (newColumn)
	{
		grid->getColumnsLayout()->insertLayout(targetPosition.x(), new QVBoxLayout);
		grid->getColumn(targetPosition.x())->addWidget(new DropLocation(this,
																		targetPosition,
																		newColumn,
																		grid));
	}
	else
		grid->getColumn(targetPosition.x())->insertWidget(targetPosition.y(), new DropLocation(this,
																							   targetPosition,
																							   false,
																							   grid));
}

void Tab::insertGridElement(GridElement *gridElement,
							QPoint &targetPosition,
							bool __attribute__((unused)) newColumn)
{
	QWidget *dropLocation = grid->getColumn(targetPosition.x())->takeAt(targetPosition.y())->widget();
	grid->getColumn(targetPosition.x())->insertWidget(targetPosition.y(), gridElement);
	cleanGrid(dropLocation);
}

GridElement *Tab::getGridElementByModule(Module *module) const
{
	return elements[module];
}
