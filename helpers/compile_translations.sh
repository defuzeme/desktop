#!/bin/bash
if [[ `uname` = "Linux" ]]; then
	cd ../client/translations
	make move
else
	cd ../client/translations
	make move_mac
fi
