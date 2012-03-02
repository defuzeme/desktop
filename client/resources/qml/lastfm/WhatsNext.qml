import QtQuick 1.1

LastfmPage {
	current_model_index: place_select.choice_index

	PageTitle {
		id: title
		title: "What's next?"
		description: "Explore new releases and personalized recommendations"
	}

	OptionsBar {
		id: selections
		y: title.height
		MultiButton {
			id: place_select
			width: parent.width / 2
			anchors.left: parent.left
			anchors.leftMargin: 5
			anchors.verticalCenter: parent.verticalCenter
			selection: ListModel {
				ListElement { name: "Recommendations" }
				ListElement { name: "New releases" }
			}
		}
	}

	ListView {
		id:view
		anchors.top: selections.bottom
		width: parent.width
		anchors.bottom: parent.bottom

		model: modelsList[current_model_index]
		delegate: Rectangle {
			height: element.height
			width: view.width
			color: "#2d2d2d"

			Element {
				id: element
				artist: _artist || ""
				album: _album || ""
				track: _track || ""
				subtitle: place_select.choice_index ? "" : context(_context1, _context2, _context3)
				image_url: _image_url
				anchors.left: parent.left
				anchors.right: parent.right
				state: view.model.element_type
			}
		}
	}

	function context(c1, c2, c3) {
		if (!c1.length)
			return ""
		else if (!c2.length)
			return "Similar to " + c1
		else if (!c3.length)
			return "Similar to " + c1 + " and " + c2
		else
			return "Similar to " + c1 + ", " + c2 + " and " + c3
	}

	property list<XmlListModel> modelsList: [
		XmlListModel {
			id: recommendationsModel
			property int limit: 100
			property string element_type: "artist"
			property string method: "user.getRecommendedArtists"
			property string suffix: ""
			property string prefix: ""
			property string api_sig: Qt.md5("api_key" + api_key + "methoduser.getRecommendedArtistssk" + session_key + secret)
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&api_sig=" + api_sig + "&api_key=" + api_key + "&sk=" + session_key
			query: "/lfm/recommendations/artist"
			XmlRole { name: "_artist"; query: "name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
			XmlRole { name: "_context1"; query: "context/artist[1]/name/string()"}
			XmlRole { name: "_context2"; query: "context/artist[2]/name/string()"}
			XmlRole { name: "_context3"; query: "context/artist[3]/name/string()"}
		},
		XmlListModel {
			id: newReleasesModel
			property int limit: 100
			property string element_type: "album"
			property string method: "user.getNewReleases"
			property string suffix: ""
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&user=" + user + "&api_key=" + api_key
			query: "/lfm/albums/album"
			XmlRole { name: "_artist"; query: "artist/name/string()" }
			XmlRole { name: "_album"; query: "name/string()" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		}
	]

}

