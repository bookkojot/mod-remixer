num=1;find . | grep -iE "(wav|mp3)$" | while read q; do ffmpeg -nostdin -i "$q" -f s8 -ar 16000 -ac 1 -y $num.bin;../strim $num.bin;let num=$num+1;done
