/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtHsuDrv.h
*
* DESCRIPTION:
*       Includes HSU function for HSU functionality
*
* DEPENDENCIES:
*       -   BSP
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __gtHsuDrvh
#define __gtHsuDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* extDrvHsuMemBaseAddrGet
*
* DESCRIPTION:
*       Get start address of HSU area
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       hsuPtrAddr      - Pointer address to beginning of HSU area
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvHsuMemBaseAddrGet
(
    OUT  GT_U32 **hsuPtrAddr
);

/*******************************************************************************
* extDrvHsuWarmRestart
*
* DESCRIPTION:
*       Performs warm restart of the 8548 cpu
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       No return
*
* COMMENTS:
*       None
*
*******************************************************************************/

GT_VOID extDrvHsuWarmRestart
(
    GT_VOID
);

/*******************************************************************************
* extDrvHsuInboundSdmaEnable
*
* DESCRIPTION:
*       This routine enables cpu inbound sdma access .
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvHsuInboundSdmaEnable
(
    GT_VOID
);

/*******************************************************************************
* extDrvHsuInboundSdmaDisable
*
* DESCRIPTION:
*       This routine disables cpu inbound sdma access .
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvHsuInboundSdmaDisable
(
    GT_VOID
);

/*******************************************************************************
* extDrvHsuInboundSdmaStateGet
*
* DESCRIPTION:
*       This routine gets the state of cpu inbound sdma access.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       enablePtr  - (pointer to) inbound sdma status .
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvHsuInboundSdmaStateGet
(
    OUT GT_BOOL *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtHsuDRv */

