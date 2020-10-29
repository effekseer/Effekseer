#!/bin/bash
set -e
set +x

if [ "$RDIR" = "" ];
then
    export RDIR="$PWD/EffekseerToolLinux"
fi


if [ "`which robocopy`" = "" ];
then
    function robocopy { 
        from=$1
        to=$2 
        include=""
        echo "Copy from $from to $to"
        args=(${@:3})
        unset "args[${#args[@]}-1]" 

        for i in "${args[@]}"; 
        do 
            include="$include --include=\"$i\""
            echo "Include $i"
            
        done
        cdir=$PWD
        cd $from
        
        rsync -av --exclude=\"*\" $include . $to

        cd $cdir
    }

fi




rm -Rf $RDIR || true
mkdir -p $RDIR

echo Compile Editor

echo Copy application

cp -Rv Dev/release $RDIR/Tool
rm -R $RDIR/Tool/linux-x64 || true
rm -R $RDIR/Tool/publish || true

echo Sample

mkdir -p $RDIR/Sample
robocopy Release/Sample $RDIR/Sample *.efkproj *.efkmodel *.efkmat *.efkefc *.txt *.png *.mqo *.fbx /S
robocopy ResourceData/samples $RDIR/Sample *.efkproj *.efkmodel *.efkmat *.efkefc *.txt *.png *.mqo *.fbx /S

echo Readme
cp -v readme_tool_win.txt $RDIR/readme.txt
cp -v docs/readme_sample.txt $RDIR/Sample/readme.txt

cp -v docs/Help_Ja.html $RDIR/Help_Ja.html
cp -v docs/Help_En.html $RDIR/Help_En.html
cp -v LICENSE_TOOL $RDIR/LICENSE_TOOL

