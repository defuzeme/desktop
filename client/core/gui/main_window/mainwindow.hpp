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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "tab.hpp"
#include "tabwidget.hpp"
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtGui/QMainWindow>

namespace Gui
{
	class GuiCore;

	/** The main window(s).
	  * Contain the menu, the tabs, and the modules grid.
	  * Up to 1 per screen.
	  **/

	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		MainWindow(GuiCore *guiCore, QWidget *parent = 0);
		~MainWindow();
		void					addTab(Tab *tab, const QString &icon, bool active = false);			///< Add a new Tab to the window
		Tab						*currentTab();														///< Return the current tab
		TabWidget* 				getTabWidget() const;
		QList<Tab*>				getTabs() const;

	private:
		QList<Tab*>				tabs;
		QHash<QWidget*, Tab*>	gridsTabs;
		TabWidget				*tabWidget;
	};
}

#endif // MAINWINDOW_HPP
