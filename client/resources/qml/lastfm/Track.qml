import QtQuick 1.0

Rectangle {
	id: track
	width: 200
	height: 34
	color: "#2d2d2d"

	property string name
	property string artist
	property string image_url

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
		source: track.image_url
	}

	Text {
		id: name
		color: "#ffffff"
		text: track.name
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
		text: track.artist
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
