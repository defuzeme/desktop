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

#include "parameterizable.hpp"
#include "exception.hpp"
#include "logger.hpp"
#include "paramscore.hpp"
#include "dbcore.hpp"
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QRegExp>

using namespace Params;

Parameterizable::Parameterizable(Parameterizable::BackEnd backEnd, QString name)
{
	setParamsBackEnd(backEnd);
	setParamsName(name);
}

void Parameterizable::registerToParamsCore(ParamsCore *paramsCore)
{
	Q_ASSERT(hasNameAndBackEnd());
	paramsCore->registerParameterizable(this);
	loadDefaultParams();
	if (_paramsBackEnd == Parameterizable::SETTINGS)
		loadFromSettings();
	if (_paramsBackEnd == Parameterizable::DATABASE)
		loadFromDatabase();
	loadFromCommandLine(paramsCore);
	copyParamsToPublic();
}

bool Parameterizable::setParamsName(const QString &name)
{
	_paramsName = name;
	return true;
}

void Parameterizable::setParamsBackEnd(Parameterizable::BackEnd backEnd, DB::DBCore *dbCore)
{
	_dbCore = dbCore;
	_paramsBackEnd = backEnd;
}

void Parameterizable::setParamsBackEnd(DB::DBCore *dbCore)
{
	setParamsBackEnd(Parameterizable::DATABASE, dbCore);
}

bool Parameterizable::commitParameters()
{
	Q_ASSERT(hasNameAndBackEnd());
	QStringList modifiedKeys;
	QHashIterator<QString, QVariant> i(parameters);
	while (i.hasNext())
	{
		i.next();
		if ((!publicParameters.contains(i.key())) || publicParameters[i.key()] != i.value())
		{
			modifiedKeys << i.key();
		}
	}

	if (_paramsBackEnd == Parameterizable::SETTINGS)
	{
		QSettings settings;

		settings.beginGroup(_paramsName);
		QHash<QString, QVariant>::const_iterator i = parameters.constBegin();
		while (i != parameters.constEnd())
		{
			settings.setValue(i.key(), i.value());
			++i;
		}
		settings.sync();
		settings.endGroup();
	}
	else if (_paramsBackEnd == Parameterizable::DATABASE)
	{
		Q_ASSERT(_dbCore);
		try
		{
			QHash<QString, QVariant>::const_iterator i = parameters.constBegin();
			while (i != parameters.constEnd())
			{
				_dbCore->setSetting("params-" + _paramsName + "-" + i.key(), i.value());
				++i;
			}
		}
		catch (Exception &e)
		{
			throw_exception(0x04, tr("Cannot commit parameters to the database"));
			QHash<QString, QVariant>::const_iterator i = publicParameters.constBegin();
			while (i != publicParameters.constEnd())
			{
				_dbCore->setSetting("params-" + _paramsName + "-" + i.key(), i.value());
				++i;
			}
		}
	}

	copyParamsToPublic();
	emit commitedValues(modifiedKeys);
	return true;
}

QVariant Parameterizable::getParameter(const QString &key) const
{
	Q_ASSERT(parameters.contains(key));
	return parameters[key];
}

QVariant Parameterizable::getParameter(const QString &key, QVariant falloutValue) const
{
	if (!parameters.contains(key))
		return falloutValue;
	return parameters[key];
}

QVariant Parameterizable::getPublicParameter(const QString &key, QVariant falloutValue) const
{
	if (!publicParameters.contains(key))
		return falloutValue;
	return publicParameters[key];
}

QVariant Parameterizable::getPublicParameter(const QString &key) const
{
	Q_ASSERT(publicParameters.contains(key));
	return publicParameters[key];
}

void Parameterizable::setParameter(const QString &key, QVariant value)
{
	QRegExp keyPattern("[a-z0-9_-]+");

	Q_ASSERT(keyPattern.exactMatch(key));
	parameters[key] = value;
}

bool Parameterizable::hasNameAndBackEnd() const
{
	if (_paramsName.isEmpty() || _paramsBackEnd == UNSET)
		return false;
	return true;
}

void Parameterizable::copyParamsToPublic()
{
	QHash<QString, QVariant>::const_iterator i = parameters.constBegin();
	while (i != parameters.constEnd())
	{
		publicParameters[i.key()] = i.value();
		++i;
	}
}

const QString& Parameterizable::paramsName() const
{
	return _paramsName;
}

Parameterizable::BackEnd Parameterizable::paramsBackEnd() const
{
	return _paramsBackEnd;
}

void Parameterizable::updatedValues(const QStringList __attribute__((unused)) &keys)
{

}

void Parameterizable::applyCmdLineParameters(const QHash<QString, QVariant>& cmdLineParameters)
{
	QHash<QString, QVariant>::const_iterator i = cmdLineParameters.constBegin();
	while (i != cmdLineParameters.constEnd())
	{
		setParameter(i.key(), i.value());
		++i;
	}
}

void Parameterizable::loadFromSettings()
{
	QSettings settings;

	settings.beginGroup(_paramsName);
	QStringList keys = settings.allKeys();
	foreach (QString key, keys)
	{
		setParameter(key, settings.value(key));
	}
	settings.endGroup();
}

void Parameterizable::loadDefaultParams()
{
	try
	{
		defineParams();
	}
	catch (Exception &e)
	{
		Q_ASSERT_X(false, "parameterizable", "Cannot load default parameters");
	}
}

void Parameterizable::loadFromCommandLine(ParamsCore *paramsCore)
{
	try
	{
		applyCmdLineParameters(paramsCore->cmdLineParameters(_paramsName));
	}
	catch (Exception &e)
	{
		throw_exception(0x09, tr("Invalid command line parameter"));
	}
}

void Parameterizable::loadFromDatabase()
{
	QRegExp rawKeyPattern("params-[a-z_]+-([a-z0-9_-]+)");

	Q_ASSERT(_dbCore);
	QHash<QString, QVariant> rawSettings = _dbCore->settingsStartingWith("params-" + _paramsName + "-");
	QHash<QString, QVariant>::const_iterator i = rawSettings.constBegin();
	while (i != rawSettings.constEnd())
	{
		QString fullKey = i.key();

		if (rawKeyPattern.exactMatch(fullKey))
			setParameter(rawKeyPattern.capturedTexts()[1], i.value());
		++i;
	}
}
