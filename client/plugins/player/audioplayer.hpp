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

#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

#include "staticplugin.hpp"
#include "audiodecoder.hpp"
#include "playqueue.hpp"
#include "audiotrack.hpp"
#include "audioplayerwidget.hpp"

namespace Player
{
	class AudioPlayer :public Audio::AudioIO
	{
		Q_OBJECT
	public:
		AudioPlayer(QString playerName);
		AudioPlayer(QString playerName, AudioPlayerWidget *widget, Audio::AudioDecoder *decoder, Audio::AudioOutputDevice *outputDevice);
		~AudioPlayer();
		void		init();
		bool		loadTrack(Queue::Queueable *elem);      ///< Load a Track into the player
		bool		isPlaying() const;                      ///< Return if the player is playing or not

		qint64						readData(char *data, qint64 maxlen, AudioIO *from);
		Audio::AudioIO				*getAudioInput(const Audio::AudioIO *output);
		const Audio::AudioIO				*getOutputDevice(const AudioIO* input) const;
		int			getCurrentTrackUid() const;
		qint64		getPlayingPosition() const;
		int			getTotalDuration() const;
		Player::PlayerWidgetState		state() const;
	signals:
		void		canIHasANewTrack();                     ///< Emited when the player wants a new track
		void		trackHasFinishedPlaying();              ///< Emited when the player reach the end of the track
		void		almostFinished();						///< Emited few seconds before end to start crossfading
		void		paused();
		void		played();
		void		stopped();
		void		finished();

	public slots:
		void		unloadTrack();
		void		play();
		void		stop();

	private slots:
		void		updatePosition();
		void		seek(int position);

	protected:
		qint64						writeData(const char *data, qint64 maxlen);
		qint64						readData(char *data, qint64 maxlen);

	private:
		int			cueAt();								///< Returns when to cue the track (start crossfade)

		bool						_updatePosition;        ///< Bool that store if we can update the position or not
		int							previousPosition;

		QTimer						*timer;

		QString						_playerName;            ///< The name of the player (Displayed on the UI)

		AudioPlayerWidget			*widget;                ///< The player widget linked to the player
		Audio::AudioDecoder			*decoder;               ///< The decoder linked to the player
		Library::AudioTrack			*currentTrack;          ///< The current track loaded in the player, 0 if no track is loaded
		Audio::AudioOutputDevice	*outputDevice;          ///< The Audio Device on which the data of this player are outputed

		Gui::Module					*uiModule;

	};
}
#endif // AUDIOPLAYER_HPP
