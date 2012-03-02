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

#include "paramscore.hpp"
#include "exception.hpp"
#include <QtCore/QRegExp>
#include <QDebug>

using namespace Params;

ParamsCore::ParamsCore(QStringList &arguments)
{
	handleCmdLineParameters(arguments);
	//_printCmdLineParameters();
}

ParamsCore::~ParamsCore()
{
}

void ParamsCore::init(Cores *cores)
{
	setParamsName("params");
	setParamsBackEnd(Parameterizable::SETTINGS);
	registerToParamsCore(cores->params());
}

void ParamsCore::aboutToQuit()
{
}

const QHash<QString, QVariant> ParamsCore::cmdLineParameters(QString paramsName) const
{
	if (!_cmdLineParameters.contains(paramsName))
		_cmdLineParameters[paramsName];
	return _cmdLineParameters[paramsName];
}

void ParamsCore::handleCmdLineParameters(QStringList &arguments)
{
	QRegExp paramsPattern("--([a-z_]+)-([a-z0-9_-]+)(=(.+))?");

	foreach (QString argument, arguments)
	{
		if (paramsPattern.exactMatch(argument))
		{
			QStringList matches = paramsPattern.capturedTexts();
			if (matches[4] != "")
				_cmdLineParameters[matches[1]][matches[2]] = matches[4];
			else
				_cmdLineParameters[matches[1]][matches[2]] = true;
		}
	}
}

void ParamsCore::registerParameterizable(Parameterizable *parameterizable)
{
	Q_ASSERT(!_parameterizables.contains(parameterizable->paramsName()));
	_parameterizables[parameterizable->paramsName()] = parameterizable;
}

void ParamsCore::_printCmdLineParameters()
{
	QHash<QString, QHash<QString, QVariant> >::const_iterator i = _cmdLineParameters.constBegin();
	while (i != _cmdLineParameters.constEnd())
	{
		qDebug() << i.key();
		QHash<QString, QVariant>::const_iterator j = i.value().constBegin();
		while (j != i.value().constEnd())
		{
			qDebug() << "  " << j.key() << "=" << j.value().toString();
			++j;
		}
		++i;
	}
}

void ParamsCore::defineParams()
{
	setParameter("first_launch", true);
}
