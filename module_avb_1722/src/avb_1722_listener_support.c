/**
 * \file avb_1722_listener_support.c
 * \brief IEC 61883-6/AVB1722 Listener support C functions
 */
#define streaming
#include "avb_1722_listener.h"
#include "avb_1722_common.h"
#include "gptp.h"
#include "avb_1722_def.h"
#include "media_output_fifo.h"
#include <string.h>
#include <print.h>
#include <xs1.h>
#include "avb_conf.h"

#ifdef AVB_1722_RECORD_ERRORS
static unsigned avb_1722_listener_dbc_discontinuity = 0;
static unsigned avb_1722_listener_seq_num_mismatch = 0;

#endif

int avb_1722_listener_process_packet(chanend buf_ctl,
                                     unsigned char Buf0[],
                                     int numBytes,
                                     avb_1722_stream_info_t *stream_info,
                                #ifdef AVB_1722_RECORD_ERRORS
                                     avb_1722_stream_debug_t *stream_debug,
                                #endif
                                     int index,
                                     int *notified_buf_ctl)
{
    int pktDataLength, dbc_value;
    AVB_DataHeader_t *pAVBHdr;
    AVB_AVB1722_CIP_Header_t *pAVB1722Hdr; 
    unsigned char *Buf = &Buf0[2];
    int avb_ethernet_hdr_size = (Buf[12]==0x81) ? 18 : 14;
    int num_samples_in_payload, num_channels_in_payload;
    pAVBHdr = (AVB_DataHeader_t *) &(Buf[avb_ethernet_hdr_size]);
    pAVB1722Hdr = (AVB_AVB1722_CIP_Header_t *) &(Buf[avb_ethernet_hdr_size + AVB_TP_HDR_SIZE]);
    unsigned char *sample_ptr;
    int prev_num_samples;
    int i;
    int num_channels = stream_info->num_channels;
    unsigned char this_seq_num = AVBTP_SEQUENCE_NUMBER(pAVBHdr);
    media_output_fifo_t *map = &stream_info->map[0];
    int stride;   
#if !AVB_1722_SAF
    int dbc_diff;
#endif

   // sanity check on number bytes in payload
#if AVB_1722_SAF
    if (numBytes <= avb_ethernet_hdr_size + AVB_TP_HDR_SIZE)
#else
    if (numBytes <= avb_ethernet_hdr_size + AVB_TP_HDR_SIZE + AVB_CIP_HDR_SIZE)
#endif
    {    
        return ERR_1722_INVALID_PACKET_SIZE;
    }   
    if (AVBTP_CD(pAVBHdr) != AVBTP_CD_DATA)
    {
        return ERR_1722_COMMAND_PACKET;      
    }
    if (AVBTP_SV(pAVBHdr) == 0)
    {
        return ERR_1722_INVALID_STREAM_ID;            
    }
    
#ifdef AVB_1722_RECORD_ERRORS
    stream_debug->packet_count++;
    if (AVBTP_HOLDOVER(pAVBHdr)) stream_debug->num_timestamp_uncertain++;
#endif

#if !AVB_1722_SAF
    dbc_value = (int) pAVB1722Hdr->DBC;
    dbc_diff = dbc_value - stream_info->dbc;
    stream_info->dbc = dbc_value;

    if (dbc_diff < 0) dbc_diff += 0x100;
#endif

    pktDataLength = NTOH_U16(pAVBHdr->packet_data_length);
#if AVB_1722_SAF
    num_samples_in_payload = pktDataLength>>2;
#else
    num_samples_in_payload = (pktDataLength-8)>>2;
#endif

    prev_num_samples = stream_info->prev_num_samples;
    stream_info->prev_num_samples = num_samples_in_payload;

    if (stream_info->chan_lock < 16)
    {
        int num_channels;

#if !AVB_1722_SAF
        if (!prev_num_samples || dbc_diff == 0)
        {
            return ERR_1722_RATE_NOT_LOCKED;
        }
#endif
     
#if AVB_1722_SAF
        num_channels = AVBTP_PROTOCOL_SPECIFIC(pAVBHdr);
#else
        num_channels = prev_num_samples / dbc_diff;
#endif
     
        if (!stream_info->num_channels_in_payload || stream_info->num_channels_in_payload != num_channels)
        {
            stream_info->num_channels_in_payload = num_channels;
            stream_info->chan_lock = 0;
            stream_info->rate = 0;
            stream_info->syt_interval = 0;
        }

        stream_info->rate += num_samples_in_payload;

        stream_info->chan_lock++;

#if !AVB_1722_SAF
        if (stream_info->chan_lock == 16)
        {
            stream_info->rate = (stream_info->rate / stream_info->num_channels_in_payload / 16);

            switch (stream_info->rate)
            {
                case 1: stream_info->rate = 8000; stream_info->syt_interval = 1; break;
                case 2: stream_info->rate = 16000; stream_info->syt_interval = 2; break;
                case 4: stream_info->rate = 32000; stream_info->syt_interval = 8; break;
                case 5: stream_info->rate = 44100; stream_info->syt_interval = 8; break;
                case 6: stream_info->rate = 48000; stream_info->syt_interval = 8; break;
                case 11: stream_info->rate = 88200; stream_info->syt_interval = 16; break;
                case 12: stream_info->rate = 96000; stream_info->syt_interval = 16; break;
                /* TODO:
                       176400:	syt_interval = 32;
                       192000:	syt_interval = 32;
                */
                default: 
                    stream_info->chan_lock = 0;
                    stream_info->rate = 0;
                    stream_info->syt_interval = 0;
                    return ERR_1722_INVALID_RATE;
            }
            
            stream_info->prev_sequence_num = this_seq_num;
        }
#endif
        return ERR_1722_RATE_NOT_LOCKED;
    }
#ifdef AVB_1722_RECORD_ERRORS
    else if (dbc_diff * num_channels != num_samples_in_payload)
    {
        stream_debug->num_dbc_discontinuity++;
    }

    if ((unsigned char)(stream_info->prev_sequence_num + 1) != this_seq_num)
    {
        stream_debug->num_seq_num_mismatch++;
    }
    
    stream_info->prev_sequence_num = this_seq_num;
#endif

#if AVB_1722_SAF
    if ((AVBTP_TV(pAVBHdr)==1))
    {
        unsigned sample_num = 0;
#else
    if ((AVBTP_TV(pAVBHdr)==1))
    {
        // See 61883-6 section 6.2 which explains that the receiver can calculate
        // which data block (sample) the timestamp refers to using the formula:
        //   index = (SYT_INTERVAL - dbc % SYT_INTERVAL) % SYT_INTERVAL

        unsigned sample_num;

        sample_num = (stream_info->syt_interval - (dbc_value & (stream_info->syt_interval-1))) & (stream_info->syt_interval-1);
#endif
        // register timestamp
        for (int i=0; i < num_channels; i++)
        {
            media_output_fifo_set_ptp_timestamp(map[i], AVBTP_TIMESTAMP(pAVBHdr), sample_num);
        }
   }

    for (i=0; i < num_channels; i++)
    {
        media_output_fifo_maintain(map[i], buf_ctl, notified_buf_ctl);
    }           


  // now send the samples
#if AVB_1722_SAF
    sample_ptr = (unsigned char *) &Buf[(avb_ethernet_hdr_size + AVB_TP_HDR_SIZE)];
#else
    sample_ptr = (unsigned char *) &Buf[(avb_ethernet_hdr_size + AVB_TP_HDR_SIZE + AVB_CIP_HDR_SIZE)];
#endif

    num_channels_in_payload = stream_info->num_channels_in_payload;

    stride = num_channels_in_payload;

    num_channels = num_channels < num_channels_in_payload ? num_channels : num_channels_in_payload;

    for (i=0; i<num_channels; i++)
    {
        media_output_fifo_strided_push(map[i], (unsigned int *) sample_ptr, stride, num_samples_in_payload);
        sample_ptr += 4;
    }

    return RET_1722_PACKET_OK;
}

