FamiTone2 audio library v1.15 by Shiru 04'17



About

FamiTone2 is an audio library for NES/Famicom. It is designed to provide 
easy way of adding music and sound effects into homebrew games and 
demos. 

Tool set supplied along with the library allows to create both music and 
sound effects using FamiTracker, a popular tracker, and export them into 
the library's internal formats. It supports all standard channels of 
2A03: two pulse, triangle, noise, and DPCM. Expansion sound chips are 
not supported. 

The main concept of the library is to have very minimalistic set of 
features that is sufficient to create music and sound effects with the 
level of quality comparable with that of the most old commercial NES 
games. It is in no way a direct replacement of FamiTracker player, it 
only supports a very limited subset of its features in order to reduce 
CPU load and memory use. That's the whole point of having a separate
library, so if you need more features, you should stick to the native
FamiTracker's driver instead.

The library is open source and released into the Public Domain. You can 
use it for any purpose without restrictions: redistribute, modify, use 
for commercial and non-commercial projects. You may credit the author, 
inform the author about your project, donate, etc, but nothing is 
obligatory, you could do it at free will. Obviously, the library 
provided AS IS without warranties of any kind, the author is not 
responsible for anything. 




How to create music

You can create music in FamiTracker version 0.3.6 or above, or with 
Musetracker. The music should met these requirements: 

- Allowed notes are C-1..D-6, Note Cut (rest note)
- Only Volume, Arpeggio, and Pitch sequences (no Pitch for noise
  channel) supported
- All instruments should have a volume envelope assigned
- Only the loop part of sequence is supported, release part is not supported
- Pitch envelope accumulate the offset at conversion time only, thus
  it can't go further -64..63, and can't accumulate the offset with loop
- Only first step of Duty envelope for pulse and noise channels is used,
  there is no Duty sequence
- 64 instruments max
- No volume column support
- Tempo is set with the global setting, speed can be changed with effect
  Fxx (1..21)
- Bxx effect for looping without an intro, does not support forward
  references
- D00 effect to cut patterns early
- No effects other than listed above are supported
- Up to 17 sub songs in a file, they will share instruments and pattern
  data

You can use DPCM samples, there are limitations too:

- Up to 63 samples with total size of ~16K
- Only samples assigned to the instrument 0 are supported,
- Note range the same as for normal instruments, C-1..D-6, instrument
  number is ignored
- You can add samples that aren't used in the music to use them as sound
  effects


You can create all music for your game in a single FTM file using the 
multi song feature, this will save memory. If allowed number of 
instruments or sub songs is not enough, you can split the music into few 
files. The library allows to change music data sets at runtime. 

After you have made the music, you need to export it in a text format 
supported by the FamiTone2 converter, text2data. 

In FamiTracker 0.4.2 or above you can use built-in text exporter, it is 
located in File\Export text. This export option is also available from 
the command line (FamiTracker music.ftm -export music.txt). This allows 
seamless integration of music conversion into your build process. The 
old text format exported with TextExporter.dll plug-in is also 
supported, but not recommended to use. 

In Musetracker you can just save it in needed format with LCtrl+LShift+S 
(by default). 

Now you can use text2data tool (text2data music.txt) to convert the 
exported text into assembly file with music data. If you don't use DPCM, 
you'll get one *.asm file as the result. If you use DPCM, there will be 
an extra *.dmc file with sample data. 

text2data outputs data in NESASM format by default, you can use -ca65 or 
-asm6 switches to get output for these assemblers. 



How to create sound effects

Sound effects could be created in FamiTracker or Musetracker as well. 
Effects can use all sound channels except DPCM, so the effects are multi 
channel. Put every effect into a separate sub song using the multi song 
feature. Unlike music, you can use most of the FamiTracker features for 
effects, except the sweep. Every effect should be finished with silence, 
and C00 command put to the last row of the effect.

Once all effects done, export them as a NSF file and use the nsf2data
converter (nsf2data sounds.nsf). You'll get *.asm file with the effects
data. 

There is a major limitation: one effect can't be larger than 255 bytes 
after conversion. FamiTracker effects like slides could increase the 
size greatly, so use them carefully. The converter will report size of 
each effect, and warn you if an effect is too large. 

In addition to these sound effects, you can play any sample,meither used 
or not used in the music, from the current DPCM bank. It will have 
priority over samples in the music, overriding them. 




How to use the library

The main version of the FamiTone2 is written in NESASM. There are 
versions for CA65 and ASM6 assemblers included into the package as well, 
they are automatically converted from the main version using a custom 
tool, nesasmc, also included into the archive. 

Include famitone.asm (famitone.s for CA65, famitone_asm6.asm for ASM6) 
into your project. Include *.asm files of your music and sounds. You can 
place music and sound effects data anywhere, including bankswitched 
memory, they only needed to be switched into the CPU address space 
before FamiTone2 calls. DPCM samples (*.bin file) are a special case. 
They should be placed at $c000 or above, with steps of 64 bytes, and 
they always should be in CPU address space to be accessible by the DPCM 
hardware. 

Next step is to setup the FamiTone. Open famitone.asm, there are few 
defines commented out in the beginning of the file. Copy them into your
code and uncomment. Now you use them to set up RAM and zero page 
locations, DPCM location, number of sound effects played at once, 
PAL/NTSC support. The latter has three options: both NTSC and PAL, NTSC 
only, or PAL only. The first case enables automatic pitch and correction 
to make the music and sound effects play the same. This is a little bit 
slower and uses a bit more ROM. Sound effects data is stored in two 
versions, for NTSC and PAL, there is a switch to use only one system to 
reduce ROM usage. 


User calls of the library:

 FamiToneInit
  A is 0 for PAL, not 0 for NTSC. X,Y are pointer to music data. The music
  data includes instruments, envelopes, sample list, and a number of sub
  songs. Call this routine once before all other calls, and every time you
  need to change music data set.

 FamiToneSfxInit
  X,Y are 16-bit address of sound effects data, one that is generated by
  the nsf2data. Call it after FamiToneInit every time you need to change
  sound effects set.

 FamiToneUpdate
  No parameters. Updates everything and writes to APU registers. Call it
  every TV frame. Usually this done in the end of the NMI handler.

 FamiToneMusicPlay
  Play music. A is number of a sub song.

 FamiToneMusicStop
  No parameters, stops any music

 FamiToneMusicPause
  A is 0 or not 0, to play or pause music that is playing currently

 FamiToneSfxPlay
  A is sound effect number 0..63, X is FT_SFX_CH0..FT_SFX_CH3 (no other
  values are allowed!). The number in X is sound effect stream. It also
  works like priority. If you start a sound effect, it will stop any
  previous effect with the same priority. There is no automatic priority
  system in FamiTone2, as it would be slow.

 FamiToneSamplePlay
  A is sample number 1..12 (i.e. note C..B with assigned sample). Play
  a sample from active DPCM bank, interrupting any other sample.


Warning: don't forget that active DPCM sound conflicts with $2002 and 
controllers polling. You should poll controllers three times in a row, 
then use matching result. 

With music and few sound effects playing at once you can expect CPU load 
about 11%. It could be reduced by disabling some of sound effect 
channels with the FT_SFX_STREAMS define. 

You can exclude sound effects and DPCM support from code through defines 
FT_SFX_ENABLE and FT_DPCM_ENABLE (not define them to disable the 
functionality). Be aware that speed change commands often placed into 
the DPCM channel by the text2data tool, they'll be missed by player if 
you disable DPCM support. To avoid this situation use text2data with 
-ch4 switch. 

If you are going to put sound update into an interrupt (NMI or other) 
and call sound effects from the main code, make sure that FT_THREAD is 
defined. 




Comparisons

The numbers below could give you idea what to expect in terms of 
resources use, and what could suit better for certain application. 

FamiTracker 0.3.7 and 0.4.2 are listed here because these were actual 
versions when the original FamiTone and FamiTone2 has been released. 
FamiTone has all options enabled, including four sound effect channels. 
FamiTracker player compiled without banking and/or sound expansion 
chips, the size is provided by the NSF exporter for tracks. 

CPU load is measured for peak time. Average time is usually about 2-3 
times less, but the load peaks occurs at various points during play 
rather than on initializing, this could introduce random lag frames in a 
game that tops CPU frame time. Thus peak load considered more important 
than average load. 

The measurements below were done with FamiTone2 v1.0, the numbers may
be slightly different for newer versions.


Music data sizes (not including DPCM), bytes:

                            FT 0.3.7  FT 0.4.6  FamiTone  FamiTone2
							
After The Rain                7847      6059      6315      4676
Danger Streets                4825      4840      5522      4354
Lan Master (4 sub songs)     12373     10062     12072      7753
Lawn Mower (5 sub songs)      9248      6281      8594      4514

Memory usage by player, bytes:

               RAM     ZP     ROM
FT 0.3.7:      241     21    5128
FT 0.4.2:      245     20    5547
FamiTone:      183      7    1493
FamiTone2:     186      3    1636

CPU time peak usage by player, cycles:

                            FT 0.3.7  FT 0.4.2  FamiTone  FamiTone2
							
After The Rain                6946      7439      3673      3103
Danger Streets                5678      6020      3439      2827



Data formats

Music channel streams:

%0nnnnnn0 is a note (0 rest note, 1..60 are C-1..D-6)
%0nnnnnn1 is a note followed by a single empty row
%1iiiiii0 is an instrument number 0..63 (actual number minus one,
          default is 1)
%1rrrrrr1 is a number of empty rows-1 (0..60), or a special tag

%11111011 is speed, next byte is speed
%11111101 is end of the stream, two next bytes are loop pointer
%11111111 is a reference, three next bytes are number of rows (excluding
          empty ones) and pointer

There are no octaves in the Noise channel, notes codes are always in 
1..16 range. There is no instrument numbers in the DPCM channel.



Envelopes:

%00000000 is end of the envelope, next byte is loop offset
%0rrrrrrr is a number of repeats of previous output value
%1nnnnnnn is output value + 192 (it is in -64..63 range)

Envelope data can't be longer than 255 bytes.



Sound effects:

In a sound effect bank first there is two pointers to list of the effect
pointers, one for NTSC and another for PAL. Then there are lists of
pointers to effects itself.

$00      is end of the effect
$01..$7f is a number of frames to skip before reading next block of data
$80..$8a is a remapped register number followed by a byte of data

Sound effect data can't be longer than 255 bytes. Registers are remapped:

$4000 $80
$4002 $81
$4003 $82
$4004 $83
$4006 $84
$4007 $85
$4008 $86
$400a $87
$400b $88
$400c $89
$400e $8a



Thanks to

jsr (FamiTracker)
Gradualore (plug-in system for FamiTracker)
rainwarrior (bug reports, built-in text export in FamiTracker)
Memblers (library's name)
kevtris
Gil
B00daW
ManicGenius
thefox
UncleSporky
kulor
MovieMovies1
Erik Gough
Lauri Kasanen
Denny R. Walter
NesDev community



List of products using FamiTone

Is the library actually any good? You can find out the answer by
hearing it in the action in these homebrew games:

2048 (by tsone)
Alter Ego
BASIC Championship Wrestling
Beerslinger
Blow 'em Out
Chase
Cheril the Goddess
Crypto
Filthy Kitchen
Flappy Bird (by Nioreh)
Flappy Jack (modified FamiTone2)
Function
Germ Squashers
Gorodki
Jet Paco
Karate Kick
Lan Master
Lawn Mower
Melanchony Of Existance: Chapter 0
Milionesy
MineShaft
Mini Brix Battle
MultiDude
Nebs 'n Debs
Perfect Pair
Rock Paper Scissors Lizard Sbock
Russian Roulette
Quest Forge: By Order of Kings
Ralph 4
Sgt. Helmet - Training Day
Sinking Feeling
Sir Ababol
Snakky
Spacey McRacey
SplatooD
Star Versus
Super Painter
Super Uwol!
Tailgate Party
Waddles The Duck
Wǒ Xiang Niào Niào
Yun
Zombiejack
Zooming Secretary




Contacts

mailto:shiru@mail.ru
http://shiru.untergrund.net/
http://justgiving.com/Shiru