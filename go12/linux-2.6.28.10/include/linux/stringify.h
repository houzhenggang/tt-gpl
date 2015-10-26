#ifndef __LINUX_STRINGIFY_H
#define __LINUX_STRINGIFY_H

/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */

#ifdef __STDC__
#define __stringify_1(x...)	#x
#define __stringify(x...)	__stringify_1(x)
#else	/* Support gcc -traditional, without commas. */
#define __stringify_1(x)	#x
#define __stringify(x)		__stringify_1(x)
#endif

#endif	/* !__LINUX_STRINGIFY_H */
