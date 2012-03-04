![](http://defuze.me/images/logo.png)

defuze.me desktop application for Windows, Mac OS and Linux
===========================================================

This is the source code for the [defuze.me](http://defuze.me) desktop application, to be used with the [defuze.me web server](https://github.com/defuzeme/server) and the [defuze.me mobile application](https://github.com/defuzeme/android).

What is it for?
---------------

The application provides audio-player functionalities and additional radio-station oriented features.

* Common audio formats playback (mp3, wma, oga, aac, wav, flac, a4c and more)
* Balance and volume mixers
* Normal and automatically generated playlists
* Events/shows scheduler
* Last.fm integration

Moreover, the player and the playqueue can be managed remotely by either the website or the mobile application.

How to compile/run it
---------------------

Core technologies are Qt and ffmpeg.

### Windows

You will need

* Windows Xp or later
* Qt 4.7 (for minGW) or later (including the core/gui/sql/network/script/declarative/svg/xml/xmlpatterns modules)
* QtMobility 1.1 or later

You can build the app using QtCreator or `qmake` directly. If you use QtCreator, do not forget to run qmake before compiling anyway.
In both cases, the target directory has to be `client/your-build-directory/your-build-config/`.
Before to be able to run the app, you may have to copy

* DLLs libraries from `client/client-build-desktop/{debug|release}/*.dll` into the target directory
* DLLs for your Qt version into the target directory (furnished DLLs are for Qt 4.8.0)
* the resources and translations folders (`client/client-build-desktop/resources` and `client/client-build-desktop/translations`) into the build directory

You can now run `client.exe`, using `client/your-build-directory/` as working directory.

### Linux

You will need

* Almost any linux (following commands are given for Ubuntu 64bits)
* Qt 4.7 or later
* QtMobility 1.1 or later
* g++, FFmpeg, libTag

Here is a quick install line for Ubuntu:

    $ apt-get install g++ qtcreator qtmobility-dev libbz2-dev libvorbis-dev libavformat-dev libavcodec-dev libavutil-dev libfaac-dev libmp3lame-dev libtag1-dev zlib1g-dev

You can build the app using QtCreator or `qmake` directly.
In both cases, the target directory has to be `client-build-desktop/.`

    $ cd client-build-desktop
    $ qmake ../client/client.pro
    $ make

You can now run `client_x86_64` from the `client-build-desktop/.` directory.

    $ ./client_x86_64

Lastfm API credentials
----------------------

You may want to change the API key/secret. You can do so by modifying `client/resources/qml/lastfm/LastfmAPICredentials.js`.

Does it work?
-------------

The application is still in beta stage. Consequently it have severe known issues, including crashes, and data loss situations.
It might be use for testing purpose, but is not ready for production yet.

License
-------

This program is free software covered by the [GNU LGPLv3 license](http://defuze.me/en/license).
