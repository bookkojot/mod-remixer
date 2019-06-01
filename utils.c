#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint32_t adler32(uint8_t *data, size_t len)
/*
    where data is the location of the data in physical memory and
    len is the length of the data in bytes
*/
{
    uint32_t a = 1, b = 0;
    size_t index;

    // Process each byte of the data in order
    for (index = 0; index < len; ++index)
    {
        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
}



int toHex(unsigned char *buf, int len, char *out){
char hex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
char preh[50];
char pret[16];
int chpos=0;
int addr=0;
int num;
int q,w=0;
memset(preh,' ',48);
memset(pret,0,17);
preh[48]=0;
*out=0;
for(q=0;q<len;q++){
    num=(unsigned int)buf[q];
    pret[chpos]=(num<32 || num>=127)?'.':buf[q];
    preh[chpos*3]=hex[num>>4];
    preh[chpos*3+1]=hex[num&0xf];
chpos++;
    if(chpos==16 || q==len-1){
        w+=sprintf(out+w,"%.8x: %.48s | %.16s\n",addr,preh,pret);

memset(preh,' ',48);
memset(pret,0,16);
chpos=0;
        addr=q+1;
    }
}

return(w);
}


