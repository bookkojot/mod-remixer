open(dd,$ARGV[0]);
read(dd,$file,-s(dd));

($idText,$songName,$la,$trackerName,$version,$headerSize,$songLength,$restartPosition,
$numberChannels,$numberPatterns,$numberInstruments,$flags,
$defaultTempo,$defaultBPM,=unpack("a17",substr($file,0,20));
print "Song name: $songName\n";
for($q=0;$q<31;$q++){

($sampleName,$sampleLen,$finetune,$volume,$loopPoint,$loopLength)=
unpack("a22nCCnn",substr($file,20+$q*30,30));

print "".(20+$q*30)." Instrument (".($q+1)."): $sampleName, tune:$finetune, len: $sampleLen -> $filename\n";

$samplesSum+=$sampleLen*2;

if($sampleLen>0){

$filename=$sampleName;
$filename=~s/[^a-z0-9_]+/_/gs;
$filename=sprintf("sample%.5d_%s.bin",$q+1,$filename);

push(@samples,[$filename,$sampleLen*2]);

$inname="bank-ga/".($q+1).".bin";
print "Reading sample $inname\n";
open(ii,$inname);# or die "Can't read $inname";
read(ii,$sample,-s(ii));

$sampleName=$inname;
$sampleLen=int(length($sample)/2);
$finetune=0;
$loopPoint=0;
$loopLength=$sampleLen*0;

$newsamplestable.=pack("a22nCCnn",$sampleName,$sampleLen,$finetune,$volume,$loopPoint,$loopLength);
$newsamples.=substr($sample,0,$sampleLen*2);
}

}


$newsamplestable.="\x00" x (30*31-length($newsamplestable));

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

open(oo,">patched.mod");
print oo substr($file,0,20).$newsamplestable.substr($file,950,$offset-950).$newsamples;
close(oo);

foreach(@samples){
($filename,$len)=@{$_};
if($len){
print "Ripping $filename, $len bytes...\n";
open(oo,">".$filename);
print oo substr($file,$offset,$len);
close(oo);
`ffmpeg -v 0 -f s8 -ar 16000 -ac 1 -i "$filename" -ar 44100 -ac 1 -y "$filename.wav"`;
$offset+=$len;

}
}


