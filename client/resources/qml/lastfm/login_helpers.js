function storeSessionKey() {
	var db = openDatabaseSync("qmlDB", "1.0", "The qml offline storage", 100000)
	db.transaction(
				function(tx) {
					tx.executeSql('CREATE TABLE IF NOT EXISTS Settings(key TEXT, value TEXT)');
					tx.executeSql('INSERT INTO Settings VALUES(?, ?)', [ 'lastfm_login', login ]);
					tx.executeSql('INSERT INTO Settings VALUES(?, ?)', [ 'lastfm_session_key', key ]);
				}
				)
}

function getStoredSessionKey() {
	console.log("Trying to retreive login/key pair")
	var db = openDatabaseSync("qmlDB", "1.0", "The qml offline storage", 100000)
	db.transaction(
				function(tx) {
					tx.executeSql('CREATE TABLE IF NOT EXISTS Settings(key TEXT, value TEXT)');
					var login_output = tx.executeSql('SELECT * FROM Settings WHERE key = "lastfm_login" LIMIT 1');
					var key_output = tx.executeSql('SELECT * FROM Settings WHERE key = "lastfm_session_key" LIMIT 1');
					if (login_output.rows.length > 0 && key_output.rows.length > 0) {
						login = login_output.rows.item(0).value
						key = key_output.rows.item(0).value
					}
				}
				)
}

function requestForModel(url, model, error_callback) {
	var doc = new XMLHttpRequest()
	doc.onreadystatechange = function() {
				if (doc.readyState == XMLHttpRequest.DONE)
					if (doc.status == 200)
						model.xml = doc.responseText
					else
						error_callback()
			}
	doc.open("GET", url)
	doc.send()
}

function sessionChecker() {
	var sig = Qt.md5("api_key" + api_key + "limit1methoduser.getRecommendedArtistssk" + key + secret)
	var url = "http://ws.audioscrobbler.com/2.0/?method=user.getRecommendedArtists&api_key=" +
			api_key + "&limit=1&sk=" + key + "&api_sig=" + sig
	requestForModel(url, check_model, function() {
						console.log("Session check error")
						key = ""
						login = ""
					})
}

function tokenGetter() {
	var sig = Qt.md5("api_key" + api_key + "methodauth.getToken" + secret)
	var url = "http://ws.audioscrobbler.com/2.0/?method=auth.getToken&api_key=" + api_key + "&api_sig=" + sig
	requestForModel(url, token_model, function() {
						console.log("Token get error")
						connecting = false
					})
}

function sessionGetter() {
	var sig = Qt.md5("api_key" + api_key + "methodauth.getSessiontoken" + api_token + secret)
	var url = "http://ws.audioscrobbler.com/2.0/?method=auth.getSession&api_key=" + api_key +
			"&token=" + api_token + "&api_sig=" + sig
	requestForModel(url, session_model, function() {
						console.log("Session get error")
						connecting = false
						api_token = ""
						authorized_by_user = false
					})
}

function userLoader() {
	var url = "http://ws.audioscrobbler.com/2.0/?method=user.getInfo&api_key=" + api_key + "&user=" + login
	requestForModel(url, user_model, function() {
						console.log("User get error")
					})
}
