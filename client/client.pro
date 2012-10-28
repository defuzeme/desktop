QT += core gui widgets sql network script multimedia svg xml xmlpatterns concurrent opengl quick1

!win32 {
	ARCHI = $$system(uname -m)
	TARGET = client_$${ARCHI}

	system("mkdir -p $${OUT_PWD}/$${TARGET}.app/Contents/resources")
	system("mkdir -p $${OUT_PWD}/$${TARGET}.app/Contents/translations")
	system("chmod +x ../helpers/*.sh")
	system("../helpers/compile_resources.sh")
	system("../helpers/compile_translations.sh")
}
else {
	TARGET = Defuze.me
	RC_FILE = defuze.me.rc

	system(cd ../helpers && compile_resources.bat)
	system(cd ../helpers && compile_translations.bat)
}

TEMPLATE = app

LIBS +=  -L../client/libraries/ffmpeg/libs -lavformat -lavcodec -lavutil

unix:!macx {
	LIBS += -lbz2 -lm -lfaac -lmp3lame -lvorbis -lvorbisenc -lz -ltag
}

macx {
	LIBS += -L../client/libraries/ffmpeg/libs/macx_$${ARCHI} -lbz2 -lm -lfaac -lmp3lame -lvorbis -lvorbisenc -lz
	LIBS += -L../client/libraries/taglib/libs/macx_$${ARCHI} -ltag
}

win32 {
	LIBS += -L../client/libraries/taglib/libs/ -ltag.dll
}

SOURCES += \
	main.cpp \
	starter.cpp \
	core/core.cpp \
	core/cores.cpp \
	core/gui/guicore.cpp \
	core/db/dbcore.cpp \
	core/db/migration.cpp \
	core/db/location.cpp \
	core/params/parameterizable.cpp \
	utilities/logger.cpp \
	utilities/exception.cpp \
	core/gui/qtwin.cpp \
	core/gui/main_window/mainwindow.cpp \
	core/gui/main_window/tabwidget.cpp \
	core/db/migrations/configurations_migration.cpp \
	core/db/migrations/settings_migration.cpp \
	core/network/networkcore.cpp \
	core/network/parser.cpp \
	core/network/parsers/jsonparser.cpp \
	core/network/api/authenticator.cpp \
	core/network/remote/doorman.cpp \
	core/network/remote/remotesock.cpp \
	core/network/remote/remoteevent.cpp \
	core/audio/audiocore.cpp \
	core/params/paramscore.cpp \
	core/network/api/loginview.cpp \
	core/network/remote/authview.cpp \
	core/gui/uiconfiguration.cpp \
	core/gui/main_window/tab.cpp \
	core/gui/grid_element/gridelement.cpp \
	core/gui/grid_element/gridelementmovewidget.cpp \
	core/audio/audiodecoder.cpp \
	core/gui/module/module.cpp\
	plugins/queue/playqueue.cpp \
	plugins/queue/queueable.cpp \
	plugins/queue/container.cpp \
	plugins/queue/playqueuewidget.cpp \
	core/gui/module/modulewidget.cpp \
	core/gui/module/modulefactory.cpp \
	plugins/plugin.cpp \
	plugins/plugins.cpp \
	plugins/staticplugin.cpp \
	core/db/migrations/audio_tracks_migration.cpp \
	plugins/library/library.cpp \
	plugins/library/librarywidget.cpp \
	plugins/library/librarymodel.cpp \
	plugins/queue/queuebreak.cpp \
	plugins/remote/remotecontrol.cpp \
	plugins/player/audioplayerwidget.cpp \
	plugins/mixer/audioslice.cpp \
	core/gui/main_window/gridwidget.cpp \
	core/audio/audiomixer.cpp \
	core/audio/audioio.cpp \
	core/audio/audioencoder.cpp \
	plugins/library/source.cpp \
	core/db/migrations/sources_migration.cpp \
	plugins/library/audiotrack.cpp \
	plugins/queue/queuetrack.cpp \
	utilities/defuzemeapplication.cpp \
	plugins/player/mainplayer.cpp \
	plugins/player/mainplayerwidget.cpp \
	utilities/locker.cpp \
	plugins/player/audioplayer.cpp \
	plugins/web_service/servicesync.cpp \
	core/gui/grid_element/droplocation.cpp \
	plugins/web_service/servicerequest.cpp \
	plugins/mixer/audiomixerplugin.cpp \
	plugins/mixer/audiomixerwidget.cpp \
	plugins/mixer/audioslicewidget.cpp \
	plugins/mixer/audioeffectvolumewidget.cpp \
	plugins/mixer/audioeffectwidget.cpp \
	core/audio/audioeffect.cpp \
	core/audio/audioeffectvolume.cpp \
	plugins/mixer/vumeter.cpp \
	plugins/mixer/vumeterdisplay.cpp \
	plugins/library/parameters/sourcesparams.cpp \
	core/gui/parameters/parameterspage.cpp \
	core/gui/parameters/parameterscategory.cpp \
	core/gui/parameters/parametersmodule.cpp \
	plugins/mixer/audioeffectbalancewidget.cpp \
	core/audio/audioeffectbalance.cpp \
	core/gui/main_window/cornerwidget.cpp \
	core/network/websocket/websocket.cpp \
	core/network/websocket/webframe.cpp \
	core/audio/audiooutputdevicethread.cpp \
	core/audio/audiooutputdevice.cpp \
	utilities/thread.cpp \
	plugins/notification/status.cpp \
	plugins/notification/statuswidget.cpp \
	plugins/notification/message.cpp \
	core/db/migrations/playlists_migration.cpp \
	plugins/scheduler/scheduler.cpp \
	plugins/scheduler/schedulermodel.cpp \
	plugins/scheduler/schedulerwidget.cpp \
	core/gui/about.cpp \
	utilities/ui/clearablelineedit.cpp \
	plugins/queue/playqueuelistview.cpp \
	plugins/queue/controlswidget.cpp \
	plugins/lastfm/lastfmplugin.cpp \
	plugins/lastfm/lastfmwidget.cpp \
	core/db/migrations/scheduler_migration.cpp \
	plugins/scheduler/eventmodel.cpp \
	plugins/queue/playqueuedelegate.cpp \
	plugins/scheduler/eventwidget.cpp \
	plugins/scheduler/eventcolor.cpp \
	utilities/models/treeitem.cpp \
	utilities/models/treemodel.cpp \
	plugins/library/libraryitem.cpp \
	plugins/library/librarydelegate.cpp \
	plugins/library/audiotrackloader.cpp \
	plugins/scheduler/eventplaylist.cpp \
	plugins/scheduler/eventplaylistitem.cpp \
	plugins/lists/listsplugin.cpp \
	plugins/lists/listswidget.cpp \
    plugins/lists/listsdelegate.cpp \
    plugins/lists/listsmodel.cpp \
    plugins/lists/listsitem.cpp \
    plugins/lists/liststreeview.cpp \
    utilities/models/editabletreeitem.cpp \
    utilities/models/editabletreemodel.cpp \
    plugins/player/playerlabel.cpp

HEADERS  += \
	main.hpp \
	starter.hpp \
	core/core.hpp \
	core/cores.hpp \
	core/gui/guicore.hpp \
	core/db/dbcore.hpp \
	core/db/migration.hpp \
	core/db/location.hpp \
	core/db/migration_list.hpp \
	core/params/parameterizable.hpp \
	utilities/singleton.hpp \
	utilities/logger.hpp \
	utilities/exception.hpp \
	core/gui/qtwin.h \
	core/gui/main_window/mainwindow.hpp \
	core/gui/main_window/tabwidget.hpp \
	core/network/networkcore.hpp \
	core/network/parser.hpp \
	core/network/parsers/jsonparser.hpp \
	core/network/api/authenticator.hpp \
	core/network/remote/doorman.hpp \
	core/network/remote/remotesock.hpp \
	core/network/remote/remoteevent.hpp \
	core/audio/audiocore.hpp \
	core/params/paramscore.hpp \
	core/network/api/loginview.hpp \
	core/network/remote/authview.hpp \
	core/gui/uiconfiguration.hpp \
	core/gui/main_window/tab.hpp \
	core/gui/grid_element/gridelement.hpp \
	core/gui/grid_element/gridelementmovewidget.hpp \
	core/audio/audiodecoder.hpp \
	core/gui/module/module.hpp\
	plugins/queue/playqueue.hpp \
	plugins/queue/queueable.hpp \
	plugins/queue/container.hpp \
	plugins/queue/playqueuewidget.hpp \
	core/gui/module/modulewidget.hpp \
	core/gui/module/modulefactory.hpp \
	plugins/plugin.hpp \
	plugins/plugins.hpp \
	plugins/staticplugin.hpp \
	plugins/library/library.hpp \
	plugins/library/librarymodel.hpp \
	plugins/queue/queuebreak.hpp \
	plugins/remote/remotecontrol.hpp \
	plugins/player/audioplayerwidget.hpp \
	plugins/mixer/audioslice.hpp \
	core/gui/main_window/gridwidget.hpp \
	core/audio/audiomixer.hpp \
	core/audio/audioio.hpp \
	core/audio/audioencoder.hpp \
	plugins/library/source.hpp \
	plugins/library/audiotrack.hpp \
	plugins/queue/queuetrack.hpp \
	utilities/defuzemeapplication.hpp \
	plugins/player/mainplayer.hpp \
	plugins/player/mainplayerwidget.hpp \
	utilities/locker.hpp \
	plugins/player/audioplayer.hpp \
	plugins/web_service/servicesync.hpp \
	core/gui/grid_element/droplocation.hpp \
	plugins/web_service/servicerequest.hpp \
	plugins/mixer/audiomixerplugin.hpp \
	plugins/mixer/audiomixerwidget.hpp \
	plugins/mixer/audioslicewidget.hpp \
	plugins/mixer/audioeffectvolumewidget.hpp \
	plugins/mixer/audioeffectwidget.hpp \
	core/audio/audioeffect.hpp \
	core/audio/audioeffectvolume.hpp \
	plugins/mixer/vumeter.hpp \
	plugins/mixer/vumeterdisplay.hpp \
	plugins/library/parameters/sourcesparams.hpp \
	core/gui/parameters/parameterspage.hpp \
	core/gui/parameters/parameterscategory.hpp \
	core/gui/parameters/parametersmodule.hpp \
	plugins/mixer/audioeffectbalancewidget.hpp \
	core/audio/audioeffectbalance.hpp \
	core/gui/main_window/cornerwidget.hpp \
	core/network/websocket/websocket.hpp \
	core/network/websocket/webframe.hpp \
	core/audio/audiooutputdevicethread.hpp \
	core/audio/audiooutputdevice.hpp \
	utilities/thread.hpp \
	plugins/notification/status.hpp \
	plugins/notification/message.hpp \
	plugins/notification/statuswidget.hpp \
	plugins/library/librarywidget.hpp \
	plugins/scheduler/scheduler.hpp \
	plugins/scheduler/schedulermodel.hpp \
	plugins/scheduler/schedulerwidget.hpp \
	core/gui/about.hpp \
	utilities/ui/clearablelineedit.hpp \
	plugins/queue/playqueuelistview.hpp \
	plugins/queue/controlswidget.hpp \
	plugins/lastfm/lastfmplugin.hpp \
	plugins/lastfm/lastfmwidget.hpp \
	plugins/scheduler/eventmodel.hpp \
	plugins/queue/playqueuedelegate.hpp \
	plugins/scheduler/eventwidget.hpp \
	plugins/scheduler/eventcolor.hpp \
	utilities/models/treeitem.hpp \
	utilities/models/treemodel.hpp \
	plugins/library/libraryitem.hpp \
	plugins/library/librarydelegate.hpp \
	plugins/library/audiotrackloader.hpp \
	plugins/scheduler/eventplaylist.hpp \
	plugins/scheduler/eventplaylistitem.hpp \
	plugins/lists/listsplugin.hpp \
	plugins/lists/listswidget.hpp \
    plugins/lists/listsdelegate.hpp \
    plugins/lists/listsmodel.hpp \
    plugins/lists/listsitem.hpp \
    plugins/lists/liststreeview.hpp \
    utilities/models/editabletreeitem.hpp \
    utilities/models/editabletreemodel.hpp \
    plugins/player/playerlabel.hpp

INCLUDEPATH += \
	core \
	core/gui \
	core/gui/main_window \
	core/gui/grid_element \
	core/gui/module \
	core/gui/parameters \
	core/db \
	core/db/migrations \
	core/params \
	core/network \
	core/network/parsers \
	core/network/api \
	core/network/remote \
	core/network/websocket \
	core/audio \
	utilities \
	utilities/validators \
	utilities/ui \
	utilities/models \
	plugins \
	plugins/queue \
	plugins/library \
	plugins/playlists \
	plugins/lists \
	plugins/scheduler \
	plugins/library/parameters \
	plugins/remote \
	plugins/player \
	plugins/mixer \
	plugins/web_service \
	plugins/notification \
	plugins/lastfm \
	libraries/ffmpeg/includes \
	libraries/taglib/includes


OTHER_FILES += \
	resources/starter.qrc \
	resources/about.qrc \
	resources/lastfm.qrc \
	plugins/lastfm/LastfmView.qml \
	plugins/lastfm/YourCharts.qml \
	plugins/lastfm/PageTitle.qml \
	plugins/lastfm/GlobalCharts.qml \
	plugins/lastfm/MultiButton.qml \
	plugins/lastfm/RecentTracks.qml \
	plugins/lastfm/Element.qml \
	plugins/lastfm/VisualCount.qml \
	plugins/lastfm/helper.js \
	plugins/lastfm/LastfmPage.qml \
	plugins/lastfm/globals.js \
	plugins/lastfm/OptionsBar.qml \
	plugins/lastfm/ExtendedOptionsBar.qml \
	plugins/lastfm/LoginPage.qml \
	plugins/lastfm/LineEdit.qml \
	plugins/lastfm/login_helpers.js \
	plugins/lastfm/WhatsNext.qml \
    plugins/lastfm/LastfmAPICredentials.js

FORMS += \
	core/network/api/loginview.ui \
	core/network/remote/authview.ui \
	core/gui/grid_element/gridelement.ui \
	plugins/queue/playqueuewidget.ui \
	plugins/library/librarywidget.ui \
	plugins/player/audioplayerwidget.ui \
	core/gui/main_window/gridwidget.ui \
	plugins/player/mainplayerwidget.ui \
	core/gui/grid_element/droplocation.ui \
	plugins/mixer/audiomixerwidget.ui \
	plugins/mixer/audioslicewidget.ui \
	plugins/mixer/audioeffectvolumewidget.ui \
	plugins/mixer/vumeter.ui \
	plugins/library/parameters/sourcesparams.ui \
	plugins/scheduler/eventwidget.ui \
	plugins/scheduler/schedulerwidget.ui \
	core/gui/parameters/parameterscategory.ui \
	core/gui/parameters/parametersmodule.ui \
	plugins/mixer/audioeffectbalancewidget.ui \
	core/gui/main_window/cornerwidget.ui \
	plugins/notification/statuswidget.ui \
	plugins/notification/message.ui \
	core/gui/about.ui \
	plugins/queue/controlswidget.ui \
	plugins/lastfm/lastfmwidget.ui \
	plugins/lists/listswidget.ui

RESOURCES += \
	resources/icons.qrc

TRANSLATIONS = translations/defuzeme_fr.ts


