import QtQuick 1.0

Rectangle {
	id: count
	z:1
	color: "#2d2d2d"
	anchors.verticalCenter: parent.verticalCenter
	anchors.right: parent.right
	width: visual_count.width + trans.height

	property int playcount
	property int max_playcount
	property int max_width
	property int idx
	property string prefix: ""
	property string suffix: ""
	property string fixed_text

	Rectangle {
		id: trans
		anchors.right: visual_count.left
		anchors.verticalCenter: parent.verticalCenter
		width: visual_count.height
		height: visual_count.height
		rotation: -90
		color: "lightgray"
	}

	Rectangle {
		id: visual_count
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		width:(playcount / max_playcount) * max_width
		height: (count.height / 2)
		color:"lightgrey"
		Text {
			text: {
				if (fixed_text.length > 0)
					return fixed_text
				else if (!playcount)
					return ""
				else if (idx)
					return playcount
				else
					return prefix + playcount + suffix
			}
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter
			anchors.rightMargin: 5
			font.bold: true
		}
	}
}
