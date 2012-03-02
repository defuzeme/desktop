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

#ifndef PARAMSCORE_HPP
#define PARAMSCORE_HPP

#include "core.hpp"
#include "parameterizable.hpp"
#include <QtCore/QHash>
#include <QtCore/QVariant>

namespace Params
{

	/** Register all Parameterizable classes and handle the command line arguments.
	  *
	  * \todo explain the parameters priorities when command line arguments are used.
	  **/

	class ParamsCore : public Core, public Parameterizable
	{
	public:
		ParamsCore(QStringList &arguments);
		~ParamsCore();
		void init(Cores *cores);
		void aboutToQuit();

		void registerParameterizable(Parameterizable *parameterizable);
		const QHash<QString, QVariant> cmdLineParameters(QString paramsName) const;

	private:
		void handleCmdLineParameters(QStringList &arguments);
		void _printCmdLineParameters();
		void defineParams();

		QHash<QString, QHash<QString, QVariant> > _cmdLineParameters;									///< Store the parameters setted via the command line
		QHash<QString, Parameterizable*> _parameterizables;
	};

}

#endif // PARAMSCORE_HPP
