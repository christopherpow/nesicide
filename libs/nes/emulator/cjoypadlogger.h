#if !defined ( JOYPAD_LOGGER_H )
#define JOYPAD_LOGGER_H

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#define JOYPAD_LOGGER_DEFAULT_DEPTH 262144

#pragma pack(1)
typedef struct
{
   unsigned int  cycle;
   unsigned char data;
} JoypadLoggerInfo;
#pragma pack()

class CJoypadLogger
{
public:
   void ClearSampleBuffer ( void );
   JoypadLoggerInfo* AddSample ( unsigned int cycle, unsigned char data );
   bool ReallocateLoggerMemory ( int newDepth );
   unsigned int GetNumSamples ( void ) const
   {
      return m_samples;
   }
   JoypadLoggerInfo* GetSample ( unsigned int sample );
   void SetSample ( unsigned int sample, unsigned char data );
   JoypadLoggerInfo* GetLastSample ( void );

   CJoypadLogger();
   ~CJoypadLogger();

protected:
   unsigned int m_cursor;
   unsigned int m_samples;
   unsigned int m_sampleBufferDepth;
   JoypadLoggerInfo* m_pSampleBuffer;
};

JoypadLoggerInfo* nesGetInputSample ( int32_t port, int sample );
void nesSetInputSample ( int32_t port, JoypadLoggerInfo* sample );

#endif
