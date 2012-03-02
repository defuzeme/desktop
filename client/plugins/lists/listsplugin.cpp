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

#include "listsplugin.hpp"

using namespace Lists;

ListsPlugin::ListsPlugin()
{
}

ListsPlugin::~ListsPlugin()
{
}

void ListsPlugin::init()
{
	widget = new ListsWidget(this);
	model = new ListsModel(this);
	widget->getTreeViewWidget()->setModel(model);
	Gui::Module *listsModule = Gui::ModuleFactory::create("lists", QPoint(0, 1), widget);
	listsModule->submitForDisplay();
}

void ListsPlugin::aboutToQuit()
{
	delete model;
	delete widget;
}

ListsWidget *ListsPlugin::getWidget() const
{
	return widget;
}

ListsModel *ListsPlugin::getModel() const
{
	return model;
}

QMap<int, QString> ListsPlugin::getPlaylists(bool dynamic)
{
	QSqlQuery query;
	QMap<int, QString> listsMap;

	query.prepare("SELECT id, name FROM playlists WHERE is_dynamic = :dynamic");
	query.bindValue(":dynamic", dynamic);
	query.exec();
	while (query.next())
		listsMap[query.value(0).toInt()] = query.value(1).toString();
	return listsMap;
}

QMap<int, QString> ListsPlugin::getNormalPlaylists()
{
	return getPlaylists(false);
}

QMap<int, QString> ListsPlugin::getDynamicPlaylists()
{
	return getPlaylists(true);
}

QList<int> ListsPlugin::getTracksForPlaylist(int id)
{
	QSqlQuery query;

	query.prepare("SELECT is_dynamic, definition FROM playlists WHERE id = :id");
	query.bindValue(":id", id);
	query.exec();

	QList<int> tracksList;
	if (query.next())
	{
		if (query.value(0).toBool())
		{
			QVariantMap data = Network::JsonParser().parse(query.value(1).toByteArray()).toMap();

			QList<QVariant> separators = data["separators"].toList();
			QStringList rightValues;
			QStringList whereStrList;
			int j = 0;
			foreach (QVariant separatorRawContents, separators)
			{
				QStringList leftValues;
				QVariantMap serapatorContents = separatorRawContents.toMap();
				QString subKind = serapatorContents.keys().first();
				QList<QVariant> params = serapatorContents[subKind].toList();
				foreach (QVariant param, params)
				{
					if (subKind == "artists")
						subKind = "album_artist";
					else if (subKind == "albums")
						subKind = "album";
					else if (subKind == "genres")
						subKind = "genre";
					leftValues << subKind;
					rightValues << param.toString();
				}
				QStringList wheres;
				for (int i = 0 ; i < leftValues.size() ; ++i)
				{
					wheres << (leftValues.at(i) + " = :ph" + QString::number(j));
					j++;
				}
				whereStrList << "(" +  wheres.join(" OR ") + ")";
			}

			QString whereStr = whereStrList.join(" AND ");

			QSqlQuery subQuery;
			subQuery.prepare("SELECT id FROM audio_tracks WHERE " + whereStr);
			for (int i = 0 ; i < rightValues.size() ; ++i)
			{
				subQuery.bindValue(":ph" + QString::number(i), rightValues.at(i));
			}
			subQuery.exec();
			while (subQuery.next())
			{
				tracksList << subQuery.value(0).toInt();
			}
		}
		else
		{
			QList<QVariant> data = Network::JsonParser().parse(query.value(1).toByteArray()).toList();
			foreach (QVariant trackId, data)
				tracksList << trackId.toInt();
		}
	}
	return tracksList;
}

QList<int> ListsPlugin::getTracksForNormalPlaylist(int id)
{
	return getTracksForPlaylist(id);
}
