char* message = "HELLO WORLD";

unsigned char palette[4] = { 0x17, 0x38, 0x38, 0x38 };

#define PPUCTRL (unsigned char*)0x2000
#define PPUMASK (unsigned char*)0x2001
#define PPUSTAT (unsigned char*)0x2002
#define PPUADDR (unsigned char*)0x2006
#define PPUDATA (unsigned char*)0x2007

void waitForVblank(void)
{
   for ( ; ; ) 
   {
      if ( (*PPUSTAT)&0x80 )
      {
         break;
      }
   }
   return;
}

void main(void)
{
   int i;
   unsigned char* pmsg = message;
   
   // enable vblank
   (*PPUCTRL) = (unsigned char)0x90;
   
   // change palette during vblank
   waitForVblank();
   
   // set monochrome palette
   (*PPUADDR) = (unsigned char)0x3F;
   (*PPUADDR) = (unsigned char)0x00;
   
   for ( i = 0; i < 8; i++ )
   {
      (*PPUDATA) = (*(palette+0));
      (*PPUDATA) = (*(palette+1));
      (*PPUDATA) = (*(palette+2)); 
      (*PPUDATA) = (*(palette+3));
   }
   
   // write message
   (*PPUADDR) = (unsigned char)0x20;
   (*PPUADDR) = (unsigned char)0x42;
   
   while ( *pmsg )
   {
      (*PPUDATA) = (*pmsg);
      pmsg++;
   }
   
   // turn on rendering
   (*PPUMASK) = (unsigned char)0x18; 
   
	while(1)
	{
      (*PPUCTRL) = 0x90;
	}
}
