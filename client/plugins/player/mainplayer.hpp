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

#ifndef MAINPLAYER_HPP
#define MAINPLAYER_HPP

#include "staticplugin.hpp"
#include "audiodecoder.hpp"
#include "playqueue.hpp"
#include "audiotrack.hpp"
#include "audioplayerwidget.hpp"
#include "audioplayer.hpp"
#include "audiomixer.hpp"
#include "audiooutputdevice.hpp"

namespace Player
{
	class MainPlayerWidget;
	class MainPlayer :public QObject, public StaticPlugin
	{
		Q_OBJECT
	public:
		MainPlayer(QString playerName);
		~MainPlayer();
		void		init();
		void		setPlayers(AudioPlayerWidget *A, AudioPlayerWidget *B);
		bool		play();
		bool		pause();
		bool		isPlaying() const;
		AudioPlayer*	firstPlayer();
		AudioPlayer*	secondPlayer();
	signals:
		void		played();
		void		paused();
		void		stopped();

		void		lplay(int uid, int totalTime);
		void		lpause(int uid, qint64 elapsedTime, int totalTime);
		void		lstop(int uid, qint64 elapsedTime, int totalTime);
		void		lnext(int oldUid, int newUid, qint64 oldElapsedTime, int oldTotalTime, int newTotalTime);
	public slots:
		bool		crossfadeNext();
		bool		stop();
	private slots:
		void		crossfadeFinished();
		bool		loadTracks();
		bool		playA();
		bool		playB();
		void		newElem(Queue::Queueable*elem);
		void		removeElem(Queue::Queueable *elem);
		void		finished();
	private:
		void				emitPlayPause();

		AudioPlayer			*playingPlayer;
		AudioPlayerWidget	*playerWidgetA;
		AudioPlayerWidget	*playerWidgetB;

		AudioPlayer			*playerA;
		AudioPlayer			*playerB;
		Audio::AudioMixer	*mixerA;
		Audio::AudioMixer	*mixerB;
		Audio::AudioDecoder	*decoderA;
		Audio::AudioDecoder	*decoderB;

		QString				_playerName;

		Queue::PlayQueue	*playQueue;
		MainPlayerWidget	*widget;

		Gui::Module			*uiModule;

		Audio::AudioOutputDevice* outputDeviceA;
		Audio::AudioOutputDevice* outputDeviceB;

		bool				isCrossfading;
	};
}
#endif // MAINPLAYER_HPP
