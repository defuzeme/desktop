function reloadAll(modelsList) {
	for (var i = 0 ; i < modelsList.length ; ++i)
		modelsList[i].reload()
}

function anyLoading(modelsList) {
	for (var i = 0 ; i < modelsList.length ; ++i)
		if (modelsList[i].status === XmlListModel.Loading)
			return true
	return false
}

function globalProgress(modelsList) {
	var out = 0
	for (var i = 0 ; i < modelsList.length ; ++i)
		out += modelsList[i].progress
	return out / modelsList.length
}

