import QtQuick 1.0
import "globals.js" as Globals

Rectangle {
	id: element
	width: 200
	height: 34
	color: Globals.background_color

	property string title
	property string subtitle
	property string image_url

	property string track
	property string album
	property string artist

	states: [
		State {
			name: "artist"
			PropertyChanges {
				target: element
				title: element.artist
			}
		},
		State {
			name: "album"
			PropertyChanges {
				target: element
				title: element.album
				subtitle: element.artist
			}
		},
		State {
			name: "track"
			PropertyChanges {
				target: element
				title: element.track
				subtitle: element.artist
			}
		}
	]

	Image {
		id: image
		width: 34
		sourceSize.height: 34
		sourceSize.width: 34
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 0
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.left: parent.left
		anchors.leftMargin: 0
		source: element.image_url
	}

	Text {
		id: name
		color: "#ffffff"
		text: element.title
		anchors.right: parent.right
		anchors.rightMargin: 0
		wrapMode: Text.NoWrap
		anchors.left: parent.left
		anchors.leftMargin: 40
		font.bold: true
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignLeft
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 14
		font.pixelSize: 15
	}

	Text {
		id: artist
		color: "#ffffff"
		text: element.subtitle
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignLeft
		anchors.right: parent.right
		anchors.rightMargin: 0
		anchors.left: parent.left
		anchors.leftMargin: 40
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 0
		anchors.top: parent.top
		anchors.topMargin: 20
		font.pixelSize: 12
	}
}

