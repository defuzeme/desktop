import QtQuick 1.0

Rectangle {
	id: artist
	width: 200
	height: 34
	color: "#2d2d2d"

	property string name
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
		source: artist.image_url
	}

	Text {
		id: name
		width: 160
		color: "#ffffff"
		text: artist.name
		wrapMode: Text.NoWrap
		anchors.left: parent.left
		anchors.leftMargin: 40
		font.bold: true
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignLeft
		anchors.top: parent.top
		anchors.topMargin: 0
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 0
		font.pixelSize: 15
	}
}
