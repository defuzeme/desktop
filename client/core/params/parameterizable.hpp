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

#ifndef PARAMETERIZABLE_HPP
#define PARAMETERIZABLE_HPP

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QHash>
#include <QtGui/QValidator>

#define INT_VALIDATOR new QIntValidator
//#define INT_RANGE_VALIDATOR(min, max) new QIntValidator(min, max)
//#define INT_TOP_VALIDATOR(max) new IntTopValidator(max)
//#define INT_BOTTOM_VALIDATOR(min) new IntBottomValidator(min)
#define DOUBLE_VALIDATOR new QDoubleValidator
//#define DOUBLE_RANGE_VALIDATOR(min, max) new QDoubleValidator(s)
//#define DOUBLE_TOP_VALIDATOR(max) new IntTopValidator(max)
//#define DOUBLE_BOTTOM_VALIDATOR(min) new IntBottomValidator(min)
#define BASIC_REGEXP_VALIDATOR(regexp) new QRegExpValidator(QRegExp(regexp))
#define REGEXP_VALIDATOR(regexp) new QRegExpValidator(regexp)
//#define URL_VALIDATOR new UrlValidator
//#define URL_VALIDATOR(protocol) new UrlValidator(protocol)

namespace DB
{
class DBCore;
}

namespace Params
{

class ParamsCore;

/** Provide an easy way to handle parameters.
  *
  * Parameters are simple values which will be manipulate graphically by the user.
  * For now, all type which can be converted in String via a QVariant are accepted
  * (this exclude the lists).
  *
  * \todo Handle numbers list
  *
  * A parameters has a unique name.
  *
  * The parameters may be stored in 3 different ways :
  * - In a database (then the name is the table name)
  * - In a QSettings instance
  * - In memory (no data persistance)
  *
  * TODO : explain the commit system
  *
  * \todo Validators
  **/

	class Parameterizable : public QObject
	{
		Q_OBJECT
	public:
		enum BackEnd																				///< How are stored the parameters?
		{
			DATABASE,																				///< In a database table
			SETTINGS,																				///< In a QSettings (Win -> register, *nix -> file)
			NONE,  																					///< Parameters  are not stored. Useful for tmp instances, or subclasses.
			UNSET
		};

		Parameterizable(Parameterizable::BackEnd backEnd = Parameterizable::UNSET, QString name = "");
		QVariant						getPublicParameter(const QString &key) const;
		QVariant						getPublicParameter(const QString &key, QVariant falloutValue) const;
		const QString&					paramsName() const;
		Parameterizable::BackEnd	paramsBackEnd() const;
		void registerToParamsCore(ParamsCore *paramsCore);
		void setParamsBackEnd(DB::DBCore *dbCore);
		void setParamsBackEnd(Parameterizable::BackEnd backEnd, DB::DBCore *dbCore = 0);

	protected slots:
		virtual void updatedValues(const QStringList &keys);

	signals:
		void commitedValues(const QStringList &keys);

	protected:
		bool setParamsName(const QString &name);
		bool commitParameters();
		QVariant getParameter(const QString &key) const;
		QVariant getParameter(const QString &key, QVariant falloutValue) const;
		void setParameter(const QString &key, QVariant value);
		void addValidator(QString key, QValidator*);
		void deleteValidators();

		virtual void defineParams()=0;																///< Use by subclasses to define parameters

		QHash<QString, QVariant> parameters;
		QHash<QString, QVariant> publicParameters;
		QHash<QString, QValidator*> paramsValidators;

	private:
		bool hasNameAndBackEnd() const;																///< Check if name and backend are set
		void copyParamsToPublic();
		void loadFromSettings();
		void loadDefaultParams();
		void loadFromCommandLine(ParamsCore *paramsCore);
		void loadFromDatabase();
		void applyCmdLineParameters(const QHash<QString, QVariant>& cmdLineParameters);
		QString _paramsName;
		BackEnd _paramsBackEnd;
		bool paramsLocked;
		DB::DBCore *_dbCore;
	};

}

#endif // PARAMETERIZABLE_HPP
