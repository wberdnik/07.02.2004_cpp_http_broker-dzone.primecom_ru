
/* MDDRIVER.C - test driver for MD2, MD4 and MD5
 */

/* Copyright (C) 1990-2, RSA Data Security, Inc. Created 1990. All
rights reserved.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

/* The following makes MD default to MD5 if it has not already been
  defined with C compiler flags.
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "global.h"

#include "md5.h"


/* Length of test block, number of test blocks.
 */

static void MDString (char *);

/* Main driver.

Arguments (may be any combination):
  -sstring - digests string
  -t       - runs time trial
  -x       - runs test script
  filename - digests file
  (none)   - digests standard input
 */
int main (argc, argv)
int argc;

char *argv[];
{
  char buf[1000];
  int i;
  MD5_CTX context;
  unsigned char digest[16];
  unsigned int len = strlen (buf);

  printf("Input string ");
  scanf("%s",buf);

  MD5Init (&context);
  MD5Update (&context, buf, len);
  MD5Final (digest, &context);

  printf ("MD5 (\"%s\") = ", buf);

  for (i = 0; i < 16; i++)
	printf ("%02x", digest[i]);

  
   scanf("%s",buf);
    return (0);
}


