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

#include "audioplayer.hpp"
#include "queueable.hpp"
#include "queuetrack.hpp"
#include "audiomixer.hpp"
#include "audiooutputdevice.hpp"
#include "status.hpp"
#include <unistd.h>

using namespace Player;

const int CrossfadeWidth = 5;

AudioPlayer::AudioPlayer(QString playerName) : decoder(0), currentTrack(0)
{
	_playerName = playerName;
	_name = playerName;
	_type = Audio::AudioPlayerType;
	widget = new AudioPlayerWidget();
	_updatePosition = true;
	previousPosition = 0;
	timer = new QTimer(this);
	open(QIODevice::ReadOnly);
	connect(timer, SIGNAL(timeout()), this, SLOT(updatePosition()));
}

AudioPlayer::AudioPlayer(QString playerName, AudioPlayerWidget *widget, Audio::AudioDecoder *decoder, Audio::AudioOutputDevice *outputDevice)
	: widget(widget), decoder(decoder), currentTrack(0), outputDevice(outputDevice)
{
	_playerName = playerName;
	_updatePosition = true;
	previousPosition = 0;
	timer = new QTimer(this);
	decoder->setPlayer(this);
	open(QIODevice::ReadOnly);
	connect(timer, SIGNAL(timeout()), this, SLOT(updatePosition()));
	connect(widget, SIGNAL(positionSlider_released(int)), SLOT(seek(int)));
//	connect(decoder, SIGNAL(endOfTrack()), this, SLOT(unloadTrack()));
}


AudioPlayer::~AudioPlayer()
{
	delete widget;
}

int		AudioPlayer::cueAt()
{
	if (currentTrack)
		return currentTrack->getDuration() - CrossfadeWidth;
	else
		return -1;
}

void	AudioPlayer::init()
{
//	connect(widget, SIGNAL(playButton_clicked()), this, SLOT(play()));
//	connect(widget, SIGNAL(stopButton_clicked()), this, SLOT(stop()));
}

bool AudioPlayer::loadTrack(Queue::Queueable *elem)
{
	if (elem && elem->isTrack())
	{
		try
		{
			PlayerWidgetState	state = widget->state();
			if (currentTrack == elem->toQueueTrack()->getTrack())
				return true;
			currentTrack = elem->toQueueTrack()->getTrack();
			widget->setState(Stopped);
			decoder->openFile(currentTrack->getAbsolutePath());
			outputDevice->setFormat(decoder->getFormat());
			decoder->start();
			widget->setTrackName(currentTrack->getTitle());
			widget->setTrackInfo(currentTrack);
			widget->setDuration(currentTrack->getDuration());
			widget->setState(state);
			return true;
		}
		catch (Exception &e)
		{
			Notification::Status::gMessage(e.msg(), Notification::ERR);
			currentTrack->remove();
			return false;
		}
	}
	else
	{
		unloadTrack();
		widget->setState(NoFile);
		return false;
	}
}

void	AudioPlayer::unloadTrack()
{
	stop();
	widget->setState(Disabled);
	decoder->closeFile();
	widget->setTrackInfo();
	widget->setTimeLabel("--:--");
	currentTrack = 0;
}

void	AudioPlayer::play()
{
	if (decoder->state() != Audio::AudioDecoder::NoFile)
	{
		_updatePosition = true;
		if (widget->state() == Playing)
		{
			timer->stop();
			widget->setState(Paused);
			outputDevice->pause();
			emit paused();
		}
		else
		{
			timer->start(100);
			widget->setState(Playing);
			outputDevice->start();
			emit played();
		}
	}
}

void	AudioPlayer::updatePosition()
{
	int position = decoder->getPlayingPosition();
	widget->setPosition(position);

	if (previousPosition < cueAt() && position >= cueAt())
		emit almostFinished();
	previousPosition = position;
	if (position >= currentTrack->getDuration() - 1)
	{
		qDebug() << "emit: finish";
		emit finished();
		stop();
	}
}

void AudioPlayer::stop()
{
	timer->stop();
	widget->setState(Stopped);
	decoder->setPlayingPosition(0);
	outputDevice->pause();
	_updatePosition = false;
	emit stopped();
}

void	AudioPlayer::seek(int position)
{
	(void)position;
	PlayerWidgetState currentState = widget->state();
	widget->setState(Paused);
//	decoder->seek(position);
	updatePosition();
	if (currentState == Playing)
		play();
}

qint64	AudioPlayer::writeData(const char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64	AudioPlayer::readData(char *data, qint64 maxlen)
{
	if (decoder)
	{
//		qDebug() << "ReadData Player";
			quint64 i = decoder->readDecodedFrame(data, maxlen);
			return i;
	}
	return 0;
}

bool	AudioPlayer::isPlaying() const
{
	if (widget->state() == Playing)
		return true;
	return false;
}

Audio::AudioIO	*AudioPlayer::getAudioInput(const Audio::AudioIO*)
{
	return 0;
}

qint64 AudioPlayer::getPlayingPosition() const
{
	if (decoder)
		return decoder->getPlayingPosition();
	return 0;
}

const Audio::AudioIO	*AudioPlayer::getOutputDevice(const Audio::AudioIO*) const
{
	return outputDevice;
}

qint64	AudioPlayer::readData(char *data, qint64 maxlen, Audio::AudioIO *from)
{
	if (from == outputDevice)
		return readData(data, maxlen);
	return 0;
}

int		AudioPlayer::getCurrentTrackUid() const
{
	if (currentTrack)
		return currentTrack->getUid();
	return 0;
}

int		AudioPlayer::getTotalDuration() const
{
	if (currentTrack)
		return currentTrack->getDuration();
	return 0;
}

Player::PlayerWidgetState AudioPlayer::state() const
{
	return widget->state();
}
