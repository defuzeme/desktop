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

#ifndef AUDIODECODER_HPP
#define AUDIODECODER_HPP

#include <QIODevice>
#include <QFile>
#include <QAudioOutput>
#include <QObject>
#include <QTimer>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QMutex>

#include "ffmpeg.hpp"
#include "audiocore.hpp"
#include "audioio.hpp"

namespace Player
{
	class AudioPlayer;
}
namespace Audio
{
	class AudioCore;

	enum FileFormat
	{
		FormatNoAudio = -1,
		FormatMP3,
		FormatWMA,
		FormatOGG,
		FormatWAV,
		FormatFLAC,
		FormatAAC,
		FormatUnknown
	};

	/** This is the class which decode the Audio.
	  ** It use FFMPEG to decode any audio Format.
	  ** This class must be instantiated by AudioCore::newAudioDecoder(...)
	  **/

	class AudioDecoder : public QObject
	{
		friend AudioDecoder* AudioCore::newAudioDecoder(QString playerName);

		Q_OBJECT
	public:
		enum	DecoderState
		{
			BadFormat = -2,
			NoFile = -1,
			Iddle,
			Decoding,
			Stopped
		};

		~AudioDecoder();

		void						openFile(QString path);						///< Open the file
		void						closeFile();

		void						start();									///< Start the audio decoder
		void						stop();										///< Stop the audio decoder

		void						setPlayingPosition(quint64 position);		///< seek the position (in bytes)
		qint64						getPlayingPosition() const;           		///< Return playingPosition
		QAudioFormat&				getFormat();
		int							bytesToSeconds(int bytes) const;

		int							duration() const;							///< Return the current audiotrack duration (sec, ffmpeg value)
		DecoderState				state() const;								///< Return the Audio state of the decoder (see AudioState Enum)
		bool						isInit() const;								///< Return if the decoder is initializated

		static FileFormat			getFileFormat(const QString& path);			///< Static Method to get the format of an audiotrack using ffmpeg
		void						run();

		qint64						bytesAvailable() const;

		void						setPlayer(const Player::AudioPlayer* player);
		bool						isPlayerPlaying() const;
		qint64						readDecodedFrame(char *data, qint64 maxlen);
	signals:
		void						decodingFinished();                         ///< Emited when the decoder finished decoding the track

	private:
		AudioDecoder(QString, AudioCore *audioCore);

		QAudioFormat				format;                                     ///< The audio format (after decoding) of the track currently loaded
		AudioCore					*_audioCore;								///< The Audio Core

		AVFormatContext				*aFormatCtx;								///< (FFMPEG) Audio Format Context
		int							audioStream;								///< (FFMPEG) Which stream is the audio stream inside the file
		AVCodecContext				*aCodecCtx;									///< (FFMPEG) Audio Codec Context
		AVCodec						*aCodec;									///< (FFMPEG) Audio Codec for the current file
		AVPacket					packet;										///< (FFMPEG) Audio Packet which store an encoded audio frame
		short						*audioBuf;									///< Buffer which store a decoded audio frame
		int							out_size;									///< Size of the decoded frame (audioBuf) in byte

		bool						init;                                       ///< Store if the player is initialized
		DecoderState				_state;                                     ///< State of the decoder, see enum DecoderState
		qint64						playingPosition;							///< Current playing position in the buffer (decodedBuffer) in byte
		qint64						decodingPosition;							///< Current decoding position in the buffer (decodedBuffer) in byte
		QBuffer						decodedBuffer;								///< Buffer that stored the decoded frame for the current file
		QMutex					    mutex;                                      ///< Mutex to lock the buffer (decodedBuffer) so we can't write and read at the same time

		const Player::AudioPlayer*	_player;				                    ///< The player linked to this decoder

		void						initCodec();                                ///< Init the codec (ffmpeg) for the newly loaded track
		void						initVars();                                 ///< Init some variables...
		void						setFormat();								///< Set the correct audio format to the audio output
		void						decodeFrame();								///< Method called in the thread to write audio frame to the audio output
	};
}

#endif // AUDIODECODER_HPP
