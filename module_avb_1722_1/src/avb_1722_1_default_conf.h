#ifndef AVB_1722_1_DEFAULT_CONF_H_
#define AVB_1722_1_DEFAULT_CONF_H_

/* Must include avb_conf.h as it may redefine the default defines here */
#include "avb_conf.h"

#ifndef AVB_1722_1_CONTROLLER_ENABLED
#define AVB_1722_1_CONTROLLER_ENABLED 1
#endif

#ifndef AVB_1722_1_TALKER_ENABLED
#define AVB_1722_1_TALKER_ENABLED 1
#endif

#ifndef AVB_1722_1_LISTENER_ENABLED
#define AVB_1722_1_LISTENER_ENABLED 1
#endif

#ifndef AVB_1722_1_ADP_VALID_TIME
#define AVB_1722_1_ADP_VALID_TIME (10)  // 31*2 = 62 seconds validity time
#define AVB_1722_1_ADP_REPEAT_TIME (AVB_1722_1_ADP_VALID_TIME/2)
#endif

#ifndef AVB_1722_1_ADP_VENDOR_ID
#define AVB_1722_1_ADP_VENDOR_ID 0x00229700u
#endif

#ifndef AVB_1722_1_ADP_MODEL_ID
#define AVB_1722_1_ADP_MODEL_ID 0
#endif

#ifndef AVB_1722_1_ADP_ENTITY_CAPABILITIES
#define AVB_1722_1_ADP_ENTITY_CAPABILITIES 0
#endif

#ifndef AVB_1722_1_ADP_TALKER_STREAM_SOURCES
#define AVB_1722_1_ADP_TALKER_STREAM_SOURCES 0
#endif

#ifndef AVB_1722_1_ADP_TALKER_CAPABILITIES
#define AVB_1722_1_ADP_TALKER_CAPABILITIES 0
#endif

#ifndef AVB_1722_1_ADP_LISTENER_STREAM_SINKS
#define AVB_1722_1_ADP_LISTENER_STREAM_SINKS 0
#endif

#ifndef AVB_1722_1_ADP_LISTENER_CAPABILITIES
#define AVB_1722_1_ADP_LISTENER_CAPABILITIES 0
#endif

#ifndef AVB_1722_1_ADP_CONTROLLER_CAPABILITIES
#define AVB_1722_1_ADP_CONTROLLER_CAPABILITIES 0
#endif

#ifndef AVB_1722_1_ADP_DEFAULT_AUDIO_FORMAT
#define AVB_1722_1_ADP_DEFAULT_AUDIO_FORMAT (AVB_1722_1_ADP_DEFAULT_AUDIO_FORMAT_2_CH|\
											 AVB_1722_1_ADP_DEFAULT_AUDIO_FORMAT_48K|\
											 AVB_1722_1_ADP_DEFAULT_AUDIO_FORMAT_MAX_STREAMS_8)
#endif

#ifndef AVB_1722_1_ADP_DEFAULT_VIDEO_FORMAT
#define AVB_1722_1_ADP_DEFAULT_VIDEO_FORMAT 0
#endif

#ifndef AVB_1722_1_ADP_ASSOCIATION_ID
#define AVB_1722_1_ADP_ASSOCIATION_ID 0
#endif

#ifndef AVB_1722_1_MAX_ENTITIES
#define AVB_1722_1_MAX_ENTITIES 4
#endif

#ifndef AVB_1722_1_MAX_LISTENERS
#define AVB_1722_1_MAX_LISTENERS 1
#endif

#ifndef AVB_1722_1_MAX_TALKERS
#define AVB_1722_1_MAX_TALKERS 1
#endif

#ifndef AVB_1722_1_MAX_LISTENERS_PER_TALKER
#define AVB_1722_1_MAX_LISTENERS_PER_TALKER 4
#endif

#ifndef AVB_1722_1_MAX_INFLIGHT_COMMANDS
#define AVB_1722_1_MAX_INFLIGHT_COMMANDS 2
#endif

/* Debug defines */

#ifndef AVB_1722_1_ADP_DEBUG_ENTITY_REMOVAL
#define AVB_1722_1_ADP_DEBUG_ENTITY_REMOVAL
#endif


#endif /* AVB_1722_1_DEFAULT_CONF_H_ */
