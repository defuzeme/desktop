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

#include "jsonparser.hpp"
#include <QDebug>

using namespace Network;

JsonParser::JsonParser()
{
}

QVariant	JsonParser::parse(const QByteArray& data) const
{
	QScriptValue	value;
	QScriptEngine	engine;
	value = engine.evaluate(QString("(") + QString(data) + ")");

	if (engine.hasUncaughtException())
		return QVariant();
	return partialParse(value);
}


QVariant	JsonParser::partialParse(const QScriptValue& value) const
{
	if (value.isArray())
	{
		QScriptValueIterator	it(value);
		QVariantList			list;

		while (it.hasNext())
		{
			it.next();
			if (!it.hasNext() && it.value().isNumber() && it.value().toInteger() == list.size())
				break;
			list << partialParse(it.value());
		}
		return list;
	}
	if (value.isObject())
	{
		QScriptValueIterator	it(value);
		QVariantMap				map;

		while (it.hasNext())
		{
			it.next();
			map[it.name()] = partialParse(it.value());
		}
		return map;
	}
	else
	{
		return value.toVariant();
	}
}

QByteArray	JsonParser::serialize(const QVariant& data) const
{
	return partialSerialize(data, 0);
}

QByteArray	JsonParser::partialSerialize(const QVariant& value, const short spaces) const
{
	QByteArray	out;

	if ( ! value.isValid() ) {
		out = "null";
	}
	else if (value.type() == QVariant::List || value.type() == QVariant::StringList)
	{
		const QVariantList &list = value.toList();
		QList<QByteArray> values;
		Q_FOREACH( const QVariant& var, list)
		{
			values << partialSerialize(var, spaces + 1);
		}
		out = "[\n" + indent(spaces + 1) + join( values, ",\n" + indent(spaces + 1)) + "\n" + indent(spaces) + "]";
	}
	else if (value.type() == QVariant::Map)
	{
		QVariantMap::const_iterator	it;

		out += "{\n";
		for(it = value.toMap().constBegin(); it != value.toMap().constEnd(); it++)
		{
			if (it != value.toMap().constBegin())
				out += ",\n";
			out += indent(spaces + 1) + sanitizeString(it.key()) + ": " + partialSerialize(it.value(), spaces + 1);
		}
		out += "\n" + indent(spaces) + "}";
	}
	else if (value.type() == QVariant::Bool)
	{
		out = (value.toBool() ? "true" : "false");
	}
	else if (value.type() == QVariant::String ||
			 value.type() == QVariant::DateTime ||
			 value.type() == QVariant::ByteArray)
	{
		out += sanitizeString(value.toString());
	}
	else
	{
		out += value.toString();
	}
	return out;
}

QByteArray	JsonParser::indent(const short spaces) const
{
	return QByteArray(spaces * 2, ' ');
}


QByteArray	JsonParser::join( const QList<QByteArray>& list, const QByteArray& sep ) const
{
	QByteArray res;
	Q_FOREACH( const QByteArray& i, list )
	{
		if ( !res.isEmpty() )
			res += sep;
		res += i;
	}
	return res;
}

QString		JsonParser::sanitizeString(const QString& input) const
{
	QString	str = input;

	str.replace( QLatin1String( "\\" ), QLatin1String( "\\\\" ) );
	// escape unicode chars
	QString result;
	const ushort* unicode = str.utf16();
	unsigned int i = 0;
	while ( unicode[ i ] ) {
		if ( unicode[ i ] < 128 ) {
			result.append( QChar( unicode[ i ] ) );
		}
		else {
			QString hexCode = QString::number( unicode[ i ], 16 ).rightJustified( 4, QLatin1Char('0') );
			result.append( QLatin1String ("\\u") ).append( hexCode );
		}
		++i;
	}
	str = result;
	str.replace( QLatin1String( "\"" ), QLatin1String( "\\\"" ) );
	str.replace( QLatin1String( "\b" ), QLatin1String( "\\b" ) );
	str.replace( QLatin1String( "\f" ), QLatin1String( "\\f" ) );
	str.replace( QLatin1String( "\n" ), QLatin1String( "\\n" ) );
	str.replace( QLatin1String( "\r" ), QLatin1String( "\\r" ) );
	str.replace( QLatin1String( "\t" ), QLatin1String( "\\t" ) );
	return QString( QLatin1String( "\"%1\"" ) ).arg( str );
}

void		JsonParser::test()
{
	JsonParser			p;
	QVariant			value;
	QByteArray			result;
	const QByteArray	json =
	"[\n"
	"  {\n"
	"    \"address\": {\n"
	"      \"city\": \"New York\",\n"
	"      \"main\": true,\n"
	"      \"postalCode\": \"10021\",\n"
	"      \"state\": \"NY\",\n"
	"      \"streetAddress\": \"21 2nd Street\"\n"
	"    },\n"
	"    \"age\": 25,\n"
	"    \"firstName\": \"John\",\n"
	"    \"lastName\": \"Smith\",\n"
	"    \"phoneNumber\": [\n"
	"      {\n"
	"        \"number\": \"212 555-1234\",\n"
	"        \"type\": \"home\"\n"
	"      },\n"
	"      {\n"
	"        \"number\": \"646 555-4567\",\n"
	"        \"type\": \"fax\"\n"
	"      }\n"
	"    ]\n"
	"  }\n"
	"]";

	value = p.parse(json);
	//qDebug() << value;
	result = p.serialize(value);
	//qDebug() << result;
	Q_ASSERT_X(result == json, "JsonParser::test", "serialization does not match original");
}
