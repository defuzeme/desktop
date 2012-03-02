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

#include "cores.hpp"
#include "guicore.hpp"
#include "paramscore.hpp"
#include "dbcore.hpp"
#include "networkcore.hpp"
#include "audiocore.hpp"

Cores::Cores(QStringList &arguments)
{
	_db = new DB::DBCore(arguments);
	_params = new Params::ParamsCore(arguments);
	_audio = new Audio::AudioCore(arguments);
	_gui = new Gui::GuiCore(arguments);
	_net = new Network::NetworkCore(arguments);
}

Cores::~Cores()
{
	delete _net;
	delete _gui;
	delete _audio;
	delete _params;
	delete _db;
}

void Cores::init()
{
	_db->init(this);
	_params->init(this);
	_audio->init(this);
	_gui->init(this);
	_net->init(this);
}

void Cores::aboutToQuit()
{
	_net->aboutToQuit();
	_gui->aboutToQuit();
	_audio->aboutToQuit();
	_params->aboutToQuit();
	_db->aboutToQuit();
	deleteLater();
}

Gui::GuiCore* Cores::gui()
{
	return _gui;
}

DB::DBCore* Cores::db()
{
	return _db;
}

Network::NetworkCore* Cores::net()
{
	return _net;
}

Audio::AudioCore* Cores::audio()
{
	return _audio;
}

Params::ParamsCore* Cores::params()
{
	return _params;
}
