/**
 * \file avb_1722_listener.h
 * \brief IEC 61883-6/AVB1722 Listener definitions
 */

#ifndef _AVB1722_LISTENER_H_ 
#define _AVB1722_LISTENER_H_ 1
#ifndef __XC__
#define streaming
#endif
#include <xccompat.h>
#include "avb_conf.h"
#include "avb_1722_def.h"
#include "media_output_fifo.h"

#ifndef MAX_INCOMING_AVB_STREAMS 
#define MAX_INCOMING_AVB_STREAMS (AVB_NUM_SINKS)
#endif

#ifndef AVB_MAX_CHANNELS_PER_STREAM 
#define AVB_MAX_CHANNELS_PER_STREAM 16
#endif

#ifndef MAX_AVB_STREAMS_PER_LISTENER
#define MAX_AVB_STREAMS_PER_LISTENER 12
#endif


typedef struct avb_1722_stream_info_t
{
    int active;                      //!< 1-bit flag to say if the stream is active
    int chan_lock;                   //!< Counter for locking onto a data stream
    int rate;
    int prev_num_samples;            //!< Number of samples in last received 1722 packet
    int num_channels_in_payload;     //!< The number of channels in the 1722 payloads
    int num_channels;
    int dbc;
    int syt_interval;
    media_output_fifo_t map[AVB_MAX_CHANNELS_PER_STREAM];
    unsigned char prev_sequence_num;
} avb_1722_stream_info_t;

typedef struct avb_1722_stream_debug_t 
{
    int packet_count;
    int num_dbc_discontinuity;
    int num_seq_num_mismatch;
    int num_timestamp_uncertain;   
} avb_1722_stream_debug_t;

enum
{
    RET_1722_PACKET_OK =            1, 
    ERR_1722_NONE =                 0,
    ERR_1722_INVALID_PACKET_SIZE =  -1,
    ERR_1722_COMMAND_PACKET =       -2,
    ERR_1722_INVALID_STREAM_ID =    -3,
    ERR_1722_INVALID_RATE =         -4,
    ERR_1722_RATE_NOT_LOCKED =      -5,

    ERR_1722_UNKNOWN =              -9999
};

int avb_1722_listener_process_packet(chanend buf_ctl, unsigned char Buf[], int numBytes,
                                    REFERENCE_PARAM(avb_1722_stream_info_t, stream_info),
                                #ifdef AVB_1722_RECORD_ERRORS
                                    REFERENCE_PARAM(avb_1722_stream_debug_t, stream_debug),
                                #endif
                                    int index, REFERENCE_PARAM(int, notified_buf_ctl));

#endif
