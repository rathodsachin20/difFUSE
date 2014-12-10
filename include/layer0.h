#ifndef __layer0_h__
#define __layer0_h__

#include "global.h"

/* Layer 0 is the interface to the disk. It implements block abstraction.
*  All requests to disk should go through layer 0.
*/

int get_block(void* buffer, block_num num);

size_t put_block(const void* buffer, block_num num);

#endif /* __layer0_h__ */
