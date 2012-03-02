import QtQuick 1.0
import "helper.js" as Helper

LastfmPage {
	current_model_index: chart_select.choice_index

	PageTitle {
		id: title
		title: "Your charts"
		description: "Your music broadcast charts"
	}

	OptionsBar {
		id: selections
		anchors.top: title.bottom
		MultiButton {
			id: time_select
			width: parent.width / 3
			anchors.left: parent.left
			anchors.leftMargin: 5
			anchors.verticalCenter: parent.verticalCenter
			selection: ListModel {
				ListElement { name: "Overall" }
				ListElement { name : "Last ..." }
			}
		}
		MultiButton {
			id: chart_select
			width: parent.width / 2
			anchors.right: parent.right
			anchors.rightMargin: 5
			anchors.verticalCenter: parent.verticalCenter
			selection: ListModel {
				ListElement { name: "Artists" }
				ListElement { name : "Albums" }
				ListElement { name : "Tracks" }
			}
		}
	}

	ExtendedOptionsBar {
		id: more_selections
		anchors.top: selections.bottom
		collapsed_if: time_select.choice_index != 1
		expanded_if: time_select.choice_index == 1
		MultiButton {
			id: uniq_select
			anchors.left: parent.left
			anchors.leftMargin: 5
			width: (parent.width / 3) * 2
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 5
			selection: ListModel {
				ListElement { name: "7 days" }
				ListElement { name: "3 months" }
				ListElement { name: "6 months" }
				ListElement { name: "12 months" }
			}
		}
	}

	ListView {
		id:view
		anchors.top: more_selections.bottom
		width: parent.width
		anchors.bottom: load_progress.top

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
				image_url: _image_url
				anchors.left: parent.left
				anchors.right: visual_count.left
				state: view.model.element_type
			}

			VisualCount {
				id: visual_count
				height: element.height
				playcount: _playcount
				max_playcount: view.model.get(0)._playcount
				max_width: view.width / 3
				suffix: " plays"
				idx: index
			}
		}
	}

	Rectangle {
		id: load_progress
		z: 1
		height: 0
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		color: "white"
		Behavior on height {PropertyAnimation{}}

		Rectangle {
			id: visual_progress
			x: 0
			z: 2
			height: parent.height
			width: 0
			color: "green"
			Behavior on width { PropertyAnimation{duration: 100} }
		}

		states: [
			State {
				name: "visible"
				PropertyChanges {
					target: load_progress
					height: 3
				}
				PropertyChanges {
					target: visual_progress
					width: parent.width * Helper.globalProgress(modelsList)
				}
				when: Helper.anyLoading(modelsList)
			}
		]
	}

	property int limit: 100
	property string period: periods.get(time_select.choice_index * (uniq_select.choice_index + 1)).name
	ListModel {
		id: periods
		ListElement { name: "overall" }
		ListElement { name: "7day" }
		ListElement { name: "3month" }
		ListElement { name: "6month" }
		ListElement { name: "12month" }
	}

	property list<XmlListModel> modelsList: [
		XmlListModel {
			id: topArtistsModel
			property string element_type: "artist"
			property string method: "user.gettopartists"
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&period=" + period + "&limit=" + limit + "&user=" + user + "&api_key=" + api_key
			query: "/lfm/topartists/artist"
			XmlRole { name: "_artist"; query: "name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "playcount/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: topAlbumsModel
			property string element_type: "album"
			property string method: "user.gettopalbums"
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&period=" + period + "&limit=" + limit + "&user=" + user + "&api_key=" + api_key
			query: "/lfm/topalbums/album"
			XmlRole { name: "_artist"; query: "artist/name/string()" }
			XmlRole { name: "_album"; query: "name/string()" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "playcount/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: topTracksModel
			property string element_type: "track"
			property string method: "user.gettoptracks"
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&period=" + period + "&limit=" + limit + "&user=" + user + "&api_key=" + api_key
			query: "/lfm/toptracks/track"
			XmlRole { name: "_artist"; query: "artist/name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "name/string()" }
			XmlRole { name: "_playcount"; query: "playcount/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		}
	]

}
