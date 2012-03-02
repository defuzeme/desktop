import QtQuick 1.0
import "helper.js" as Helper
import "login_helpers.js" as LoginHelper

LastfmPage {
	id: login_page
	property bool connected: false
	property bool connecting: false
	property bool authorized_by_user: false
	property string login
	property string realname
	property string avatar_url
	property string api_token
	property string auth_url
	property string key

	Component.onCompleted: LoginHelper.getStoredSessionKey()

	states: [
		State {
			name: "waiting_login"
			PropertyChanges {
				target: login_form
				visible: true
			}
			when: connected == false && connecting == false && key.length == 0 && login.length == 0
		},
		State {
			name: "checking_session"
			PropertyChanges {
				target: session_check
				visible: true
			}
			StateChangeScript {
				name: "load_check_model"
				script: LoginHelper.sessionChecker()
			}
			when: connected == false && connecting == false && key.length > 0 && login.length > 0
		},
		State {
			name: "getting_token"
			PropertyChanges {
				target: wait_session
				visible: true
			}
			StateChangeScript {
				name: "load_token_model"
				script: LoginHelper.tokenGetter()
			}

			when: connected == false && connecting == true && api_token.length === 0 && authorized_by_user == false
		},
		State {
			name: "getting_session"
			extend: "getting_token"
			StateChangeScript {
				name: "load_session_model"
				script: LoginHelper.sessionGetter()
			}

			when: connected == false && connecting == true && api_token.length > 0 && authorized_by_user == true
		},
		State {
			name: "waiting_browser"
			PropertyChanges {
				target: wait_browser
				visible: true
			}
			when: connected == false && connecting == true && api_token.length > 0 && authorized_by_user == false
		},
		State {
			name: "logged"
			when: connected == true
			PropertyChanges {
				target: rectangle
				visible: false
			}
			StateChangeScript {
				name: "load_user_model"
				script: LoginHelper.userLoader()
			}
		}
	]

	XmlListModel {
		id: token_model
		query: "/lfm"
		XmlRole { name: "token"; query: "token/string()" }
		XmlRole { name: "status"; query: "@status/string()" }
		XmlRole { name: "error_string"; query: "error/string()" }
		XmlRole { name: "error_code"; query: "error/@code/number()" }
		onStatusChanged: {
			if (connecting && login_page.state == "getting_token")
			{
				if (xml.length > 0 && status == XmlListModel.Ready)
				{
					if (token_model.get(0).status == "ok")
					{
						api_token = token_model.get(0).token
						auth_url = "http://www.last.fm/api/auth/?api_key=" + api_key + "&token=" + api_token
						Qt.openUrlExternally(auth_url)
					}
					else
					{
						console.log("Token get error " + token_model.get(0).error_code + ":" + token_model.get(0).error_string)
						connecting = false
					}
				}
			}
		}
	}

	XmlListModel {
		id: session_model
		query: "/lfm"
		XmlRole { name: "username"; query: "session/name/string()" }
		XmlRole { name: "session_key"; query: "session/key/string()" }
		XmlRole { name: "status"; query: "@status/string()" }
		XmlRole { name: "error_string"; query: "error/string()" }
		XmlRole { name: "error_code"; query: "error/@code/number()" }
		onStatusChanged: {
			if (connecting && login_page.state == "getting_session")
			{
				if (xml.length > 0 && status == XmlListModel.Ready)
				{
					if (session_model.get(0).status == "ok")
					{
						login = session_model.get(0).username
						key = session_model.get(0).session_key
						connected = true
						connecting = false
						LoginHelper.storeSessionKey()
					}
					else
					{
						console.log("Session get error " + session_model.get(0).error_code + ":" + session_model.get(0).error_string)
						connecting = false
						api_token = ""
						authorized_by_user = false
					}
				}
			}
		}
	}

	XmlListModel {
		id: user_model
		query: "/lfm/user"
		XmlRole { name: "real_name"; query: "realname/string()" }
		XmlRole { name: "image_url"; query: "image[2]/string()" }
		XmlRole { name: "url"; query: "url/string()" }
		XmlRole { name: "country"; query: "country/string()" }
		XmlRole { name: "playcount"; query: "playcount/number()" }
		onStatusChanged: {
			if (connected && login_page.state == "logged")
			{
				if (xml.length > 0 && status == XmlListModel.Ready)
				{
					realname = user_model.get(0).real_name
					avatar_url = user_model.get(0).image_url
				}
				else if (status == XmlListModel.Error)
				{
					console.log("User get error")
				}
			}
		}
	}

	XmlListModel {
		id: check_model
		query: "/lfm"
		XmlRole { name: "logged_user"; query: "recommendations/@user/string()" }
		XmlRole { name: "status"; query: "@status/string()" }
		XmlRole { name: "error_string"; query: "error/string()" }
		XmlRole { name: "error_code"; query: "error/@code/number()" }
		onStatusChanged: {
			if (!connected && login_page.state == "checking_session")
			{
				if (xml.length > 0 && status == XmlListModel.Ready)
				{
					if (check_model.get(0).status == "ok")
					{
						connected = true
					}
					else
					{
						console.log("Session check error " + check_model.get(0).error_code + ":" + check_model.get(0).error_string)
						key = ""
						login = ""
					}
				}
			}
		}
	}

	Rectangle {
		id: rectangle
		x: 81
		y: 70
		width: 162
		height: 120
		color: Qt.darker(parent.color)
		radius: 4
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		border.color: color

		Item {
			id: login_form
			anchors.rightMargin: 0
			anchors.leftMargin: 0
			anchors.bottomMargin: 0
			anchors.topMargin: 0
			anchors.fill: parent
			visible: false

			Image {
				id: image1
				height: 45
				anchors.top: parent.top
				anchors.topMargin: 8
				anchors.right: parent.right
				anchors.rightMargin: 8
				anchors.left: parent.left
				anchors.leftMargin: 8
				source: "http://cdn.last.fm/flatness/badges/lastfm_red.gif"
			}

			Text {
				id: text1
				x: 8
				y: 71
				color: "#ffffff"
				text: qsTr("Click 'Go' to connect to last.fm")
				wrapMode: Text.WordWrap
				font.bold: true
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignTop
				anchors.right: parent.right
				anchors.rightMargin: 8
				anchors.left: parent.left
				anchors.leftMargin: 8
				font.pixelSize: 12
			}

			Rectangle {
				id: go_rectangle
				x: 58
				y: 130
				width: 45
				height: 45
				color: rectangle.color
				radius: 4
				border.color: color
				anchors.horizontalCenterOffset: 0
				anchors.top: login_form.bottom
				anchors.topMargin: 10
				anchors.horizontalCenter: parent.horizontalCenter

				Text {
					id: text2
					color: "#ffffff"
					text: qsTr("Go")
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					anchors.rightMargin: 2
					anchors.leftMargin: 2
					anchors.bottomMargin: 2
					anchors.topMargin: 2
					anchors.fill: parent
					font.pixelSize: 24
				}

				MouseArea {
					id: submit_area
					anchors.fill: parent
					onClicked: { connecting = true }
				}
			}
		}

		Item {
			id: wait_session
			visible: false
			anchors.fill: parent

			Text {
				id: text3
				x: 55
				y: 53
				color: "#ffffff"
				text: qsTr("Please wait...")
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				font.bold: true
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: 12
			}
		}

		Item {
			id: session_check
			visible: false
			anchors.fill: parent

			Text {
				id: session_check_text
				y: 53
				color: "#ffffff"
				text: qsTr("Please wait. Authentication in progress...")
				anchors.right: parent.right
				anchors.rightMargin: 0
				anchors.left: parent.left
				anchors.leftMargin: 0
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				font.bold: true
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: 12
			}
		}

		Item {
			id: wait_browser
			visible: false
			anchors.fill: parent

			Text {
				id: text4
				x: 70
				y: 53
				width: parent.width
				color: "#ffffff"
				text: qsTr("Please grant defuze.me the access to your account using your browser.\nClick 'Go' once done, or 'Cancel' to log using another account.")
				font.bold: true
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				wrapMode: Text.WordWrap
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: 12
			}

			Rectangle {
				id: go_end
				x: 58
				y: 130
				width: 45
				height: 45
				color: rectangle.color
				radius: 4
				anchors.right: parent.right
				anchors.rightMargin: 0
				border.color: color
				anchors.top: wait_browser.bottom
				anchors.topMargin: 10
				Text {
					id: text5
					color: "#ffffff"
					text: qsTr("Go")
					font.pixelSize: 24
					anchors.fill: parent
					anchors.topMargin: 2
					anchors.rightMargin: 2
					anchors.bottomMargin: 2
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					anchors.leftMargin: 2
					verticalAlignment: Text.AlignVCenter
				}

				MouseArea {
					id: submit_area1
					anchors.fill: parent
					onClicked: {
						if (connecting && login_page.state == "waiting_browser")
							authorized_by_user = true
					}
				}
			}

			Rectangle {
				id: cancel_end
				y: 130
				width: 92
				height: 45
				color: rectangle.color
				radius: 4
				anchors.left: parent.left
				anchors.leftMargin: 0
				anchors.top: wait_browser.bottom
				border.color: color
				anchors.topMargin: 10
				Text {
					id: cancel_text
					color: "#ffffff"
					text: qsTr("Cancel")
					font.pixelSize: 24
					anchors.fill: parent
					anchors.topMargin: 2
					anchors.rightMargin: 2
					anchors.bottomMargin: 2
					font.bold: true
					anchors.leftMargin: 2
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}

				MouseArea {
					id: submit_area2
					anchors.fill: parent
					onClicked: {
						api_token = ""
						connecting = false
					}
				}
			}
		}
	}

	PageTitle {
		id: title
		title: login
		description: realname
		logo_url: avatar_url
	}

}
