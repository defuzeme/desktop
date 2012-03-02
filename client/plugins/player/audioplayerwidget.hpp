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

#ifndef AUDIOPLAYERWIDGET_HPP
#define AUDIOPLAYERWIDGET_HPP

#include "ui_audioplayerwidget.h"
#include "guicore.hpp"
#include "audiotrack.hpp"

namespace Player
{
	enum PlayerWidgetState
	{
		NoFile = -1,
		Stopped,
		Playing,
		Paused,
		Disabled
	};
	class AudioPlayerWidget : public QWidget, private Ui::AudioPlayerWidget
	{
		Q_OBJECT
		friend class MainPlayer;

	public:
		explicit AudioPlayerWidget(QWidget *parent = 0);
		PlayerWidgetState state() const;
		void	setState(PlayerWidgetState);
		void	setTrackName(const QString& name);
		void	setTimeLabel(const QString& time);
		void	setPosition(int position);
		void	setTrackInfo(Library::AudioTrack*track = 0);
		void	setDuration(int duration);
		void	invertDirection();
	protected:
		void changeEvent(QEvent *e);
	private slots:
		void on_playButton_clicked();
		void on_stopButton_clicked();

		void on_positionSlider_valueChanged(int value);
		void on_positionSlider_sliderMoved(int position);
		void on_positionSlider_sliderPressed();
		void on_positionSlider_sliderReleased();

	signals:
		void playButton_clicked();
		void stopButton_clicked();
		void positionSlider_pressed();
		void positionSlider_moved();
		void positionSlider_released(int pos);

	private:
		bool				_updatePosition;
		int					_duration;
		PlayerWidgetState	_state;
	};
}
#endif // AUDIOPLAYERWIDGET_HPP
