
#ifndef OPJ_CLOCK_H
#define OPJ_CLOCK_H
/**
@file opj_clock.h
@brief Internal function for timing

The functions in OPJ_CLOCK.C are internal utilities mainly used for timing.
*/

/** @defgroup MISC MISC - Miscellaneous internal functions */
/*@{*/

/** @name Exported functions */
/*@{*/
/* ----------------------------------------------------------------------- */

/**
Difference in successive opj_clock() calls tells you the elapsed time
@return Returns time in seconds
*/
OPJ_FLOAT64 opj_clock(void);

/* ----------------------------------------------------------------------- */
/*@}*/

/*@}*/

#endif /* OPJ_CLOCK_H */

