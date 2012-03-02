import QtQuick 1.0
import "globals.js" as Globals

Rectangle {
	id: lastfmView
	width: 400							/// Only used by the designer
	height: 400							/// Size will be automatically adjust to the underlying QDeclarativeView
	color: Globals.background_color

	property string user: login_page.login
	property bool connected: login_page.connected
	property string session_key: login_page.key
	property string api_key: "89dd449ff72a59735757e626d3c4b66a"
	property string secret: "8e2bb5475e973555c32829b589fc0e4f"
	property string country: "France"
	property string metro: "Paris"
	property int pausedTrack: 0
	property int startPlaying: 0

	states: [
		State {
			name: "not_logged"
			PropertyChanges {
				target: view
				interactive: false
			}
			PropertyChanges {
				target: nav
				height: 0
				opacity: 0
				enabled: false
			}
			when: user.length === 0 || connected === false
		}
	]

	VisualItemModel {
		id: panesModel
		LoginPage { id: login_page ; width: view.width ; height : view.height }
//		LastfmPage { width: view.width ; height : view.height
//			PageTitle {
//				title: "Search"
//				description: "Browse through the whole world of music"
//			}}
		WhatsNext { width: view.width ; height : view.height }
		RecentTracks { width: view.width ; height : view.height }
		YourCharts { width: view.width ; height : view.height }
		GlobalCharts { width: view.width ; height : view.height }
	}

	ListView {
		id: view
		x:0
		y:0
		width: parent.width
		height: parent.height - 30
		model: panesModel
		highlightMoveDuration: 500
		focus: true
		interactive: true
		orientation: ListView.Horizontal
		highlightRangeMode: ListView.StrictlyEnforceRange
		snapMode: ListView.SnapOneItem
		highlightFollowsCurrentItem:true
		boundsBehavior: Flickable.DragOverBounds
	}

	Rectangle {
		id: nav
		width: parent.width; height: 30
		anchors {bottom: parent.bottom}
		color: "gray"
		property int current: view.currentIndex
		Behavior on height { PropertyAnimation {} }
		Behavior on opacity { PropertyAnimation {} }

		ListModel {
			id: icons_model
			ListElement { name: "wrench" }
			//ListElement { name: "find" }
			ListElement { name: "star" }
			ListElement { name: "time" }
			ListElement { name: "chart" }
			ListElement { name: "world" }
		}

		Row {
			anchors.centerIn: parent
			spacing: 20

			Repeater {
				model: panesModel.count

				Rectangle {
					width: 20; height: 20
					radius: 3
					color: nav.current == index ? Qt.lighter(nav.color) : nav.color

					Image {
						source: "qrc:/icons/" + icons_model.get(index).name
						anchors.centerIn: parent
					}

					MouseArea {
						width: 20; height: nav.height - 10
						anchors.centerIn: parent
						onClicked: view.currentIndex = index
					}
				}
			}
		}
	}

	Connections {
		target: mainPlayer
		onLplay: {
			console.log("Played " + uid + ", " + totalTime)
			var artist = library.getTrack(uid).exArtist
			var title = library.getTrack(uid).exTitle
			var m_totalTime = totalTime
			if (pausedTrack != uid)
			{
				updateNowPlaying(title, artist, m_totalTime)
				var d = new Date
				startPlaying = d.getTime() / 1000
			}
		}
		onLpause: {
			console.log("Pause " + uid + ", " + elapsedTime + ", " + totalTime)
			var artist = library.getTrack(uid).exArtist
			var title = library.getTrack(uid).exTitle
			pausedTrack = uid
		}
		onLstop: {
			console.log("Stop " + uid + ", " + elapsedTime + ", " + totalTime)
			var artist = library.getTrack(uid).exArtist
			var title = library.getTrack(uid).exTitle
			var m_totalTime = totalTime
			var m_elapsedTime = elapsedTime
			pausedTrack = 0
			if (startPlaying != 0 && m_totalTime > 30 && (m_elapsedTime >= 60 * 4 || m_elapsedTime >= m_totalTime / 2))
				scrobble(title, artist, startPlaying)
			startPlaying = 0
		}
		onLnext: {
			console.log("Next " + oldUid + ", " + newUid + ", " + oldElapsedTime + ", " + oldTotalTime + ", " + newTotalTime)
			var old_artist = library.getTrack(oldUid).exArtist
			var old_title = library.getTrack(oldUid).exTitle
			var new_artist = library.getTrack(newUid).exArtist
			var new_title = library.getTrack(newUid).exTitle
			var m_totalTime = oldTotalTime
			var m_elapsedTime = oldElapsedTime
			var m_newTotalTime = newTotalTime
			pausedTrack = 0
			if (startPlaying != 0 && m_totalTime > 30 && (m_elapsedTime >= 60 * 4 || m_elapsedTime >= m_totalTime / 2))
				scrobble(old_title, old_artist, startPlaying)
			startPlaying = 0
			updateNowPlaying(new_title, new_artist, m_newTotalTime)
			var d = new Date
			startPlaying = d.getTime() / 1000
		}
	}

	function updateNowPlaying(title, artist, duration) {
		console.log("Update Now Playing for [" + title + ", " + artist + "]")

		var enTitle = encodeURI(title)
		var enArtist = encodeURI(artist)

		var sig = Qt.md5("api_key" + api_key + "artist" + artist + "duration" + duration + "methodtrack.updateNowPlayingsk" + session_key + "track" + title + secret)
		var url = "http://ws.audioscrobbler.com/2.0/"
		var params = "method=track.updateNowPlaying" + "&api_key=" + api_key + "&artist=" + enArtist + "&duration=" + duration + "&sk=" + session_key + "&track=" + enTitle + "&api_sig=" + sig

		var doc = new XMLHttpRequest()
		doc.onreadystatechange = function() {
					if (doc.readyState == XMLHttpRequest.DONE)
					{
						if (doc.status == 200)
						{}
						else
						{
							console.log("ERROR Updating Now Playing " + doc.statusText)
						}
					}
				}
		doc.open("post", url, true)
		doc.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		doc.send(params)
	}

	function scrobble(title, artist, timestamp) {
		console.log("Scrobble for [" + title + ", " + artist + "]")

		var enTitle = encodeURI(title)
		var enArtist = encodeURI(artist)

		var sig = Qt.md5("api_key" + api_key + "artist" + artist + "methodtrack.scrobblesk" + session_key + "timestamp" + timestamp + "track" + title + secret)
		var url = "http://ws.audioscrobbler.com/2.0/"
		var params = "method=track.scrobble" + "&api_key=" + api_key + "&artist=" + enArtist + "&sk=" + session_key + "&timestamp=" + timestamp + "&track=" + enTitle + "&api_sig=" + sig

		var doc = new XMLHttpRequest()
		doc.onreadystatechange = function() {
					if (doc.readyState == XMLHttpRequest.DONE)
					{
						if (doc.status == 200)
						{}
						else
						{
							console.log("ERROR Scrobble " + doc.statusText)
						}
					}
				}
		doc.open("post", url, true)
		doc.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		doc.send(params)
	}
}
