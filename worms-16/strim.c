#include <stdio.h>
#include <stdint.h>

int16_t buffer[100000];


int main(int argc, char ** argv){


FILE *f=fopen(argv[1],"rb");
int size=fread(buffer,2,sizeof(buffer),f);
fclose(f);

int q;
int s=0,e=size-1;
while(abs(buffer[s])<1500){s++;}
while(abs(buffer[e])<1500){e--;}

int max=0,v;
for(q=s;q<=e;q++){
v=abs(buffer[q]);
if(v>max){max=v;}
}

double mult=32767.0/max;
fprintf(stderr,"max volume: %d, mult: %f\n",max,mult);

for(q=s;q<=e;q++){
buffer[q]=((double)buffer[q]*mult);
if(v>max){max=v;}
}


f=fopen(argv[1],"wb");
fwrite(buffer+s,2,e-s,f);
fclose(f);

return 0;
}

