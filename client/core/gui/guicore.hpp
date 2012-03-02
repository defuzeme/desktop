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

namespace Gui {
	class GuiCore;
}

#ifndef GUICORE_HPP
#define GUICORE_HPP

#include "core.hpp"
#include "mainwindow.hpp"
#include "parameterizable.hpp"
#include "uiconfiguration.hpp"
#include "modulefactory.hpp"
#include "about.hpp"
#include <QtCore/QList>


namespace Gui
{
	class ParametersModule;

	/** The main class handling Gui.
	  **/

	class GuiCore : public Params::Parameterizable, public Core
	{
		Q_OBJECT
		friend void Module::submitForDisplay();

		enum TAB_INDEX
		{
			BROADCAST = 0,
			SCHEDUELING = 1,
			SOCIAL = 2
		};

	public:
		GuiCore(QStringList &arguments);
		~GuiCore();
		void					init(Cores *cores);
		void					aboutToQuit();
		void					popup(QWidget* window);												///< Popup the widget
		MainWindow*				getMainWindow();
		ParametersModule*		getParametersModule() const;
		void					toogleParametersModule();
		void					showAll();
		About*					getAboutDialog() const;

	signals:
		void					showPopups();

	private:
		void					addModule(Module *module);
		void					addPendingModules();

		void					addTab(const QString &title, const QString &icon, bool active = false);
		Tab*					currentTab() const;
		Tab*					getTab(TAB_INDEX tabIndex) const;
		QList<Tab*>				tabs;

		MainWindow				*mainWindow;														///< The window
		About					*aboutDialog;
		UiConfiguration			*uiConfiguration;													///< The Ui Configuration
		bool					readyToAddModules;
		QList<Module*>			pendingModules;
		ParametersModule		*parametersModule;

		void 					defineParams();
	};
}

#endif // GUICORE_HPP
