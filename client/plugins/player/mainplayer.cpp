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

#include "mainplayer.hpp"
#include "queueable.hpp"
#include "queuetrack.hpp"
#include "audiomixer.hpp"
#include "mainplayerwidget.hpp"
#include "audiomixerplugin.hpp"

#include <QDebug>

using namespace Player;

MainPlayer::MainPlayer(QString playerName) : decoderA(0), decoderB(0), playQueue(0)
{
	_playerName = playerName;
	widget = new MainPlayerWidget(this);
	uiModule = Gui::ModuleFactory::create(playerName, QPoint(1, 0), widget);
	uiModule->setSizePolicy(QSizePolicy::Maximum);
	uiModule->submitForDisplay();
	playingPlayer = 0;
	isCrossfading = false;
}

MainPlayer::~MainPlayer()
{
	delete widget;
}

void	MainPlayer::init()
{
	connect(widget, SIGNAL(nextButton_clicked()), SLOT(crossfadeNext()));
	connect(cores->audio(), SIGNAL(crossfadeFinished()), SLOT(crossfadeFinished()));

	decoderA = cores->audio()->newAudioDecoder(_playerName);
//	outputDeviceA = cores->audio()->newAudioDevice(cores->audio()->availableDevices(QAudio::AudioOutput).last());
	outputDeviceA = cores->audio()->newAudioDevice();
	playerA = new AudioPlayer("Player A", playerWidgetA, decoderA, outputDeviceA);

	decoderB = cores->audio()->newAudioDecoder("Player B");
//	outputDeviceB = cores->audio()->newAudioDevice(cores->audio()->availableDevices(QAudio::AudioOutput).last());
	outputDeviceB = cores->audio()->newAudioDevice();
	playerB = new AudioPlayer("Player B", playerWidgetB, decoderB, outputDeviceB);


	Audio::AudioMixer* masterMixer = cores->audio()->newAudioMixer("Master");

	mixerB = cores->audio()->newAudioMixer("Mixer B", playerB);
	mixerA = cores->audio()->newAudioMixer("Mixer A", playerA);
	mixerA->setOutput(masterMixer);
	mixerB->setOutput(masterMixer);

	outputDeviceA->setInput(masterMixer);
	outputDeviceB->setInput(masterMixer);

	masterMixer->addInput(mixerA);
	masterMixer->addInput(mixerB);

	Mixer::AudioMixerPlugin *m = plugins->cast<Mixer::AudioMixerPlugin>("mixer");
	m->refresh();
	playQueue = plugins->cast<Queue::PlayQueue>("queue");


	playerWidgetB->invertDirection();

	connect(playerA, SIGNAL(almostFinished()), this, SLOT(crossfadeNext()));
	connect(playerB, SIGNAL(almostFinished()), this, SLOT(crossfadeNext()));
	connect(playerWidgetA, SIGNAL(playButton_clicked()), this, SLOT(playA()));
	connect(playerWidgetB, SIGNAL(playButton_clicked()), this, SLOT(playB()));
	connect(playerWidgetA, SIGNAL(stopButton_clicked()), this, SLOT(stop()));
	connect(playerWidgetB, SIGNAL(stopButton_clicked()), this, SLOT(stop()));



	connect(playQueue, SIGNAL(addQueueElem(Queue::Queueable*)), this, SLOT(newElem(Queue::Queueable*)));
	connect(playQueue, SIGNAL(removeQueueElem(Queue::Queueable*)), this, SLOT(removeElem(Queue::Queueable*)));

	connect(playerA, SIGNAL(finished()), this, SLOT(finished()));
	connect(playerB, SIGNAL(finished()), this, SLOT(finished()));

	connect(playerA, SIGNAL(played()), SIGNAL(played()));
	connect(playerB, SIGNAL(played()), SIGNAL(played()));
	connect(playerA, SIGNAL(paused()), SIGNAL(paused()));
	connect(playerB, SIGNAL(paused()), SIGNAL(paused()));
	connect(playerA, SIGNAL(stopped()), SIGNAL(stopped()));
	connect(playerB, SIGNAL(stopped()), SIGNAL(stopped()));
	loadTracks();
}

bool	MainPlayer::isPlaying() const
{
	return (playingPlayer && playingPlayer->isPlaying());
}

AudioPlayer*	MainPlayer::firstPlayer()
{
	if (playingPlayer && playingPlayer == playerB)
		return playerB;
	else
		return playerA;
}

AudioPlayer*	MainPlayer::secondPlayer()
{
	if (playingPlayer && playingPlayer == playerB)
		return playerA;
	else
		return playerB;
}


bool	MainPlayer::loadTracks()
{
	playerA->loadTrack(playQueue->head());
	playerB->loadTrack(playQueue->next());
	playingPlayer = playerA;
	return true;
}

void	MainPlayer::removeElem(Queue::Queueable *elem)
{
	if (elem->getPosition() == 0)
	{
		firstPlayer()->loadTrack(playQueue->head());
		secondPlayer()->unloadTrack();
		secondPlayer()->loadTrack(playQueue->next());
	}
	else if (elem->getPosition() == 1)
	{
		secondPlayer()->unloadTrack();
		secondPlayer()->loadTrack(playQueue->next());
	}
}

void	MainPlayer::newElem(Queue::Queueable *elem)
{
	if (elem->getPosition() == 0)
	{
		firstPlayer()->loadTrack(playQueue->head());
		secondPlayer()->unloadTrack();
		secondPlayer()->loadTrack(playQueue->next());
	}
	else if (elem->getPosition() == 1)
	{
		secondPlayer()->loadTrack(playQueue->next());
	}
}

bool	MainPlayer::play()
{
	if (playingPlayer->isPlaying())
		return false;
	else
	{
		if (playingPlayer == playerA)
			return playA();
		else
			return playB();
		return true;
	}
}

void	MainPlayer::finished()
{
	qDebug() << "finish : pop";
	if (!isCrossfading)
		playQueue->pop();
}

bool	MainPlayer::pause()
{
	if (playingPlayer->isPlaying())
	{
		playingPlayer->play();
		return true;
	}
	else
		return false;
}

bool	MainPlayer::stop()
{
	emit lstop(playingPlayer->getCurrentTrackUid(), playingPlayer->getPlayingPosition(), playingPlayer->getTotalDuration());
	playingPlayer->stop();
	qDebug() << "EMIT STOP : uid = " << playingPlayer->getCurrentTrackUid() << " pp = " << playingPlayer->getPlayingPosition();
	return true;
}

bool	MainPlayer::crossfadeNext()
{
	bool r = false;
	AudioPlayer *op;

	if (playingPlayer == playerA)
	{
		r = playB();
		op = playerA;
	}
	else
	{
		r = playA();
		op = playerB;
	}
	if (r)
		isCrossfading = true;
	return r;
}

void	MainPlayer::crossfadeFinished()
{
	playQueue->pop();
	widget->nextButton->setEnabled(true);
	cores->audio()->stopCrossfade();
	isCrossfading = false;

}

void	MainPlayer::setPlayers(AudioPlayerWidget *A, AudioPlayerWidget *B)
{
	playerWidgetA = A;
	playerWidgetB = B;
}

bool	MainPlayer::playA()
{
	if (decoderA->state() == Audio::AudioDecoder::NoFile)
		return false;

	if (playingPlayer != playerA && playerWidgetA->state() != Playing)
	{
		cores->audio()->startCrossfade(mixerB, mixerA);
		widget->setNextButtonIcon(widget->style()->standardIcon(QStyle::SP_MediaSkipForward));
		widget->nextButton->setEnabled(false);
		playerWidgetB->playButton->setEnabled(false);
		playerWidgetB->stopButton->setEnabled(false);
		playerWidgetB->setState(Disabled);
		widget->setBNext();
		widget->setACurrent();
		qDebug() << "EMIT NEXT : olduid = " << playerB->getCurrentTrackUid() << " newuid = " << playerA->getCurrentTrackUid() << " pp = " << playerB->getPlayingPosition();
		emit lnext(playerB->getCurrentTrackUid(), playerA->getCurrentTrackUid(), playerB->getPlayingPosition(), playerB->getTotalDuration(), playerA->getTotalDuration());
		playingPlayer = playerA;
	}
	else
		emitPlayPause();
	playerA->play();
	return true;
}

void	MainPlayer::emitPlayPause()
{
	if (playingPlayer->state() != Playing)
	{
		qDebug() << "EMIT PLAY: " << " uid = " << playingPlayer->getCurrentTrackUid() ;
		emit lplay(playingPlayer->getCurrentTrackUid(), playingPlayer->getTotalDuration());
	}
	else
	{
		qDebug() << "EMIT PAUSE: " << "uid = " << playingPlayer->getCurrentTrackUid() << " pp = " << playingPlayer->getPlayingPosition();
		emit lpause(playingPlayer->getCurrentTrackUid(), playingPlayer->getPlayingPosition(), playingPlayer->getTotalDuration());
	}
}

bool	MainPlayer::playB()
{
	if (decoderB->state() == Audio::AudioDecoder::NoFile)
		return false;

	if (playingPlayer != playerB && playerWidgetB->state() != Playing)
	{
		cores->audio()->startCrossfade(mixerA, mixerB);
		widget->setNextButtonIcon(widget->style()->standardIcon(QStyle::SP_MediaSkipBackward));
		widget->nextButton->setEnabled(false);
		playerWidgetA->playButton->setEnabled(false);
		playerWidgetA->stopButton->setEnabled(false);
		playerWidgetA->setState(Disabled);
		widget->setANext();
		widget->setBCurrent();
		qDebug() << "EMIT NEXT : olduid = " << playerA->getCurrentTrackUid() << " newuid = " << playerB->getCurrentTrackUid() << " pp = " << playerA->getPlayingPosition();
		emit lnext(playerA->getCurrentTrackUid(), playerB->getCurrentTrackUid(), playerA->getPlayingPosition(), playerA->getTotalDuration(), playerB->getTotalDuration());
		playingPlayer = playerB;
	}
	else
		emitPlayPause();
	playerB->play();
	return true;
}

