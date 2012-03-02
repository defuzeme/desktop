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

#include "mainwindow.hpp"
#include "tabwidget.hpp"
#include "guicore.hpp"
#include "qtwin.h"

#include <QVBoxLayout>
#include <QtGui/qboxlayout.h>
#include <QtGui/qslider.h>
#include <QtGui/qstyle.h>
#include <QtGui/qtoolbutton.h>
#include <QPushButton>
#include "audiocore.hpp"
#include <QDebug>
#include <QFileDialog>

using namespace Gui;

MainWindow::MainWindow(GuiCore *guiCore, QWidget *parent) : QMainWindow(parent)
{
	setStyleSheet("QTabWidget::tab-bar { left: 5px; }"
				  "QTabBar::tab {"
				  "height: 31px;"
				  "margin-top: 6px;"
				  "margin-left: 6px;"
				  "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(220, 220, 220, 128), stop: 1.0 rgba(180, 180, 180, 128));"
				  "border: 1px solid rgba(255, 255, 255, 80);"
				  "border-bottom: none;"
				  "color: black;"
				  "padding: 3px 12px;"
				  "}"
				  "QTabBar::tab:!selected {"
				  "}"
				  "QTabBar::tab:selected, QTabBar::tab:hover { "
				  "margin-top: 6px;"
				  "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(105, 104, 102, 255), stop: 1.0 rgba(75, 74, 72, 255));"
				  "height: 32px;"
				  "color: white;"
				  "}");
	setWindowTitle(guiCore->getPublicParameter("window_title").toString());
	setWindowIcon(QIcon(":/icons/logo"));
	tabWidget = new TabWidget(guiCore, this);
	tabWidget->setLayoutDirection(Qt::RightToLeft);
	tabWidget->setMovable(false);
	tabWidget->setDocumentMode(true);
	tabWidget->setDrawBase(false);
	setCentralWidget(tabWidget);
	if (guiCore->getPublicParameter("aero", QVariant(true)).toBool())
	{
		if (QtWin::isCompositionEnabled())
		{
			QtWin::extendFrameIntoClientArea(this, 0, 0, 46, 0);
			setContentsMargins(0, 0, -2, -2);
		}
	}
}

MainWindow::~MainWindow()
{
}

void MainWindow::addTab(Tab *tab, const QString &icon, bool active)
{
	tab->getGrid()->setParent(tabWidget);
	tabs << tab;
	gridsTabs[tab->getGrid()] = tab;
	int idx = tabWidget->addTab(tab->getGrid(), QIcon(icon), tab->getName());
	if (active)
		tabWidget->setCurrentIndex(idx);
}

Tab* MainWindow::currentTab()
{
	return gridsTabs[tabWidget->currentWidget()];
}

TabWidget *MainWindow::getTabWidget() const
{
	return tabWidget;
}

QList<Tab*> MainWindow::getTabs() const
{
	return tabs;
}
