
#ifndef _VERSION_H_
#define _VERSION_H_

#define MITHIA_MAJOR_VERSION	0
#define MITHIA_MINOR_VERSION	1
#define MITHIA_PATCH_VERSION	0

#define MITHIA_RELEASE_FLAG	1	// 1=Develop,0=Stable

#if !defined( strcmpi )
#define strcmpi strcasecmp
#endif

#endif
