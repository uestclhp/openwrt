/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 17821 2009-11-11 09:00:00Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsIfExtEthStatsTable.h"
#include "jws_ubus_if.h"
#include "stdlib.h"
#include "jwslog.h"
#include "jwsDevBaseInfo.h"

/* create a new row in the (unsorted) table */
struct jwsIfExtEthStatsTable_entry *
jwsIfExtEthStatsTable_createEntry(
                 long  jwsIfExtEthStatsIfIndex
                );

/** Initializes the jwsIfExtEthStatsTable module */
void
init_jwsIfExtEthStatsTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_jwsIfExtEthStatsTable();
}

//  # Determine the first/last column names

/** Initialize the jwsIfExtEthStatsTable table by defining its contents and how it's structured */
void
initialize_table_jwsIfExtEthStatsTable(void)
{
    static oid jwsIfExtEthStatsTable_oid[] = {1,3,6,1,4,1,33444,2,2,2,2,2};
    size_t jwsIfExtEthStatsTable_oid_len   = OID_LENGTH(jwsIfExtEthStatsTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    int i;
    struct stJwsDevBaseInfo jwsDevBaseInfo;

    reg = netsnmp_create_handler_registration(
              "jwsIfExtEthStatsTable",     jwsIfExtEthStatsTable_handler,
              jwsIfExtEthStatsTable_oid, jwsIfExtEthStatsTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: jwsIfExtEthStatsIfIndex */
                           0);
    table_info->min_column = COLUMN_JWSIFEXTETHSTATSIFINDEX;
    table_info->max_column = COLUMN_JWSIFEXTETHSTATSTXPAUSEPKTS;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = jwsIfExtEthStatsTable_get_first_data_point;
    iinfo->get_next_data_point  = jwsIfExtEthStatsTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
    getJwsDevBaseInfo(&jwsDevBaseInfo);
    for (i = 1; i <= jwsDevBaseInfo.port_num; i++)
    {
        jwsIfExtEthStatsTable_createEntry(i);
    }
}

    /* Typical data structure for a row entry */
struct jwsIfExtEthStatsTable_entry {
    /* Index values */
//    long jwsIfExtEthStatsIfIndex;

    /* Column values */
    long jwsIfExtEthStatsIfIndex;
    long jwsIfExtEthStatsClear;
    long old_jwsIfExtEthStatsClear;
    U64 jwsIfExtEthStatsRxBytes;
    u_long jwsIfExtEthStatsRxUcastPkts;
    u_long jwsIfExtEthStatsRxBcastPkts;
    u_long jwsIfExtEthStatsRxMcastPkts;
    u_long jwsIfExtEthStatsRxPausePkts;
    U64 jwsIfExtEthStatsTxBytes;
    u_long jwsIfExtEthStatsTxUcastPkts;
    u_long jwsIfExtEthStatsTxBcastPkts;
    u_long jwsIfExtEthStatsTxMcastPkts;
    u_long jwsIfExtEthStatsTxPausePkts;

    /* Illustrate using a simple linked list */
    int   valid;
    struct jwsIfExtEthStatsTable_entry *next;
};

struct jwsIfExtEthStatsTable_entry  *jwsIfExtEthStatsTable_head;

/* create a new row in the (unsorted) table */
struct jwsIfExtEthStatsTable_entry *
jwsIfExtEthStatsTable_createEntry(
                 long  jwsIfExtEthStatsIfIndex
                ) {
    struct jwsIfExtEthStatsTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct jwsIfExtEthStatsTable_entry);
    if (!entry)
        return NULL;

    entry->jwsIfExtEthStatsIfIndex = jwsIfExtEthStatsIfIndex;
    entry->next = jwsIfExtEthStatsTable_head;
    jwsIfExtEthStatsTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
jwsIfExtEthStatsTable_removeEntry( struct jwsIfExtEthStatsTable_entry *entry ) {
    struct jwsIfExtEthStatsTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = jwsIfExtEthStatsTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        jwsIfExtEthStatsTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
jwsIfExtEthStatsTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = jwsIfExtEthStatsTable_head;
    return jwsIfExtEthStatsTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
jwsIfExtEthStatsTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct jwsIfExtEthStatsTable_entry *entry = (struct jwsIfExtEthStatsTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->jwsIfExtEthStatsIfIndex );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the jwsIfExtEthStatsTable table */
int
jwsIfExtEthStatsTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct jwsIfExtEthStatsTable_entry          *table_entry;

    int ret = SNMP_ERR_NOERROR;
    unsigned int data = 0;
    unsigned long long data64 = 0;
    char str[100] = {0};
    char value[10] = {0};

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsIfExtEthStatsTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSIFEXTETHSTATSIFINDEX:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsIfExtEthStatsIfIndex);
                break;
            case COLUMN_JWSIFEXTETHSTATSCLEAR:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "clear_select");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsClear = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsIfExtEthStatsClear);
                break;
            case COLUMN_JWSIFEXTETHSTATSRXBYTES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "rx_bytes");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data64 = atoll(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsRxBytes.low = (unsigned int)data64;
                table_entry->jwsIfExtEthStatsRxBytes.high = (unsigned int)(data64 >> 32);

                snmp_set_var_typed_value( request->requestvb, ASN_COUNTER64,
                                        (u_char *)&(table_entry->jwsIfExtEthStatsRxBytes),
                                        sizeof(table_entry->jwsIfExtEthStatsRxBytes));
                break;
            case COLUMN_JWSIFEXTETHSTATSRXUCASTPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "rx_unicast_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsRxUcastPkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsRxUcastPkts);
                break;
            case COLUMN_JWSIFEXTETHSTATSRXBCASTPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "rx_broadcast_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsRxBcastPkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsRxBcastPkts);
                break;
            case COLUMN_JWSIFEXTETHSTATSRXMCASTPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "rx_multicast_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsRxMcastPkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsRxMcastPkts);
                break;
            case COLUMN_JWSIFEXTETHSTATSRXPAUSEPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "rx_pause_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsRxPausePkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsRxPausePkts);
                break;
            case COLUMN_JWSIFEXTETHSTATSTXBYTES:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "tx_bytes");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data64 = atoll(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsTxBytes.low = (unsigned int)data64;;
                table_entry->jwsIfExtEthStatsTxBytes.high = (unsigned int)(data64 >> 32);

                snmp_set_var_typed_value( request->requestvb, ASN_COUNTER64,
                                        (u_char *)&(table_entry->jwsIfExtEthStatsTxBytes),
                                        sizeof(table_entry->jwsIfExtEthStatsTxBytes));
                break;
            case COLUMN_JWSIFEXTETHSTATSTXUCASTPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "tx_unicast_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsTxUcastPkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsTxUcastPkts);
                break;
            case COLUMN_JWSIFEXTETHSTATSTXBCASTPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "tx_broadcast_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsTxBcastPkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsTxBcastPkts);
                break;
            case COLUMN_JWSIFEXTETHSTATSTXMCASTPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "tx_multicast_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsTxMcastPkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsTxMcastPkts);
                break;
            case COLUMN_JWSIFEXTETHSTATSTXPAUSEPKTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "tx_pause_packets");
                ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsIfExtEthStatsTxPausePkts = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_COUNTER,
                                            table_entry->jwsIfExtEthStatsTxPausePkts);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

        /*
         * Write-support
         */
    case MODE_SET_RESERVE1:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsIfExtEthStatsTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSIFEXTETHSTATSCLEAR:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            default:
                netsnmp_set_request_error( reqinfo, request,
                                           SNMP_ERR_NOTWRITABLE );
                return SNMP_ERR_NOERROR;
            }
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsIfExtEthStatsTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSIFEXTETHSTATSCLEAR:
                table_entry->old_jwsIfExtEthStatsClear = table_entry->jwsIfExtEthStatsClear;
                table_entry->jwsIfExtEthStatsClear     = *request->requestvb->val.integer;

                snprintf(value, sizeof(value), "%ld", table_entry->jwsIfExtEthStatsClear);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsIfExtEthStatsIfIndex, "clear_select", value);
                ret = ubus_invoke_command("eth_stats", "eth_stats_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            }
        }
        break;

    case MODE_SET_UNDO:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsIfExtEthStatsTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSIFEXTETHSTATSCLEAR:
                table_entry->jwsIfExtEthStatsClear     = table_entry->old_jwsIfExtEthStatsClear;
                table_entry->old_jwsIfExtEthStatsClear = 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
