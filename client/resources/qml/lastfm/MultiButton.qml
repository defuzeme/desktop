import QtQuick 1.0

Rectangle {
	id: multi_button
	z: 2
	width: parent.width
	height: 22
	color: parent.color

	property ListModel selection
	property string choice: selection.get(0).name
	property int choice_index: 0
	signal selectionChanged()

	onSelectionChanged: {}

	Row {
		Repeater {
			model: selection
			Rectangle {
				id: button
				width: multi_button.width / selection.count
				height: multi_button.height
				color: "#2d2d2d"
				radius: 4
				border.width: 2
				property color label_color: "white"
				Text {
					id: label
					anchors.centerIn: parent
					text: name
					color: parent.label_color
				}
				MouseArea {
					anchors.fill: parent
					onClicked: {
						if (multi_button.choice != label.text)
						{
							multi_button.choice = label.text
							multi_button.choice_index = index
							selectionChanged()
						}
					}
				}
				states: [
					State {
						name: "selected"
						PropertyChanges {
							target: button
							color: "lightblue"
							label_color: "black"
						}
						when: multi_button.choice ==  label.text
					}
				]
			}
		}
	}
}
