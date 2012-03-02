import QtQuick 1.0

LastfmPage {

	PageTitle {
		id: title
		title: "Played tracks"
		description: "Your music broadcast history"
	}

	OptionsBar {
		id: selections
		y: title.height
		MultiButton {
			width: parent.width / 3
			height: 22
			anchors.left: parent.left
			anchors.leftMargin: 5
			anchors.verticalCenter: parent.verticalCenter
			selection: ListModel {
				ListElement { name: "Recent" }
				ListElement { name : "By date" }
			}
		}
	}

	ListView {
		id:view
		x: 0
		anchors.top: selections.bottom
		width: parent.width
		anchors.bottom: parent.bottom

		model: recentTracksModel
		delegate:Rectangle {
			height: element.height
			width: view.width
			color: "#2d2d2d"

			Element {
				id: element
				artist: _artist || ""
				album: _album || ""
				track: _track || ""
				image_url: _image_url
				anchors.left: parent.left
				anchors.right: visual_count.left
				state: "track"
			}

			VisualCount {
				id: visual_count
				height: element.height
				playcount: 1
				max_playcount: 1
				max_width: view.width / 3
				suffix: ""
				prefix: ""
				fixed_text: if (!_playdate)
								return "Now playing"
							else
								return Qt.formatDateTime(new Date(_playdate * 1000), Qt.SystemLocaleShortDate)
			}
		}

//			Row {
//			Element {
//				id: artist
//				track: _name
//				artist: _artist
//				image_url: _image_url
//				width: (view.width / 3) * 2
//				state: "track"
//			}
//			Rectangle {
//				width: view.width / 3
//				height: artist.height
//				color: "#2d2d2d"
//				Text {text: {
//						if (!_playdate)
//							return "Now playing"
//						else
//							return Qt.formatDateTime(new Date(_playdate * 1000), Qt.SystemLocaleShortDate)
//					}}

//			}
//		}
	}

	Timer {
			 interval: 30 * 1000; running: true; repeat: true
			 onTriggered: recentTracksModel.reload()
		 }

	XmlListModel {
		id: recentTracksModel
		source: "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&limit=50&user=" + user + "&api_key=" + api_key
		query: "/lfm/recenttracks/track"

		XmlRole { name: "_artist"; query: "artist/string()" }
		XmlRole { name: "_track"; query: "name/string()" }
		XmlRole { name: "_album"; query: "album/string()" }
		XmlRole { name: "_playdate"; query: "date/@uts/string()" }
		XmlRole { name: "_image_url"; query: "image[1]/string()" }
	}
}
