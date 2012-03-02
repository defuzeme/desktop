#!/bin/sh

# Set the package name
NAME='defuzeme'
# Get version number from client's main header
VERSION=`grep 'APPLICATION_VERSION' ../client/main.hpp | cut -d \" -f 2`
# Get system architecture
ARCHI=$1
if test -z $1
then
	ARCHI=`uname -m`
fi
# Convert to debian CONTROL architecture
DEB_ARCHI='i386'
if [ $ARCHI = "x86_64" ]
then
	DEB_ARCHI='amd64'
fi
BIN_DIR="usr/local/bin"
APP_DIR="usr/local/lib/defuze.me"

DEBFILE="$NAME-$VERSION-$DEB_ARCHI.deb"

echo "- building deb file for $ARCHI"

echo "- create temporary filesystem"
mkdir -p deb_root/$BIN_DIR
mkdir -p deb_root/$APP_DIR
cp ../client-build-desktop/client_$ARCHI deb_root/$BIN_DIR/defuzeme
cp ../client-build-desktop/resources/starter.rcc deb_root/$APP_DIR/starter.rcc
cp ../client-build-desktop/resources/lastfm.rcc deb_root/$APP_DIR/lastfm.rcc

SIZE=`du --summarize deb_root/ | sed 's/\t.*//g'`
echo "- package size: $SIZE"

echo "- create control file"
mkdir -p deb_root/DEBIAN
cd deb_root/DEBIAN
echo "Package: $NAME" > control
echo "Version: $VERSION" >> control
echo "Architecture: $DEB_ARCHI" >> control
echo "Installed-Size: $SIZE" >> control
echo "Depends: libqt4-core (>= 4.7), libqt4-gui (>= 4.7), libqt4-sql (>= 4.7), libqt4-sql-sqlite (>= 4.7), libqt4-declarative (>= 4.7), libbz2-dev, libfaac0, libmp3lame0, libvorbis-dev, ffmpeg, libqtmultimediakit1, libtag1-vanilla" >> control
echo "Maintainer: defuze.me team [team@defuze.me]" >> control
echo "Description: This package contains the defuze.me radio automation software binary." >> control
cd ../../

echo "- create licence file"
cp ../client/LICENSE.txt deb_root/DEBIAN/copyright

echo "- create debian archive"
dpkg -b deb_root $DEBFILE

echo "- clear filesystem"
rm -rf deb_root

echo "- move deb file to installers folder"
mv $DEBFILE ../installers
