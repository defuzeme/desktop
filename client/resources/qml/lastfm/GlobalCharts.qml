import QtQuick 1.0

LastfmPage {
	current_model_index: place_select.choice_index * 2 + uniq_select.choice_index * 2 + chart_select.choice_index

	PageTitle {
		id: title
		title: "Global charts"
		description: "Last week's worldwide or place-specific charts"
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
				ListElement { name: "World" }
				ListElement { name: "France" }
				ListElement { name: "Paris"}
			}
		}
		MultiButton {
			id: chart_select
			width: parent.width / 3
			anchors.right: parent.right
			anchors.rightMargin: 5
			anchors.verticalCenter: parent.verticalCenter
			selection: ListModel {
				ListElement { name: "Artists" }
				ListElement { name: "Tracks" }
			}
		}
	}

	ExtendedOptionsBar {
		id: more_selections
		anchors.top: selections.bottom
		collapsed_if: place_select.choice_index != 2
		expanded_if: place_select.choice_index == 2
		MultiButton {
			id: uniq_select
			anchors.left: parent.left
			anchors.leftMargin: 5
			width: parent.width / 2
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 5
			selection: ListModel {
				ListElement { name: "All Paris" }
				ListElement { name: "Unique to Paris" }
			}
		}
	}

	ListView {
		id:view
		anchors.top: more_selections.bottom
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
				image_url: _image_url
				anchors.left: parent.left
				anchors.right: visual_count.left
				state: view.model.element_type
			}

			VisualCount {
				id: visual_count
				height: element.height
				playcount: _playcount || 0
				max_playcount: view.model.get(0)._playcount || 0
				max_width: view.width / 3
				suffix: view.model.suffix
				prefix: view.model.prefix
				idx: index
			}
		}
	}

	property list<XmlListModel> modelsList: [
		XmlListModel {
			id: topArtistsModel
			property int limit: 100
			property string element_type: "artist"
			property string method: "chart.getTopArtists"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/artists/artist"
			XmlRole { name: "_artist"; query: "name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: topTracksModel
			property int limit: 100
			property string element_type: "track"
			property string method: "chart.getTopTracks"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/tracks/track"
			XmlRole { name: "_artist"; query: "artist/name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "name/string()" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: nationArtistsModel
			property int limit: 100
			property string element_type: "artist"
			property string method: "geo.getTopArtists"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&country=" + country + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/topartists/artist"
			XmlRole { name: "_artist"; query: "name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: nationTracksModel
			property int limit: 100
			property string element_type: "track"
			property string method: "geo.getTopTracks"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&country=" + country + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/toptracks/track"
			XmlRole { name: "_artist"; query: "artist/name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "name/string()" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: metroArtistsModel
			property int limit: 100
			property string element_type: "artist"
			property string method: "geo.getMetroArtistChart"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&country=" + country + "&metro=" + metro + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/topartists/artist"
			XmlRole { name: "_artist"; query: "name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: metroTracksModel
			property int limit: 100
			property string element_type: "track"
			property string method: "geo.getMetroTrackChart"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&country=" + country + "&metro=" + metro + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/toptracks/track"
			XmlRole { name: "_artist"; query: "artist/name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "name/string()" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: uniqueArtistsModel
			property int limit: 100
			property string element_type: "artist"
			property string method: "geo.getMetroUniqueArtistChart"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&country=" + country + "&metro=" + metro + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/topartists/artist"
			XmlRole { name: "_artist"; query: "name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		},
		XmlListModel {
			id: uniqueTracksModel
			property int limit: 100
			property string element_type: "track"
			property string method: "geo.getMetroUniqueTrackChart"
			property string suffix: " listeners"
			property string prefix: ""
			source: "http://ws.audioscrobbler.com/2.0/?method=" + method + "&country=" + country + "&metro=" + metro + "&limit=" + limit + "&api_key=" + api_key
			query: "/lfm/toptracks/track"
			XmlRole { name: "_artist"; query: "artist/name/string()" }
			XmlRole { name: "_album"; query: "" }
			XmlRole { name: "_track"; query: "name/string()" }
			XmlRole { name: "_playcount"; query: "listeners/number()" }
			XmlRole { name: "_image_url"; query: "image[1]/string()" }
		}
	]

}
