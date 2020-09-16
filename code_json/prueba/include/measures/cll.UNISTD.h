#ifndef __CLL_UNISTD_H
#define __CLL_UNISTD_H

/*
 * This header provides a series of functions that are able
 * to take measures from a UNIX environment.
 */

/* 
 * PROVIDES THE FOLLOWING OBSERVABLES:
 *    CLOCK
 */

/* CLOCK */

#ifdef Sunfire
#  include "cll.UNISTD.Sunfire.h"
#else
#ifdef linuxintel
#  include "cll.UNISTD.linuxintel.h"
#else /* default architecture */
#  include "cll.UNISTD.default.h"
#endif /* linuxintel */ 
#endif /* Sunfire */
#endif /* __CLL_UNISD_H */
