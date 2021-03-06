#include <Copyright.h>

/********************************************************************************
* gtPortCtrl.c
*
* DESCRIPTION:
*       API implementation for switch port control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <msApi.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>
#include <gtSem.h>

/*******************************************************************************
* gprtSetForceFc
*
* DESCRIPTION:
*       This routine set the force flow control state.
*
* INPUTS:
*       port  - the logical port number.
*       force - GT_TRUE for force flow control  or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetForceFc
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_BOOL    force
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_PORT_STP_STATE  state;
	GT_U16			tmpData;

    DBG_INFO(("gprtSetForceFc Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (IS_IN_DEV_GROUP(dev,DEV_FC_WITH_VALUE))
    {
        if(force)
            data = 3;
        else
            data = 0;

		if(IS_IN_DEV_GROUP(dev,DEV_88E6390_FAMILY))
		{
#if 0
			/* Set pointer, 0x10:Port Flow Control Mode */
			retVal = hwSetPortRegField(dev,hwPort,QD_REG_FLOW_CONTROL,8,7,0x10);
			if(retVal != GT_OK)
			{
				DBG_INFO(("Failed.\n"));
				return retVal;
			}
			retVal = hwGetPortRegField(dev,hwPort,QD_REG_FLOW_CONTROL,0,8,&tmpData);
			if(retVal != GT_OK)
			{
				DBG_INFO(("Failed.\n"));
				return retVal;
			}
			data = (1 << 15) | (0x10 << 8) | (data << 2) | tmpData;		
			retVal = hwWritePortReg(dev,hwPort,QD_REG_FLOW_CONTROL,data);
#else
			GT_U8 tmpDatac;
			retVal = gportPointDataGet(dev,hwPort,QD_REG_FLOW_CONTROL,0x10,&tmpDatac);
			if(retVal != GT_OK)
			{
				DBG_INFO(("Failed.\n"));
				return retVal;
			}
			tmpDatac &= ~0x0c;
			tmpData = tmpDatac;
			retVal = gportPointDataSet(dev,hwPort,QD_REG_FLOW_CONTROL,0x10, ((data << 2) | tmpData));

#endif
		}
		else
		{
			retVal = hwSetPortRegField(dev,hwPort,QD_REG_PCS_CONTROL,6,2,data);
		}
   
        if(retVal != GT_OK)
        {
            DBG_INFO(("Failed.\n"));
        }
        else
        {
            DBG_INFO(("OK.\n"));
        }
        return retVal;        
    }

    /* Port should be disabled before Set Force Flow Control bit */
    retVal = gstpGetPortState(dev,port, &state);
    if(retVal != GT_OK)
    {
        DBG_INFO(("gstpGetPortState failed.\n"));
        return retVal;
    }

    retVal = gstpSetPortState(dev,port, GT_PORT_DISABLE);
    if(retVal != GT_OK)
    {
        DBG_INFO(("gstpSetPortState failed.\n"));
        return retVal;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(force, data);

    /* Set the force flow control bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_FLOW_CONTROL,15,1,data);  
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    /* Restore original stp state. */
    if(gstpSetPortState(dev,port, state) != GT_OK)
    {
        DBG_INFO(("gstpSetPortState failed.\n"));
        return GT_FAIL;
    }

    return retVal;
}



/*******************************************************************************
* gprtGetForceFc
*
* DESCRIPTION:
*       This routine get the force flow control state.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       force - GT_TRUE for force flow control  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetForceFc
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_BOOL    *force
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
	GT_U8			mask;

    DBG_INFO(("gprtGetForceFc Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (IS_IN_DEV_GROUP(dev,DEV_FC_WITH_VALUE))
    {
		if(IS_IN_DEV_GROUP(dev,DEV_88E6390_FAMILY))
		{
#if 0
			/* Set pointer, 0x10:Port Flow Control Mode */
			retVal = hwSetPortRegField(dev,hwPort,QD_REG_FLOW_CONTROL,8,7,0x10);
			if(retVal != GT_OK)
			{
				DBG_INFO(("Failed.\n"));
				return retVal;
			}
			retVal = hwGetPortRegField(dev,hwPort,QD_REG_FLOW_CONTROL,2,2,&data);
			mask = 0x2;
#else
			GT_U8 datac;
			retVal = gportPointDataGet(dev,hwPort,QD_REG_FLOW_CONTROL,0x10,&datac);
			if(retVal != GT_OK)
			{
				DBG_INFO(("Failed.\n"));
				return retVal;
			}
			data = 0;
			data = datac;
			data >>= 2;
			mask = 0x1;
#endif
		}
		else
		{
			retVal = hwGetPortRegField(dev,hwPort,QD_REG_PCS_CONTROL,6,2,&data);
			mask = 0x1;
		}
     
        if(retVal != GT_OK)
        {
            DBG_INFO(("Failed.\n"));
        }
        else
        {
            DBG_INFO(("OK.\n"));
        }

        if(data & mask)
            *force = GT_TRUE;
        else
            *force = GT_FALSE;
            
        return retVal;        
    }

    /* Get the force flow control bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_FLOW_CONTROL,15,1,&data);  
    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *force);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetUseCoreTag
*
* DESCRIPTION:
*        This routine set the UseCoreTag bit in Port Control Register.
*        When this bit is cleared to a zero, ingressing frames are considered
*        Tagged if the 16-bits following the frame's Source Address is 0x8100.
*        When this bit is set to a one, ingressing frames are considered Tagged
*        if the 16-bits following the frame's Source Address is equal to the 
*        CoreTag register value.
*
* INPUTS:
*       port  - the logical port number.
*       force - GT_TRUE for force flow control  or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetUseCoreTag
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_BOOL    force
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetUseCoreTag Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_CORE_TAG))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(force, data);

    /* Set the UseCoreTag bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,15,1,data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}



/*******************************************************************************
* gprtGetUseCoreTag
*
* DESCRIPTION:
*       This routine get the Use Core Tag state.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       force - GT_TRUE for using core tag register  or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetUseCoreTag
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_BOOL    *force
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetUseCoreTag Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_CORE_TAG))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the UseCoreTag bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,15,1,&data);
    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *force);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetTrailerMode
*
* DESCRIPTION:
*       This routine set the egress trailer mode.
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE for add trailer or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetTrailerMode
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_BOOL    mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetTrailerMode Called.\n"));

    /* check if device supports this feature */
    if (!((IS_IN_DEV_GROUP(dev,DEV_TRAILER)) ||
          (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5)) ||
          (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P4P5))))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if(hwPort < 4)
    {
        /* check if device supports this feature for this port */
        if ((IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5)) ||
            (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P4P5)))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }
    else if(hwPort == 4)
    {
        /* check if device supports this feature for this port*/
        if (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    /* Set the trailer mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,14,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}



/*******************************************************************************
* gprtGetTrailerMode
*
* DESCRIPTION:
*       This routine get the egress trailer mode.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE for add trailer or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetTrailerMode
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_BOOL    *mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetTrailerMode Called.\n"));

    /* check if device supports this feature */
    if (!((IS_IN_DEV_GROUP(dev,DEV_TRAILER)) ||
          (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5)) ||
          (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P4P5))))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if(hwPort < 4)
    {
        /* check if device supports this feature for this port */
        if ((IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5)) ||
            (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P4P5)))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }
    else if(hwPort == 4)
    {
        /* check if device supports this feature for this port */
        if (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    /* Get the Trailer mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,14,1,&data);
    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}




/*******************************************************************************
* gprtSetIngressMode
*
* DESCRIPTION:
*       This routine set the ingress mode.
*
* INPUTS:
*       port - the logical port number.
*       mode - the ingress mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIngressMode
(
    IN  GT_QD_DEV      *dev,
    IN GT_LPORT        port,
    IN GT_INGRESS_MODE mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetIngressMode Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Gigabit Switch does not support this status. */
    if (IS_IN_DEV_GROUP(dev,DEV_GIGABIT_SWITCH))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if device supports this feature */
    switch (mode)
    {
        case (GT_UNMODIFY_INGRESS):
            break;

        case (GT_TRAILER_INGRESS):
            if (!((IS_IN_DEV_GROUP(dev,DEV_TRAILER)) ||
                  (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5)) ||
                  (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P4P5))))
     {
                DBG_INFO(("Given ingress mode is not supported by this device\n"));
                return GT_NOT_SUPPORTED;
            }
            break;

        case (GT_UNTAGGED_INGRESS):
            if(!(IS_IN_DEV_GROUP(dev,DEV_TAGGING)))
            {
                DBG_INFO(("Given ingress mode is not supported by this device\n"));
                return GT_NOT_SUPPORTED;
            }
            break;

        case (GT_CPUPORT_INGRESS):
            if(!(IS_IN_DEV_GROUP(dev,DEV_IGMP_SNOOPING)))
            {
                DBG_INFO(("Given ingress mode is not supported by this device\n"));
                return GT_NOT_SUPPORTED;
            }

            if(hwPort != GT_LPORT_2_PORT(dev->cpuPortNum))
            {
                DBG_INFO(("Given ingress mode is supported by CPU port only\n"));
                return GT_NOT_SUPPORTED;
            }

            break;

        default:
            DBG_INFO(("Failed.\n"));
            return GT_FAIL;
    }

    /* Set the Ingress Mode.        */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,8,2,(GT_U16)mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}



/*******************************************************************************
* gprtGetIngressMode
*
* DESCRIPTION:
*       This routine get the ingress mode.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - the ingress mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIngressMode
(
    IN  GT_QD_DEV      *dev,
    IN  GT_LPORT        port,
    OUT GT_INGRESS_MODE *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetIngressMode Called.\n"));

    /* Gigabit Switch does not support this status. */
    if (IS_IN_DEV_GROUP(dev,DEV_GIGABIT_SWITCH))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);
    /* Get the Ingress Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 8, 2,&data);
    *mode = data;
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetMcRateLimit
*
* DESCRIPTION:
*       This routine set the port multicast rate limit.
*
* INPUTS:
*       port - the logical port number.
*       rate - GT_TRUE to Enable, GT_FALSE for otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetMcRateLimit
(
    IN  GT_QD_DEV   *dev,
    IN GT_LPORT     port,
    IN GT_MC_RATE   rate
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetMcRateLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* this feature only exits in 6051, 6052, and 6012. It is replaced with
     * Rate Cotrol Register in the future products, starting from clippership
     */
    if((retVal = IS_VALID_API_CALL(dev,hwPort, DEV_MC_RATE_PERCENT)) != GT_OK)
        return retVal;

    /* Set the multicast rate limit.    */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,2,2,(GT_U16)rate);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}



/*******************************************************************************
* gprtGetMcRateLimit
*
* DESCRIPTION:
*       This routine Get the port multicast rate limit.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       rate - GT_TRUE to Enable, GT_FALSE for otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetMcRateLimit
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    OUT GT_MC_RATE  *rate
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read data        */

    DBG_INFO(("gprtGetMcRateLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* this feature only exits in 6051, 6052, and 6012. It is replaced with
     * Rate Cotrol Register in the future products, starting from clippership
     */
    if((retVal = IS_VALID_API_CALL(dev,hwPort, DEV_MC_RATE_PERCENT)) != GT_OK)
        return retVal;

    /* Get the multicast rate limit.    */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 2, 2,&data);
    *rate = data;
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/* the following two APIs are added to support fullsail and clippership */

/*******************************************************************************
* gprtSetIGMPSnoop
*
* DESCRIPTION:
*         This routine set the IGMP Snoop. When set to one and this port receives
*        IGMP frame, the frame is switched to the CPU port, overriding all other 
*        switching decisions, with exception for CPU's Trailer.
*        CPU port is determined by the Ingress Mode bits. A port is considered 
*        the CPU port if its Ingress Mode are either GT_TRAILER_INGRESS or 
*        GT_CPUPORT_INGRESS.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for IGMP Snoop or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIGMPSnoop
(
    IN  GT_QD_DEV   *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetIGMPSnoop Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if((retVal = IS_VALID_API_CALL(dev,hwPort, DEV_IGMP_SNOOPING)) != GT_OK)
      return retVal;

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set the IGMP Snooping mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,10,1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}



/*******************************************************************************
* gprtGetIGMPSnoop
*
* DESCRIPTION:
*       This routine get the IGMP Snoop mode.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE: IGMP Snoop enabled
*           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIGMPSnoop
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetIGMPSnoop Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if((retVal = IS_VALID_API_CALL(dev,hwPort, DEV_IGMP_SNOOPING)) != GT_OK)
      return retVal;

    /* Get the Ingress Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 10, 1, &data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/* the following two APIs are added to support clippership */

/*******************************************************************************
* gprtSetHeaderMode
*
* DESCRIPTION:
*       This routine set ingress and egress header mode of a switch port. 
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE for header mode  or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetHeaderMode
(
    IN  GT_QD_DEV   *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetHeaderMode Called.\n"));

    /* only devices beyond quarterdeck (6052) has this feature */
    /* Fullsail (DEV_QD_88E6502) is an exception, and does not support this feature */
    if(IS_VALID_API_CALL(dev,port, DEV_HEADER|DEV_HEADER_P5|DEV_HEADER_P4P5) != GT_OK)
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if(hwPort < 4)
    {
        if ((IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5)) ||
            (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P4P5)))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }
    else if(hwPort == 4)
    {
        if (IS_IN_DEV_GROUP(dev,DEV_HEADER_P5))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    /* Set the header mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,11,1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}



/*******************************************************************************
* gprtGetHeaderMode
*
* DESCRIPTION:
*       This routine gets ingress and egress header mode of a switch port. 
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE: header mode enabled
*           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetHeaderMode
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetHeaderMode Called.\n"));

    /* only devices beyond quarterdeck (6052) has this feature */
    /* Fullsail (DEV_QD_88E602) is an exception, and does not support this feature */
    if(IS_VALID_API_CALL(dev,port, DEV_HEADER|DEV_HEADER_P5|DEV_HEADER_P4P5) != GT_OK)
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if(hwPort < 4)
    {
        if ((IS_IN_DEV_GROUP(dev,DEV_TRAILER_P5)) ||
            (IS_IN_DEV_GROUP(dev,DEV_TRAILER_P4P5)))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }
    else if(hwPort == 4)
    {
        if (IS_IN_DEV_GROUP(dev,DEV_HEADER_P5))
        {
            DBG_INFO(("GT_NOT_SUPPORTED.\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    /* Get the Header Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 11, 1, &data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/* the following four APIs are added to support Octane */

/*******************************************************************************
* gprtSetProtectedMode
*
* DESCRIPTION:
*       This routine set protected mode of a switch port. 
*        When this mode is set to GT_TRUE, frames are allowed to egress port
*        defined by the 802.1Q VLAN membership for the frame's VID 'AND'
*        by the port's VLANTable if 802.1Q is enabled on the port. Both must
*        allow the frame to Egress.
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE for protected mode or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetProtectedMode
(
    IN  GT_QD_DEV   *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetProtectedMode Called.\n"));

    /* Check if this feature is supported */
    if(IS_VALID_API_CALL(dev,port, DEV_PORT_SECURITY) != GT_OK)
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_CROSS_CHIP_VLAN))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Set the protected mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,3,1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetProtectedMode
*
* DESCRIPTION:
*       This routine gets protected mode of a switch port. 
*        When this mode is set to GT_TRUE, frames are allowed to egress port
*        defined by the 802.1Q VLAN membership for the frame's VID 'AND'
*        by the port's VLANTable if 802.1Q is enabled on the port. Both must
*        allow the frame to Egress.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE: header mode enabled
*           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetProtectedMode
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetProtectedMode Called.\n"));

    if(IS_VALID_API_CALL(dev,port, DEV_PORT_SECURITY) != GT_OK)
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_CROSS_CHIP_VLAN))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the protected Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 3, 1, &data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gprtSetForwardUnknown
*
* DESCRIPTION:
*       This routine set Forward Unknown mode of a switch port. 
*        When this mode is set to GT_TRUE, normal switch operation occurs.
*        When this mode is set to GT_FALSE, unicast frame with unknown DA addresses
*        will not egress out this port.
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE for protected mode or GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetForwardUnknown
(
    IN  GT_QD_DEV   *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetForwardUnknown Called.\n"));

    if(IS_VALID_API_CALL(dev,port, DEV_PORT_SECURITY|DEV_EGRESS_FLOOD) != GT_OK)
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

	data |= 0x2;

    /* Set the forward unknown mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,2,2, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetForwardUnknown
*
* DESCRIPTION:
*       This routine gets Forward Unknown mode of a switch port. 
*        When this mode is set to GT_TRUE, normal switch operation occurs.
*        When this mode is set to GT_FALSE, unicast frame with unknown DA addresses
*        will not egress out this port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE: header mode enabled
*           GT_FALSE otherwise
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetForwardUnknown
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetForwardUnknown Called.\n"));

    if(IS_VALID_API_CALL(dev,port, DEV_PORT_SECURITY|DEV_EGRESS_FLOOD) != GT_OK)
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

	data |= 0x2;

    /* Get the forward unknown Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 2, 2, &data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetDropOnLock
*
* DESCRIPTION:
*        This routine set the Drop on Lock. When set to one, Ingress frames will
*        be discarded if their SA field is not in the ATU's address database.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for Unknown SA drop or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDropOnLock
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!IS_IN_DEV_GROUP(dev,DEV_CASCADE_PORT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set the DropOnLock mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,14,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}



/*******************************************************************************
* gprtGetDropOnLock
*
* DESCRIPTION:
*        This routine gets DropOnLock mode.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: DropOnLock enabled,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDropOnLock
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!IS_IN_DEV_GROUP(dev,DEV_CASCADE_PORT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the DropOnLock Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 14, 1, &data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*Liu, W/R per port reg InitalPri (0x4)*/

/*******************************************************************************
* gprtSetInitialPri
*
* DESCRIPTION:
*       This routine set initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            GT_INITIAL_PRI_DEFAULT
*               
*            GT_INITIAL_PRI_TAG
*                
*            GT_INITIAL_PRI_IP
*                
*            GT_INITIAL_PRI_TAG_IP
*                
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_INITIAL_PRI type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetInitialPri
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    IN  GT_INITIAL_PRI      mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16            data;

    DBG_INFO(("gprtSetInitialPri Called.\n"));

    /* check if device supports the feature */
    if (!IS_IN_DEV_GROUP(dev, DEV_INITIAL_PRI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    data = (GT_U16) mode;

    /* Set the GT_INITIAL_PRI mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,4,2,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetInitialPri
*
* DESCRIPTION:
*       This routine gets initial Priority assignment.
*       Each frame entering a port is assigned a Frame Priority(FPri) and 
*       a Queue Priority (QPri). This mode can be assigned as follows:
*            GT_INITIAL_PRI_DEFAULT
*               
*            GT_INITIAL_PRI_TAG
*                
*            GT_INITIAL_PRI_IP
*                
*            GT_INITIAL_PRI_TAG_IP
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_INITIAL_PRI type
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetInitialPri
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    OUT GT_INITIAL_PRI      *mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetInitialPri Called.\n"));

    /* check if device supports the feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_INITIAL_PRI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* get the GT_INITIAL_PRI type            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,4,2,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    *mode = (GT_INITIAL_PRI) data;

    return retVal;
}



/*******************************************************************************
* gprtSetDoubleTag
*
* DESCRIPTION:
*        This routine set the Ingress Double Tag Mode. When set to one, 
*        ingressing frames are examined to see if they contain an 802.3ac tag.
*        If they do, the tag is removed and then the frame is processed from
*        there (i.e., removed tag is ignored). Essentially, untagged frames
*        remain untagged, single tagged frames become untagged and double tagged
*        frames become single tagged.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for DoulbeTag mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDoubleTag
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetDoubleTag Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!IS_IN_DEV_GROUP(dev,DEV_INGRESS_DOUBLE_TAGGING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set the DoubleTag mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,9,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}



/*******************************************************************************
* gprtGetDoubleTag
*
* DESCRIPTION:
*        This routine gets DoubleTag mode.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: DoubleTag enabled,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDoubleTag
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetDoubleTag Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!IS_IN_DEV_GROUP(dev,DEV_INGRESS_DOUBLE_TAGGING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the DoubleTag Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 9, 1, &data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetInterswitchPort
*
* DESCRIPTION:
*        This routine set Interswitch Port. When set to one, 
*        it indicates this port is a interswitch port used to communicated with
*        CPU or to cascade with another switch device.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for Interswitch port or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetInterswitchPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetInterswitchPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!IS_IN_DEV_GROUP(dev,DEV_CASCADE_PORT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set the InterswitchPort.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,8,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}



/*******************************************************************************
* gprtGetInterswitchPort
*
* DESCRIPTION:
*        This routine gets InterswitchPort.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: This port is interswitch port,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetInterswitchPort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetInterswitchPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!IS_IN_DEV_GROUP(dev,DEV_CASCADE_PORT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the InterswitchPort Mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 8, 1, &data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gprtSetLearnDisable
*
* DESCRIPTION:
*        This routine enables/disables automatic learning of new source MAC
*        addresses on the given port ingress
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for disable or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetLearnDisable
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          pav;

    DBG_INFO(("gprtSetLearnDisable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);
   
    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);


    if (IS_IN_DEV_GROUP(dev, DEV_NO_LEARN_DISABLE_BIT))
    {
	/* Set the port's PAV to all zeros */
		if(mode)
			pav = 0;
		else
			pav = (GT_U16)(1 << port);
		retVal = gpavSetPAV(dev, port, pav);
    }
    else
    {	
	 /* Set the LearnDisable mode. */
		retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_VLAN_MAP,11,1,data);
    }
	   
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetLearnDisable
*
* DESCRIPTION:
*        This routine gets LearnDisable setup
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: Learning disabled on the given port ingress frames,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetLearnDisable
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
)
{
   GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */
    GT_U16          pav;

    DBG_INFO(("gprtGetLearnDisable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (IS_IN_DEV_GROUP(dev, DEV_NO_LEARN_DISABLE_BIT))
    {
		retVal = gpavGetPAV(dev, port, &pav);
		if(retVal != GT_OK)
		{
        	DBG_INFO(("Failed.\n"));
			return retVal;
    	}
        if (pav)
			data = 0;
		else
			data = 1;
	}
    else
    {
		/* Get the LearnDisable Mode.            */
		retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_VLAN_MAP, 11, 1, &data);
    }

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gprtSetIgnoreFCS
*
* DESCRIPTION:
*        This routine sets FCS Ignore mode. When this bit is set to a one,
*        the last four bytes of frames received on this port are overwritten with
*        a good CRC and the frames will be accepted by the switch.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for ignore FCS or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIgnoreFCS
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL         mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetIgnoreFCS Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!(IS_IN_DEV_GROUP(dev, DEV_IGNORE_FCS))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set the IgnoreFCS mode.            */
    if ((IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY)) ||
        (IS_IN_DEV_GROUP(dev,DEV_ENHANCED_FE_SWITCH)) ||
        (IS_IN_DEV_GROUP(dev,DEV_FE_AVB_FAMILY)))
    {
        retVal = hwSetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,15,1,data );
    }
    else
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_VLAN_MAP,10,1,data);
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetIgnoreFCS
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE: Ignore FCS on the given port's ingress frames,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIgnoreFCS
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetIgnoreFCS Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Only Gigabit Switch supports this status. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_IGNORE_FCS))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the IgnoreFCS Mode.            */
    if ((IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY)) ||
        (IS_IN_DEV_GROUP(dev,DEV_ENHANCED_FE_SWITCH) ||
        (IS_IN_DEV_GROUP(dev,DEV_FE_AVB_FAMILY))))
    {
        retVal = hwGetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,15,1,&data );
    }
    else
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_VLAN_MAP, 10, 1, &data);
    }

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gprtSetAllowedBad
*
* DESCRIPTION:
*        Allow receiving frames on this port with a bad FCS..
*
* INPUTS:
*        port - the logical port number.
*        allowed - GT_TRUE, frames receviced on this port with a CRC error are not 
*        discarded.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetAllowedBad
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_BOOL         allowed
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetAllowedBad Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Opus supports this status. */
    if (!(IS_IN_DEV_GROUP(dev, DEV_ALLOWED_BAD_FCS))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(allowed, data);

    /* Set Allowed bad FCS..            */
    retVal = hwSetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,14,1,data );

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetAllowedBad
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* DESCRIPTION:
*        Allow receiving frames on this port with a bad FCS..
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        allowed - GT_TRUE, frames receviced on this port with a CRC error are not 
*        discarded.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetAllowedBad
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *allowed
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetAllowedBad Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Opus supports this status. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_ALLOWED_BAD_FCS))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get Allowed bad FCS..            */
    retVal = hwGetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,14,1,&data );
    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *allowed);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetVTUPriOverride
*
* DESCRIPTION:
*        VTU Priority Override. The following modes are supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PRI_OVERRIDE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtSetVTUPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_PRI_OVERRIDE        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetVTUPriOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PRIORITY_OVERRIDE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    switch (mode)
    {
        case PRI_OVERRIDE_NONE:
            data = 0;
            break;
        case PRI_OVERRIDE_FRAME_QUEUE:
            if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                data = 3;
            }
            else
            {
                data = 1;
            }
            break;
        case PRI_OVERRIDE_FRAME:
            if (!IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                DBG_INFO(("Bad Parameter\n"));
                return GT_BAD_PARAM;
            }
            data = 1;
            break;
        case PRI_OVERRIDE_QUEUE:
            if (!IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                DBG_INFO(("Bad Parameter\n"));
                return GT_BAD_PARAM;
            }
            data = 2;
            break;
        default:
            DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
    }

    /* Set the VTUPri Override mode.            */
    if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PRI_OVERRIDE,10,2,data);
    }
    else
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2,14,1,data);
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetVTUPriOverride
*
* DESCRIPTION:
*        VTU Priority Override. The following modes are supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's VID (in the VTU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_PRI_OVERRIDE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtGetVTUPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_PRI_OVERRIDE        *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetVTUPriOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PRIORITY_OVERRIDE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the VTUPriOverride Mode.            */
    if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PRI_OVERRIDE,10,2,&data);
    }
    else
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 14, 1, &data);
        if(data == 1) 
            data = 3;
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    switch (data)
    {
        case 0:
            *mode = PRI_OVERRIDE_NONE;
            break;
        case 3:
            *mode = PRI_OVERRIDE_FRAME_QUEUE;
            break;
        case 1:
            *mode = PRI_OVERRIDE_FRAME;
            break;
        case 2:
            *mode = PRI_OVERRIDE_QUEUE;
            break;
        default:
            DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
    }

    return GT_OK;
}


/*******************************************************************************
* gprtSetSAPriOverride
*
* DESCRIPTION:
*        SA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PRI_OVERRIDE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtSetSAPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
	IN  GT_PRI_OVERRIDE mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetSAPriOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PRIORITY_OVERRIDE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    switch (mode)
    {
        case PRI_OVERRIDE_NONE:
            data = 0;
            break;
        case PRI_OVERRIDE_FRAME_QUEUE:
            if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                data = 3;
            }
            else
            {
                data = 1;
            }
            break;
        case PRI_OVERRIDE_FRAME:
            if (!IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                DBG_INFO(("Bad Parameter\n"));
                return GT_BAD_PARAM;
            }
            data = 1;
            break;
        case PRI_OVERRIDE_QUEUE:
            if (!IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                DBG_INFO(("Bad Parameter\n"));
                return GT_BAD_PARAM;
            }
            data = 2;
            break;
        default:
            DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
    }

    /* Set the SAPriOverride mode.            */
    if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PRI_OVERRIDE,12,2,data);
    }
    else
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2,13,1,data);
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetSAPriOverride
*
* DESCRIPTION:
*        SA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's SA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_PRI_OVERRIDE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtGetSAPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_PRI_OVERRIDE        *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetSAPriOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PRIORITY_OVERRIDE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the SAPriOverride Mode.            */
    if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PRI_OVERRIDE,12,2,&data);
    }
    else
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 13, 1, &data);
        if(data == 1) 
            data = 3;
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    switch (data)
    {
        case 0:
            *mode = PRI_OVERRIDE_NONE;
            break;
        case 3:
            *mode = PRI_OVERRIDE_FRAME_QUEUE;
            break;
        case 1:
            *mode = PRI_OVERRIDE_FRAME;
            break;
        case 2:
            *mode = PRI_OVERRIDE_QUEUE;
            break;
        default:
            DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
* gprtSetDAPriOverride
*
* DESCRIPTION:
*        DA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PRI_OVERRIDE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is invalid
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtSetDAPriOverride
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_PRI_OVERRIDE        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetDAPriOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PRIORITY_OVERRIDE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    switch (mode)
    {
        case PRI_OVERRIDE_NONE:
            data = 0;
            break;
        case PRI_OVERRIDE_FRAME_QUEUE:
            if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                data = 3;
            }
            else
            {
                data = 1;
            }
            break;
        case PRI_OVERRIDE_FRAME:
            if (!IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                DBG_INFO(("Bad Parameter\n"));
                return GT_BAD_PARAM;
            }
            data = 1;
            break;
        case PRI_OVERRIDE_QUEUE:
            if (!IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
            {
                DBG_INFO(("Bad Parameter\n"));
                return GT_BAD_PARAM;
            }
            data = 2;
            break;
        default:
            DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
    }

    /* Set the DAPriOverride mode.            */
    if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PRI_OVERRIDE,14,2,data);
    }
    else
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2,12,1,data);
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetDAPriOverride
*
* DESCRIPTION:
*        DA Priority Override. The following mode is supported:
*            PRI_OVERRIDE_NONE -
*                Normal frame priority processing occurs.
*            PRI_OVERRIDE_FRAME -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's FPri (frame priority).
*                If the frame egresses tagged, the priority in the frame will be
*                this new priority value.
*            PRI_OVERRIDE_QUEUE -
*                Priority assigned to the frame's DA (in the ATU table) is used
*                to overwite the frame's QPri (queue priority).
*                QPri is used internally to map the frame to one of the egress
*                queues inside the switch.
*            PRI_OVERRIDE_FRAME_QUEUE -
*                Both frame and queue overrides take place on the frame.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_PRI_OVERRIDE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        PRI_OVERRIDE_FRAME and PRI_OVERRIDE_QUEUE modes are supported only on
*        certain switch device. Please refer to the device datasheet.
*
*******************************************************************************/
GT_STATUS gprtGetDAPriOverride
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_PRI_OVERRIDE        *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetDAPriOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PRIORITY_OVERRIDE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the DAPriOverride Mode.            */
    if (IS_IN_DEV_GROUP(dev,DEV_EXT_PRIORITY_OVERRIDE))
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PRI_OVERRIDE,14,2,&data);
    }
    else
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 12, 1, &data);
        if(data == 1) 
            data = 3;
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    switch (data)
    {
        case 0:
            *mode = PRI_OVERRIDE_NONE;
            break;
        case 3:
            *mode = PRI_OVERRIDE_FRAME_QUEUE;
            break;
        case 1:
            *mode = PRI_OVERRIDE_FRAME;
            break;
        case 2:
            *mode = PRI_OVERRIDE_QUEUE;
            break;
        default:
            DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
    }

    return GT_OK;
}


/*******************************************************************************
* gprtSetCPUPort
*
* DESCRIPTION:
*        This routine sets CPU Port number. When Snooping is enabled on this port
*        or when this port is configured as an Interswitch Port and it receives a 
*        To_CPU frame, the switch needs to know what port on this device the frame 
*        should egress.
*
* INPUTS:
*        port - the logical port number.
*        cpuPort - CPU Port number or interswitch port where CPU Port is connected
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetCPUPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_LPORT     cpuPort
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetCPUPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);
    data = (GT_U16)GT_LPORT_2_PORT(cpuPort);

    if (!IS_IN_DEV_GROUP(dev,DEV_CPU_DEST_PER_PORT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Set the CPU Port.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2,0,4,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetCPUPort
*
* DESCRIPTION:
*        This routine gets CPU Logical Port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        cpuPort - CPU Port's logical number
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetCPUPort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_LPORT     *cpuLPort
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetCPUPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_CPU_DEST_PER_PORT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the CPUPort.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 0, 4, &data);

    *cpuLPort = GT_PORT_2_LPORT((GT_U8)data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetLockedPort
*
* DESCRIPTION:
*        This routine sets LockedPort. When it's set to one, CPU directed 
*        learning for 802.1x MAC authentication is enabled on this port. In this
*        mode, an ATU Miss Violation interrupt will occur when a new SA address
*        is received in a frame on this port. Automatically SA learning and 
*        refreshing is disabled in this mode.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for Locked Port, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetLockedPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetLockedPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_LOCKED_PORT)) )
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set Locked Port.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 13, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetLockedPort
*
* DESCRIPTION:
*        This routine gets Locked Port mode for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if LockedPort, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetLockedPort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetLockedPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_LOCKED_PORT)) )
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the LockedPort. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 13, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetIgnoreWrongData
*
* DESCRIPTION:
*        This routine sets Ignore Wrong Data. If the frame's SA address is found 
*        in the database and if the entry is 'static' or if the port is 'locked'
*        the source port's bit is checked to insure the SA has been assigned to 
*        this port. If the SA is NOT assigned to this port, it is considered an 
*        ATU Member Violation. If the IgnoreWrongData is set to GT_FALSE, an ATU
*        Member Violation interrupt will be generated. If it's set to GT_TRUE,
*        the ATU Member Violation error will be masked and ignored.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE for IgnoreWrongData, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIgnoreWrongData
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_IGNORE_WRONG_DAT)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set IgnoreWrongData.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 12, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetIgnoreWrongData
*
* DESCRIPTION:
*        This routine gets Ignore Wrong Data mode for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if IgnoreWrongData, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIgnoreWrongData
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_IGNORE_WRONG_DAT)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the IgnoreWrongData. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 12, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetDiscardTagged
*
* DESCRIPTION:
*        When this bit is set to a one, all non-MGMT frames that are processed as 
*        Tagged will be discarded as they enter this switch port. Priority only 
*        tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to discard tagged frame, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDiscardTagged
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_DISCARD_TAGGED)) )
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set DiscardTagged. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 9, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetDiscardTagged
*
* DESCRIPTION:
*        This routine gets DiscardTagged bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if DiscardTagged bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDiscardTagged
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_DISCARD_TAGGED)) )
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the DiscardTagged. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 9, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetDiscardUntagged
*
* DESCRIPTION:
*        When this bit is set to a one, all non-MGMT frames that are processed as 
*        Untagged will be discarded as they enter this switch port. Priority only 
*        tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to discard untagged frame, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDiscardUntagged
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetDiscardUntagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_DISCARD_TAGGED)) )
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set DiscardUnTagged. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 8, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetDiscardUntagged
*
* DESCRIPTION:
*        This routine gets DiscardUntagged bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if DiscardUntagged bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDiscardUntagged
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetDiscardUnTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_DISCARD_TAGGED)) )
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the DiscardUnTagged. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 8, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gprtSetMapDA
*
* DESCRIPTION:
*        When this bit is set to a one, normal switch operation will occur where a 
*        frame's DA address is used to direct the frame out the correct port.
*        When this be is cleared to a zero, the frame will be sent out the port(s) 
*        defined by ForwardUnknown bits or the DefaultForward bits even if the DA 
*        is ound in the address database.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to use MapDA, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetMapDA
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_MAP_DA)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set MapDA. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 7, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetMapDA
*
* DESCRIPTION:
*        This routine gets MapDA bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if MapDA bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetMapDA
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_MAP_DA)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the MapDA. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 7, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetDefaultForward
*
* DESCRIPTION:
*        When this bit is set to a one, normal switch operation will occurs and 
*        multicast frames with unknown DA addresses are allowed to egress out this 
*        port (assuming the VLAN settings allow the frame to egress this port too).
*        When this bit is cleared to a zero, multicast frames with unknown DA 
*        addresses will not egress out this port.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to use DefaultForward, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetDefaultForward
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetDefaultForward Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set DefaultForward. */
    if (!IS_IN_DEV_GROUP(dev,DEV_EGRESS_FLOOD))
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 6, 1, data);
    }
    else
    {
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 3, 1, data);
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetDefaultForward
*
* DESCRIPTION:
*        This routine gets DefaultForward bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if DefaultForward bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDefaultForward
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetDefaultForward Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the DefaultForward. */
    if (!IS_IN_DEV_GROUP(dev,DEV_EGRESS_FLOOD))
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 6, 1, &data);
    }
    else
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 3, 1, &data);
    }

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gprtSetEgressMonitorSource
*
* DESCRIPTION:
*        When this bit is cleared to a zero, normal network switching occurs.
*        When this bit is set to a one, any frame that egresses out this port will
*        also be sent to the EgressMonitorDest Port
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to set EgressMonitorSource, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetEgressMonitorSource
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set EgressMonitorSource. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 5, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetEgressMonitorSource
*
* DESCRIPTION:
*        This routine gets EgressMonitorSource bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if EgressMonitorSource bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetEgressMonitorSource
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the EgressMonitorSource. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 5, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetIngressMonitorSource
*
* DESCRIPTION:
*        When this be is cleared to a zero, normal network switching occurs.
*        When this bit is set to a one, any frame that egresses out this port will
*        also be sent to the EgressMonitorDest Port
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to set EgressMonitorSource, GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetIngressMonitorSource
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set IngressMonitorSource. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 4, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetIngressMonitorSource
*
* DESCRIPTION:
*        This routine gets IngressMonitorSource bit for the given port
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode  - GT_TRUE if IngressMonitorSource bit is set, GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetIngressMonitorSource
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_88E6093_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the IngressMonitorSource. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2, 4, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetMessagePort
*
* DESCRIPTION:
*        When the Learn2All bit is set to one, learning message frames are 
*        generated. These frames will be sent out all ports whose Message Port is 
*        set to one.
*         If this feature is used, it is recommended that all Marvell Tag ports, 
*        except for the CPU's port, have their MessagePort bit set to one. 
*        Ports that are not Marvell Tag ports should not have their Message Port
*        bit set to one.
*        
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to make this port a Message Port. GT_FALSE, otherwise.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetMessagePort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_TRUNK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set Message Port. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 15, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetMessagePort
*
* DESCRIPTION:
*        When the Learn2All bit is set to one, learning message frames are 
*        generated. These frames will be sent out all ports whose Message Port is 
*        set to one.
*         If this feature is used, it is recommended that all Marvell Tag ports, 
*        except for the CPU's port, have their MessagePort bit set to one. 
*        Ports that are not Marvell Tag ports should not have their Message Port
*        bit set to one.
*
*        
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to make this port a Message Port. GT_FALSE, otherwise.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetMessagePort
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL     *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_TRUNK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the Message Port. */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 15, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetTrunkPort
*
* DESCRIPTION:
*        This function enables/disables and sets the trunk ID.
*        
* INPUTS:
*        port - the logical port number.
*        en - GT_TRUE to make the port be a member of a trunk with the given trunkId.
*             GT_FALSE, otherwise.
*        trunkId - valid ID is 0 ~ 15. (0 ~ 31 for Opus)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if trunkId is neither valid nor INVALID_TRUNK_ID
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetTrunkPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL         en,
    IN GT_U32        trunkId
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_TRUNK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(en, data);

    if(en == GT_TRUE)
    {
        /* need to enable trunk. so check the trunkId */
    if (IS_IN_DEV_GROUP(dev,DEV_TRUNK_1))
	{
        if (!IS_TRUNK_ID_VALID_1(dev, trunkId))
        {
            DBG_INFO(("GT_BAD_PARAM\n"));
            return GT_BAD_PARAM;
        }
	}
	else
	{
        if (!IS_TRUNK_ID_VALID(dev, trunkId))
        {
            DBG_INFO(("GT_BAD_PARAM\n"));
            return GT_BAD_PARAM;
        }
	}

        /* Set TrunkId. */
        if (IS_IN_DEV_GROUP(dev,DEV_TRUNK_NEW_ID_LOCATION))
        {
          if (IS_IN_DEV_GROUP(dev,DEV_TRUNK_1))
            retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 8, 5, (GT_U16)trunkId);
		  else
            retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 8, 4, (GT_U16)trunkId);
        }
        else
        {
            retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 4, 4, (GT_U16)trunkId);
        }

        if(retVal != GT_OK)
        {
               DBG_INFO(("Failed.\n"));
            return retVal;    
        }

    }
    else
    {
        /* 
           Need to reset trunkId for 88E6095 rev0.
        */
        if (IS_IN_DEV_GROUP(dev,DEV_NEW_FEATURE_IN_REV) &&
            ((GT_DEVICE_REV)dev->revision < GT_REV_1))
        {
            trunkId = 0;
        
            /* Set TrunkId. */
            retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 4, 4, (GT_U16)trunkId);
            if(retVal != GT_OK)
            {
                   DBG_INFO(("Failed.\n"));
                return retVal;    
            }
        }
    }

    /* Set TrunkPort bit. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 14, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetTrunkPort
*
* DESCRIPTION:
*        This function returns trunk state of the port.
*        When trunk is disabled, trunkId field won't have valid value.
*        
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_TRUE, if the port is a member of a trunk,
*             GT_FALSE, otherwise.
*        trunkId - 0 ~ 15)31 for Opus), valid only if en is GT_TRUE
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetTrunkPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    OUT GT_BOOL     *en,
    OUT GT_U32        *trunkId
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_TRUNK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    data = 0;

    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 14, 1, &data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }

    BIT_2_BOOL(data, *en);

    if (IS_IN_DEV_GROUP(dev,DEV_TRUNK_NEW_ID_LOCATION))
    {
      if (IS_IN_DEV_GROUP(dev,DEV_TRUNK_1))
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 8, 5, &data);
	  else
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 8, 4, &data);
    }
    else
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL1, 4, 4, &data);
    }

    *trunkId = (GT_U32)data;


    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine gets the Discard Broadcast Mode. If the mode is enabled,
*        all the broadcast frames to the given port will be discarded.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*        en - GT_TRUE, if enabled,
*             GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetDiscardBCastMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetDiscardBCastMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_DROP_BCAST))
    {
        if (!IS_IN_DEV_GROUP(dev,DEV_NEW_FEATURE_IN_REV) || 
            ((GT_DEVICE_REV)dev->revision < GT_REV_1))
        {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    data = 0;
    if (IS_IN_DEV_GROUP(dev,DEV_DROP_BCAST_2))
    {
      retVal = hwGetPortRegField(dev,hwPort, 0x19, 13, 1, &data);
	}
    else if (IS_IN_DEV_GROUP(dev,DEV_DROP_BCAST_1))
    {
      retVal = hwGetPortRegField(dev,hwPort, 0x5, 13, 1, &data);
	}
	else
	{
      retVal = hwGetPortRegField(dev,hwPort, 0x15, 6, 1, &data);
    }

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }

    BIT_2_BOOL(data, *en);

    return GT_OK;
}


/*******************************************************************************
* gprtSetDiscardBCastMode
*
* DESCRIPTION:
*       This routine sets the Discard Broadcast mode.
*        If the mode is enabled, all the broadcast frames to the given port will 
*        be discarded.
*
* INPUTS:
*       port - logical port number
*        en - GT_TRUE, to enable the mode,
*             GT_FALSE, otherwise.
*
* OUTPUTS:
*        None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetDiscardBCastMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
)
{
    GT_U16          data;           /* Used to poll the data */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetDiscardBCastMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_DROP_BCAST))
    {
        if (!IS_IN_DEV_GROUP(dev,DEV_NEW_FEATURE_IN_REV) || 
            ((GT_DEVICE_REV)dev->revision < GT_REV_1))
        {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(en, data);

    if (IS_IN_DEV_GROUP(dev,DEV_DROP_BCAST_2))
    {
      retVal = hwSetPortRegField(dev,hwPort, 0x19, 13, 1, data);
	}
    else if (IS_IN_DEV_GROUP(dev,DEV_DROP_BCAST_1))
    {
      retVal = hwSetPortRegField(dev,hwPort, 0x5, 13, 1, data);
	}
 	else
    {
      retVal = hwSetPortRegField(dev,hwPort, 0x15, 6, 1, data);
	}

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }

    return GT_OK;
}

/*******************************************************************************
* gprtGetFCOnRateLimitMode
*
* DESCRIPTION:
*       This routine returns mode that tells if ingress rate limiting uses Flow 
*        Control. When this mode is enabled and the port receives frames over the 
*        limit, Ingress Rate Limiting will be performed by stalling the 
*        link partner using flow control, instead of discarding frames.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*        en - GT_TRUE, if the mode is enabled,
*             GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        In order for this mode to work, Flow Control and Rate Limiting
*        should be configured properly.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetFCOnRateLimitMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetFCOnRateLimitMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_BURST_RATE))
    {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
    }

    data = 0;

    retVal = hwGetPortRegField(dev,hwPort, 0x15, 4, 2, &data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }

    if (data == 0x3)
        *en = GT_TRUE;
    else
        *en = GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* gprtSetFCOnRateLimitMode
*
* DESCRIPTION:
*       This routine sets the mode that tells if ingress rate limiting uses Flow 
*        Control. When this mode is enabled and the port receives frames over the 
*        limit, Ingress Rate Limiting will be performed by stalling the 
*        link partner using flow control, instead of discarding frames.
*
* INPUTS:
*       port - logical port number
*        en - GT_TRUE, to enable the mode,
*             GT_FALSE, otherwise.
*
* OUTPUTS:
*        None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       This routine won't configure Flow Control or Rate Limiting.
*        In order for this mode to work, Flow Control and Rate Limiting
*        should be configured properly.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetFCOnRateLimitMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetFCOnRateLimitMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_BURST_RATE))
    {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    if (en)
        data = 0x3;
    else
        data = 0;

    retVal = hwSetPortRegField(dev,hwPort, 0x15, 4, 2, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }

    return GT_OK;
}


/*******************************************************************************
* gprtSetSAFiltering
*
* DESCRIPTION:
*        This routine set the Source Address(SA) fitering method.
*            GT_SA_FILTERING_DISABLE :
*                no frame will be filtered.
*            GT_SA_DROP_ON_LOCK :
*                discard if SA field is not in the ATU's address database.
*            GT_SA_DROP_ON_UNLOCK : 
*                discard if SA field is in the ATU's address database as Static 
*                entry with a PortVec of all zeros.
*            GT_SA_DROP_TO_CPU : 
*                Ingressing frames will be mapped to the CPU Port if their SA 
*                field is in the ATU's address database as Static entry with a 
*                PortVec of all zeros. Otherwise, the frames will be discarded 
*                if their SA field is not in the ATU's address database or if this
*                port's bit is not set in the PortVec bits for the frame's SA.
*        
* INPUTS:
*       port - the logical port number.
*       mode - GT_SA_FILTERING structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtSetSAFiltering
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_SA_FILTERING    mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16            data;

    DBG_INFO(("gprtSetSAFiltering Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_SA_FILTERING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    data = (GT_U16) mode;

    /* Set the SA Filtering bits.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,14,2,data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtGetSAFiltering
*
* DESCRIPTION:
*        This routine gets the Source Address(SA) fitering method.
*            GT_SA_FILTERING_DISABLE :
*                no frame will be filtered.
*            GT_SA_DROP_ON_LOCK :
*                discard if SA field is not in the ATU's address database.
*            GT_SA_DROP_ON_UNLOCK : 
*                discard if SA field is in the ATU's address database as Static 
*                entry with a PortVec of all zeros.
*            GT_SA_DROP_TO_CPU : 
*                Ingressing frames will be mapped to the CPU Port if their SA 
*                field is in the ATU's address database as Static entry with a 
*                PortVec of all zeros. Otherwise, the frames will be discarded 
*                if their SA field is not in the ATU's address database or if this
*                port's bit is not set in the PortVec bits for the frame's SA.
*        
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_SA_FILTERING structure
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtGetSAFiltering
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_SA_FILTERING    *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16            data;

    DBG_INFO(("gprtSetSAFiltering Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_SA_FILTERING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the SA Filtering bits.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,14,2,&data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    *mode = (GT_SA_FILTERING)data;

    return retVal;
}


/*******************************************************************************
* gprtSetARPtoCPU
*
* DESCRIPTION:
*        When ARPtoCPU (or ARP Mirror) is set to GT_TRUE, ARP frames are mirrored 
*        to the CPU port.
*        
* INPUTS:
*       port - the logical port number.
*       mode - GT_TRUE, to map ARP frames to CPU Port,
*               GT_FALSE, otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtSetARPtoCPU
(
    IN GT_QD_DEV  *dev,
    IN GT_LPORT   port,
    IN GT_BOOL    mode
)
{
    GT_U16            data;
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetARPtoCPU Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_ARP_TO_CPU))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set the ARPtoCPU bits.  */
    if (IS_IN_DEV_GROUP(dev,DEV_FASTETH_SWITCH))
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,8,1,data);
    else
        retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2,6,1,data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtGetARPtoCPU
*
* DESCRIPTION:
*        When ARPtoCPU (or ARP Mirror) is set to GT_TRUE, ARP frames are mirrored 
*        to the CPU port.
*        
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_TRUE, to map ARP frames to CPU Port,
*               GT_FALSE, otherwise.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS gprtGetARPtoCPU
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_BOOL    *mode
)
{
    GT_U16            data;
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetARPtoCPU Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device supports the feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_ARP_TO_CPU))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the ARPtoCPU bits.  */
    if (IS_IN_DEV_GROUP(dev,DEV_FASTETH_SWITCH))
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,8,1,&data);
    else
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL2,6,1,&data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    BIT_2_BOOL(data, *mode);

    return retVal;
}


/*******************************************************************************
* gprtSetEgressFlood
*
* DESCRIPTION:
*       This routine set Egress Flooding Mode.
*        Frames with unknown DA (Destination Address that is not in ATU database)
*        generally flood out all the ports. This mode can be used to prevent
*        those frames from egressing this port as follows:
*            GT_BLOCK_EGRESS_UNKNOWN
*                do not egress frame with unknown DA (both unicast and multicast)
*            GT_BLOCK_EGRESS_UNKNOWN_MULTICAST
*                do not egress frame with unknown multicast DA
*            GT_BLOCK_EGRESS_UNKNOWN_UNICAST
*                do not egress frame with unknown unicast DA
*            GT_BLOCK_EGRESS_NONE
*                egress all frames with unknown DA
*
* INPUTS:
*       port - the logical port number.
*       mode - GT_EGRESS_FLOOD type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetEgressFlood
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    IN  GT_EGRESS_FLOOD      mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16            data;

    DBG_INFO(("gprtSetEgressFlood Called.\n"));

    /* check if device supports the feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_EGRESS_FLOOD))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    data = (GT_U16) mode;

    /* Set the Egress Flood mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,2,2,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetEgressFlood
*
* DESCRIPTION:
*       This routine gets Egress Flooding Mode.
*        Frames with unknown DA (Destination Address that is not in ATU database)
*        generally flood out all the ports. This mode can be used to prevent
*        those frames from egressing this port as follows:
*            GT_BLOCK_EGRESS_UNKNOWN
*                do not egress frame with unknown DA (both unicast and multicast)
*            GT_BLOCK_EGRESS_UNKNOWN_MULTICAST
*                do not egress frame with unknown multicast DA
*            GT_BLOCK_EGRESS_UNKNOWN_UNICAST
*                do not egress frame with unknown unicast DA
*            GT_BLOCK_EGRESS_NONE
*                egress all frames with unknown DA
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - GT_EGRESS_FLOOD type
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetEgressFlood
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT     port,
    OUT GT_EGRESS_FLOOD      *mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetEgressFlood Called.\n"));

    /* check if device supports the feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_EGRESS_FLOOD))
    {
        DBG_INFO(("GT_NOT_SUPPORTED.\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* get the Egress Flood mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL,2,2,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    *mode = (GT_EGRESS_FLOOD) data;

    return retVal;
}


/*******************************************************************************
* gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*            GT_PORT_SCHED_WEIGHTED_RRB - use 8,4,2,1 weighted fair scheduling
*            GT_PORT_SCHED_STRICT_PRI - use a strict priority scheme
*
*		 For Opus and Peridot:
*			 GT_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            GT_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
*
* INPUTS:
*        port - the logical port number
*        mode - GT_PORT_SCHED_MODE enum type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPortSched
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_PORT_SCHED_MODE        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetPortSched Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!((IS_IN_DEV_GROUP(dev,DEV_PORT_SCHEDULE)) ||
        (IS_IN_DEV_GROUP(dev,DEV_PORT_MIXED_SCHEDULE))))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_MIXED_SCHEDULE_1))
    {
        switch(mode)
        {
            case GT_PORT_SCHED_WEIGHTED_RRB:
                data = 0;
                break;
            case GT_PORT_SCHED_STRICT_PRI:
                data = 3;
                break;
            case GT_PORT_SCHED_STRICT_PRI3:
                data = 1;
                break;
            case GT_PORT_SCHED_STRICT_PRI2_3:
                data = 2;
                break;
            default:
                return GT_BAD_PARAM;
        }

        retVal = hwSetPortRegField(dev,hwPort, QD_REG_RATE_CTRL, 12,2,data);
        
        if(retVal != GT_OK)
        {
            DBG_INFO(("Failed.\n"));
        }
        else
        {
            DBG_INFO(("OK.\n"));
        }
        return retVal;

    }
	else
    {
        switch(mode)
        {
            case GT_PORT_SCHED_WEIGHTED_RRB_1:
                data = 0;
                break;
			case GT_PORT_SCHED_STRICT_PRI7:
				data = 1;
				break;
			case GT_PORT_SCHED_STRICT_PRI7_6:
				data = 2;
				break;
			case GT_PORT_SCHED_STRICT_PRI7_6_5:
				data = 3;
				break;
			case GT_PORT_SCHED_STRICT_PRI7_6_5_4:
				data = 4;
				break;
			case GT_PORT_SCHED_STRICT_PRI7_6_5_4_3:
				data = 5;
				break;
			case GT_PORT_SCHED_STRICT_PRI7_6_5_4_3_2:
				data = 6;
				break;
            case GT_PORT_SCHED_STRICT_PRI_1:
                data = 7;
                break;
            default:
                return GT_BAD_PARAM;
        }

        retVal = gportSetQueueCtrl(dev,port,0,(GT_U8)data);
        
        if(retVal != GT_OK)
        {
            DBG_INFO(("Failed.\n"));
        }
        else
        {
            DBG_INFO(("OK.\n"));
        }
        return retVal;

    }

    data = mode;

    /* Set the gprtSetPortSched mode.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION,14,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetPortSched
*
* DESCRIPTION:
*        This routine gets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*            GT_PORT_SCHED_WEIGHTED_RRB - use 8,4,2,1 weighted fair scheduling
*            GT_PORT_SCHED_STRICT_PRI - use a strict priority scheme
*
*		 For Opus and Peridot:
*			 GT_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            GT_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - GT_PORT_SCHED_MODE enum type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPortSched
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_PORT_SCHED_MODE        *mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetPortSched Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!((IS_IN_DEV_GROUP(dev,DEV_PORT_SCHEDULE)) ||
        (IS_IN_DEV_GROUP(dev,DEV_PORT_MIXED_SCHEDULE))))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_MIXED_SCHEDULE_1))
    {
        retVal = hwGetPortRegField(dev,hwPort, QD_REG_RATE_CTRL, 12,2,&data);
        if(retVal != GT_OK)
        {
            DBG_INFO(("Failed.\n"));
            return retVal;
        }

        switch(data)
        {       
			case 0:
                *mode = GT_PORT_SCHED_WEIGHTED_RRB;
                break;
            case 1:
                *mode = GT_PORT_SCHED_STRICT_PRI3;
                break;
            case 2:
                *mode = GT_PORT_SCHED_STRICT_PRI2_3;
                break;
            case 3:
                *mode = GT_PORT_SCHED_STRICT_PRI;
                break;
            default:
                return GT_BAD_PARAM;
        }

        return GT_OK;

    }
	else
    {
		GT_U8 datac;
        retVal = gportGetQueueCtrl(dev,port,0, (GT_U8 *)&datac);
        if(retVal != GT_OK)
        {
            DBG_INFO(("Failed.\n"));
            return retVal;
        }

        switch(datac)
        {
			case 0x0:
                *mode = GT_PORT_SCHED_WEIGHTED_RRB_1;
                break;
            case 0x1:
                *mode = GT_PORT_SCHED_STRICT_PRI7;
                break;
            case 0x2:
                *mode = GT_PORT_SCHED_STRICT_PRI7_6;
                break;
            case 0x3:
                *mode = GT_PORT_SCHED_STRICT_PRI7_6_5;
                break;
			case 0x4:
                *mode = GT_PORT_SCHED_STRICT_PRI7_6_5_4;
                break;
            case 0x5:
                *mode = GT_PORT_SCHED_STRICT_PRI7_6_5_4_3;
                break;
            case 0x6:
                *mode = GT_PORT_SCHED_STRICT_PRI7_6_5_4_3_2;
                break;
			case 0x7:
				*mode = GT_PORT_SCHED_STRICT_PRI_1;
            default:
                return GT_BAD_PARAM;
        }

        return GT_OK;

    }

    /* Get the gprtGetPortSched mode.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION,14,1,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    *mode = (GT_PORT_SCHED_MODE)data;

    return retVal;
}


/*******************************************************************************
* gprtSetProviderTag
*
* DESCRIPTION:
*        This routine sets Provider Tag which indicates the provider tag (Ether 
*        Type) value that needs to be matched to in ingress to determine if a
*        frame is Provider tagged or not.
*
* INPUTS:
*        port - the logical port number
*        tag  - Provider Tag (Ether Type)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetProviderTag
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_U16        tag
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetProviderTag Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PROVIDER_TAG))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Set Provider Tag.            */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PROVIDER_TAG, 0, 16, tag);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetProviderTag
*
* DESCRIPTION:
*        This routine gets Provider Tag which indicates the provider tag (Ether 
*        Type) value that needs to be matched to in ingress to determine if a
*        frame is Provider tagged or not.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        tag  - Provider Tag (Ether Type)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetProviderTag
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *tag
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetProviderTag Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PROVIDER_TAG))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get Provider Tag.            */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PROVIDER_TAG, 0, 16, tag);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtSetPauseLimitOut
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be transmitted
*        from this port. When full duplex Flow Control is enabled on this port,
*        these bits are used to limit the number of Pause refresh frames that can 
*        be generated from this port to keep this port's link partner from sending
*        any data.
*        Setting this value to 0 will allow continuous Pause frame refreshes to 
*        egress this port as long as this port remains congested.
*        Setting this value to 1 will allow 1 Pause frame to egress from this port
*        for each congestion situation.
*        Setting this value to 2 will allow 2 Pause frames to egress from this port
*        for each congestion situation, etc.
*        
* INPUTS:
*        port - the logical port number
*        limit - the max number of Pause refresh frames for each congestion situation
*                ( 0 ~ 0xFF)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if limit > 0xFF
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPauseLimitOut
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_U16        limit
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (limit > 0xFF)
    {
        DBG_INFO(("Bad Parameter\n"));
        return GT_BAD_PARAM;
    }

    /* Set Pause Limit.  */
	if (IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT_1))
	{
		retVal = gportSetFlowCtrl(dev,port,1,(GT_U8)limit);
	}
	else
	{
		retVal = hwSetPortRegField(dev,hwPort, QD_REG_LIMIT_PAUSE_CONTROL, 8, 8, limit);
	}
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetPauseLimitOut
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be transmitted
*        from this port. When full duplex Flow Control is enabled on this port,
*        these bits are used to limit the number of Pause refresh frames that can 
*        be generated from this port to keep this port's link partner from sending
*        any data.
*        Setting this value to 0 will allow continuous Pause frame refreshes to 
*        egress this port as long as this port remains congested.
*        Setting this value to 1 will allow 1 Pause frame to egress from this port
*        for each congestion situation.
*        Setting this value to 2 will allow 2 Pause frames to egress from this port
*        for each congestion situation, etc.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        limit - the max number of Pause refresh frames for each congestion situation
*                ( 0 ~ 0xFF)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPauseLimitOut
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *limit
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get Pause Limit.  */
	if (IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT_1))
	{
		retVal = gportGetFlowCtrl(dev,port,1,(GT_U8*)limit);
	}
	else
	{
		retVal = hwGetPortRegField(dev,hwPort, QD_REG_LIMIT_PAUSE_CONTROL, 8, 8, limit);
	}
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtSetPauseLimitIn
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be received
*        on this port. When a port has flow Control enabled, this value can be 
*        used to limit how long this port can be Paused off to prevent a port stall
*        through jamming.
*        When this value is in the range of 0x01 to 0xFF, and a frame is ready to
*        be transmitted out this port, but it cannot be transmitted due to the port
*        being jammed, this limit mechanism starts. The limit mechanism starts 
*        counting new Pause refresh frames or counts of 16 consecutive collisions.
*        If the counter reaches the value set through this API, the following event
*        will occur:
*            1) Port's ForceFC is enabled,
*            2) Port's FCValue is cleared to a zero, and
*            3) Jam Limit Interrupt is asserted.
*        This effectively disables Flow Control on the port once the Pause timer 
*        expires. If a frame gets transmitted out this port before the counter
*        reaches this limit, then this limit mechanism counter resets back to zero.
*
*        Setting this value to 0 will allow continuous jamming to be received on 
*        this port without the Port's ForceFC and FCValue getting modified.
*
*        The modification of Port's ForceFC and FCValue is the only indication that 
*        the limit was reached on this port.
*        
* INPUTS:
*        port - the logical port number
*        limit - the max number of continuous Pause refresh frames for each trasmition
*                ( 0 ~ 0xFF)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if limit > 0xFF
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPauseLimitIn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_U16        limit
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetPauseLimitIn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT) || 
		IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT_1)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (limit > 0xFF)
    {
        DBG_INFO(("Bad Parameter\n"));
        return GT_BAD_PARAM;
    }

    /* Set Pause Limit.  */
	if (IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT_1))
	{
		retVal = gportSetFlowCtrl(dev,port,0,(GT_U8)limit);
	}
	else
	{
		retVal = hwSetPortRegField(dev,hwPort, QD_REG_LIMIT_PAUSE_CONTROL, 0, 8, limit);
	}
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetPauseLimitIn
*
* DESCRIPTION:
*        Limit the number of continuous Pause refresh frames that can be received
*        on this port. When a port has flow Control enabled, this value can be 
*        used to limit how long this port can be Paused off to prevent a port stall
*        through jamming.
*        When this value is in the range of 0x01 to 0xFF, and a frame is ready to
*        be transmitted out this port, but it cannot be transmitted due to the port
*        being jammed, this limit mechanism starts. The limit mechanism starts 
*        counting new Pause refresh frames or counts of 16 consecutive collisions.
*        If the counter reaches the value set through this API, the following event
*        will occur:
*            1) Port's ForceFC is enabled,
*            2) Port's FCValue is cleared to a zero, and
*            3) Jam Limit Interrupt is asserted.
*        This effectively disables Flow Control on the port once the Pause timer 
*        expires. If a frame gets transmitted out this port before the counter
*        reaches this limit, then this limit mechanism counter resets back to zero.
*
*        Setting this value to 0 will allow continuous jamming to be received on 
*        this port without the Port's ForceFC and FCValue getting modified.
*
*        The modification of Port's ForceFC and FCValue is the only indication that 
*        the limit was reached on this port.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        limit - the max number of continuous Pause refresh frames for each trasmition
*                ( 0 ~ 0xFF)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPauseLimitIn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *limit
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetPauseLimitIn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!(IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT) || 
		IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT_1)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get Pause Limit.  */
	if (IS_IN_DEV_GROUP(dev,DEV_PAUSE_LIMIT_1))
	{
		retVal = gportGetFlowCtrl(dev,port,0,(GT_U8*)limit);
	}
	else
	{
		retVal = hwGetPortRegField(dev,hwPort, QD_REG_LIMIT_PAUSE_CONTROL, 0, 8, limit);
	}
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtSetQtoPause
*
* DESCRIPTION:
*        When IEEE 802.3x flow control is enabled on this port and a properly formed
*		 Pause frame is recieved on this port, this QtoPause value is used to
*		 determine which output queues on this port are Paused off or not.
*        
* INPUTS:
*        port - the logical port number
*        queues - output queues that are paused off
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetQtoPause
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U8		 queues
)
{
	GT_STATUS       retVal;         /* Functions return value.      */

	DBG_INFO(("gprtSetQtoPause Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	retVal = gportSetFlowCtrl(dev,port,0x17,queues);
	return retVal;
}

/*******************************************************************************
* gprtGetQtoPause
*
* DESCRIPTION:
*        When IEEE 802.3x flow control is enabled on this port and a properly formed
*		 Pause frame is recieved on this port, this QtoPause value is used to
*		 determine which output queues on this port are Paused off or not.
*        
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        queues - output queues that are paused off
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetQtoPause
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_U8		 *queues
)
{
	GT_STATUS       retVal;         /* Functions return value.      */

	DBG_INFO(("gprtGetQtoPause Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	retVal = gportGetFlowCtrl(dev,port,0x17,queues);
	return retVal;
}


/*******************************************************************************
* gprtSetRxPFCEn
*
* DESCRIPTION:
*        This rountine sets receive priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetRxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_BOOL			en
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		data;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtSetRxPFCEn Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	BOOL_2_BIT(en,data);

	switch(pri)
	{
		case 0:
			offset = 3;
			point = 0x18;
			break;
		case 1:
			offset = 7;
			point = 0x18;
			break;
		case 2:
			offset = 3;
			point = 0x19;
			break;
		case 3:
			offset = 7;
			point = 0x19;
			break;
		case 4:
			offset = 3;
			point = 0x1A;
			break;
		case 5:
			offset = 7;
			point = 0x1A;
			break;
		case 6:
			offset = 3;
			point = 0x1B;
			break;
		case 7:
			offset = 7;
			point = 0x1B;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);
	if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
		return retVal;
    }

	tmpData &= ~(1 << offset);
	tmpData |= data << offset;

	retVal = gportSetFlowCtrl(dev,port,point,tmpData);
	return retVal;
}

/*******************************************************************************
* gprtGetRxPFCEn
*
* DESCRIPTION:
*        This rountine gets receive priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetRxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_BOOL			*en
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		data;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtGetRxPFCEn Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	switch(pri)
	{
		case 0:
			offset = 3;
			point = 0x18;
			break;
		case 1:
			offset = 7;
			point = 0x18;
			break;
		case 2:
			offset = 3;
			point = 0x19;
			break;
		case 3:
			offset = 7;
			point = 0x19;
			break;
		case 4:
			offset = 3;
			point = 0x1A;
			break;
		case 5:
			offset = 7;
			point = 0x1A;
			break;
		case 6:
			offset = 3;
			point = 0x1B;
			break;
		case 7:
			offset = 7;
			point = 0x1B;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);

	data = (tmpData >> offset) & 0x1;
	BIT_2_BOOL(data,*en);
	return retVal;
}

/*******************************************************************************
* gprtSetTxPFCEn
*
* DESCRIPTION:
*        This rountine sets transmit priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetTxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_BOOL			en
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		data;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtSetTxPFCEn Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	BOOL_2_BIT(en,data);

	switch(pri)
	{
		case 0:
			offset = 3;
			point = 0x1C;
			break;
		case 1:
			offset = 7;
			point = 0x1C;
			break;
		case 2:
			offset = 3;
			point = 0x1D;
			break;
		case 3:
			offset = 7;
			point = 0x1D;
			break;
		case 4:
			offset = 3;
			point = 0x1E;
			break;
		case 5:
			offset = 7;
			point = 0x1E;
			break;
		case 6:
			offset = 3;
			point = 0x1F;
			break;
		case 7:
			offset = 7;
			point = 0x1F;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);
	if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
		return retVal;
    }

	tmpData &= ~(1 << offset);
	tmpData |= data << offset;

	retVal = gportSetFlowCtrl(dev,port,point,tmpData);
	return retVal;
}

/*******************************************************************************
* gprtGetTxPFCEn
*
* DESCRIPTION:
*        This rountine gets transmit priority flow control enable.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 en - GT_TRUE for enable, GT_FALSE for disable
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetTxPFCEn
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_BOOL			*en
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		data;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtGetTxPFCEn Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	switch(pri)
	{
		case 0:
			offset = 3;
			point = 0x1C;
			break;
		case 1:
			offset = 7;
			point = 0x1C;
			break;
		case 2:
			offset = 3;
			point = 0x1D;
			break;
		case 3:
			offset = 7;
			point = 0x1D;
			break;
		case 4:
			offset = 3;
			point = 0x1E;
			break;
		case 5:
			offset = 7;
			point = 0x1E;
			break;
		case 6:
			offset = 3;
			point = 0x1F;
			break;
		case 7:
			offset = 7;
			point = 0x1F;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);

	data = (tmpData >> offset) & 0x1;
	BIT_2_BOOL(data,*en);
	return retVal;
}

/*******************************************************************************
* gprtSetRxPFCtoQ
*
* DESCRIPTION:
*        This rountine sets receive priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 queue - priority queues 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetRxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_U8			queue
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtSetRxPFCtoQ Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	switch(pri)
	{
		case 0:
			offset = 0;
			point = 0x18;
			break;
		case 1:
			offset = 4;
			point = 0x18;
			break;
		case 2:
			offset = 0;
			point = 0x19;
			break;
		case 3:
			offset = 4;
			point = 0x19;
			break;
		case 4:
			offset = 0;
			point = 0x1A;
			break;
		case 5:
			offset = 4;
			point = 0x1A;
			break;
		case 6:
			offset = 0;
			point = 0x1B;
			break;
		case 7:
			offset = 4;
			point = 0x1B;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);
	if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
		return retVal;
    }

	tmpData &= ~(0x7 << offset);
	tmpData |= queue << offset;

	retVal = gportSetFlowCtrl(dev,port,point,tmpData);
	return retVal;
}

/*******************************************************************************
* gprtGetRxPFCtoQ
*
* DESCRIPTION:
*        This rountine gets receive priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 queue - priority queues 
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtGetRxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_U8			*queue
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtGetRxPFCtoQ Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	switch(pri)
	{
		case 0:
			offset = 0;
			point = 0x18;
			break;
		case 1:
			offset = 4;
			point = 0x18;
			break;
		case 2:
			offset = 0;
			point = 0x19;
			break;
		case 3:
			offset = 4;
			point = 0x19;
			break;
		case 4:
			offset = 0;
			point = 0x1A;
			break;
		case 5:
			offset = 4;
			point = 0x1A;
			break;
		case 6:
			offset = 0;
			point = 0x1B;
			break;
		case 7:
			offset = 4;
			point = 0x1B;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);
	*queue = (tmpData >> offset) & 0x7;

	return retVal;
}

/*******************************************************************************
* gprtSetTxPFCtoQ
*
* DESCRIPTION:
*        This rountine sets transmit priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*		 queue - priority queues 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtSetTxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	IN  GT_U8			queue
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtSetTxPFCtoQ Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	switch(pri)
	{
		case 0:
			offset = 0;
			point = 0x1C;
			break;
		case 1:
			offset = 4;
			point = 0x1C;
			break;
		case 2:
			offset = 0;
			point = 0x1D;
			break;
		case 3:
			offset = 4;
			point = 0x1D;
			break;
		case 4:
			offset = 0;
			point = 0x1E;
			break;
		case 5:
			offset = 4;
			point = 0x1E;
			break;
		case 6:
			offset = 0;
			point = 0x1F;
			break;
		case 7:
			offset = 4;
			point = 0x1F;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);
	if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
		return retVal;
    }

	tmpData &= ~(0x7 << offset);
	tmpData |= queue << offset;

	retVal = gportSetFlowCtrl(dev,port,point,tmpData);
	return retVal;
}

/*******************************************************************************
* gprtGetTxPFCtoQ
*
* DESCRIPTION:
*        This rountine gets transmit priority flow control frame's Priority X(0~7)
*		 to Egress Queue mapping.
*        
* INPUTS:
*        port - the logical port number
*        pri - which priority that needs to be set
*
* OUTPUTS:
*		 queue - priority queues 
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/

GT_STATUS gprtGetTxPFCtoQ
(
	IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN  GT_U8			pri,
	OUT GT_U8			*queue
)
{
	GT_STATUS	retVal;
	GT_U8		point;
	GT_U8		tmpData;
	GT_U8		offset;
	
	DBG_INFO(("gprtGetTxPFCtoQ Called.\n"));
	
	if (!IS_IN_DEV_GROUP(dev,DEV_FLOW_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	switch(pri)
	{
		case 0:
			offset = 0;
			point = 0x1C;
			break;
		case 1:
			offset = 4;
			point = 0x1C;
			break;
		case 2:
			offset = 0;
			point = 0x1D;
			break;
		case 3:
			offset = 4;
			point = 0x1D;
			break;
		case 4:
			offset = 0;
			point = 0x1E;
			break;
		case 5:
			offset = 4;
			point = 0x1E;
			break;
		case 6:
			offset = 0;
			point = 0x1F;
			break;
		case 7:
			offset = 4;
			point = 0x1F;
			break;
		default:
			DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
	}

	retVal = gportGetFlowCtrl(dev,port,point,&tmpData);
	*queue = (tmpData >> offset) & 0x7;

	return retVal;
}

/*******************************************************************************
* gprtSetFrameMode
*
* DESCRIPTION:
*        Frame Mode is used to define the expected Ingress and the generated Egress
*        tagging frame format for this port as follows:
*            GT_FRAME_MODE_NORMAL -
*                Normal Network mode uses industry standard IEEE 802.3ac Tagged or 
*                Untagged frames. Tagged frames use an Ether Type of 0x8100.
*            GT_FRAME_MODE_DSA -
*                DSA mode uses a Marvell defined tagged frame format for 
*                Chip-to-Chip and Chip-to-CPU connections.
*            GT_FRAME_MODE_PROVIDER -
*                Provider mode uses user definable Ether Types per port 
*                (see gprtSetPortEType/gprtGetPortEType API).
*            GT_FRAME_MODE_ETHER_TYPE_DSA -
*                Ether Type DSA mode uses standard Marvell DSA Tagged frame info 
*                flowing a user definable Ether Type. This mode allows the mixture
*                of Normal Network frames with DSA Tagged frames and is useful to 
*                be used on ports that connect to a CPU.
*        
* INPUTS:
*        port - the logical port number
*        mode - GT_FRAME_MODE type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_BAD_PARAM - if mode is unknown
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetFrameMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_FRAME_MODE    mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_FRAME_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    switch (mode)
    {
        case GT_FRAME_MODE_NORMAL:
        case GT_FRAME_MODE_DSA:
        case GT_FRAME_MODE_PROVIDER:
        case GT_FRAME_MODE_ETHER_TYPE_DSA:
            break;
        default:
            DBG_INFO(("Bad Parameter\n"));
            return GT_BAD_PARAM;
    }

    /* Set Frame Mode.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 8, 2, (GT_U16)mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtGetFrameMode
*
* DESCRIPTION:
*        Frame Mode is used to define the expected Ingress and the generated Egress
*        tagging frame format for this port as follows:
*            GT_FRAME_MODE_NORMAL -
*                Normal Network mode uses industry standard IEEE 802.3ac Tagged or 
*                Untagged frames. Tagged frames use an Ether Type of 0x8100.
*            GT_FRAME_MODE_DSA -
*                DSA mode uses a Marvell defined tagged frame format for 
*                Chip-to-Chip and Chip-to-CPU connections.
*            GT_FRAME_MODE_PROVIDER -
*                Provider mode uses user definable Ether Types per port 
*                (see gprtSetPortEType/gprtGetPortEType API).
*            GT_FRAME_MODE_ETHER_TYPE_DSA -
*                Ether Type DSA mode uses standard Marvell DSA Tagged frame info 
*                flowing a user definable Ether Type. This mode allows the mixture
*                of Normal Network frames with DSA Tagged frames and is useful to 
*                be used on ports that connect to a CPU.
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - GT_FRAME_MODE type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetFrameMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_FRAME_MODE    *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_FRAME_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get Pause Limit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_CONTROL, 8, 2, &data);
    *mode = data;
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtSetHoldAt1
*
* DESCRIPTION:
*        Hold Aging ATU Entries at an Entry State value of 1. When this feature
*        is set to GT_TRUE, ATU entries associated with this port will age down 
*        to an Entry State of 0x1, but will not go to 0x0 (0x0 would purge the
*        entry)
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to hold aging ATU entry with Entry State of 1, 
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetHoldAt1
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_AGE_HOLD))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set HoldAt1 */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 15, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetHoldAt1
*
* DESCRIPTION:
*        Hold Aging ATU Entries at an Entry State value of 1. When this feature
*        is set to GT_TRUE, ATU entries associated with this port will age down 
*        to an Entry State of 0x1, but will not go to 0x0 (0x0 would purge the
*        entry)
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to hold aging ATU entry with Entry State of 1, 
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetHoldAt1
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_AGE_HOLD))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get HoldAt1 */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 15, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetIntOnAgeOut
*
* DESCRIPTION:
*        Interrupt on Age Out. When aging is enabled, all non-static address 
*        entries in the ATU's address database are periodically aged.
*        When this feature is set to GT_TRUE and an entry associated with this 
*        port is aged out, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable AgeOutViloation interrupt
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetIntOnAgeOut
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetIntOnAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_AGE_OUT_INT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set Age Out Interrupt Enable Mode. */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 14, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetIntOnAgeOut
*
* DESCRIPTION:
*        Interrupt on Age Out. When aging is enabled, all non-static address 
*        entries in the ATU's address database are periodically aged.
*        When this feature is set to GT_TRUE and an entry associated with this 
*        port is aged out, an AgeOutViolation will be captured for that entry.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to enable AgeOutViloation interrupt
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetIntOnAgeOut
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetIntOnAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_AGE_OUT_INT))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get IntOnAgeOut */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 14, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetRefreshLocked
*
* DESCRIPTION:
*        Auto Refresh known addresses when port is Locked. Already known addresses 
*        will be auto refreshed when this feature is enabled. When this feature
*        is disabled, auto refreshing will not occur on Locked ports.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable Auto Refresh known addresses on locked port
*               GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetRefreshLocked
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL        mode
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_AUTO_REFRESH_LOCKED))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* Set RefreshLocked */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 11, 1, data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetRefreshLocked
*
* DESCRIPTION:
*        Auto Refresh known addresses when port is Locked. Already known addresses 
*        will be auto refreshed when this feature is enabled. When this feature
*        is disabled, auto refreshing will not occur on Locked ports.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE to enable Auto Refresh known addresses on locked port
*               GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetRefreshLocked
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL      *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if device allows to force a flowcontrol disabled */
    if (!IS_IN_DEV_GROUP(dev,DEV_AUTO_REFRESH_LOCKED))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get RefreshLocked */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_ASSOCIATION, 11, 1, &data);

    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}


/*******************************************************************************
* gprtSetPortEType
*
* DESCRIPTION:
*        This routine sets the port's special Ether Type. This Ether Type is used
*        for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode API).
*
* INPUTS:
*        port  - the logical port number
*        etype - port's special ether type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtSetPortEType
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port,
    IN GT_ETYPE        etype
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetPortEType Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_ETYPE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    data = (GT_U16)etype;

    /* Set the EtherType.            */
    retVal = hwWritePortReg(dev,hwPort, QD_REG_PORT_ETH_TYPE,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetPortEType
*
* DESCRIPTION:
*        This routine retrieves the port's special Ether Type. This Ether Type is used
*        for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode API).
*
* INPUTS:
*        port  - the logical port number
*
* OUTPUTS:
*        etype - port's special ether type
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
*******************************************************************************/
GT_STATUS gprtGetPortEType
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_ETYPE    *etype
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetPortEType Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_ETYPE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the EtherType.            */
    retVal = hwReadPortReg(dev,hwPort, QD_REG_PORT_ETH_TYPE,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    *etype = (GT_ETYPE) data;

    return retVal;
}


/*******************************************************************************
* gsysSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*        from or to a given port.
*
* INPUTS:
*        port - the logical port number
*       mode - GT_JUMBO_MODE (1522, 2048, or 10240)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysSetJumboMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    IN  GT_JUMBO_MODE   mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gsysSetJumboMode Called.\n"));

    if (mode > GT_JUMBO_MODE_10240)
    {
        DBG_INFO(("Bad Parameter\n"));
        return GT_BAD_PARAM;
    }

    if (!IS_IN_DEV_GROUP(dev,DEV_JUMBO_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Set the Jumbo Fram Size bit.               */
    retVal = hwSetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,12,2,(GT_U16)mode);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gsysGetJumboMode
*
* DESCRIPTION:
*       This routine gets the max frame size allowed to be received and transmitted
*        from or to a given port.
*
* INPUTS:
*        port  - the logical port number.
*
* OUTPUTS:
*       mode - GT_JUMBO_MODE (1522, 2048, or 10240)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysGetJumboMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_JUMBO_MODE   *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gsysGetJumboMode Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_JUMBO_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get Jumbo Frame Mode.            */
    retVal = hwGetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,12,2,&data );

    *mode = (GT_JUMBO_MODE)data;

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}
 

/*******************************************************************************
* gprtGet200Base
*
* DESCRIPTION:
*        200 Base mode. This bit can be used to change the port's Px_GTXCLK
*        frequency to 50MHz to support 200 BASE mode as follows:
*        0 = 25MHz Px_GTXCLK
*        1 = 50MHz Px_GTXCLK
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - 0 for 100Mbps, 1 for 200Mbps
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        C_Mode should be set to 0x2 in order for this API to work
*
*******************************************************************************/
GT_STATUS gprtGet200Base
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_U32      *mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16        data;

    DBG_INFO(("gprtGet200Base Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_200BASE_CFG))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the high error rate bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PORT_STATUS,6,1, &data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    *mode = data;

    /* return */
    return retVal;
}

/*******************************************************************************
* gprtSet200Base
*
* DESCRIPTION:
*        200 Base mode. This bit can be used to change the port's Px_GTXCLK
*        frequency to 50MHz to support 200 BASE mode as follows:
*        0 = 25MHz Px_GTXCLK
*        1 = 50MHz Px_GTXCLK
*
* INPUTS:
*        port - the logical port number.
*        mode - 0 for 100Mbps, 1 for 200Mbps
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        C_Mode should be set to 0x2 in order for this API to work
*
*******************************************************************************/
GT_STATUS gprtSet200Base
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32      mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSet200Base Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_200BASE_CFG))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

    /* Get the high error rate bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PORT_STATUS,6,1,(GT_U16)mode&0x1);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    /* return */
    return retVal;
}


/*******************************************************************************
* gprtGetSwitchReg
*
* DESCRIPTION:
*       This routine reads Switch Port Registers.
*
* INPUTS:
*       port    - logical port number
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSwitchReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
)
{
    GT_U16          u16Data;           /* The register's read data.    */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetSwitchReg Called.\n"));

    hwPort = GT_LPORT_2_PORT(port);

    /* Get Phy Register. */
    if(hwReadPortReg(dev,hwPort,(GT_U8)regAddr,&u16Data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    *data = u16Data;

    return GT_OK;
}

/*******************************************************************************
* gprtSetSwitchReg
*
* DESCRIPTION:
*       This routine writes Switch Port Registers.
*
* INPUTS:
*       port    - logical port number
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSwitchReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
)
{
    GT_U8           hwPort;         /* the physical port number     */
    
    DBG_INFO(("gprtSetSwitchReg Called.\n"));

    hwPort = GT_LPORT_2_PORT(port);

    if(hwWritePortReg(dev,hwPort,(GT_U8)regAddr,data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    return GT_OK;
}


/*******************************************************************************
* gprtGetGlobalReg
*
* DESCRIPTION:
*       This routine reads Switch Global Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetGlobalReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
)
{
    GT_U16          u16Data;           /* The register's read data.    */

    DBG_INFO(("gprtGetGlobalReg Called.\n"));

    /* Get Phy Register. */
    if(hwReadGlobalReg(dev,(GT_U8)regAddr,&u16Data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    *data = u16Data;

    return GT_OK;
}

/*******************************************************************************
* gprtSetGlobalReg
*
* DESCRIPTION:
*       This routine writes Switch Global Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetGlobalReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
)
{
    DBG_INFO(("gprtSetGlobalReg Called.\n"));

    if(hwWriteGlobalReg(dev,(GT_U8)regAddr,data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* gprtGetGlobal2Reg
*
* DESCRIPTION:
*       This routine reads Switch Global 2 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetGlobal2Reg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
)
{
    GT_U16          u16Data;           /* The register's read data.    */

    DBG_INFO(("gprtGetGlobal2Reg Called.\n"));

    /* Get Phy Register. */
    if(hwReadGlobal2Reg(dev,(GT_U8)regAddr,&u16Data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    *data = u16Data;

    return GT_OK;
}

/*******************************************************************************
* gprtSetGlobal2Reg
*
* DESCRIPTION:
*       This routine writes Switch Global2 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetGlobal2Reg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
)
{
    DBG_INFO(("gprtSetGlobal2Reg Called.\n"));

    if(hwWriteGlobal2Reg(dev,(GT_U8)regAddr,data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    return GT_OK;
}
/*******************************************************************************
* gprtGetGlobal3Reg
*
* DESCRIPTION:
*       This routine reads Switch Global 3 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetGlobal3Reg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
)
{
    GT_U16          u16Data;           /* The register's read data.    */

    DBG_INFO(("gprtGetGlobal3Reg Called.\n"));

    /* Get Phy Register. */
    if(hwReadGlobal3Reg(dev,(GT_U8)regAddr,&u16Data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    *data = u16Data;

    return GT_OK;
}

/*******************************************************************************
* gprtSetGlobal3Reg
*
* DESCRIPTION:
*       This routine writes Switch Global3 Registers.
*
* INPUTS:
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetGlobal3Reg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
)
{
    DBG_INFO(("gprtSetGlobal3Reg Called.\n"));

    if(hwWriteGlobal3Reg(dev,(GT_U8)regAddr,data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }

    return GT_OK;
}

/* New functions in 88E6320 or later */
/*******************************************************************************
* gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set port queue control data to the Port Queue Control register. 
*        The registers of Port Queue control are.
*         Hard Queue Limits register space
*         Reserved for future Hard Queue Limits use
*
* INPUTS:
*        port  - logical port number
*        point - Pointer to the Port Queue Control register.
*        data  - Port Queue Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetQueueCtrl
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_U32           point,
    IN  GT_U8            data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    IN  GT_U16            tmpData;
    GT_U8           hwPort;         /* the physical port number     */

    if (!IS_IN_DEV_GROUP(dev,DEV_QUEUE_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

    if (point > 0x80)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    if (data &0xffffff00)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);


    /* Wait until the Port Queue Control is ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, PORT_ACCESS);
      regAccess.rw_reg_list[0].reg = 0x1d;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->vtuRegsSem);
        return retVal;
      }
    }
#else
       tmpData = 1;
    while(tmpData == 1)
       {
        retVal = hwGetPortRegField(dev,hwPort, 0x1d,15,1,&tmpData);
           if(retVal != GT_OK)
           {
            gtSemGive(dev,dev->tblRegsSem);
               return retVal;
           }
    }
#endif

    tmpData =  (GT_U16)((1 << 15) | (point << 8) | data);

    retVal = hwWritePortReg(dev,hwPort, 0x1d, tmpData);
    if(retVal != GT_OK)
       {
           DBG_INFO(("Failed.\n"));
           gtSemGive(dev,dev->tblRegsSem);
           return retVal;
    }

    gtSemGive(dev,dev->tblRegsSem);

       return retVal;

}


/*******************************************************************************
* gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get port queue control data from the Port Queue Control register. 
*        The registers of Port Queue control are.
*         Hard Queue Limits register space
*         Reserved for future Hard Queue Limits use
*
* INPUTS:
*        port  - logical port number
*        point - Pointer to the Port Queue Control register.
*
* OUTPUTS:
*        data  - Port Queue Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetQueueCtrl
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_U32           point,
    OUT GT_U8            *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    OUT  GT_U16            tmpData;
    GT_U8           hwPort;         /* the physical port number     */
    int count=0x10;

    if (!IS_IN_DEV_GROUP(dev,DEV_QUEUE_CONTROL))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

    if (point > 0x80)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Port Queue Control is ready. */

    do {
        retVal = hwGetPortRegField(dev,hwPort, 0x1d,15,1,&tmpData);
        if(retVal != GT_OK)
           {
               DBG_INFO(("Failed.\n"));
            gtSemGive(dev,dev->tblRegsSem);
               return retVal;
        }
		if((count--)==0) return GT_FAIL;
    } while (tmpData&0x8000);

    *data = tmpData&0xff;

    gtSemGive(dev,dev->tblRegsSem);


    return retVal;
}

/*******************************************************************************
* gprtGetDebugCounter
*
* DESCRIPTION:
*        Get Port Debug Counter, bad counter and good counter.
*
* INPUTS:
*        port  - logical port number
*
* OUTPUTS:
*        badCounter  - When CtrMode is cleared to a zero (Global 1 offset 0x1C) this
*  counter increments each time a frame enters this port that was an error on the
*  wire. It does not matter if the frame�s CRC is fixed by ForceGoodFCS (Port
*  offset 0x08) being set to a one, this counter will still increment. A CRC error
*  frame is one that is 64 bytes to MaxFrameSize (Global 1, offset 0x04) with a
*  bad CRC (including alignment errors but not dribbles). Fragments and
*  properly formed frames are not counted. The RxBadFrames counter counts
*  frames that are counted in the MIB counters as InUndersize, InOversize,
*  InJabber, InRxErr and InFCSErr.
*  When CtrMode is set to a one this counter increments each time a transmit
*  collision occurs on this port.
*        goodCounter  - When CtrMode is cleared to a zero (Global 1 offset 0x1C) this
*  counter increments each time a frame enters this port that was not an error
*  frame on the wire. It does not matter if the frame was filtered or discarded,
*  only that the frame was received as good on the wire (i.e., its wire size is in the
*  range of 64 bytes to MaxFrameSize (Global 1, offset 0x04) and its CRC was
*  good. The RxGoodFrames counter counts frames that are not counted
*  above as long as they are not being counted in the MIB counters as
*  InFragments.
*  When CtrMode is set to a one this counter increments each time a frame is
*  transmitted out this port.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetDebugCounter
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    OUT  GT_U8            *badCounter,
    OUT  GT_U8            *goodCounter
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    OUT  GT_U16            tmpData;
    GT_U8           hwPort;         /* the physical port number     */

    if (!IS_IN_DEV_GROUP(dev,DEV_DEBUG_COUNTER))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

	if (IS_IN_DEV_GROUP(dev,DEV_88E6321_FAMILY))
		retVal = hwReadPortReg(dev,hwPort, 0x1e, &tmpData);
	else
		retVal = hwReadPortReg(dev,hwPort, 0x1f, &tmpData);
    if(retVal != GT_OK)
    {
      DBG_INFO(("Failed.\n"));
      return retVal;
    }

    *goodCounter = tmpData&0xff;
    *badCounter = (tmpData>>8)&0xff;

    return retVal;
}


/*******************************************************************************
* gprtSetCutThrough
*
* DESCRIPTION:
*        Set port Cut Through configuration.
*
* INPUTS:
*        port  - logical port number
*        cutThru - Cut through configuration.
*                    enableSelect;     Port Enable Select. 
*                    enable;           Cut Through enable. 
*                    cutThruQueue;     Cut Through Queues. 
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetCutThrough
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    IN  GT_CUT_THROUGH   *cutThru
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    OUT  GT_U16            tmpData;
    GT_U8           hwPort;         /* the physical port number     */

    if (!IS_IN_DEV_GROUP(dev,DEV_CUT_THROUGH))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);
	
	/*
    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }
	*/
	
    tmpData = (cutThru->enableSelect<<12)|(cutThru->enable<<8)|cutThru->cutThruQueue;
	if (IS_IN_DEV_GROUP(dev,DEV_88E6321_FAMILY))
		retVal = hwWritePortReg(dev,hwPort, 0x1f, tmpData);
	else
		retVal = hwWritePortReg(dev,hwPort, 0x1e, tmpData);
    if(retVal != GT_OK)
    {
      DBG_INFO(("Failed.\n"));
      return retVal;
    }

    return retVal;
}

/*******************************************************************************
* gprtGetCutThrough
*
* DESCRIPTION:
*        Get port Cut Through configuration.
*
* INPUTS:
*        port  - logical port number
*
* OUTPUTS:
*        cutThru - Cut through configuration.
*                    enableSelect;     Port Enable Select. 
*                    enable;           Cut Through enable. 
*                    cutThruQueue;     Cut Through Queues. 
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetCutThrough
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT         port,
    OUT GT_CUT_THROUGH   *cutThru
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    OUT  GT_U16            tmpData;
    GT_U8           hwPort;         /* the physical port number     */

    if (!IS_IN_DEV_GROUP(dev,DEV_CUT_THROUGH))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

	/*
    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }
	*/

	if (IS_IN_DEV_GROUP(dev,DEV_88E6321_FAMILY))
		retVal = hwReadPortReg(dev,hwPort, 0x1f, &tmpData);
	else
		retVal = hwReadPortReg(dev,hwPort, 0x1e, &tmpData);
    if(retVal != GT_OK)
    {
      DBG_INFO(("Failed.\n"));
      return retVal;
    }

    cutThru->enableSelect = (tmpData>>12)&0xf;
    cutThru->enable = (tmpData&0x100)?1:0;
    cutThru->cutThruQueue =tmpData&0xff;

    return retVal;
}

/*******************************************************************************
* gsysSetDefQpri
*
* DESCRIPTION:
*       This routine Set Default queue priority.
*
* INPUTS:
*       port - the logical port number
*       defque - default queue priority structure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysSetDefQpri
(
  IN  GT_QD_DEV    *dev,
  IN  GT_LPORT    port,
  IN  GT_DEF_QUE_PRI  *defque
)
{
  GT_STATUS       retVal;         /* Functions return value.      */
  GT_U8           hwPort;         /* the physical port number     */
  GT_U16		  data;

  DBG_INFO(("gsysSetDefQpri Called.\n"));

  if (!IS_IN_DEV_GROUP(dev,DEV_DEF_QPRI))
  {
    DBG_INFO(("GT_NOT_SUPPORTED\n"));
    return GT_NOT_SUPPORTED;
  }

  /* translate LPORT to hardware port */
  hwPort = GT_LPORT_2_PORT(port);

  if (!IS_IN_DEV_GROUP(dev,DEV_DEF_QPRI_1))
  {
	data = ((defque->use_def_que&1)<<2) | (defque->def_que&0x3);

    /* Set the default queue priority.               */
    retVal = hwSetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,1,3, data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }
  }
  else
  {
	data = (defque->def_que&0x7);

    /* Set the default queue priority.               */
    retVal = hwSetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,0,3, data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }
  }


  DBG_INFO(("OK.\n"));
  return GT_OK;
}

/*******************************************************************************
* gsysGetDefQpri
*
* DESCRIPTION:
*       This routine Get Default queue priority.
*
* INPUTS:
*       port - the logical port number
*
* OUTPUTS:
*       defque - default queue priority structure
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gsysGetDefQpri
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_DEF_QUE_PRI  *defque
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16 data;

    DBG_INFO(("gsysGetDefQpri Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_DEF_QPRI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Set the default queue priority.               */
  if (!IS_IN_DEV_GROUP(dev,DEV_DEF_QPRI_1))
  {
    retVal = hwGetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,1,3, &data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }
	defque->def_que = data&0x3;
	data >>= 2;
	defque->use_def_que = data&1;
  }
  else
  {
    retVal = hwGetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,0,3, &data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }
	defque->def_que = data&0x7;
  }

  DBG_INFO(("OK.\n"));
  return GT_OK;
}

/*******************************************************************************
* gprtSetAllowVid0
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*        allowed - GT_TRUE, Allow frames received with a VID of 0x000 
*                  will NOT be updated with the port�s Default VID.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetAllowVid0
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_BOOL         allowed
)
{
    GT_U16          data;           
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetAllowVid0 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Opus and Peridot supports this status. */
    if (!(IS_IN_DEV_GROUP(dev, DEV_ALLOWED_VID_2_0))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(allowed, data);

    /* Set Allowed Vid is 0..            */
    retVal = hwSetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,3,1,data );

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetAllowVid0
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        allowed - GT_TRUE, Allow frames received with a VID of 0x000 
*                  will NOT be updated with the port�s Default VID.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetAllowVid0
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_BOOL        *allowed
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16          data;           /* to keep the read valve       */

    DBG_INFO(("gprtGetAllowVid0 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Opus and Peridot supports this status. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_ALLOWED_VID_2_0))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get Allowed bad FCS..            */
    retVal = hwGetPortRegField(dev,hwPort,QD_REG_PORT_CONTROL2,3,1,&data );
    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *allowed);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}



/*******************************************************************************
* gportPointDataSet
*
* DESCRIPTION:
*        Common set indirect register pointer control data from the port 
*        function registers. The function register are.
*          Flow Control
*
* INPUTS:
*       port  - the logical port number.
*        funcReg - port function register.
*        point - Pointer to following Control register.
*          Flow Control
*        data  - Scratch and Misc. Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportPointDataSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT   port,
    IN  GT_U8             funcReg,
    IN  GT_U8            point,
    IN  GT_U8            data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U8        hwPort;         /* the physical port number     */
    IN  GT_U16   count, tmpData;
    DBG_INFO(("gportPointDataSet Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (data &0xffffff00)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

    /* program QoS Weight Table, 4 sequences at a time */

    /* Wait until the Scratch and Misc control is ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, PORT_ACCESS);
      regAccess.rw_reg_list[0].reg = funcReg;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->vtuRegsSem);
        return retVal;
      }
    }
#else
    count = 5;
    tmpData = 1;
    while(tmpData ==1)
    {
        retVal = hwGetPortRegField(dev,hwPort, funcReg,15,1,&tmpData);
           if(retVal != GT_OK)
           {
            gtSemGive(dev,dev->tblRegsSem);
               return retVal;
           }
	  if(--count==0) break;
    }
#endif

  tmpData =  (GT_U16)((1 << 15) | (point << 8) | data);

  retVal = hwWritePortReg(dev,hwPort, funcReg, tmpData);
  if(retVal != GT_OK)
  {
     DBG_INFO(("Failed.\n"));
     gtSemGive(dev,dev->tblRegsSem);
     return retVal;
  }

  gtSemGive(dev,dev->tblRegsSem);

  return retVal;

}






/*******************************************************************************
* gportPointDataGet
*
* DESCRIPTION:
*        Common get indirect register pointer control data from the port 
*        function registers. The function register are.
*          Flow Control
*
* INPUTS:
*       port  - the logical port number.
*        funcReg - port function register.
*        point - Pointer to following Control register.
*          Flow Control
*
* OUTPUTS:
*        data -  Control data read from the indirect register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportPointDataGet
(
    IN   GT_QD_DEV         *dev,
    IN   GT_LPORT   port,
    IN   GT_U8            funcReg,
    IN   GT_U8            point,
    OUT  GT_U8            *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
    GT_U16            tmpData;
    int count=0x10;
    DBG_INFO(("gportPointDataGet Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

    /* program QoS Weight Table, 4 sequences at a time */

	tmpData =  (GT_U16)((point << 8) | 0);
    retVal = hwWritePortReg(dev,hwPort, funcReg, tmpData);
	if(retVal != GT_OK)
	{
	   DBG_INFO(("Failed.\n"));
	   gtSemGive(dev,dev->tblRegsSem);
	   return retVal;
	}
	
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 2;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, PORT_ACCESS);
      regAccess.rw_reg_list[0].reg = funcReg;
      regAccess.rw_reg_list[0].data = 15;
      regAccess.rw_reg_list[1].cmd = HW_REG_READ;
      regAccess.rw_reg_list[1].addr = CALC_SMI_DEV_ADDR(dev, 0, PORT_ACCESS);
      regAccess.rw_reg_list[1].reg = funcReg;
      regAccess.rw_reg_list[1].data = 0;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->vtuRegsSem);
        return retVal;
     }
     tmpData = qdLong2Short(regAccess.rw_reg_list[1].data);
    }
#else
    do {
        retVal = hwReadPortReg(dev,hwPort, funcReg, &tmpData);
        if(retVal != GT_OK)
           {
               DBG_INFO(("Failed.\n"));
            gtSemGive(dev,dev->tblRegsSem);
               return retVal;
        }
		if((count--)==0) return GT_FAIL;
    } while (tmpData&0x8000);
#endif

    *data = tmpData&0xff;

    gtSemGive(dev,dev->tblRegsSem);


    return retVal;
}

/*******************************************************************************
* gportSetFlowCtrl
*
* DESCRIPTION:
*        Set Flow control data from the Flow Control register. 
*        The register of Flow control are:
*          Port Limit In 
*          Port Limit Out 
*          Port Flow Control Mode 
*          IEEE 802.3x Queues to Pause 
*          PFC Priority to Tx Queue Mapping 
*          Rx Queue to PFC Priority Mapping 
*         
* INPUTS:
*        point - Pointer to the Flow control register.
*        data  - Flow Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportSetFlowCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    IN  GT_U8            data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    DBG_INFO(("gportSetFlowCtrl Called.\n"));

    if (!(IS_IN_DEV_GROUP(dev, DEV_FLOW_CONTROL))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }


   if (point > GT_FLOW_CTRL_REG_MAX)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

	retVal = gportPointDataSet(dev, port, QD_REG_FLOW_CONTROL, point, data);
	if(retVal != GT_OK)
	{
	   DBG_INFO(("gportPointDataSet Failed.\n"));
	   return retVal;
	}


  return retVal;

}

/*******************************************************************************
* gportGetFlowCtrl
*
* DESCRIPTION:
*        Get Flow control data from the Flow Control register. 
*        The register of Flow control are:
*          Port Limit In 
*          Port Limit Out 
*          Port Flow Control Mode 
*          IEEE 802.3x Queues to Pause 
*          PFC Priority to Tx Queue Mapping 
*          Rx Queue to PFC Priority Mapping 
*         
* INPUTS:
*        point - Pointer to the Flow control register.
*
* OUTPUTS:
*        data  - Flow Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportGetFlowCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    OUT  GT_U8            *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    DBG_INFO(("gportGetFlowCtrl Called.\n"));

    if (!(IS_IN_DEV_GROUP(dev, DEV_FLOW_CONTROL))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (point > GT_FLOW_CTRL_REG_MAX)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

	retVal = gportPointDataGet(dev, port, QD_REG_FLOW_CONTROL, point, data);
	if(retVal != GT_OK)
	{
	   DBG_INFO(("gportPointDataGet Failed.\n"));
	   return retVal;
	}

    return retVal;
}


/*******************************************************************************
* gprtSetPortCtrl3
*
* DESCRIPTION:
*        Set port control features in port control 3 .
*
* INPUTS:
*        port - the logical port number.
*        control - Combine Following control bits.
*         GT_PORT_CONTROL_3_USECFI_YELLOW 
*         GT_PORT_CONTROL_3_USEDEI_YELLOW   
*         GT_PORT_CONTROL_3_UPDATECFI_YELLOW
*         GT_PORT_CONTROL_3_UPDATEDEI_YELLOW 
*         GT_PORT_CONTROL_3_USEIP_YELLOW
*		  GT_PORT_CONTROL_3_IPAVB_EN
*		  GT_PORT_CONTROL_3_DROP_BC
*		  GT_PORT_CONTROL_3_PAV_4_INT_CPU
*		  GT_PORT_CONTROL_3_MAP_2_INT_CPU
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS: 
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetPortCtrl3
(
    IN GT_QD_DEV       *dev,
    IN GT_LPORT        port,
    IN GT_U16         control
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtSetPortCtrl3 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Opus and Peridot support this status. */
    if (!(IS_IN_DEV_GROUP(dev, DEV_PORT_CONTROL_3))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }


    /* Set port control 3 features..            */
	retVal = hwWritePortReg(dev,hwPort,QD_REG_PORT_CONTROL_3,control);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}


/*******************************************************************************
* gprtGetPortCtrl3
*
* DESCRIPTION:
*        This routine gets Ignore FCS setup
*
* DESCRIPTION:
*        Allow VID of Zero.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        control - Combine Following control bits.
*         GT_PORT_CONTROL_3_USECFI_YELLOW 
*         GT_PORT_CONTROL_3_USEDEI_YELLOW   
*         GT_PORT_CONTROL_3_UPDATECFI_YELLOW
*         GT_PORT_CONTROL_3_UPDATEDEI_YELLOW 
*         GT_PORT_CONTROL_3_USEIP_YELLOW
*		  GT_PORT_CONTROL_3_IPAVB_EN
*		  GT_PORT_CONTROL_3_DROP_BC
*		  GT_PORT_CONTROL_3_PAV_4_INT_CPU
*		  GT_PORT_CONTROL_3_MAP_2_INT_CPU
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetPortCtrl3
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT    port,
    OUT GT_U16        *control
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gprtGetPortCtrl3 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Opus and Peridot supports this status. */
    if (!(IS_IN_DEV_GROUP(dev,DEV_PORT_CONTROL_3))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	retVal = hwReadPortReg(dev,hwPort,QD_REG_PORT_CONTROL_3,control);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gportSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
*        The register(pointer) of Queue control are:
*            GT_QUEUE_CFG_PORT_SCHEDULE
*            GT_QUEUE_CFG_FILTER_Q_EN
*            GT_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            GT_QUEUE_CFG_DANAMIC_Q_PRUNNING
*            GT_QUEUE_CFG_DANAMIC_Q_STATUS
*            GT_QUEUE_CFG_DANAMIC_Q_YELLOW_CTRL
*            GT_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            GT_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            GT_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            GT_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            GT_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            GT_QUEUE_CFG_QOS_GAIN_SET
*            GT_QUEUE_CFG_PFC_GAIN_SET
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportSetQueueCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    IN  GT_U8            data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
	GT_U8 func;
    DBG_INFO(("gportSetQueueCtrl Called.\n"));

    if (!(IS_IN_DEV_GROUP(dev, DEV_QUEUE_CONTROL))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    if ((IS_IN_DEV_GROUP(dev, DEV_QUEUE_CONTROL_1))) 
    {
		func = QD_REG_QUEUE_CONTROL_1;
	}
	else
    {
		func = QD_REG_QUEUE_CONTROL;
	}

    if (point > GT_QUEUE_CTRL_REG_MAX)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

	retVal = gportPointDataSet(dev, port, func, point, data);
	if(retVal != GT_OK)
	{
	   DBG_INFO(("gportPointDataSet Failed.\n"));
	   return retVal;
	}


  return retVal;

}

/*******************************************************************************
* gportGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
*        The register(pointer) of Queue control are:
*            GT_QUEUE_CFG_PORT_SCHEDULE
*            GT_QUEUE_CFG_FILTER_Q_EN
*            GT_QUEUE_CFG_PORT_SCRATCH_0_REG,  (0 - 1)
*            GT_QUEUE_CFG_DANAMIC_Q_PRUNNING
*            GT_QUEUE_CFG_DANAMIC_Q_STATUS
*            GT_QUEUE_CFG_DANAMIC_Q_YELLOW_CTRL
*            GT_QUEUE_CFG_H_Q_LIMIT_REG         (0 - 7)
*            GT_QUEUE_CFG_Y_Q_LIMIT_REG         (0 - 7)
*            GT_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (0 - 7, increment=2)
*            GT_QUEUE_CFG_PFC_THRESHOLD_END     (0 - 7, increment=2)
*            GT_QUEUE_CFG_IN_Q_COUNT            (0 - 7)
*            GT_QUEUE_CFG_QOS_GAIN_SET
*            GT_QUEUE_CFG_PFC_GAIN_SET
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*
* OUTPUTS:
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gportGetQueueCtrl
(
    IN  GT_QD_DEV         *dev,
    IN  GT_LPORT          port,
    IN  GT_U8             point,
    OUT  GT_U8            *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
	GT_U8 func;
    DBG_INFO(("gportGetQueueCtrl Called.\n"));

    if (!(IS_IN_DEV_GROUP(dev, DEV_QUEUE_CONTROL))) 
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    if ((IS_IN_DEV_GROUP(dev, DEV_QUEUE_CONTROL_1))) 
    {
		func = QD_REG_QUEUE_CONTROL_1;
	}
	else
    {
		func = QD_REG_QUEUE_CONTROL;
	}

    if (point > GT_QUEUE_CTRL_REG_MAX)
    {
        DBG_INFO(("GT_BAD_PARAM\n"));
        return GT_BAD_PARAM;
    }

	retVal = gportPointDataGet(dev, port, func, point, data);
	if(retVal != GT_OK)
	{
	   DBG_INFO(("gportPointDataGet Failed.\n"));
	   return retVal;
	}

    return retVal;
}








