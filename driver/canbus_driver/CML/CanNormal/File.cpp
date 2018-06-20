/********************************************************/
/*                                                      */
/*  Copley Motion Libraries                             */
/*                                                      */
/*  Copyright (c) 2010 Copley Controls Corp.            */
/*                     http://www.copleycontrols.com    */
/*                                                      */
/********************************************************/

/***************************************************************************/
/** \file
  This file contains code used to parse CME-2 type files.
  */
/***************************************************************************/

#include "CML.h"

#ifdef CML_FILE_ACCESS_OK
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif


CML_NAMESPACE_START()

#ifdef CML_FILE_ACCESS_OK
/***************************************************************************/
/**
  Read a string from the passed file.  The string is read until the EOF or end
  of line is encountered.  If the line is too long to fit in the passed buffer,
  the line will be clipped, but the fill will be read until the EOL and any 
  extra data will be lost.
  @param fp The file pointer of an open file
  @param buff The buffer where the data will be returned
  @param max The size of the buffer.
  @return A pointer to an error object, or NULL on success.
  */
/***************************************************************************/
const Error *ReadLine( void *ptr, char *buff, int max )
{
   FILE *fp = (FILE *)ptr;

   for( int i=0; ; )
   {
      int c = getc(fp);

      if( c == EOF || c == '\n' )
      {
	 buff[i] = 0;
	 return 0;
      }

      if( i < (max-1) )
	 buff[i++] = c;
   }
}


/***************************************************************************/
/**
  Split the passed string into segments delimited by a character.
  This also strips white space at the start and end of each segment.
  @param buff The string to split.  This should end with a terminating zero.
  @param seg An array of character pointers which will point to the split
  line segments on return
  @param max The maximum number of segments to split the line into.
  @param delim The delimiter (comma by default)
  @return The number of segments the line was split into
  */
/***************************************************************************/
int SplitLine( char *buff, char **seg, int max, char delim )
{
   int i;
   for( i=0; i<max && *buff; i++ )
   {
      // Skip leading white space
      while( isspace(*buff) )
	 buff++;

      if( i==0 && *buff==0 )
	 break;

      // Grab start of next segment
      seg[i] = buff;

      // Find end
      while( *buff && *buff != delim )
	 buff++;

      // Strip trailing white space
      for( char *ptr = buff-1; ptr >= seg[i] && isspace(*ptr); *ptr-- = 0 ){}

      // End segment
      if( *buff == delim )
	 *buff++ = 0;
   }
   return i;
}

/***************************************************************************/
/**
  Convert an ASCII string into a 32-bit integer value.
  @param str The string to convert
  @param i The integer value will be returned here
  @param base The base to use during the conversion
  @return An error pointer or NULL on success
  */
/***************************************************************************/
const Error *StrToInt32( char *str, int32 &i, int base )
{
   char *end;
   i = (int32)strtol( str, &end, base );

   if( *end )
      return &AmpFileError::format;
   return 0;
}

/***************************************************************************/
/**
  Convert an ASCII string into a 32-bit unsigned integer value.
  @param str The string to convert
  @param i The integer value will be returned here
  @param base The base to use during the conversion
  @return An error pointer or NULL on success
  */
/***************************************************************************/
const Error *StrToUInt32( char *str, uint32 &i, int base )
{
   char *end;
   i = (int32)strtoul( str, &end, base );

   if( *end )
      return &AmpFileError::format;
   return 0;
}

/***************************************************************************/
/**
  Convert an ASCII string into a 16-bit integer value.
  @param str The string to convert
  @param i The integer value will be returned here
  @param base The base to use during the conversion
  @return An error pointer or NULL on success
  */
/***************************************************************************/
const Error *StrToInt16( char *str, int16 &i, int base )
{
   int32 l;
   const Error *err = StrToInt32( str, l, base );
   if( err ) return err;

   if( (l < -32768) || (l>32767) )
      return &AmpFileError::range;

   i = (int16)l;
   return 0;
}

/***************************************************************************/
/**
  Convert an ASCII string into a 16-bit unsigned integer value.
  @param str The string to convert
  @param i The integer value will be returned here
  @param base The base to use during the conversion
  @return An error pointer or NULL on success
  */
/***************************************************************************/
const Error *StrToUInt16( char *str, uint16 &i, int base )
{
   uint32 l;
   const Error *err = StrToUInt32( str, l, base );
   if( err ) return err;

   if( (l>65535) )
      return &AmpFileError::range;

   i = (uint16)l;
   return 0;
}

/***************************************************************************/
/**
  Convert an ASCII string (as read from a .ccx file) into an output pin
  configuration & mask value.
  @param str The string to decode
  @param cfg The configuration will be returned here
  @param mask1 The first output pin mask value will be returned here.
  @param mask2 The second output pin mask value will be returned here.
  @return An error pointer or NULL on success
  */
/***************************************************************************/
const Error *StrToOutCfg( char *str, OUTPUT_PIN_CONFIG &cfg, uint32 &mask1, uint32 &mask2 )
{
   const Error *err = 0;
   char *seg[4];
   int16 c;
   
   // The second mask is optional.  If it's not in the 
   // file then it defaults to zero.
   mask2 = 0;

   switch( SplitLine( str, seg, 4, ':' ) )
   {
      case 3:
	 err = StrToUInt32( seg[2], mask2, 16 );

      case 2:
	 if( !err ) err = StrToInt16( seg[0], c, 16 );
	 if( !err ) err = StrToUInt32( seg[1], mask1, 16 );
	 cfg = (OUTPUT_PIN_CONFIG)c;
	 break;

      default:
	 return &AmpFileError::format;
   }

   return err;
}
/***************************************************************************/
/**
  Convert an ASCII string (as read from a .ccx file) into a set of filter
  coefficients.
  @param str The string to decode
  @param flt The filter structure to be filled 
  @return An error pointer or NULL on success
  */
/***************************************************************************/
const Error *StrToFilter( char *str, Filter &flt )
{
   char *seg[10];

   int ct = SplitLine( str, seg, 10, ':' );

   if( (ct!=7) && (ct!=9) )
      return &AmpFileError::format;

   int32 coef[9];

   const Error *err = 0;
   for( int i=0; i<ct; i++ )
   {
      if( !err ) 
	 err = StrToInt32( seg[i], coef[i] );
   }

   if( !err )
      err = flt.LoadFromCCX( coef, ct );

   return err;
}

const Error *StrToHostCfg( char *str, char *hostCfg )
{
   char *seg[20];

   if( SplitLine( str, seg, 20, ':' ) != 20 )
      return &AmpFileError::format;

   const Error *err = 0;
   for( int i=0; i<20; i++ )
   {
      int16 c;
      err = StrToInt16( seg[i], c );
      if( err ) return err;

      hostCfg[2*i  ] = (char)(c>>8);
      hostCfg[2*i+1] = (char)(c);
   }

   return 0;
}

/***************************************************************************/
/**
Convert the homing method from the 16-bit value in the ccx file to a CANopen
standard homing method.
*/
/***************************************************************************/
#define HOME_BITS_MODE    0x000F
#define HOME_BITS_DIR     0x0010
#define HOME_BITS_NDX     0x0020
#define HOME_BITS_NDXIN   0x0040
#define HOME_BITS_LOW     0x0100
COPLEY_HOME_METHOD HomeMethodConvert( uint16 x )
{
   // The home method stored in the ccx file is formatted as follows
   //  xxxxxxxxxxxxxxxx
   //  ............\\\\---- MODE:     General home mode.  See below for details.
   //  ...........\-------- DIR:      Direction (0=positive, 1=negative)
   //  ..........\--------- INDEX:    Home on an index if set
   //  .........\---------- NDX_IN:   Index select.  if set, use inner index, outter for clear
   //  ........\----------- NDXFALL:  If set, capture falling edge of index, else rising.
   //  .......\------------ LOW:      Selects home switch edge (0=high, 1=low)
   //  ......\------------- ZERO:     If set, move to zero after home.
   //
   // Not all of these bits map into the CANopen home method, but we do the best
   // that we can here.

   switch( x & HOME_BITS_MODE )
   {
      // Home to index
      case 0:
	 if( !(x & HOME_BITS_NDX) )
	    return CHM_NONE;
	 else if( x & HOME_BITS_DIR )
	    return CHM_NDX_NEG;
	 else
	    return CHM_NDX_POS;

      // Home to limit switch
      case 1:
	 if( x & HOME_BITS_NDX )
	    return (x&HOME_BITS_DIR) ? CHM_NLIM_ONDX : CHM_PLIM_ONDX;
	 else
	    return (x&HOME_BITS_DIR) ? CHM_NLIM : CHM_PLIM;

      // Constant home switch
      case 2:
	 if( !(x & HOME_BITS_NDX) )
	    return (x&HOME_BITS_DIR) ? CHM_NHOME : CHM_PHOME;

	 else if( x & HOME_BITS_NDXIN )
	    return (x&HOME_BITS_DIR) ? CHM_NHOME_INDX : CHM_PHOME_INDX;

	 else
	    return (x&HOME_BITS_DIR) ? CHM_NHOME_ONDX : CHM_PHOME_ONDX;

      // Momentary home switch
      case 3:
	 if( !(x & HOME_BITS_NDX) )
	 {
	    if( x & HOME_BITS_LOW )
	       return (x&HOME_BITS_DIR) ? CHM_LHOME_NEG : CHM_LHOME_POS;
	    else
	       return (x&HOME_BITS_DIR) ? CHM_UHOME_NEG : CHM_UHOME_POS;
	 }

	 else if( x & HOME_BITS_NDXIN )
	 {
	    if( x & HOME_BITS_LOW )
	       return (x&HOME_BITS_DIR) ? CHM_LHOME_INDX_NEG : CHM_LHOME_INDX_POS;
	    else
	       return (x&HOME_BITS_DIR) ? CHM_UHOME_INDX_NEG : CHM_UHOME_INDX_POS;
	 }

	 else
	 {
	    if( x & HOME_BITS_LOW )
	       return (x&HOME_BITS_DIR) ? CHM_LHOME_ONDX_NEG : CHM_LHOME_ONDX_POS;
	    else
	       return (x&HOME_BITS_DIR) ? CHM_UHOME_ONDX_NEG : CHM_UHOME_ONDX_POS;
	 }

      // Home to hard stop
      case 4:
	 if( !(x & HOME_BITS_NDX) )
	    return (x&HOME_BITS_DIR) ? CHM_HARDSTOP_NEG : CHM_HARDSTOP_POS;
	 else
	    return (x&HOME_BITS_DIR) ? CHM_HARDSTOP_ONDX_NEG : CHM_HARDSTOP_ONDX_POS;

      // Default to no movement.
      default:
	 return CHM_NONE;
   }
}

#endif

CML_NAMESPACE_END()
