/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsPortCtrlInit.h
*
* DESCRIPTION:
*       Port Control Initialization
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlInit_H
#define __mvHwsPortCtrlInit_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Initialization API */

/*******************************************************************************
* mvPortCtrlHwInit
*
* DESCRIPTION: Initialize HW related tasks
*******************************************************************************/
GT_STATUS mvPortCtrlHwInit(void);

/*******************************************************************************
* mvPortCtrlSwInit
*
* DESCRIPTION: Initialize Application related tasks
*              - Initialize Firmware - AAPL structures
*              - Initialize Database
*              - Initialize Real-time Log
*              - Initialize OS Resources
*******************************************************************************/
GT_STATUS mvPortCtrlSwInit(void);

/*******************************************************************************
* mvPortCtrlOsResourcesInit
*
* DESCRIPTION: Allocate and Initialize OS Resources
*              - Allocate system memory queues
*              - Allocate system semaphores
*              - Allocate system processes
*******************************************************************************/
GT_STATUS mvPortCtrlOsResourcesInit(void);

/*******************************************************************************
* mvPortCtrlSyncLock
* mvPortCtrlSyncUnlock
*
* DESCRIPTION: These two functions are used to synchronize the start of
*              execution of all processes
*              Once system is running is is used to protect SM execution 
*******************************************************************************/
GT_STATUS mvPortCtrlSyncLock(void);
GT_STATUS mvPortCtrlSyncUnlock(void);

/*******************************************************************************
* mvPortCtrlProcessDelay
*
* DESCRIPTION: Delay process execution for [x] msec
*******************************************************************************/
GT_STATUS mvPortCtrlProcessDelay(GT_U32 duration);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlInit_H */





