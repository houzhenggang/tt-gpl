/*****************************************************************************
* Copyright 2006 - 2008 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL"). 
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/




/*=============================================================================

Project  :
Module   :
File     :  $RCSfile:  $
Revision :  $Revision: #7 $

FILE DESCRIPTION:
Support service for bulk transactions. Bulk auxiliary messages may be sent
alongside (or instead of) the bulk data transfer itself.


=============================================================================*/

#include <linux/broadcom/vc03/vcos.h>
#include <linux/broadcom/vc03/vchi/vchi.h>
#include <linux/broadcom/vc03/vchi/connection.h>
#include <linux/broadcom/vc03/vchi/bulk_aux_service.h>
#include <linux/broadcom/vc03/vchi/control_service.h>
#include <linux/broadcom/vc03/vchi/endian.h>

/******************************************************************************
Private typedefs
******************************************************************************/

typedef struct {
   VCHI_CONNECTION_SERVICE_HANDLE_T open_handle;
   VCHI_CONNECTION_T * connection;
} BULK_AUX_SERVICE_INFO_T;



/******************************************************************************
Static data
******************************************************************************/


// function to handle a BULX message
static void bulk_aux_callback( void *callback_param, //my service local param
                               VCHI_CALLBACK_REASON_T reason,
                               void *handle ); //for transmitting msg's only


/******************************************************************************
Global functions
******************************************************************************/

/***********************************************************
 * Name: vchi_bulk_aux_service_init
 *
 * Arguments:  VCHI_CONNECTION_T *connections,
 *             const uint32_t num_connections
 *
 * Description: Routine to init the bulk auxiliary service
 *
 * Returns: int32_t - success == 0
 *
 ***********************************************************/
int32_t vchi_bulk_aux_service_init( VCHI_CONNECTION_T **connections,
                                    const uint32_t num_connections )
{
   int32_t success = 0;
   uint32_t count = 0;
   BULK_AUX_SERVICE_INFO_T *bulk_aux_info;

   os_assert(num_connections <= VCHI_MAX_NUM_CONNECTIONS);

   bulk_aux_info = os_malloc( VCHI_MAX_NUM_CONNECTIONS * sizeof(BULK_AUX_SERVICE_INFO_T), 0, "vchi:bulk_aux_info" );
   memset( bulk_aux_info, 0,VCHI_MAX_NUM_CONNECTIONS * sizeof(BULK_AUX_SERVICE_INFO_T) );

   for( count = 0; count < num_connections; count++ )
   {
      // record the connection info
      bulk_aux_info[count].connection = connections[count];
      
      // create the server (this is a misnomer as the the BULX service acts as both client and server
      success += (connections[count])->api->service_connect((connections[count])->state,MAKE_FOURCC("BULX"),0,0,VC_TRUE,bulk_aux_callback,&bulk_aux_info[count],VC_FALSE,VC_FALSE,VC_FALSE,&bulk_aux_info[count].open_handle);
      os_assert(success == 0);
   }

   return success;
}


/***********************************************************
 * Name: vchi_bulk_aux_service_close
 *
 * Arguments:  VCHI_CONNECTION_T *connections,
 *             const uint32_t num_connections
 *
 * Description: Routine to init the control service
 *
 * Returns: int32_t - success == 0
 *
 ***********************************************************/
int32_t vchi_bulk_aux_service_close( void )
{
   int32_t success = 0;
#if 1
   os_assert(0);
#else
   uint32_t count = 0;

   for( count = 0; count < VCHI_MAX_NUM_CONNECTIONS; count++ )
   {
      success = bulk_aux_info[count].connection->api->service_disconnect( bulk_aux_info[count].open_handle );
      if (success != 0) break;
   }
#endif

   return(success);
}



/***********************************************************
 * Name: bulk_aux_callback
 *
 * Arguments: void *callback_param
 *            const VCHI_CALLBACK_REASON_T reason
 *            void *handle
 *
 * Description: Handles callbacks for received messages
 *
 * Returns: -
 *
 ***********************************************************/
static void bulk_aux_callback( void *callback_param, //my service local param
                               const VCHI_CALLBACK_REASON_T reason,
                               void *handle )
{
BULK_AUX_SERVICE_INFO_T * service_info = callback_param;

   switch(reason)
   {
      case VCHI_CALLBACK_MSG_AVAILABLE:
         service_info->connection->api->bulk_aux_received(service_info->connection->state);
         break;
      case VCHI_CALLBACK_MSG_SENT:
         service_info->connection->api->bulk_aux_transmitted(service_info->connection->state, handle);
         break;
      case VCHI_CALLBACK_BULK_RECEIVED:
      case VCHI_CALLBACK_BULK_DATA_READ:
      case VCHI_CALLBACK_BULK_SENT:      // bulk auxiliary service doesn't use bulk transfers(!)
         os_assert(0);
         break;
   }
}

size_t vchi_bulk_aux_service_form_header( void *dest,
                                          size_t dest_len,
                                          fourcc_t service_id,
                                          MESSAGE_TX_CHANNEL_T channel,
                                          uint32_t total_size,
                                          uint32_t chunk_size,
                                          uint32_t crc,
                                          uint32_t data_size,
                                          int16_t data_shift,
                                          uint16_t head_bytes,
                                          uint16_t tail_bytes )
{
   uint8_t *message = dest;
   if ( dest_len < BULX_HEADER_SIZE )
   {
      os_assert( 0 );
      return 0;
   }

   vchi_writebuf_fourcc( &message[BULX_SERVICE_OFFSET],    service_id );
   vchi_writebuf_uint32( &message[BULX_SIZE_OFFSET],       total_size );
   vchi_writebuf_uint32( &message[BULX_CHUNKSIZE_OFFSET],  chunk_size );
   vchi_writebuf_uint32( &message[BULX_CRC_OFFSET],        crc        );
   vchi_writebuf_uint32( &message[BULX_DATA_SIZE_OFFSET],  data_size  );
   vchi_writebuf_uint16( &message[BULX_DATA_SHIFT_OFFSET], data_shift ); // actually int16
   message[BULX_CHANNEL_OFFSET] = channel;
   message[BULX_RESERVED_OFFSET] = 0;
   vchi_writebuf_uint16( &message[BULX_HEAD_SIZE_OFFSET],  head_bytes );
   vchi_writebuf_uint16( &message[BULX_TAIL_SIZE_OFFSET],  tail_bytes );

   return BULX_HEADER_SIZE;
}


/****************************** End of file ***********************************/
