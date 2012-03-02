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

#include "audioplayerwidget.hpp"
#include <QDebug>
#include "audiocore.hpp"

using namespace Player;

AudioPlayerWidget::AudioPlayerWidget(QWidget *parent) :
	QWidget(parent)
{
    setupUi(this);
	playButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
	stopButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaStop));
	_state = Stopped;
	_duration = 0;
	_updatePosition = true;
}

void AudioPlayerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void	AudioPlayerWidget::invertDirection()
{
	setLayoutDirection(Qt::RightToLeft);
	positionSlider->setLayoutDirection(Qt::LeftToRight);
	//trackNameLabel->setAlignment(Qt::AlignRight);
}

void AudioPlayerWidget::on_playButton_clicked()
{
	emit playButton_clicked();
}

void AudioPlayerWidget::on_stopButton_clicked()
{
	emit stopButton_clicked();
}

void AudioPlayerWidget::setState(PlayerWidgetState s)
{
	positionSlider->setEnabled(true);
	stopButton->setEnabled(true);
	playButton->setEnabled(true);
	playButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
	if (s == Playing)
	{
		playButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
		_state = Playing;
	}
	else if (s == Paused)
		_state = Paused;
	else if (s == Stopped)
	{
		_state = Stopped;
		positionSlider->setValue(0);
		timeLabel->setText("00:00");
		stopButton->setEnabled(false);
	}
	else if (s == Disabled)
	{
		_state = Disabled;
		positionSlider->setEnabled(false);
		stopButton->setEnabled(false);
		playButton->setEnabled(false);

	}
	else
	{
		_state = NoFile;
		timeLabel->setText("--:--");
		positionSlider->setEnabled(false);
		stopButton->setEnabled(false);
	}
}

PlayerWidgetState AudioPlayerWidget::state() const
{
	return _state;
}

void	AudioPlayerWidget::setTimeLabel(const QString &text)
{
	timeLabel->setText(text);
}

void	AudioPlayerWidget::setDuration(int duration)
{
	_duration = duration;
	positionSlider->setRange(0, duration);
	positionSlider->setValue(0);
}

void	AudioPlayerWidget::setTrackName(const QString& trackName)
{
	trackNameLabel->setText(trackName);
}

void	AudioPlayerWidget::setTrackInfo(Library::AudioTrack *track)
{
	if (track != 0)
	{
		trackNameLabel->setText(track->getTitle());
//		artistLabel->setText(track->getArtist());
//		albumLabel->setText(track->getAlbum());
	}
	else
	{
		trackNameLabel->setText("No track loaded");
//		artistLabel->setText("Artist");
//		albumLabel->setText("Album");
	}
}

void	AudioPlayerWidget::setPosition(int position)
{
	if (_updatePosition && _state != NoFile)
	{
		positionSlider->setValue(position);
		timeLabel->setText(Audio::AudioCore::formatSeconds(position));
	}
}

void AudioPlayerWidget::on_positionSlider_sliderMoved(int)
{
	_updatePosition = false;
}

void AudioPlayerWidget::on_positionSlider_sliderPressed()
{
	_updatePosition = false;
}

void AudioPlayerWidget::on_positionSlider_sliderReleased()
{
	_updatePosition = true;
	emit positionSlider_released(positionSlider->value());
}

void AudioPlayerWidget::on_positionSlider_valueChanged(int value)
{
	timeLabel->setText(Audio::AudioCore::formatSeconds(value));
}
