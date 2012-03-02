import QtQuick 1.0

OptionsBar {
	id: extended_options_bar
	state: "collapsed"

	property bool expanded_if: false
	property bool collapsed_if: true

	Behavior on height { PropertyAnimation {} }
	Behavior on opacity { PropertyAnimation {} }

	states: [
		State {
			name: "expanded"
			PropertyChanges {
				target: extended_options_bar
				height: 27
				opacity: 1
			}
			when: expanded_if
		},
		State {
			name: "collapsed"
			PropertyChanges {
				target: extended_options_bar
				height: 0
				opacity: 0
			}
			when: collapsed_if
		}
	]
}
