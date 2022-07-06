#!/bin/bash
#this script is used to combine speaker n9 bins to one bin


combine_btN9_bins(){
  # the last argument is destitnation
  echo "Function name:  ${FUNCNAME}"
  echo "The number of positional parameter : $#"
  echo "All parameters or arguments passed to the function: '$@'"
  theFirstBin="$1"
  numberOfBinsAppended="$(($#-2))"
  destinationOfBin="${@:$#}"
  #echo "combine_btN9_bins : $numberOfBinsAppended"
  #echo "\$PWD : ${PWD}"
  echo -n -e "\x0$numberOfBinsAppended" > tmp && dd if=tmp of=$theFirstBin bs=1 seek=7 count=1 conv=notrunc && rm -f tmp
  cat ${@:1:$#-1} > $destinationOfBin
}

combine_btN9_bins ab155x_patch_e2_hdr_headset.bin ab155x_patch_e2_hdr_speaker.bin ab155x_patch_e2_hdr_earbud_for_speaker.bin ab155x_patch_hdr.bin
