/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 17821 2009-11-11 09:00:00Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsTrapConfigGroup.h"

#include "jws_ubus_if.h"
#include "stdio.h"

/* create a new row in the (unsorted) table */
struct jwsTrapPortConfigTable_entry *
jwsTrapPortConfigTable_createEntry(
                 long  jwsTrapPortId
                ) ;
/* create a new row in the (unsorted) table */
struct jwsTrapRingConfigTable_entry *
jwsTrapRingConfigTable_createEntry(
                 long  jwsTrapRingId
                ) ;

/** Initializes the jwsTrapConfigGroup module */
void
init_jwsTrapConfigGroup(void)
{
    static oid jwsTrapPowerSupplyEnableValue_oid[] = { 1,3,6,1,4,1,33444,2,2,100,1,1 };

  DEBUGMSGTL(("jwsTrapConfigGroup", "Initializing\n"));

    netsnmp_register_scalar(
        netsnmp_create_handler_registration("jwsTrapPowerSupplyEnableValue", handle_jwsTrapPowerSupplyEnableValue,
                               jwsTrapPowerSupplyEnableValue_oid, OID_LENGTH(jwsTrapPowerSupplyEnableValue_oid),
                               HANDLER_CAN_RWRITE
        ));
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_jwsTrapPortConfigTable();
    initialize_table_jwsTrapRingConfigTable();
}

int
handle_jwsTrapPowerSupplyEnableValue(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    int ret;
    int data = 0;
    char str[100] = {0};
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */
    
    switch(reqinfo->mode) {

        case MODE_GET:
            ret = ubus_invoke_command("trap", "trap_get", get_ret_handler_cb, 
                                "{\"get_args\":[\"power_trap\"]}");
            if (ret != 0)
            {
                return SNMP_ERR_NOSUCHNAME;
            }
            data = atoi(jws_read_buf_str());

            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
                                     (u_char *) &data,
                                     sizeof(data));
            break;

        /*
         * SET REQUEST
         *
         * multiple states in the transaction.  See:
         * http://www.net-snmp.org/tutorial-5/toolkit/mib_module/set-actions.jpg
         */
        case MODE_SET_RESERVE1:
                /* or you could use netsnmp_check_vb_type_and_size instead */
            ret = netsnmp_check_vb_type(requests->requestvb, ASN_INTEGER);
            if ( ret != SNMP_ERR_NOERROR ) {
                netsnmp_set_request_error(reqinfo, requests, ret );
            }
            break;

        case MODE_SET_RESERVE2:
            /* XXX malloc "undo" storage buffer */
            if (0/* XXX if malloc, or whatever, failed: */) {
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_RESOURCEUNAVAILABLE);
            }
            break;

        case MODE_SET_FREE:
            /* XXX: free resources allocated in RESERVE1 and/or
               RESERVE2.  Something failed somewhere, and the states
               below won't be called. */
            break;

        case MODE_SET_ACTION:
            data = *requests->requestvb->val.integer;
            snprintf(str, sizeof(str), "{\"set_args\":[{\"power_trap\":\"%d\"}]}", data);
            ret = ubus_invoke_command("trap", "trap_set", set_ret_handler_cb, str);
            if (ret != 0)
            {
                    return SNMP_ERR_NOSUCHNAME;
            }
            /* XXX: perform the value change here */
            if (0/* XXX: error? */) {
                //netsnmp_set_request_error(reqinfo, requests, /* some error */);
            }
            break;

        case MODE_SET_COMMIT:
            /* XXX: delete temporary storage */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_COMMITFAILED);
            }
            break;

        case MODE_SET_UNDO:
            /* XXX: UNDO and return to previous value for the object */
            if (0/* XXX: error? */) {
                /* try _really_really_ hard to never get to this point */
                netsnmp_set_request_error(reqinfo, requests, SNMP_ERR_UNDOFAILED);
            }
            break;

        default:
            /* we should never get here, so this is a really bad error */
            snmp_log(LOG_ERR, "unknown mode (%d) in handle_jwsTrapPowerSupplyEnableValue\n", reqinfo->mode );
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

//  # Determine the first/last column names

/** Initialize the jwsTrapPortConfigTable table by defining its contents and how it's structured */
void
initialize_table_jwsTrapPortConfigTable(void)
{
    static oid jwsTrapPortConfigTable_oid[] = {1,3,6,1,4,1,33444,2,2,100,1,2};
    size_t jwsTrapPortConfigTable_oid_len   = OID_LENGTH(jwsTrapPortConfigTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;
    int i;

    reg = netsnmp_create_handler_registration(
              "jwsTrapPortConfigTable",     jwsTrapPortConfigTable_handler,
              jwsTrapPortConfigTable_oid, jwsTrapPortConfigTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: jwsTrapPortId */
                           0);
    table_info->min_column = COLUMN_JWSTRAPPORTID;
    table_info->max_column = COLUMN_JWSTRAPPORTRXTHRESHOLDPERCENTAGE;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = jwsTrapPortConfigTable_get_first_data_point;
    iinfo->get_next_data_point  = jwsTrapPortConfigTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
    for (i = 1; i <= 10; i++)
    {
        jwsTrapPortConfigTable_createEntry(i);
    }
}

    /* Typical data structure for a row entry */
struct jwsTrapPortConfigTable_entry {
    /* Index values */
    //long jwsTrapPortId;

    /* Column values */
    long jwsTrapPortId;
    long jwsTrapPortStatusEnableValue;
    long old_jwsTrapPortStatusEnableValue;
    long jwsTrapPortTxThresholdEnableValue;
    long old_jwsTrapPortTxThresholdEnableValue;
    long jwsTrapPortTxThresholdPercentage;
    long old_jwsTrapPortTxThresholdPercentage;
    long jwsTrapPortRxThresholdEnableValue;
    long old_jwsTrapPortRxThresholdEnableValue;
    long jwsTrapPortRxThresholdPercentage;
    long old_jwsTrapPortRxThresholdPercentage;

    /* Illustrate using a simple linked list */
    int   valid;
    struct jwsTrapPortConfigTable_entry *next;
};

struct jwsTrapPortConfigTable_entry  *jwsTrapPortConfigTable_head;

/* create a new row in the (unsorted) table */
struct jwsTrapPortConfigTable_entry *
jwsTrapPortConfigTable_createEntry(
                 long  jwsTrapPortId
                ) {
    struct jwsTrapPortConfigTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct jwsTrapPortConfigTable_entry);
    if (!entry)
        return NULL;

    entry->jwsTrapPortId = jwsTrapPortId;
    entry->next = jwsTrapPortConfigTable_head;
    jwsTrapPortConfigTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
jwsTrapPortConfigTable_removeEntry( struct jwsTrapPortConfigTable_entry *entry ) {
    struct jwsTrapPortConfigTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = jwsTrapPortConfigTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        jwsTrapPortConfigTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
jwsTrapPortConfigTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = jwsTrapPortConfigTable_head;
    return jwsTrapPortConfigTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
jwsTrapPortConfigTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct jwsTrapPortConfigTable_entry *entry = (struct jwsTrapPortConfigTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->jwsTrapPortId );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the jwsTrapPortConfigTable table */
int
jwsTrapPortConfigTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct jwsTrapPortConfigTable_entry          *table_entry;

    int ret = SNMP_ERR_NOERROR;
    int data = 0;
    char str[100] = {0};
    char value[50] = {0};

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsTrapPortConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPPORTID:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapPortId);
                break;
            case COLUMN_JWSTRAPPORTSTATUSENABLEVALUE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_trap");
                ret = ubus_invoke_command("trap", "trap_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsTrapPortStatusEnableValue = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapPortStatusEnableValue);
                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDENABLEVALUE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_tx_shreshold_enable");
                ret = ubus_invoke_command("trap", "trap_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsTrapPortTxThresholdEnableValue = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapPortTxThresholdEnableValue);
                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDPERCENTAGE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_tx_shreshold_perc");
                ret = ubus_invoke_command("trap", "trap_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsTrapPortTxThresholdPercentage = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapPortTxThresholdPercentage);
                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDENABLEVALUE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_rx_shreshold_enable");
                ret = ubus_invoke_command("trap", "trap_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsTrapPortRxThresholdEnableValue = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapPortRxThresholdEnableValue);
                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDPERCENTAGE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_rx_shreshold_perc");
                ret = ubus_invoke_command("trap", "trap_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsTrapPortRxThresholdPercentage = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapPortRxThresholdPercentage);
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
            table_entry = (struct jwsTrapPortConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPPORTSTATUSENABLEVALUE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDENABLEVALUE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDPERCENTAGE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDENABLEVALUE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDPERCENTAGE:
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
            table_entry = (struct jwsTrapPortConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPPORTSTATUSENABLEVALUE:
                table_entry->old_jwsTrapPortStatusEnableValue = table_entry->jwsTrapPortStatusEnableValue;
                table_entry->jwsTrapPortStatusEnableValue     = *request->requestvb->val.integer;
                
                snprintf(value, sizeof(value), "%ld", table_entry->jwsTrapPortStatusEnableValue);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_trap", value);
                ret = ubus_invoke_command("trap", "trap_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDENABLEVALUE:
                table_entry->old_jwsTrapPortTxThresholdEnableValue = table_entry->jwsTrapPortTxThresholdEnableValue;
                table_entry->jwsTrapPortTxThresholdEnableValue     = *request->requestvb->val.integer;
                
                snprintf(value, sizeof(value), "%ld", table_entry->jwsTrapPortTxThresholdEnableValue);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_tx_shreshold_enable", value);
                ret = ubus_invoke_command("trap", "trap_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDPERCENTAGE:
                table_entry->old_jwsTrapPortTxThresholdPercentage = table_entry->jwsTrapPortTxThresholdPercentage;
                table_entry->jwsTrapPortTxThresholdPercentage     = *request->requestvb->val.integer;
                
                snprintf(value, sizeof(value), "%ld", table_entry->jwsTrapPortTxThresholdPercentage);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_tx_shreshold_perc", value);
                ret = ubus_invoke_command("trap", "trap_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDENABLEVALUE:
                table_entry->old_jwsTrapPortRxThresholdEnableValue = table_entry->jwsTrapPortRxThresholdEnableValue;
                table_entry->jwsTrapPortRxThresholdEnableValue     = *request->requestvb->val.integer;
                
                snprintf(value, sizeof(value), "%ld", table_entry->jwsTrapPortRxThresholdEnableValue);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_rx_shreshold_enable", value);
                ret = ubus_invoke_command("trap", "trap_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDPERCENTAGE:
                table_entry->old_jwsTrapPortRxThresholdPercentage = table_entry->jwsTrapPortRxThresholdPercentage;
                table_entry->jwsTrapPortRxThresholdPercentage     = *request->requestvb->val.integer;
                
                snprintf(value, sizeof(value), "%ld", table_entry->jwsTrapPortRxThresholdPercentage);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsTrapPortId, "port_rx_shreshold_perc", value);
                ret = ubus_invoke_command("trap", "trap_set", 
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
            table_entry = (struct jwsTrapPortConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPPORTSTATUSENABLEVALUE:
                table_entry->jwsTrapPortStatusEnableValue     = table_entry->old_jwsTrapPortStatusEnableValue;
                table_entry->old_jwsTrapPortStatusEnableValue = 0;
                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDENABLEVALUE:
                table_entry->jwsTrapPortTxThresholdEnableValue     = table_entry->old_jwsTrapPortTxThresholdEnableValue;
                table_entry->old_jwsTrapPortTxThresholdEnableValue = 0;
                break;
            case COLUMN_JWSTRAPPORTTXTHRESHOLDPERCENTAGE:
                table_entry->jwsTrapPortTxThresholdPercentage     = table_entry->old_jwsTrapPortTxThresholdPercentage;
                table_entry->old_jwsTrapPortTxThresholdPercentage = 0;
                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDENABLEVALUE:
                table_entry->jwsTrapPortRxThresholdEnableValue     = table_entry->old_jwsTrapPortRxThresholdEnableValue;
                table_entry->old_jwsTrapPortRxThresholdEnableValue = 0;
                break;
            case COLUMN_JWSTRAPPORTRXTHRESHOLDPERCENTAGE:
                table_entry->jwsTrapPortRxThresholdPercentage     = table_entry->old_jwsTrapPortRxThresholdPercentage;
                table_entry->old_jwsTrapPortRxThresholdPercentage = 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
//  # Determine the first/last column names

/** Initialize the jwsTrapRingConfigTable table by defining its contents and how it's structured */
void
initialize_table_jwsTrapRingConfigTable(void)
{
    static oid jwsTrapRingConfigTable_oid[] = {1,3,6,1,4,1,33444,2,2,100,1,3};
    size_t jwsTrapRingConfigTable_oid_len   = OID_LENGTH(jwsTrapRingConfigTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;
    int i;

    reg = netsnmp_create_handler_registration(
              "jwsTrapRingConfigTable",     jwsTrapRingConfigTable_handler,
              jwsTrapRingConfigTable_oid, jwsTrapRingConfigTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: jwsTrapRingId */
                           0);
    table_info->min_column = COLUMN_JWSTRAPRINGID;
    table_info->max_column = COLUMN_JWSTRAPRINGSTATUSENABLEVALUE;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = jwsTrapRingConfigTable_get_first_data_point;
    iinfo->get_next_data_point  = jwsTrapRingConfigTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
    for (i = 1; i <= 5; i++)
    {
        jwsTrapRingConfigTable_createEntry(i);
    }
}

    /* Typical data structure for a row entry */
struct jwsTrapRingConfigTable_entry {
    /* Index values */
    //long jwsTrapRingId;

    /* Column values */
    long jwsTrapRingId;
    long jwsTrapRingStatusEnableValue;
    long old_jwsTrapRingStatusEnableValue;

    /* Illustrate using a simple linked list */
    int   valid;
    struct jwsTrapRingConfigTable_entry *next;
};

struct jwsTrapRingConfigTable_entry  *jwsTrapRingConfigTable_head;

/* create a new row in the (unsorted) table */
struct jwsTrapRingConfigTable_entry *
jwsTrapRingConfigTable_createEntry(
                 long  jwsTrapRingId
                ) {
    struct jwsTrapRingConfigTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct jwsTrapRingConfigTable_entry);
    if (!entry)
        return NULL;

    entry->jwsTrapRingId = jwsTrapRingId;
    entry->next = jwsTrapRingConfigTable_head;
    jwsTrapRingConfigTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
jwsTrapRingConfigTable_removeEntry( struct jwsTrapRingConfigTable_entry *entry ) {
    struct jwsTrapRingConfigTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = jwsTrapRingConfigTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        jwsTrapRingConfigTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
jwsTrapRingConfigTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = jwsTrapRingConfigTable_head;
    return jwsTrapRingConfigTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
jwsTrapRingConfigTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct jwsTrapRingConfigTable_entry *entry = (struct jwsTrapRingConfigTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->jwsTrapRingId );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the jwsTrapRingConfigTable table */
int
jwsTrapRingConfigTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct jwsTrapRingConfigTable_entry          *table_entry;

    int ret = SNMP_ERR_NOERROR;

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsTrapRingConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPRINGID:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapRingId);
                break;
            case COLUMN_JWSTRAPRINGSTATUSENABLEVALUE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsTrapRingStatusEnableValue);
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
            table_entry = (struct jwsTrapRingConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPRINGSTATUSENABLEVALUE:
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
            table_entry = (struct jwsTrapRingConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPRINGSTATUSENABLEVALUE:
                table_entry->old_jwsTrapRingStatusEnableValue = table_entry->jwsTrapRingStatusEnableValue;
                table_entry->jwsTrapRingStatusEnableValue     = *request->requestvb->val.integer;
                break;
            }
        }
        break;

    case MODE_SET_UNDO:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsTrapRingConfigTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSTRAPRINGSTATUSENABLEVALUE:
                table_entry->jwsTrapRingStatusEnableValue     = table_entry->old_jwsTrapRingStatusEnableValue;
                table_entry->old_jwsTrapRingStatusEnableValue = 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}