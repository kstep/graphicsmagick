/*
  Copyright (C) 2003 GraphicsMagick Group
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.

  Interfaces to deal with marshalling quantums to and from a bit-stream.
  Written by Bob Friesenhahn, September 2003
 
*/
#ifndef _MAGICK_BIT_STREAM_H
#define _MAGICK_BIT_STREAM_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

  /*
    Bit stream "handle"
  */
  typedef struct _BitStream
  {
    const unsigned char *byte;
    int bits_available;
  } BitStream;

  /*
    Initialize structure used by ReadBitStream
  */
  static inline void InitializeBitStream(BitStream *bit_stream, const unsigned char *bytes)
  {
    bit_stream->byte=bytes;
    bit_stream->bits_available=8;
  }

  /*
    Return the requested number of bits from the current position in a
    bit stream.
  */

  static inline unsigned int ReadBitStream(BitStream *bit_stream,
    const unsigned int requested_bits)
  {
    static const unsigned int BitMasks[9] =
      {
        /*
          Same as (~(~0 << retrieve_bits))
        */
        0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
      };

    unsigned int
      current_bits = 0,
      quantum = 0;

    while (current_bits < requested_bits)
      {
        register unsigned int
          retrieve_bits;

        retrieve_bits = (requested_bits - current_bits);
        if (retrieve_bits > bit_stream->bits_available)
          retrieve_bits = bit_stream->bits_available;

        quantum = (quantum << retrieve_bits) |
          ((*bit_stream->byte >> (bit_stream->bits_available - retrieve_bits))
           & BitMasks[retrieve_bits]);

        current_bits += retrieve_bits;
        bit_stream->bits_available -= retrieve_bits;
        if (bit_stream->bits_available == 0)
          {
            bit_stream->byte++;
            bit_stream->bits_available=8;
          }
      }
    return quantum;
  }

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _MAGICK_BIT_STREAM_H */
