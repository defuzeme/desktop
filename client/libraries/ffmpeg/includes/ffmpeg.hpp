#ifndef _FFMPEG_HPP
#define _FFMPEG_HPP

extern "C" {
#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif


#define __STDC_CONSTANT_MACROS

#include <stdint.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavcodec/opt.h"
#include "libavutil/rational.h"
#include "libavutil/avstring.h"
#include "libswscale/swscale.h"
}

#endif // _FFMPEG_HPP
