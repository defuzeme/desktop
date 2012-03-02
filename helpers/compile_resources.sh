#!/bin/bash
if [[ `uname` = "Linux" ]]; then
	cd ../client/resources
	make move
else
	cd ../client/resources
	make move_mac
fi
