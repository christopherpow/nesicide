// cnes.h: interface for the CNES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( NES_H )
#define NES_H

class CNES  {
public:
	CNES();
	virtual ~CNES();

   static bool RUN ( unsigned char* joy );
   static void REPLAY ( bool enable ) { m_bReplay = enable; }
   static bool REPLAY () { return m_bReplay; }
   static unsigned int FRAME () { return m_frame; }

   static void RESET ( void );

protected:
   static bool         m_bReplay;
   static unsigned int m_frame;
};

#endif // !defined(AFX_NES_H__075DAA92_13A4_4462_9903_FBD07E97E908__INCLUDED_)
