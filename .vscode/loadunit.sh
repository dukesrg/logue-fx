#!/bin/bash
MANIFEST=$(unzip -jp $1 "*/manifest.json" | grep -E 'platform|module' | sed -r 's/^.*(platform|module).?*"(.?*)".*$/\2/')
PLATFORM=$(echo $MANIFEST | cut -f1 -d' ')
MODULE=$(echo $MANIFEST | cut -f2 -d' ')
declare -A PORTNAMES=(["prologue"]="prologue" [minilogue-xd]="minilogue xd" ["nutekt-digital"]="NTS-1 digital kit")
PORTNAME=${PORTNAMES[$PLATFORM]}
LOGUECLIPORTS=$(logue-cli probe -l | grep -E "$PORTNAME.*(KBD/KNOB|SOUND)" | sed -r s/^.*\(in\|out\).?*\([0-9]+\).*$/\\2/)
[ -z $LOGUECLIPORTS ] && echo $PORTNAME is not connected && exit 1
declare -A MAXSLOTS=(["osc"]="15" ["modfx"]="15" ["delfx"]="7" ["revfx"]="7")
logue-cli load -i$(echo $LOGUECLIPORTS | cut -f1 -d' ') -o$(echo $LOGUECLIPORTS | cut -f2 -d' ') -s${MAXSLOTS[$MODULE]} -u$1 -v
declare -A MIDIDATA=(["prologue modfx"]="cc 88 127 cc 92 127 cc 96 0 cc 96 127" ["prologue delfx"]="cc 89 0 cc 94 127 cc 97 0 cc 97 127" ["prologue revfx"]="cc 89 127 cc 94 127 cc 97 0 cc 97 127" ["prologue osc"]="cc 103 127 cc 53 0 cc 53 127" ["minilogue-xd modfx"]="cc 88 127 cc 92 127 cc 96 0 cc 96 127" ["minilogue-xd delfx"]="cc 89 0 cc 89 127 cc 93 127" ["minilogue-xd revfx"]="cc 90 0 cc 90 127 cc 94 127" ["minilogue-xd osc"]="cc 103 127 cc 53 0 cc 53 127" ["nutekt-digital modfx"]="cc 88 0 cc 88 127" ["nutekt-digital delfx"]="cc 89 0 cc 89 127" ["nutekt-digital revfx"]="cc 90 0 cc 90 127" ["nutekt-digital osc"]="cc 53 0 cc 53 127")
echo ${MIDIDATA[$PLATFORM $MODULE]} | sendmidi dev $(sendmidi list | grep "$PORTNAME") ch 1 --
