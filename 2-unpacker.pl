use Data::Dumper;
use Digest::MD5 qw(md5 md5_hex md5_base64);

$outdir="unpacked";

mkdir($outdir,0777);

die if ! $ARGV[0];

opendir(ddd,$ARGV[0]);
@files=map{$ARGV[0].'/'.$_}grep{/\.mod$/}readdir(ddd);

foreach $filename(@files){
open(dd,$filename);
read(dd,$file,-s(dd));

$songLen=unpack("C",substr($file,950,1));
$songLenSig=unpack("C",substr($file,951,1));
$positions=substr($file,952,128);
$max=0;
for($q=0;$q<128;$q++){
$val=unpack("C",substr($positions,$q,1));
if($val>$max){$max=$val;}
}
$offset=1084+($max+1)*4*64*4;

$songName=unpack("A20",substr($file,0,20));
print "Song name: $songName\n";
for($q=0;$q<31;$q++){

($sampleName,$sampleLen,$finetune,$volume,$loopPoint,$loopLength)=unpack("a22nCCnn",substr($file,20+$q*30,30));
#print "".(20+$q*30)." Instrument (".($q+1)."): $sampleName, tune:$finetune, len: $sampleLen -> $filename\n";
if($sampleLen>0){
$sampleLen*=2;

$data=substr($file,$offset,$sampleLen);
$offset+=$sampleLen;


#$filename=$sampleName;
#$filename=~s/[^a-z0-9_]+/_/gs;
#$filename=sprintf("%s/sample%.5d_%s.bin",$outdir,$q+1,$filename);
$filename=sprintf("%s/%s.bin",$outdir,md5_hex($data),$filename);

open(oo,">".$filename);
print oo $data;
close(oo);

}

}

}

