tiles="$(ls | grep Tile)"
montage -geometry 64x64 -tile 16x16 -background "#00000000" $tiles Atlas.png

