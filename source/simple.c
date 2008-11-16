/*-----------------------------------------------------------------------------------------
Copyright (c) 2007, Mukunda Johnson

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the owners nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-----------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "defs.h"
#include "mas.h"
#include "simple.h"
#include "samplefix.h"

// OMG CREDITS TO CODA, LOOK AT THIS AWESOME COED
u32 readbits(u8* buffer, unsigned int pos, unsigned int size) {
	u32 result=0;
	u32 i;
	
	for(i=0;i<size;i++) {
		u32 byte_pos;
		u32 bit_pos;
		byte_pos = (pos+i)>>3;
		bit_pos = (pos+i)&7;
		result |= ( (buffer[byte_pos] >> bit_pos) & 1 ) << i;
	}
	return result;
}

int get_ext( char* filename )
{
	int strl = strlen(filename);
	int x;
	u32 a=0;
	if( strl < 4 )
		return INPUT_TYPE_UNK;
	for( x = 0; x < 4; x++ )
	{
		if( filename[strl-x-1] != '.' )
		{
			a |= tolower(filename[strl-x-1]) << (x*8);
		}
		else
			break;
	}
	//a = tolower( filename[strl-1] ) | (tolower( filename[strl-1] )<<8) | tolower( filename[strl-2]<<16 ) | tolower( filename[strl-3]<<24 );
	switch( a )
	{
	case 'mod':
		return INPUT_TYPE_MOD;
	case 's3m':
		return INPUT_TYPE_S3M;
	case 'txt':
		return INPUT_TYPE_TXT;
	case 'wav':
		return INPUT_TYPE_WAV;
	case 'msl':
		return INPUT_TYPE_MSL;
	case 'xm':
		return INPUT_TYPE_XM;
	case 'it':
		return INPUT_TYPE_IT;
	case 'h':
		return INPUT_TYPE_H;
	}
	return INPUT_TYPE_UNK;
}

u32 calc_samplen_ex2( Sample* s )
{
	if( s->loop_type == 0 )
	{
		return s->sample_length;
	}
	else
	{
		return s->loop_end;
	}
}

u32 calc_samplooplen( Sample* s )
{
	u32 a;
	if( s->loop_type == 1 )
	{
		a = s->loop_end - s->loop_start;
		return a;
	}
	else if( s->loop_type == 2 )
	{
		a = (s->loop_end-s->loop_start) *2;
		return a;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}

u32 calc_samplen( Sample* s )
{
	if( s->loop_type == 1 )
	{
		return s->loop_end;
	}
	else if( s->loop_type == 2 )
	{
		return (s->loop_end-s->loop_start)+s->loop_end;
	}
	else
	{
		return s->sample_length;
	}
}

u8 sample_dsformat( Sample* samp )
{
	if( samp->format & SAMPF_COMP )
	{
		return 2;
	}
	else
	{
		if( samp->format & SAMPF_SIGNED )
		{
			if( samp->format & SAMPF_16BIT )
				return 1;
			else
				return 0;
		}
		else
		{
			if( !(samp->format & SAMPF_16BIT) )
				return 3;
			else
				return 3; // error
		}
	}
}

u8 sample_dsreptype( Sample* samp )
{
	if( samp->loop_type )
		return 1;
	else
		return 2;
}

int clamp_s8( int value )
{
	if( value < -128 ) value = -128;
	if( value > 127 ) value = 127;
	return value;
}

int clamp_u8( int value )
{
	if( value < 0 ) value = 0;
	if( value > 255 ) value = 255;
	return value;
}

