/** no_prev.c
 **
 ** This dummy file is used on systems without any previewer.
 ** Its main purpose is to simplify the definition of a generic makefile
 ** which must work even if no real file $(PREVIEWER).c exists.
 **
 ** 93/04/16  HWW  V 1.00
 **/

#include <stdio.h>
void    Eprintf (const char*, ...);

void	PicBuf_to_Dummy()
{
  Eprintf("Sorry - preview not supported here\n");
}

