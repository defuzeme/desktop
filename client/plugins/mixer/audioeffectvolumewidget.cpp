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

#include "audioeffectvolumewidget.hpp"

AudioEffectVolumeWidget::AudioEffectVolumeWidget(Audio::AudioEffectVolume *effect, QWidget *parent) :
	AudioEffectWidget(effect, parent)
{
	setupUi(this);
	effectNameLabel->setText("Volume");
	muteButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaVolume));
	_isMuted = false;
	volumeSlider->setValue(static_cast<Audio::AudioEffectVolume*>(effect)->volume());
}

void AudioEffectVolumeWidget::on_muteButton_clicked()
{
	if (_isMuted == true)
	{
		_isMuted = false;
		muteButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaVolume));
		volumeSlider->setDisabled(false);
		static_cast<Audio::AudioEffectVolume*>(effect)->volume() = volumeSlider->value();
	}
	else
	{
		_isMuted = true;
		muteButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaVolumeMuted));
		volumeSlider->setDisabled(true);
		static_cast<Audio::AudioEffectVolume*>(effect)->volume() = 0;
	}
}

void	AudioEffectVolumeWidget::setVolume(int volume)
{
	volumeSlider->setValue(volume);
}

void	AudioEffectVolumeWidget::setLevels(double left, double right)
{
	vumeter->setLevels(left, right);
}

void AudioEffectVolumeWidget::on_volumeSlider_valueChanged(int position)
{
	static_cast<Audio::AudioEffectVolume*>(effect)->volume() = position;
}
