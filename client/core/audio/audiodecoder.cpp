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

#include <QDebug>
#include <QMutexLocker>
#include "audiodecoder.hpp"
#include "exception.hpp"
#include "ffmpeg.hpp"
#include "audiocore.hpp"
#include "audiomixer.hpp"
#include "audioplayer.hpp"
#include "status.hpp"

using namespace Audio;

AudioDecoder::AudioDecoder(QString, AudioCore *audioCore)
{
	_audioCore = audioCore;
	_player = 0;
	initVars();
}

AudioDecoder::~AudioDecoder()
{
	closeFile();
}


void	AudioDecoder::openFile(QString file)
{
	closeFile();
	if (av_open_input_file(&aFormatCtx, file.toLocal8Bit().data(), NULL, 0, NULL) != 0)
		throw_exception(0x01, tr("Unable to open the audio file: %1").arg(file));
	if (av_find_stream_info(aFormatCtx) < 0)
		throw_exception(0x02, tr("Unable to find stream info in the file: %1").arg(file));

	for(unsigned int i = 0; i < aFormatCtx->nb_streams; i++)
		if(aFormatCtx->streams[i]->codec->codec_type == 1 && audioStream < 0)
			audioStream = i;

	if (audioStream == -1)
		throw_exception(0x03, tr("Unable to open the audio stream from: %1").arg(file));

	initCodec();
	setFormat();

	_state = init? Iddle : BadFormat;
}

void	AudioDecoder::closeFile()
{
	if (state() == Decoding)
		stop();
	if(aCodecCtx)
		avcodec_close(aCodecCtx);
	if(aFormatCtx)
		av_close_input_file(aFormatCtx);
	if (decodedBuffer.isOpen())
		decodedBuffer.close();
	initVars();
}

void	AudioDecoder::initVars()
{
	_state = NoFile;
	aFormatCtx = 0;
	aCodecCtx = 0;
	aCodec = 0;
	out_size = 0;
	packet.size = 0;
	audioStream = -1;
	playingPosition = 0;
	decodingPosition = 0;
	decodedBuffer.open(QBuffer::ReadWrite);
	init = false;
}

void	AudioDecoder::initCodec()
{
	aCodecCtx=aFormatCtx->streams[audioStream]->codec;
	if (!aCodecCtx->sample_rate)
		aCodecCtx->sample_rate = 44100;
	if (!aCodecCtx->channels)
		aCodecCtx->channels = 2;

	aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
	if(aCodec == 0)
		throw_exception(0x04, tr("Unsupported audio codec"));
	if(avcodec_open(aCodecCtx, aCodec) < 0)
		throw_exception(0x05, tr("Can't open codec"));
	init = true;
}

void	AudioDecoder::setFormat()
{
	format.setFrequency(aCodecCtx->sample_rate);
	format.setChannels(aCodecCtx->channels);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);
}

bool	AudioDecoder::isInit() const
{
	return init;
}

int	min(int a, int b)
{
	return (a < b ? a : b);
}

void AudioDecoder::decodeFrame()
{
	QMutexLocker	locker(&mutex);
	int len = 0;
	if (init && state() == Decoding)
	{
		if (out_size > 0)
		{
			decodedBuffer.write((char*)audioBuf, out_size);
			decodingPosition += out_size;
			out_size = 0;
		}
		else if (packet.size > 0)
		{
			out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
			len = avcodec_decode_audio3(aCodecCtx, audioBuf, &out_size, &packet);
			if (len < 0)
			{
				packet.size = 0;
				out_size = 0;
				qDebug() << "Unable to decode audio packet";
				return;
			}
			else
			{
				packet.size -= len;
				packet.data += len;
			}
		}
		else if (av_read_frame(aFormatCtx, &packet) >= 0)
		{
		}
		else
		{
			qDebug() << "decoder stop decoding, buffer size: " << decodedBuffer.size();
			//Notification::Status::gMessage(tr("Decoding finished: %1 MB").arg(decodedBuffer.size()/1024/1024), Notification::OK);
			_state = Stopped;
			emit decodingFinished();
		}
	}
}

void	AudioDecoder::stop()
{
	QMutexLocker	locker(&mutex);
	if (init)
		_state = Stopped;
}

void	AudioDecoder::start()
{
	QMutexLocker	locker(&mutex);
	if (init && (state() == Stopped || state() == Iddle))
	{
		if (state() == Iddle)
			audioBuf = (short*) av_malloc((AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE)* sizeof(*audioBuf));
		_state = Decoding;
		qDebug() << "decoder start decoding";
	}
}

AudioDecoder::DecoderState	AudioDecoder::state() const
{
	return _state;
}

int		AudioDecoder::duration() const
{
	if (!aFormatCtx && state() == NoFile)
		return 0;
	return aFormatCtx->duration / AV_TIME_BASE;
}

int		AudioDecoder::bytesToSeconds(int bytes) const
{
	return (bytes/ aCodecCtx->sample_rate / 2 / aCodecCtx->channels);
}

FileFormat	AudioDecoder::getFileFormat(const QString& path)
{
	AVFormatContext *afctx;
	FileFormat f;
	if (av_open_input_file(&afctx, path.toStdString().c_str(), NULL, 0, NULL) != 0)
		return FormatNoAudio;
	av_find_stream_info(afctx);
	if (QString(afctx->iformat->name) == "mp3")
		f = FormatMP3;
	else if (QString(afctx->iformat->name) == "asf")
		f = FormatWMA;
	else if (QString(afctx->iformat->name) == "ogg")
		f = FormatOGG;
	else if (QString(afctx->iformat->name) == "wav")
		f = FormatWAV;
	else if (QString(afctx->iformat->name) == "flac")
		f = FormatFLAC;
	else if (QString(afctx->iformat->name) == "mov,mp4,m4a,3gp,3g2,mj2")
		f = FormatAAC;
	else
		f = FormatUnknown;
	av_close_input_file(afctx);
	return f;
}

void	AudioDecoder::run()
{
	if(state() != NoFile)
	{
		if (state() == Decoding)
			decodeFrame();
	}
}

qint64	AudioDecoder::readDecodedFrame(char *data, qint64 maxlen)
{
	if (decodedBuffer.isOpen())
	{
		mutex.lock();
		decodedBuffer.seek(playingPosition);
		qint64 i = decodedBuffer.read(data, maxlen);
		playingPosition += i;
//		qDebug() << "readData(" << maxlen << ") = " << i;
		decodedBuffer.seek(decodingPosition);
		mutex.unlock();
		return i;
	}
	else
		return 0;
}

qint64 AudioDecoder::bytesAvailable() const
{
	if (decodedBuffer.isOpen())
		return decodedBuffer.size(); //+ QIODevice::bytesAvailable();
	return 0;
}

QAudioFormat& AudioDecoder::getFormat()
{
	return format;
}

void	AudioDecoder::setPlayer(const Player::AudioPlayer *player)
{
	_player = player;
}

bool	AudioDecoder::isPlayerPlaying() const
{
	if (_player && _player->isPlaying())
		return true;
	return false;
}

void	AudioDecoder::setPlayingPosition(quint64 position)
{
	playingPosition = position;
}

qint64  AudioDecoder::getPlayingPosition() const
{
	if (state() != NoFile && state() != BadFormat)
		return bytesToSeconds(playingPosition);
	return 0;
}

/*
  **
  ** Old functions
  **

int		AudioDecoder::currentPosition() const
{
	int pos = 0;
	if (state() != NoFile)
	{
		if (_audioOutput->getType() == AudioMixerType)
			pos = bytesToSeconds(_currentFrame - static_cast<AudioMixer*>(_audioOutput)->inputBufferSize(_name));
		else
			pos = bytesToSeconds(_currentFrame);
	}
	return pos;
}

void	AudioDecoder::seek(int pos)
{
	if (_state == Playing)
		pause();
	_currentPosition = pos;
	_currentFrame = pos  *  aCodecCtx->sample_rate * 2 * aCodecCtx->channels;
	int64_t timeToSeek = av_rescale(pos * 1000, aFormatCtx->streams[audioStream]->time_base.den ,aFormatCtx->streams[audioStream]->time_base.num);;
	timeToSeek /= 1000;
	avformat_seek_file(aFormatCtx, audioStream, 0, timeToSeek, timeToSeek, AVSEEK_FLAG_FRAME);
	avcodec_flush_buffers(aCodecCtx);
}
*/
