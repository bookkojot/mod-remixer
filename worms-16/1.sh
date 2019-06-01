num=1;find . | grep -i wav$ | while read q; do
ffmpeg -v 0 -nostdin -i "$q" -f s16le -ar 16000 -ac 2 -y $num.bin;
./strim $num.bin
let num=$num+1;done
