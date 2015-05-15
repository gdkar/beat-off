#ifndef __CORE_TIME_H__
#define __CORE_TIME_H__

typedef long mbeat_t;
typedef double sec_t;
typedef long  nsec_t;
#define NS_PER_SEC 1000000000L
#define SEC_PER_NS 1e-9

#define MB2B(x) ((double) (x) / 1000.)
#define B2MB(x) ((long) ((x) * 1000.))

#endif
