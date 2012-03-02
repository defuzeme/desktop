import QtQuick 1.0
import "globals.js" as Globals

Rectangle {
	id: page_title
	z: 1
	width: parent.width
	height: 42
	gradient: Gradient {
		GradientStop {
			position: 0
			color: Globals.title_gradient_top
		}

		GradientStop {
			position: 0.680
			color: Globals.title_gradient_bottom
		}
	}

	property string title
	property string description
	property string logo_url


	Row
	{
		id: row1
		anchors.fill: parent

		Item
		{
			id: item1
			width: logo_url.length ? 42 : 0
			height: 42

			Image {
				id: image1
				x: 89
				y: 112
				width: logo_url.length ? 36 : 0
				height: 36
				smooth: true
				sourceSize.height: 36
				sourceSize.width: 36
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				source: logo_url
			}
		}

		Item
		{
			width: page_title.width - page_title.height
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 0
			anchors.top: parent.top
			anchors.topMargin: 0

			Text {
				id: title
				height: 28
				color: Globals.title_text_color
				text: page_title.title
				font.bold: true
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignLeft
				anchors.top: parent.top
				anchors.topMargin: 0
				anchors.right: parent.right
				anchors.rightMargin: 0
				anchors.left: parent.left
				anchors.leftMargin: 5
				font.pixelSize: 20
			}

			Text {
				id: description
				y: 48
				height: 12
				color: Globals.title_description_color
				text: page_title.description
				verticalAlignment: Text.AlignVCenter
				anchors.right: parent.right
				anchors.rightMargin: 0
				anchors.left: parent.left
				anchors.leftMargin: 5
				anchors.bottom: parent.bottom
				anchors.bottomMargin: 2
				font.pixelSize: 12
			}
		}
	}
}
