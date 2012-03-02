import QtQuick 1.0

Rectangle {

	property int size
	property string text: text_input.text

	id: rectangle1
	width: 200
	height: size + 16
	radius: 4
	border.width: 2
	border.color: "#000000"

	TextInput {
		id: text_input
		width: 187
		height: size + 8
		text: ""
		cursorVisible: true
		font.pixelSize: size
		anchors.rightMargin: 4
		anchors.leftMargin: 4
		anchors.bottomMargin: 4
		anchors.topMargin: 4
		anchors.fill: parent
		focus: true
	}
}
