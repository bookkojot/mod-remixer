open(dd,$ARGV[0]);
read(dd,$file,-s(dd));

$songName=unpack("A20",substr($file,0,20));
print "Song name: $songName\n";
for($q=0;$q<31;$q++){

($sampleName,$sampleLen,$finetune,$volume,$loopPoint,$loopLength)=
unpack("A22nCCSS",substr($file,20+$q*30,30));

$filename=$sampleName;
$filename=~s/[^a-z0-9_]+/_/gs;
$filename=sprintf("sample%.5d_%s.bin",$q+1,$filename);

print "".(20+$q*30)." Instrument (".($q+1)."): $sampleName, len: $sampleLen -> $filename\n";

push(@samples,[$filename,$sampleLen*2]);
$samplesSum+=$sampleLen*2;
}


$songLen=unpack("C",substr($file,950,1));
$songLenSig=unpack("C",substr($file,951,1));
#if($songLenSig!=127){die "?$songLenSig != 127";}
$positions=substr($file,952,128);
$max=0;
for($q=0;$q<128;$q++){
$val=unpack("C",substr($positions,$q,1));
if($val>$max){$max=$val;}
}
print "Max: $max, song len: $songLen, total $samplesSum bytes in samples\n";

$offset=1084+($max+1)*4*64*4;
$offsetReal=(-s(dd))-$samplesSum;
print "offsets: $offset / $offsetReal\n";

foreach(@samples){
($filename,$len)=@{$_};
if($len){
print "Ripping $filename, $len bytes...\n";
open(oo,">".$filename);
print oo substr($file,$offset,$len);
close(oo);
`ffmpeg -v 0 -f s8 -ar 16000 -ac 1 -i "$filename" -ar 44100 -ac 1 -y "$filename.wav"`;

$inname="K/".(++$count).".bin";
print "Reading sample $inname\n";
open(ii,$inname) or die "Can't read $inname";
read(ii,$sample,-s(ii));
$sample=$sample x 10;

substr($file,$offset,$len)=substr($sample,0,$len);

$offset+=$len;

}
}


open(oo,">patched.mod");
print oo $file;
close(oo);
