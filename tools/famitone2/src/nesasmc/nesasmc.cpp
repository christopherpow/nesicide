#define _CRT_SECURE_NO_WARNINGS	//to allow fopen etc be used in VC2008 Express without warnings

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>


#define ASM6	0
#define CA65	1


unsigned char *src,*out;
int src_size,out_size;
int src_ptr,dst_ptr;


void putstr(const char *str)
{
	int i;

	for(i=0;i<(int)strlen(str);i++) out[dst_ptr++]=str[i];
}



bool replace_define(const char *def,const char *replace)
{
	if(!memcmp(&src[src_ptr],def,strlen(def)))
	{
		putstr(replace);
		src_ptr+=strlen(def);
		return true;
	}

	return false;
}



int process(const char *name,int mode)
{
	char filename[1024];
	FILE *file;
	int i,ps;
	bool equ;

	src_ptr=0;
	dst_ptr=0;

	while(src_ptr<src_size)
	{
		if(src[src_ptr]==';')//comment, skip to the next row
		{
			while(src_ptr<src_size)
			{
				if(src[src_ptr]==0x0a) break;
				out[dst_ptr++]=src[src_ptr++];
			}
			continue;
		}

		if(src[src_ptr]==0x0a)//new row, check for a label
		{
			out[dst_ptr++]=src[src_ptr++];
			if(src_ptr>=src_size) break;

			if(mode==CA65)//ca65 requires .define instead of a label for a define
			{
				if(replace_define("FT_DPCM_ENABLE"  ,".define FT_DPCM_ENABLE  1")) continue;
				if(replace_define("FT_SFX_ENABLE"   ,".define FT_SFX_ENABLE   1")) continue;
				if(replace_define("FT_THREAD"       ,".define FT_THREAD       1")) continue;
				if(replace_define("FT_PAL_SUPPORT"  ,".define FT_PAL_SUPPORT  1")) continue;
				if(replace_define("FT_NTSC_SUPPORT" ,".define FT_NTSC_SUPPORT 1")) continue;
				if(replace_define("FT_PITCH_FIX"    ,"FT_PITCH_FIX = (FT_PAL_SUPPORT|FT_NTSC_SUPPORT)")) continue;
			}

			if(src[src_ptr]>=0x20&&src[src_ptr]!=' '&&src[src_ptr]!=';')//not a space, next line, tab, or a comment
			{
				if(src[src_ptr]=='.')//beginning of a local label
				{
					out[dst_ptr++]='@';//both ca65 and asm6 use @ for local labels instead of .
					src_ptr++;
				}

				if(mode==CA65)//ca65 requires : after a label (not for equ)
				{
					ps=src_ptr;
					equ=false;

					while(ps<src_size)//check if there is equ on the line
					{
						if(!memcmp(&src[ps],"equ",3)||!memcmp(&src[ps],"EQU",3)||src[ps]=='=')
						{
							equ=true;
							break;
						}
						if(src[ps]==0x0d||src[ps]==';') break;
						ps++;
					}

					if(!equ)//add : after a label if necessary
					{
						while(src_ptr<src_size)
						{
							if(src[src_ptr]<0x20||src[src_ptr]==' ') break;
							out[dst_ptr++]=src[src_ptr++];
						}
						if(src[src_ptr-1]!=':') out[dst_ptr++]=':';
					}
				}
			}
		}

		if(src[src_ptr]=='.'&&src[src_ptr-1]==' ')//label reference
		{
			out[dst_ptr++]='@';//both ca65 and asm6 use @ for local labels instead of .
			src_ptr++;
		}

		if(!memcmp(&src[src_ptr],"LOW",3))
		{
			if(mode==CA65) putstr(".lobyte");
			if(mode==ASM6) putstr("<");
			src_ptr+=3;
		}

		if(!memcmp(&src[src_ptr],"HIGH",4))
		{
			if(mode==CA65) putstr(".hibyte");
			if(mode==ASM6) putstr(">");
			src_ptr+=4;
		}

		if(src[src_ptr]=='[')//both ca65 and asm6 needs () instead of []
		{
			putstr("(");
			src_ptr++;
		}
		if(src[src_ptr]==']')
		{
			putstr(")");
			src_ptr++;
		}

		if(mode==CA65)//ca65 needs changes in .ifdef/.ifndef
		{
			if(!memcmp(&src[src_ptr],".ifndef",7))//.ifndef to .if(! )
			{
				putstr(".if(!");
				src_ptr+=7;
				while(src_ptr<src_size)
				{
					if(src[src_ptr]>0x20) break;
					src_ptr++;
				}
				while(src_ptr<src_size)
				{
					if(src[src_ptr]==' '||src[src_ptr]==0x09||src[src_ptr]<0x20) break;
					out[dst_ptr++]=src[src_ptr++];
				}
				putstr(")");
			}
			if(!memcmp(&src[src_ptr],".ifdef",6))//.ifdef to .if( )
			{
				putstr(".if(");
				src_ptr+=6;
				while(src_ptr<src_size)
				{
					if(src[src_ptr]>0x20) break;
					src_ptr++;
				}
				while(src_ptr<src_size)
				{
					if(src[src_ptr]==' '||src[src_ptr]==0x09||src[src_ptr]<0x20) break;
					out[dst_ptr++]=src[src_ptr++];
				}
				putstr(")");
			}
		}

		if(mode==ASM6)//asm6 need db,dw,byte,word without .
		{
			if(!memcmp(&src[src_ptr],".byte",5)) src_ptr++;
			if(!memcmp(&src[src_ptr],".word",5)) src_ptr++;
			if(!memcmp(&src[src_ptr],".db",3)) src_ptr++;
			if(!memcmp(&src[src_ptr],".dw",3)) src_ptr++;
		}

		if(mode==CA65)//ca65 needs byte,word
		{
			if(!memcmp(&src[src_ptr],".db",3))
			{
				src_ptr+=3;
				putstr(".byte");
			}
			if(!memcmp(&src[src_ptr],".dw",3))
			{
				src_ptr+=3;
				putstr(".word");
			}
		}

		out[dst_ptr++]=src[src_ptr++];
	}

	out_size=dst_ptr;

	strcpy(filename,name);
	i=strlen(filename);

	if(mode==CA65)
	{
		filename[i-3]='s';
		filename[i-2]=0;
	}
	if(mode==ASM6)
	{
		filename[i-4]='_';
		filename[i+0]='6';
		filename[i+1]='.';
		filename[i+2]='a';
		filename[i+3]='s';
		filename[i+4]='m';
		filename[i+5]=0;
	}

	file=fopen(filename,"wb");
	
	if(!file)
	{
		printf("Error: Can't create file\n");
		free(src);
		free(out);
		return 1;
	}

	fwrite(out,out_size,1,file);
	fclose(file);

	return 0;
}



int main(int argc,char *argv[])
{
	FILE *file;

	if(argc<2)
	{
		printf("NESASM source code converter for FamiTone2\n");
		printf("by Shiru (shiru@mail.ru) 01'14\n");
		printf("Usage: nesasmc filename.asm\n");
		return 0;
	}

	file=fopen(argv[1],"rb");

	if(!file)
	{
		printf("Error: Can't open file %s\n",argv[1]);
		return 1;
	}

	fseek(file,0,SEEK_END);
	src_size=ftell(file);
	fseek(file,0,SEEK_SET);
	src=(unsigned char*)malloc(src_size+16);
	out=(unsigned char*)malloc(src_size*2);
	fread(src,src_size,1,file);
	fclose(file);

	process(argv[1],CA65);
	process(argv[1],ASM6);

	free(src);
	free(out);

	return 0;
}

