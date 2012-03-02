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

#include "guicore.hpp"
#include "parametersmodule.hpp"
#include "exception.hpp"

using namespace Gui;

GuiCore::GuiCore(QStringList __attribute__((unused)) &arguments)
{
	mainWindow = NULL;
	readyToAddModules = false;
	uiConfiguration = new UiConfiguration();
	ModuleFactory::initModuleFactory(this);
}

GuiCore::~GuiCore()
{
	if (mainWindow)
		delete mainWindow;
}

void GuiCore::init(Cores *c)
{
	cores = c;
	setParamsName("gui");
	setParamsBackEnd(Parameterizable::NONE);
	registerToParamsCore(cores->params());
	mainWindow = new MainWindow(this);
	addTab(tr("Broadcast"), ":/icons/note", true);
	addTab(tr("Scheduling"), ":/icons/calendar");
	addTab(tr("Social"), ":/icons/social");
	aboutDialog = new About;
	//parametersModule = new ParametersModule(this, mainWindow);
	readyToAddModules = true;
	addPendingModules();
}

void GuiCore::aboutToQuit()
{
}

MainWindow* GuiCore::getMainWindow()
{
	return mainWindow;
}

ParametersModule *GuiCore::getParametersModule() const
{
	return parametersModule;
}

void GuiCore::defineParams()
{
	setParameter("maximized", true);
	setParameter("windowed", true);
	setParameter("aero", true);
	setParameter("window_title", "Defuze.me");
}

void	GuiCore::popup(QWidget* window)
{
	window->setParent(mainWindow);
	window->setWindowFlags(Qt::Tool);
	if (mainWindow->isVisible())
		window->show();
	else
		connect(this, SIGNAL(showPopups()), window, SLOT(show()));
}

void GuiCore::addTab(const QString &title, const QString &icon, bool active)
{
	mainWindow->addTab(new Tab(title), icon, active);
}

Tab* GuiCore::currentTab() const
{
	return mainWindow->currentTab();
}

Tab* GuiCore::getTab(TAB_INDEX tabIndex) const
{
	if (mainWindow->getTabs().size() >= tabIndex + 1)
		return mainWindow->getTabs().at(tabIndex);
	else
		return 0;
}

void GuiCore::addModule(Module *module)
{
	if (readyToAddModules)
	{
		Tab *tab;
		tab = getTab((TAB_INDEX)module->getTabIndex());
		Q_ASSERT(tab);
		tab->addModule(module);
	}
	else
	{
		pendingModules << module;
		emit module->displayPending();
	}
}

void GuiCore::addPendingModules()
{
	if (!readyToAddModules)
		return;
	while (pendingModules.size())
		addModule(pendingModules.takeFirst());
}

void GuiCore::toogleParametersModule()
{
	if (parametersModule->isHidden())
		parametersModule->show();
	else
		parametersModule->hide();
}

void	GuiCore::showAll()
{
	mainWindow->show();
	if (getPublicParameter("maximized").toBool())
		mainWindow->showMaximized();
	emit showPopups();
}

About* GuiCore::getAboutDialog() const
{
	return aboutDialog;
}
