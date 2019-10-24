#!/bin/bash

#
# set system specific configuration
#

APP=TurbulenceInflowTool
SOURCEDIR=../../../TurbulenceInflowTool
VERSION=v1.0.2

QT_DEPLOY=/Users/pmackenz/Qt/5.13.1/clang_64/bin/macdeployqt
#
# ---- do not change anything below this line -----------------------------------
#

#
# identify release build directory
#

LONG=`grep BuildConfiguration $SOURCEDIR/$APP.pro.user | grep Release`
START=${LONG#*">"}
build_folder=${START%"<"*}


if [ -d $build_folder ]
then
    (
	(
	cd $build_folder
	echo "switching to $PWD"

	if [ -d $APP.app ]
	then

	    #
	    # creating the dmg package
	    #

	    if [ ! -x $QT_DEPLOY ]
	    then
		echo "no executable macdeploy tool found"
		exit 1
	    fi

	    #$QT_DEPLOY $APP.app -fs=HFS+ -dmg
	    $QT_DEPLOY $APP.app

	else

	    echo
	    echo "*** Build Release version of TurbulenceInflowTool before running this script. ***"
	    echo
	    exit 1

	fi
	)

	echo "back to $PWD"

	cp -r $build_folder/$APP.app ./packages/nheri.simcenter.tinf/data/.

        $HOME/Qt/Tools/QtInstallerFramework/3.1/bin/binarycreator \
		-c config/macconfig.xml \
		-p packages \
		--offline-only \
		TInF-MacOS_installer-${VERSION}.dmg

	echo done.

    )

else
    echo
    echo "*** cannot find $build_folder ***"
    echo
    exit 2
fi

