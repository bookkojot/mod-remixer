#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>


char *bank="words";

uint8_t buffer[10000000];

#pragma pack(1)
typedef struct{
uint16_t version;
uint32_t offset;
uint16_t song_length;
uint16_t restart_position;
uint16_t channels;
uint16_t patterns;
uint16_t instruments;
uint16_t flags;
uint16_t def_tempo;
uint16_t def_BPM;
uint8_t pattern_table[256];
}FILE_HEADER;

typedef struct{
uint32_t size;
uint8_t name[22];
uint8_t type;
uint16_t samples;
}INST_HEADER1;

typedef struct{
uint32_t size;
uint8_t number[96];
uint16_t volume_envelope[24];
uint16_t panning_envelope[24];
}INST_HEADER2;

typedef struct{
uint32_t sample_length;
uint32_t sample_loop_start;
uint32_t sample_loop_length;
int8_t volume;
int8_t finetune;
uint8_t flags;
int8_t panning;
int8_t relative_note_number;
uint8_t reserved;
char name[22];
}SAMPLE_HEADER;

char debug[50000];


void encode(int8_t *data, int len){
int8_t tmp=*data;
data++;
len--;
while(len--){
*data=(*data) - tmp;
tmp+=*data;
data++;
}

}

void encode16(int16_t *data, int len){
len/=2;
int16_t tmp=*data;
data++;
len--;
while(len--){
*data=(*data) - tmp;
tmp+=*data;
data++;
}

}

void decode(int8_t *data, int len){
data++;
len--;
while(len--){
*data=(*data) + (*(data-1));
data++;
}

}

void decode16(int16_t *data, int len){
len/=2;
data++;
len--;
while(len--){
*data=(*data) + (*(data-1));
data++;
}

}

typedef struct{
char *filename;
int size;
int used;
} BANKFILE;

BANKFILE bankFiles[50000];
int bankFilesPos=0;

void loadBank(){
struct dirent *dir;
struct stat st;
char filename[1024];
DIR *dd=opendir(bank);
do{
dir=readdir(dd);
if(dir && strstr(dir->d_name,".bin")){
sprintf(filename,"%s/%s",bank,dir->d_name);
stat(filename, &st);
bankFiles[bankFilesPos].filename=strdup(filename);
bankFiles[bankFilesPos].size=st.st_size;
bankFiles[bankFilesPos].used=0;
bankFilesPos++;
}
}while(dir);
printf("loaded %d files\n",bankFilesPos);
}

char *bankFind(int target){
int q,weight,min_weight=0xFFFFFF,min_index=0;
for(q=0;q<bankFilesPos;q++){
if(bankFiles[q].used){continue;}
weight=abs(bankFiles[q].size-target)+(random()%2000)-1000;
if(weight<min_weight){min_weight=weight;min_index=q;}
}
bankFiles[min_index].used=1;
return(bankFiles[min_index].filename);
}

int main(int argc, char ** argv){

loadBank();
//int t;for(t=0;t<100;t++){printf("test: %s\n",bankFind(50000));}exit(0);

FILE *f=fopen(argv[1],"rb");
int size=fread(buffer,1,sizeof(buffer),f);
fclose(f);

if(memcmp(buffer,"Extended Module: ",17)!=0 || buffer[37]!=0x1a){
fprintf(stderr,"Wrong signature\n");
exit(1);
}

FILE_HEADER *header=(FILE_HEADER*)(buffer+58);
int headerSize=60+header->offset;


printf("channels:%d,patterns:%d,instruments:%d, version: %x\n",header->channels,header->patterns,header->instruments,header->version);

uint8_t *cur=buffer+headerSize;
int q,s,i;
INST_HEADER1 *inst;;
INST_HEADER2 *inst2;
SAMPLE_HEADER *samp;
char filename[256];

for(q=0;q<header->patterns;q++){
/*
printf("-pat %d---------------\n",q);
toHex(cur,64,debug);
printf(debug);*/
if(cur[0]!=9){
printf("header size must be 9\n");
exit(1);
}
if(cur[4]!=0){
printf("header[5] must be 0\n");
exit(1);
}
int hsize=9+(cur[7]|(cur[8]<<8));
printf("skipping %d (%x) bytes\n",hsize,hsize);
cur+=hsize;
}
// patterns done
toHex(cur,64,debug);
printf(debug);

for(i=0;i<header->instruments;i++){
inst=(INST_HEADER1 *)cur;
printf("instrument: %.22s, size: %d, type: %d (must be 0), samples: %d\n",inst->name,inst->size,inst->type,inst->samples);
toHex(inst->name,22,debug);
printf(debug);
cur+=inst->size;

int total_samples=0;
uint8_t *sample_base=cur;
for(s=0;s<inst->samples;s++){

samp=(SAMPLE_HEADER *)cur;
printf(" sample: name:\"%.22s\", size: %d bytes, flags: %x, volume: %d\n",samp->name,samp->sample_length,samp->flags,samp->volume);
int is16=samp->flags&0x10;
printf("is 16: %d\n",is16);
sprintf(filename,"xm-%p.bin",cur);

int8_t *target=sample_base+sizeof(SAMPLE_HEADER)*inst->samples+total_samples;
char *newfile;
FILE *r;


if(samp->sample_length>0){
// for dump
FILE *sout=fopen(filename,"wb");
if(is16){
decode16((int16_t*)target,samp->sample_length);
} else {
decode(target,samp->sample_length);
}
fwrite(target,1,samp->sample_length,sout);
fclose(sout);
}

// for replace
if(samp->sample_length>0){
int max_volume=0,v,vv;
if(!is16){
for(v=0;v<samp->sample_length;v++){
vv=abs(target[v]);
if(vv>max_volume){max_volume=vv;}
}
}

memset(target,0,samp->sample_length);

if(!is16){
newfile=bankFind(samp->sample_length);
printf("replacement with %s\n",newfile);
r=fopen(newfile,"rb");
fread(target,1,samp->sample_length,r);
fclose(r);

int myvolume=0;
for(v=0;v<samp->sample_length;v++){
vv=abs(target[v]);
if(vv>myvolume){myvolume=vv;}
}

double mult=(double)max_volume/myvolume;
printf("my:%d, max: %d, mult: %f\n",myvolume,max_volume,mult);
for(v=0;v<samp->sample_length;v++){
int sample=(double)target[v]*mult;
if(sample<-127){sample=-127;}
if(sample>127){sample=127;}
target[v]=sample;
if(vv>myvolume){myvolume=target[v];}
}


encode(target,samp->sample_length);
} else {
newfile=bankFind(samp->sample_length);
printf("replacement with %s\n",newfile);
r=fopen(newfile,"rb");
fread(target,1,samp->sample_length,r);
fclose(r);
encode16((int16_t*)target,samp->sample_length);
}

memset(samp->name,0,22);
strncpy(samp->name,newfile,22);

//samp->volume=1;
}
//toHex(cur,512,debug);
//rintf(debug);

cur+=sizeof(SAMPLE_HEADER);
total_samples+=samp->sample_length;
//toHex(cur,512,debug);
//printf(debug);

}
cur+=total_samples;

}

FILE *fout=fopen("patched.xm","wb");
fwrite(buffer,1,cur-buffer,fout);
fclose(fout);

toHex(cur+0x111+4-14,512,debug);
toHex(cur,512,debug);
printf(debug);

return 0;
}


