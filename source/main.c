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

// this is
// VERSION 1.8d

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "mas.h"
#include "mod.h"
#include "s3m.h"
#include "xm.h"
#include "it.h"
#include "gba.h"
#include "nds.h"
#include "files.h"
#include "errors.h"
#include "simple.h"
#include "msl.h"
#include "systems.h"

extern void kiwi_start(void);

int target_system;

bool ignore_sflags;
int PANNING_SEP;

int number_of_inputs;

#ifdef SUPER_ASCII
/*#define USAGE "\
浜様様様様様様様様様様様融\n\
� MAXMOD UTILITY v1.8d \x02 �\n\
把陳陳陳陳陳陳陳陳陳陳陳槻陳陳跳\n\
� Usage: mutil [options] input �\n\
把陳陳陳陳陳陳陳陳陳陳陳陳陳陳超\n\
�  \x0e Supports MOD/S3M/XM/IT \x0e  �\n\
把陳陳陳陳陳賃陳陳陳陳陳陳陳陳槻陳陳陳�\n\
� Option     � Description            �\n\
把陳陳陳陳陳津陳陳陳陳陳陳陳陳陳陳陳陳�\n\
� -d         � NDS Mode               �\n\
� -b         � ROM output (GBA/NDS)   �\n\
� -m         � MAS output             �\n\
� -i         � Ignore sample flags    �\n\
� -p<0-9>    � Set panning separation �\n\
� -v         � Verbose output         �\n\
� -o<output> � Specify output file    �\n\
� -h<header> � Specify header file    �\n\
藩様様様様様溶様様様様様様様様様様様様�\n"*/
#define USAGE "\
\n\
浜様様様様様様様様様様様融       浜様様様様様様様様様様様様様様融\n\
� Maxmod Utility v1.8d \x2 紺�     �                              紺�\n\
把陳陳陳陳陳陳陳陳陳陳陳槻陳陳跳 �   (C) 2008 Mukunda Johnson   紺�\n\
� Usage: mmutil [option] input 紺�                              紺�\n\
把陳陳陳陳陳陳陳陳陳陳陳陳陳陳超尉陳陳陳卍陳陳陳陳陳陳陳陳陳陳陳彊�\n\
�  \xe Supports MOD/S3M/XM/IT \xe  紺旭旭旭虻                       紺�\n\
把陳陳陳陳陳賃陳陳陳陳陳陳陳陳槻陳陳陳薫� This utility is dis-  紺�\n\
� Option     � Description            紺� tributed under a BSD  紺�\n\
把陳陳陳陳陳津陳陳陳陳陳陳陳陳陳陳陳陳彊� license. See COPYING  紺�\n\
� -d         � NDS Mode               紺� for terms of use.     紺�\n\
� -b         � ROM output (GBA/NDS)   紺�                       紺�\n\
� -m         � MAS output             紺� Source code is avail- 紺�\n\
� -i         � Ignore sample flags    紺� able with the Maxmod  紺�\n\
� -p<0-9>    � Set panning separation 紺� distribution.         紺�\n\
� -v         � Verbose output         紺�                       紺�\n\
� -o<output> � Specify output file    紺�    www.maxmod.org     紺�\n\
� -h<header> � Specify header file    紺�                       紺�\n\
藩様様様様様溶様様様様様様様様様様様様式藩様様様様様様様様様様様式�\n\
  旭旭旭旭旭旭旭旭旭旭旭旭旭旭旭旭旭旭旭� 旭旭旭旭旭旭旭旭旭旭旭旭�\n\
\n"

#else
#define USAGE "\
MAXMOD UTILITY v1.8d\n\
Usage: mutil [options] input\n\
  Supports MOD/S3M/XM/IT\n\
  \n\
  Options\n\
  -d		 NDS Mode\n\
  -b		 ROM output (GBA/NDS)\n\
  -m		 MAS output\n\
  -i		 Ignore sample flags\n\
  -p<0-9>	 Set default panning separation (used in mod/s3m)\n\
  -v		 Verbose output\n\
  -o<output>	 Specify output file\n\
  -h<header>	 Specify header file\n"

#endif

void print_usage( void )
{
	printf( USAGE );
}

void print_error( int err )
{
	switch( err )
	{
	case ERR_INVALID_MODULE:
		printf( "Invalid module!\n" );
		break;
	case ERR_MANYARGS:
		printf( "Too many arguments!\n" );
		break;
	case ERR_NOINPUT:
		printf( "No input file!\n" );
		break;
	case ERR_NOWRITE:
		printf( "Unable to write file!\n" );
		break;
	case ERR_BADINPUT:
		printf( "Cannot parse input filename!\n" );
		break;
	}
}

int GetYesNo( void )
{
	char c = 0;
	c = tolower(getchar());
	while( getchar() != '\n' );
	while( c != 'y' && c != 'n' )
	{
		printf( "Was that a yes? " );
		c = tolower(getchar());
		while( getchar() != '\n' );
	}
	return c == 'y' ? 1 : 0;
}

int main(int argc, char* argv[])
{
	char* str_input=NULL;
	char* str_output=NULL;
	char* str_header=NULL;
	
	MAS_Module mod;

	int strl;

	int input_type;

	int strp;
	int strpi;

	bool g_flag=false;
	bool v_flag=false;
	bool m_flag=false;
	bool z_flag=false;
	int a;

	int output_size;

	ignore_sflags=false;

	number_of_inputs=0;

	PANNING_SEP = 128;
	
	for( a = 1; a < argc; a++ )
	{
		if( argv[a][0] == '-' )
		{
			if( argv[a][1] == 'b' )
				g_flag=true;
			else if( argv[a][1] == 'v' )
				v_flag=true;
			else if( argv[a][1] == 'd' )
				target_system = SYSTEM_NDS;
			else if( argv[a][1] == 'i' )
				ignore_sflags=true;
			else if( argv[a][1] == 'p' )
				PANNING_SEP = ((argv[a][2] - '0') * 256)/9;
			else if( argv[a][1] == 'o' )
				str_output = argv[a]+2;
			else if( argv[a][1] == 'h' )
				str_header = argv[a]+2;
			else if( argv[a][1] == 'm' )
				m_flag=true;
			else if( argv[a][1] == 'z' )
				z_flag=true;
		}
		else if( !str_input )
		{
			str_input = argv[a];
			number_of_inputs=1;
		}
		else
		{
			number_of_inputs++;
		}
	}

	if( z_flag )
	{
		kiwi_start();
		
	}
	
	if( number_of_inputs==0 )
	{
		print_usage();
		return 0;
	}

	if( m_flag || g_flag )
	{
		if( number_of_inputs != 1 )
		{
			printf( "Too much input! Only one file is allowed in this mode.\n" );
			return -1;
		}
		else
		{
			if( !str_output )
			{
				if( strlen(str_input) < 4 )
				{
					print_error( ERR_BADINPUT );
					return -1;
				}
				strp = strlen(str_input);
				str_output = (char*)malloc( strp+2 );
				memcpy( str_output, str_input, strlen(str_input) );
				strp=strlen(str_output)-1;
				
				for( strpi=strp; str_output[strpi] != '.' && strpi != 0; strpi-- );
				if( strpi == 0 )
				{
					print_error( ERR_BADINPUT );
					return -1;
				}
				
				str_output[strpi++] = '.';
				if( !g_flag )
				{
					str_output[strpi++] = 'm';
					str_output[strpi++] = 'a';
					str_output[strpi++] = 's';
					str_output[strpi++] = 0;
				}
				else
				{
					if( target_system == SYSTEM_GBA )
					{
						str_output[strpi++] = 'g';
						str_output[strpi++] = 'b';
						str_output[strpi++] = 'a';
						str_output[strpi++] = 0;
					}
					else if( target_system == SYSTEM_NDS )
					{
						str_output[strpi++] = 'n';
						str_output[strpi++] = 'd';
						str_output[strpi++] = 's';
						str_output[strpi++] = 0;
					}
					else
					{
						// error!
					}
				}
				str_output[strpi++] = 0;
			}
		}
	}
	else
	{
		if( !str_output )
		{
			printf( "No output file! (-o option)\n" );
			return -1;
		}
	}
	
	// check the extension of the filename
	strl=strlen(str_input);
	if( strl < 4 )
	{
		print_error( ERR_BADINPUT );
		return -1;
	}
	input_type = get_ext( str_input );
	
	if( g_flag || m_flag )
	{
		if( file_open_read( str_input ) )
		{
			// file not found
			print_error( ERR_NOINPUT );
			return -1;
		}
		
		switch( input_type )
		{
		case INPUT_TYPE_MOD:
			if( Load_MOD( &mod, v_flag ) )
			{
				print_error( ERR_INVALID_MODULE );
				file_close_read();
				return -1;
			}
			break;
		case INPUT_TYPE_S3M:
			if( Load_S3M( &mod, v_flag ) )
			{
				print_error( ERR_INVALID_MODULE );
				file_close_read();
				return -1;
			}
			break;
		case INPUT_TYPE_XM:
			if( Load_XM( &mod, v_flag ) )
			{
				print_error( ERR_INVALID_MODULE );
				file_close_read();
				return -1;
			}
			break;
		case INPUT_TYPE_IT:
			if( Load_IT( &mod, v_flag ) )
			{
				// ERROR!
				print_error( ERR_INVALID_MODULE );
				file_close_read();
				return -1;
			}
			break;
		case INPUT_TYPE_WAV:
			// ...
			break;
		}
		
		file_close_read();
		
		if( file_exists( str_output ) )
		{
			printf( "Output file exists! Overwrite? (y/n) " );
			if( !GetYesNo() )
			{
				printf( "Operation Canceled!\n" );
				return -1;
			}
			
		}
		
		if( file_open_write( str_output ) )
		{
			print_error( ERR_NOWRITE );
			return -1;
		}

		if( g_flag )
		{
			if( target_system == SYSTEM_GBA )
			{
				if( v_flag )
					printf( "Writing .GBA.......\n" );
				Write_GBA();
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
			}
			else if( target_system == SYSTEM_NDS )
			{
				if( v_flag )
					printf( "Writing .NDS.......\n" );
				Write_NDS();
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
				write32(BYTESMASHER);
			}
		}
		else
		{
			printf( "Writing .MAS...........\n" );
		}

		// output MAS
		output_size = Write_MAS( &mod, v_flag, false );
		
		file_close_write();

		if( g_flag && target_system == SYSTEM_NDS )
			Validate_NDS( str_output, output_size );

		Delete_Module( &mod );
		if( v_flag )
		{
#ifdef SUPER_ASCII
			printf( "Success! \x02 " );
#else
			printf( "Success! :) " );
#endif
			if( g_flag && target_system == SYSTEM_GBA )
			{
				if( output_size < 262144 )
				{
					printf("ROM can be multibooted!\n" );
				}
				else
				{
					printf( "\n" );
				}
			}
			else
			{
				printf( "\n");
			}
		}
	}
	else
	{
		MSL_Create( argv, argc, str_output, str_header, v_flag );
	}
	return 0;
}
