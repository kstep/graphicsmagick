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

  static const unsigned int BitAndMasks[9] =
    {
      /*
        Same as (~(~0 << retrieve_bits))
      */
      0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
    };

  /*
    Bit stream reader "handle"
  */
  typedef struct _BitStreamReadHandle
  {
    const unsigned char
      *bytes;

    unsigned int
      bits_remaining;
  } BitStreamReadHandle;

  /*
    Initialize Bit Stream for reading
  */
  static inline void BitStreamInitializeRead(BitStreamReadHandle *bit_stream,
                                             const unsigned char *bytes)
  {
    bit_stream->bytes          = bytes;
    bit_stream->bits_remaining = 8;
  }

  /*
    Return the requested number of bits from the current position in a
    bit stream. Stream is read in most-significant bit/byte "big endian"
    order.
  */
  static inline unsigned int BitStreamMSBRead(BitStreamReadHandle *bit_stream,
                                              const unsigned int requested_bits)
  {
    register unsigned int
      remaining_quantum_bits,
      quantum;

    remaining_quantum_bits = requested_bits;
    quantum = 0;

    while (remaining_quantum_bits > 0)
      {
        register unsigned int
          octet_bits;

        octet_bits = remaining_quantum_bits;
        if (octet_bits > bit_stream->bits_remaining)
          octet_bits = bit_stream->bits_remaining;

        remaining_quantum_bits -= octet_bits;
        bit_stream->bits_remaining -= octet_bits;

        quantum = (quantum << octet_bits) |
          ((*bit_stream->bytes >> (bit_stream->bits_remaining))
           & BitAndMasks[octet_bits]);

        if (bit_stream->bits_remaining == 0)
          {
            bit_stream->bytes++;
            bit_stream->bits_remaining=8;
          }
      }
    return quantum;
  }

  /*
    Bit stream writer "handle"
  */
  typedef struct _BitStreamWriteHandle
  {
    unsigned char
      *bytes;

    unsigned int
      bits_remaining;
  } BitStreamWriteHandle;

  /*
    Initialize Bit Stream for writing
  */
  static inline void BitStreamInitializeWrite(BitStreamWriteHandle *bit_stream,
                                              unsigned char *bytes)
  {
    bit_stream->bytes          = bytes;
    bit_stream->bits_remaining = 8;
  }

  /*
    Write quantum using the specified number of bits at the current
    position in the bit stream. Stream is written in most-significant
    bit/byte "big endian" order.
  */
  static inline void BitStreamMSBWrite(BitStreamWriteHandle *bit_stream,
                                       const unsigned int requested_bits,
                                       const unsigned int quantum)
  {
    register unsigned int
      remaining_quantum_bits = requested_bits;

    while (remaining_quantum_bits > 0)
      {
        register unsigned int
          octet_bits;

        octet_bits = remaining_quantum_bits;
        if (octet_bits > bit_stream->bits_remaining)
          octet_bits = bit_stream->bits_remaining;

        remaining_quantum_bits -= octet_bits;

        if (bit_stream->bits_remaining == 8)
          *bit_stream->bytes = 0;

        bit_stream->bits_remaining -= octet_bits;

        *bit_stream->bytes |=
          (((quantum >> (remaining_quantum_bits)) &
            BitAndMasks[octet_bits]) << (bit_stream->bits_remaining));

        if (bit_stream->bits_remaining == 0)
          {
            bit_stream->bytes++;
            bit_stream->bits_remaining=8;
          }
      }
  }

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* _MAGICK_BIT_STREAM_H */
