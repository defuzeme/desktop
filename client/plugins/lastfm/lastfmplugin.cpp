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

#include "lastfmplugin.hpp"

using namespace Lastfm;

LastfmPlugin::LastfmPlugin()
{

}

void LastfmPlugin::init()
{
	widget = new LastfmWidget(static_cast<Player::MainPlayer*>(plugins->get("player")),
							  static_cast<Library::LibraryPlugin*>(plugins->get("library")));

	Gui::Module *lastfmModule = Gui::ModuleFactory::create("lastfm", QPoint(0, 0), widget, 2);
	lastfmModule->submitForDisplay();

	Gui::Module *tmp = Gui::ModuleFactory::create("tmp", QPoint(1, 0), new Gui::ModuleWidget, 2);
	tmp->submitForDisplay();

	Gui::Module *tmp2 = Gui::ModuleFactory::create("tmp2", QPoint(2, 0), new Gui::ModuleWidget, 2);
	tmp2->submitForDisplay();
}
