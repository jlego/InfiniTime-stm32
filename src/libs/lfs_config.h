#pragma once

#ifndef LFS_TRACE
#ifdef LFS_YES_TRACE
#define LFS_TRACE(fmt, ...)
#else
#define LFS_TRACE(...)
#endif
#endif

#ifndef LFS_DEBUG
#ifndef LFS_NO_DEBUG
#define LFS_DEBUG(fmt, ...)
#else
#define LFS_DEBUG(...)
#endif
#endif

#ifndef LFS_WARN
#ifndef LFS_NO_WARN
#define LFS_WARN(fmt, ...)
#else
#define LFS_WARN(...)
#endif
#endif

#ifndef LFS_ERROR
#ifndef LFS_NO_ERROR
#define LFS_ERROR(fmt, ...)
#else
#define LFS_ERROR(...)
#endif
#endif

#undef LFS_CONFIG

#undef LFS_UTIL_H
#include <littlefs/lfs_util.h>