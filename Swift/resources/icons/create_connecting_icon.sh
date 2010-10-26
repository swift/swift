set -x

all_files=
for i in 0 10 20 30 40 50 60 70 80 90 100; do
  target=connecting.$i.png
	if [[ "$i" != "0" ]]; then
		all_files="$all_files $target"
	fi
	if [[ "$i" != "100" ]]; then
    all_files_inv="$target $all_files_inv"
	fi
  cp online.png $target
  mogrify -modulate 100,$i $target
done
convert -delay 8 -background none $all_files $all_files_inv connecting.mng
rm connecting.*.png
