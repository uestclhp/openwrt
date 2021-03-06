/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDefCheetah.c
*
* DESCRIPTION:
*       This file includes the definition and initialization of the interrupts
*       init. parameters tables. -- Cheetah devices
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah.h>

#include <cpss/generic/bridge/private/prvCpssGenBrgFdbIsrSig.h>

/************************************ Cheetah ******************************/
/*      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 */
#define BITS(x,y)  (FIELD_MASK_MAC(x, (1 + (y) - (x))))

/* NSBM Non - Sum - Bit - Mask (except self)         */
#define GLOBAL_NSBM          (BITS(0,8))
#define MISC_NSBM            (BITS(0,10))
#define GPP_NSBM             (BITS(0,8))
#define RXSDMA_NSBM          (BITS(0,21))
#define TXSDMA_NSBM          (BITS(0,24))
#define PCI_NSBM             (BITS(0,28))
#define TRISPMIB_NSBM        (BITS(0,28))
#define PORT_NSBM            (BITS(0,9))
#define HGS_PORT_NSBM        (BITS(0,7))
#define HGS_MIB_NSBM         (BITS(0,10))
#define POLICY_NSBM          (BITS(0,1))
#define BRIDGE_NSBM          (BITS(0,28))
#define INGRESS_STC_NSBM     (BITS(0,27))
#define EGRESS_STC_NSBM      (BITS(0,27))
#define TXQ_FLASH_NSBM       (BITS(0,28))
#define TXQ_WD_NSBM          (BITS(0,28))
#define TXQ_GEN_NSBM         (BITS(0,10))
#define BUF_MEM_NSBM         (BITS(0,29))
#define BUF_MNG_R0_NSBM      (BITS(0,20))
#define BUF_MNG_R1_NSBM      0xFFFFFFFF
#define MAC_NSBM             (BITS(0,16))
#define GOP_NSBM             (BITS(0,3))
#define HX_PORT_NSBM         (BITS(0,15))

/* default for Giga Port interrupt mask register */
#define GIG_PORT_DEF_MASK  0xE6

/* User defined register 1 - used instead of non-existing addresses in DX106 */
#define STUB_ADDRESS 0xF4

/* This array maps an interrupt index to its    */
/* relevant interrupt mask registers address,   */
/* cell i indicates the address for interrupts  */
/* (32 * i  --> 32 * (i + 1) - 1).              */
static GT_U32 cheetahMaskRegMapArr[] =
{
    0x00000034,     /* GLOBAL          interrupt index    0.. 31 */
    0x0000003C,     /* Miscellaneous   interrupt index   32.. 63 */
    0x018001C0,     /* GPP             interrupt index   64.. 95 */
    0x00002814,     /* RxSDMA          interrupt index   96..127 */
    0x00002818,     /* TxSDMA          interrupt index  128..159 */
    0x00000118,     /* PCI             interrupt index  160..191 */
    0x04805110,     /* Tri-speed port  interrupt index  192..223 */
    0x0A800024,     /* PORT0           interrupt index  224..255 */
    0x0A800124,     /* PORT1           interrupt index  256..287 */
    0x0A800224,     /* PORT2           interrupt index  288..319 */
    0x0A800324,     /* PORT3           interrupt index  320..351 */
    0x0A800424,     /* PORT4           interrupt index  352..383 */
    0x0A800524,     /* PORT5           interrupt index  384..415 */
    0x0A800624,     /* PORT6           interrupt index  416..447 */
    0x0A800724,     /* PORT7           interrupt index  448..479 */
    0x0A800824,     /* PORT8           interrupt index  480..511 */
    0x0A800924,     /* PORT9           interrupt index  512..543 */
    0x0A800A24,     /* PORT10          interrupt index  544..575 */
    0x0A800B24,     /* PORT11          interrupt index  576..607 */
    0x0A800C24,     /* PORT12          interrupt index  608..639 */
    0x0A800D24,     /* PORT13          interrupt index  640..671 */
    0x0A800E24,     /* PORT14          interrupt index  672..703 */
    0x0A800F24,     /* PORT15          interrupt index  704..735 */
    0x0A801024,     /* PORT16          interrupt index  736..767 */
    0x0A801124,     /* PORT17          interrupt index  768..799 */
    0x0A801224,     /* PORT18          interrupt index  800..831 */
    0x0A801324,     /* PORT19          interrupt index  832..863 */
    0x0A801424,     /* PORT20          interrupt index  864..895 */
    0x0A801524,     /* PORT21          interrupt index  896..927 */
    0x0A801624,     /* PORT22          interrupt index  928..959 */
    0x0A801724,     /* PORT23          interrupt index  960..991 */
    0x0A803F24,     /* PORT63 (CPU)    interrupt index  992..1023 */
    0x0A801818,     /* PORT24          interrupt index  1024..1055 */
    0x0A801918,     /* PORT25          interrupt index  1056..1087 */
    0x0A801A18,     /* PORT26          interrupt index  1088..1119 */
    0x0180013C,     /* HG.StackMIB     interrupt index  1120..1151 */
    0x0B800014,     /* PolicyEngine    interrupt index  1152..1183 */
    0x02040134,     /* BRIDGE          interrupt index  1184..1215 */
    0x0B000024,     /* IngressSTC      interrupt index  1216..1247 */
    0x01800104,     /* TXQ             interrupt index  1248..1279 */
    0x01800114,     /* TXQFlash        interrupt index  1280..1311 */
    0x0180010C,     /* TXQWD           interrupt index  1312..1343 */
    0x0180011C,     /* TXQGEN          interrupt index  1344..1375 */
    0x01800134,     /* EgrerssSTC      interrupt index  1376..1407 */
    0x07800028,     /* BuffMem         interrupt index  1408..1439 */
    0x03000044,     /* BuffMngR0       interrupt index  1440..1471 */
    0x0300004C,     /* BuffMngR1       interrupt index  1472..1503 */
    0x0600001C,     /* MACTable        interrupt index  1504..1535*/
    0x04004004,     /* GOP0 MIB        interrupt index  1536..1567 */
    0x04804004,     /* GOP1 MIB        interrupt index  1568..1599 */
    0x05004004,     /* GOP2 MIB        interrupt index  1600..1631 */
    0x05804004      /* GOP3 MIB        interrupt index  1632..1663 */
};

/* the same as cheetahMaskRegMapArr except the PCI that is replaced with the
   GLOBAL */
static GT_U32 cheetahMaskRegMapArr_SMI[sizeof(cheetahMaskRegMapArr)/4];

/* the same as cheetahMaskRegMapArr_SMI except DX106 not have all addresses .
   so those registers "bypassed" by using user defined register address */
static GT_U32 cheetahMaskRegMapArr_SMI_DX106[sizeof(cheetahMaskRegMapArr_SMI)/4];

/* HX SMI default maks - same as cheetahMaskRegMapArr with additional registers for ports 25 & 26 */
static GT_U32 cheetahMaskRegMapArr_SMI_HX[sizeof(cheetahMaskRegMapArr)/4 + 2];

/* HX PCI default maks */
static GT_U32 cheetahMaskRegMapArr_PCI_HX[sizeof(cheetahMaskRegMapArr)/4 + 2];


/* This array maps an interrupt index to its relevant interrupt mask reg    */
/* default value, cell i indicates the address for interrupts               */
/* (32 * i  --> 32 * (i + 1) - 1).                                          */
/* summary bits are turned on and the specific cause mask is enabled when   */
/* bound by appl.                                                           */
static GT_U32 cheetahMaskRegDefaultSummaryArr[] =
{
    0x17FFF800,         /* GLOBAL         */
    0x00000000,         /* Miscellaneous  */
    0x00000000,         /* GPP            */
    0x00000000,         /* RxSDMA         */
    0x00000000,         /* TxSDMA         */
    0x17EFF800,         /* PCI            */
    0x1FFFFFFE,         /* Tri-speed port */
    0x00000000,         /* PORT0          */
    0x00000000,         /* PORT1          */
    0x00000000,         /* PORT2          */
    0x00000000,         /* PORT3          */
    0x00000000,         /* PORT4          */
    0x00000000,         /* PORT5          */
    0x00000000,         /* PORT6          */
    0x00000000,         /* PORT7          */
    0x00000000,         /* PORT8          */
    0x00000000,         /* PORT9          */
    0x00000000,         /* PORT10         */
    0x00000000,         /* PORT11         */
    0x00000000,         /* PORT12         */
    0x00000000,         /* PORT13         */
    0x00000000,         /* PORT14         */
    0x00000000,         /* PORT15         */
    0x00000000,         /* PORT16         */
    0x00000000,         /* PORT17         */
    0x00000000,         /* PORT18         */
    0x00000000,         /* PORT19         */
    0x00000000,         /* PORT20         */
    0x00000000,         /* PORT21         */
    0x00000000,         /* PORT22         */
    0x00000000,         /* PORT23         */
    0x00000000,         /* PORT63 (CPU)   */
    0x00000000,         /* PORT24         */
    0x00000000,         /* PORT25         */
    0x00000000,         /* PORT26         */
    0x00000000,         /* HG.StackMIB    */
    0x00000000,         /* PolicyEngine   */
    0x00000000,         /* BRIDGE         */
    0x00000000,         /* IngressSTC     */
    0x0000007E,         /* TXQ            */
    0x00000000,         /* TXQFlush       */
    0x00000000,         /* TXQWD          */
    0x00000000,         /* TXQGEN         */
    0x00000000,         /* EgrerssSTC     */
    0x00000000,         /* BuffMem        */
    0x00000000,         /* BuffMngR0      */
    0x00000000,         /* BuffMngR1      */
    0x00000000,         /* MACTable       */
    0x00000000,         /* GOP0 MIB       */
    0x00000000,         /* GOP1 MIB       */
    0x00000000,         /* GOP2 MIB       */
    0x00000000,         /* GOP3 MIB       */
    0x00000000,         /* HX port 25 - relevant only for DX249/269 */
    0x00000000          /* HX port 26 - relevant only for DX249/269 */
};

/*
 * Typedef: struct cheetahIntrScanArr
 *
 * Description: Basic struct for accessing interrupt register in hierarchy
 *              tree.
 *
 * Fields:
 *      bitNum          - Sum bit num in upper hierarchy, representing this
 *                        register.
 *      isGpp           - Is hierarchy connected to another device.
 *      gppId           - The gpp Id represented by this node (Valid only if
 *                        isGpp is GT_TRUE).
 *      gppFuncPtr      - Pointer to isrFunc to be called if isGPP == GT_TRUE.
 *      causeRegAddr    - Address of the interrupt cause register to scan.
 *      maskRegAddr     - Address of the interrupt mask register to update after
 *                        scanning.
 *      pRegReadFunc    - A pointer to a function to be used for reading the
 *                        interrupt cause register.
 *      pRegWriteFunc   - A pointer to a function to be used for writing the
 *                        interrupt mask register.
 *      startIdx        - The start interrupt index representing the interrupts
 *                        to be scanned.
 *      endIdx          - The end interrupt index representing the interrupts to
 *                        be scanned.
 *      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 *      subIntrListLen  - List length sub interrupts list.
 *      subIntrScan     - pointer to sum bit interrupt register structure.
 *      nextIntrScan    - pointer to next interrupt struct in hierarchy.
 */

static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC cheetahIntrScanArr[] =
{
    /* PCI Interrupt Summary Cause*/
    {0, GT_FALSE,0, NULL, 0x114, 0x118,
        prvCpssDrvHwPpPortGroupReadInternalPciReg,
        prvCpssDrvHwPpPortGroupWriteInternalPciReg,
        PRV_CPSS_CH_PCI_MASTER_RD_ERROR_E,/* should be 1 to avoid scanning the
                                              * summary bit of the PCI main
                                              * cause interrupt */
        PRV_CPSS_CH_PCI_PRE_EGRESS_SUM_E,      /* enumerators range 1- > 10*/
        0x7ff, 0x7ff, 0x0, 16, NULL, NULL},

             /* Miscellaneous    */
        {11,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_CH_EB_AUQ_PENDING_E,
            MISC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /*Buffers Memory and MAC Errors  */
        {12,GT_FALSE,0, NULL, 0x07800024, 0x07800028,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_BM_TQ_PARITY_ERROR_PORT0_E,
            PRV_CPSS_CH_BUF_MEM_MAC_ERROR_E,
            BUF_MEM_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* Transmit Queue */
        {13, GT_FALSE,0, NULL, 0x01800100, 0x01800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_TXQ_SUM_WATCHDOG_E,
            PRV_CPSS_CH_TXQ_SUM_HGS_MACMIB_E,
            0x0, 0x0, 0xFFFFFFFF, 6,NULL, NULL},

             /* Transmit Queue WatchDog */
             {1, GT_FALSE,0, NULL, 0x01800108, 0x0180010C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_TQ_WATCHDOG_EX_PORT0_E,
                 PRV_CPSS_CH_TQ_WATCHDOG_EX_PORT_CPU_63_E,
                TXQ_WD_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* Transmit Queue Flash */
             {2, GT_FALSE,0, NULL, 0x01800110, 0x01800114,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_TQ_TXQ2_FLUSH_PORT0_E,
                 PRV_CPSS_CH_TQ_TXQ2_FLUSH_PORT_CPU_63_E,
                 TXQ_FLASH_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Transmit Queue General */
             {3, GT_FALSE,0, NULL, 0x01800118, 0x0180011C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_TQ_SNIFF_DESC_DROP_E,
                 PRV_CPSS_CH_TXQ_GEN_BUF_FULL_E,
                 TXQ_GEN_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* GPP */
             {4, GT_FALSE,0, NULL, 0x018001CC, 0x018001C0,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_GPP_INTERRUPT1_E,
                 PRV_CPSS_CH_GPP_INTERRUPT8_E,
                 GPP_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

               /* Egress STC */
             {5, GT_FALSE,0, NULL, 0x01800130, 0x01800134,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_EGRESS_STC_PORT0_E,
                 PRV_CPSS_CH_EGRESS_STC_PORT26_E,
                 EGRESS_STC_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* HyperG.Stack-MACMIB */
             {6, GT_FALSE,0, NULL, 0x01800138, 0x0180013C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_HGSMIB_PORT_24_COUNTER_WRAP_E,
                 PRV_CPSS_CH_XSMI_WRITE_DONE_E,
                 HGS_MIB_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


        /* L2 Bridge  */
        {14, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_EB_NA_FIFO_FULL_E,
            PRV_CPSS_CH_EB_SECURITY_BREACH_UPDATE_E,
            BRIDGE_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


           /* Buffer Management Register 0*/
        {15, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_BM_EMPTY_CLEAR_E,
            PRV_CPSS_CH_BM_PORT_RX_FULL_E,
            BUF_MNG_R0_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

           /* Buffer Management Register 1*/
        {16, GT_FALSE,0, NULL, 0x03000048, 0x0300004C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_BM_MAX_BUFF_REACHED_PORT0_E,
            PRV_CPSS_CH_BM_MAX_BUFF_REACHED_PORT_CPU_63_E,
            BUF_MNG_R1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* MAC Table,
           maskRcvIntrEn - both AUProcCompletedInt and AgeViaTriggerEnded
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_MAC_NUM_OF_HOP_EXP_E,
            PRV_CPSS_CH_MAC_FIFO_2_CPU_EXCEEDED_E,
            MAC_NSBM, 0x0, 0xFFFFFDEF, 0,NULL, NULL},

        /* Ports - Tri-speed ports MIB counters  */
        {18, GT_FALSE,0, NULL, 0x04004010, 0x04805110,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_3SPPMIB_GOP3MIB_SUM_E,
            PRV_CPSS_CH_3SPPMIB_PORT23_SUM_E,
            0x0, 0x0, 0xFFFFFFFF, 28,NULL, NULL},

            /* GOP3 MIB  */
            {1, GT_FALSE,0, NULL, 0x05804000, 0x05804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_3_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_3_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP2 MIB  */
            {2, GT_FALSE,0, NULL, 0x05004000, 0x05004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_2_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_2_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP1 MIB  */
            {3, GT_FALSE,0, NULL, 0x04804000, 0x04804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_1_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_1_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP0 MIB  */
            {4, GT_FALSE,0, NULL, 0x04004000, 0x04004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_0_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_0_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 0  */
            {5, GT_FALSE,0, NULL, 0x0A800020, 0x0A800024,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT0_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT0_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 1  */
            {6, GT_FALSE,0, NULL, 0x0A800120, 0x0A800124,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT1_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT1_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 2  */
            {7, GT_FALSE,0, NULL, 0x0A800220, 0x0A800224,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT2_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT2_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 3  */
            {8, GT_FALSE,0, NULL, 0x0A800320, 0x0A800324,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT3_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT3_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 4  */
            {9, GT_FALSE,0, NULL, 0x0A800420, 0x0A800424,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT4_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT4_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 5  */
            {10, GT_FALSE,0, NULL, 0x0A800520, 0x0A800524,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT5_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT5_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 6  */
            {11, GT_FALSE,0, NULL, 0x0A800620, 0x0A800624,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT6_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT6_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* Port 7  */
            {12, GT_FALSE,0, NULL, 0x0A800720, 0x0A800724,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT7_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT7_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 8  */
            {13, GT_FALSE,0, NULL, 0x0A800820, 0x0A800824,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT8_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT8_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 9  */
            {14, GT_FALSE,0, NULL, 0x0A800920, 0x0A800924,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT9_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT9_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 10  */
            {15, GT_FALSE,0, NULL, 0x0A800A20, 0x0A800A24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT10_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT10_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 11  */
            {16, GT_FALSE,0, NULL, 0x0A800B20, 0x0A800B24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT11_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT11_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 12  */
            {17, GT_FALSE,0, NULL, 0x0A800C20, 0x0A800C24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT12_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT12_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 13  */
            {18, GT_FALSE,0, NULL, 0x0A800D20, 0x0A800D24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT13_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT13_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 14  */
            {19, GT_FALSE,0, NULL, 0x0A800E20, 0x0A800E24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT14_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT14_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 15  */
            {20, GT_FALSE,0, NULL, 0x0A800F20, 0x0A800F24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT15_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT15_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 16  */
            {21, GT_FALSE,0, NULL, 0x0A801020, 0x0A801024,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT16_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT16_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 17  */
            {22, GT_FALSE,0, NULL, 0x0A801120, 0x0A801124,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT17_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT17_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 18  */
            {23, GT_FALSE,0, NULL, 0x0A801220, 0x0A801224,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT18_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT18_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 19  */
            {24, GT_FALSE,0, NULL, 0x0A801320, 0x0A801324,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT19_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT19_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 20  */
            {25, GT_FALSE,0, NULL, 0x0A801420, 0x0A801424,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT20_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT20_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 21  */
            {26, GT_FALSE,0, NULL, 0x0A801520, 0x0A801524,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT21_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT21_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 22  */
            {27, GT_FALSE,0, NULL, 0x0A801620, 0x0A801624,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT22_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT22_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 23  */
            {28, GT_FALSE,0, NULL, 0x0A801720, 0x0A801724,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT23_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT23_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* CPU Port SUM  */
        {19, GT_FALSE,0, NULL, 0x0A803F20, 0x0A803F24,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT_CPU_63_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT_CPU_63_E,
            PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Tx SDMA  */
        {21, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_TX_BUFFER_QUEUE0_E,
            PRV_CPSS_CH_TX_END_QUEUE7_E,
            TXSDMA_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Rx SDMA  */
        {22, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_RX_BUFFER_QUEUE0_E,
            PRV_CPSS_CH_SDMA_RX_PACKET_CNT_OFF_E,
            RXSDMA_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* PCL  */
        {23, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_PCL_ADDR_OUT_RANGE_E,
            PRV_CPSS_CH_PCL_ADDR_OUT_RANGE_E,
            POLICY_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Port 24  */
        {24, GT_FALSE,0, NULL, 0x0A801814, 0x0A801818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT24_E,
            PRV_CPSS_CH_XAUI_PHY_PORT24_E,
            HGS_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Port 25  */
        {25, GT_FALSE,0, NULL, 0x0A801914, 0x0A801918,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT25_E,
            PRV_CPSS_CH_XAUI_PHY_PORT25_E,
            HGS_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Port 26  */
        {26, GT_FALSE,0, NULL, 0x0A801A14, 0x0A801A18,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT26_E,
            PRV_CPSS_CH_XAUI_PHY_PORT26_E,
            HGS_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Ingress STC  */
        {28, GT_FALSE,0, NULL, 0x0B000020, 0x0B000024,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_MAC_SFLOW_PORT0_E,
            PRV_CPSS_CH_MAC_SFLOW_PORT26_E,
            INGRESS_STC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

};

/*
 * Typedef: struct cheetahHxIntrScanArr
 *
 * Description: Basic struct for accessing interrupt register in hierarchy
 *              tree.
 *
 * Fields:
 *      bitNum          - Sum bit num in upper hierarchy, representing this
 *                        register.
 *      isGpp           - Is hierarchy connected to another device.
 *      gppId           - The gpp Id represented by this node (Valid only if
 *                        isGpp is GT_TRUE).
 *      gppFuncPtr      - Pointer to isrFunc to be called if isGPP == GT_TRUE.
 *      causeRegAddr    - Address of the interrupt cause register to scan.
 *      maskRegAddr     - Address of the interrupt mask register to update after
 *                        scanning.
 *      pRegReadFunc    - A pointer to a function to be used for reading the
 *                        interrupt cause register.
 *      pRegWriteFunc   - A pointer to a function to be used for writing the
 *                        interrupt mask register.
 *      startIdx        - The start interrupt index representing the interrupts
 *                        to be scanned.
 *      endIdx          - The end interrupt index representing the interrupts to
 *                        be scanned.
 *      nonSumBitMask   - bit mask of non sum bits in the register
 *      rwBitMask       - bit mask of R/W bits that should be cleared by write.
 *      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.
 *      subIntrListLen  - List length sub interrupts list.
 *      subIntrScan     - pointer to sum bit interrupt register structure.
 *      nextIntrScan    - pointer to next interrupt struct in hierarchy.
 */

static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC cheetahHxIntrScanArr[] =
{
    /* Global Interrupt Summary Cause */
    {0, GT_FALSE,0, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_CH_MAS_READ_ERR_E, /* should be 1 to avoid scanning the
                                              * summary bit of the PCI main
                                              * cause interrupt */
        PRV_CPSS_CH_GLOBAL_PRE_EGRESS_SUM_E,    /* enumerators range 1- > 10*/
        GLOBAL_NSBM, 0x0, BITS(11,28), 16, NULL, NULL},
        /* maskRcvIntrEn consists of bits used for "edge" ISR to unmask after
          interrupt scan. */

             /* Miscellaneous    */
        {11,GT_FALSE,0, NULL, 0x00000038, 0x0000003C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_MISC_TWSI_TIME_OUT_E,
            PRV_CPSS_CH_EB_AUQ_PENDING_E,
            MISC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /*Buffers Memory and MAC Errors  */
        {12,GT_FALSE,0, NULL, 0x07800024, 0x07800028,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_BM_TQ_PARITY_ERROR_PORT0_E,
            PRV_CPSS_CH_BUF_MEM_MAC_ERROR_E,
            BUF_MEM_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* Transmit Queue */
        {13, GT_FALSE,0, NULL, 0x01800100, 0x01800104,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_TXQ_SUM_WATCHDOG_E,
            PRV_CPSS_CH_TXQ_SUM_HGS_MACMIB_E,
            0x0, 0x0, 0xFFFFFFFF, 6,NULL, NULL},

             /* Transmit Queue WatchDog */
             {1, GT_FALSE,0, NULL, 0x01800108, 0x0180010C,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_TQ_WATCHDOG_EX_PORT0_E,
                 PRV_CPSS_CH_TQ_WATCHDOG_EX_PORT_CPU_63_E,
                TXQ_WD_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

            /* Transmit Queue Flash */
             {2, GT_FALSE,0, NULL, 0x01800110, 0x01800114,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_TQ_TXQ2_FLUSH_PORT0_E,
                 PRV_CPSS_CH_TQ_TXQ2_FLUSH_PORT_CPU_63_E,
                 TXQ_FLASH_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* Transmit Queue General */
             {3, GT_FALSE,0, NULL, 0x01800118, 0x0180011C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_TQ_SNIFF_DESC_DROP_E,
                 PRV_CPSS_CH_TXQ_GEN_BUF_FULL_E,
                 TXQ_GEN_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* GPP */
             {4, GT_FALSE,0, NULL, 0x018001CC, 0x018001C0,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_GPP_INTERRUPT1_E,
                 PRV_CPSS_CH_GPP_INTERRUPT8_E,
                 GPP_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

               /* Egress STC */
             {5, GT_FALSE,0, NULL, 0x01800130, 0x01800134,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_EGRESS_STC_PORT0_E,
                 PRV_CPSS_CH_EGRESS_STC_PORT26_E,
                 EGRESS_STC_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},

             /* HyperG.Stack-MACMIB */
             {6, GT_FALSE,0, NULL, 0x01800138, 0x0180013C,
                 prvCpssDrvHwPpPortGroupIsrRead,
                 prvCpssDrvHwPpPortGroupIsrWrite,
                 PRV_CPSS_CH_HGSMIB_PORT_24_COUNTER_WRAP_E,
                 PRV_CPSS_CH_XSMI_WRITE_DONE_E,
                 HGS_MIB_NSBM, 0x0, 0xFFFFFFFF, 0, NULL, NULL},


        /* L2 Bridge  */
        {14, GT_FALSE,0, NULL, 0x02040130, 0x02040134,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_EB_NA_FIFO_FULL_E,
            PRV_CPSS_CH_EB_SECURITY_BREACH_UPDATE_E,
            BRIDGE_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


           /* Buffer Management Register 0*/
        {15, GT_FALSE,0, NULL, 0x03000040, 0x03000044,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_BM_EMPTY_CLEAR_E,
            PRV_CPSS_CH_BM_PORT_RX_FULL_E,
            BUF_MNG_R0_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

           /* Buffer Management Register 1*/
        {16, GT_FALSE,0, NULL, 0x03000048, 0x0300004C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_BM_MAX_BUFF_REACHED_PORT0_E,
            PRV_CPSS_CH_BM_MAX_BUFF_REACHED_PORT_CPU_63_E,
            BUF_MNG_R1_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


        /* MAC Table,
           maskRcvIntrEn - both AUProcCompletedInt and AgeViaTriggerEnded
           should never be masked to avoid missed events situation.  */
        {17, GT_FALSE,0, NULL, 0x06000018, 0x0600001C,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_MAC_NUM_OF_HOP_EXP_E,
            PRV_CPSS_CH_MAC_FIFO_2_CPU_EXCEEDED_E,
            MAC_NSBM, 0x0, 0xFFFFFDEF, 0,NULL, NULL},

        /* Ports - Tri-speed ports MIB counters  */
        {18, GT_FALSE,0, NULL, 0x04004010, 0x04805110,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_3SPPMIB_GOP3MIB_SUM_E,
            PRV_CPSS_CH_3SPPMIB_PORT23_SUM_E,
            0x0, 0x0, 0xFFFFFFFF, 28,NULL, NULL},

            /* GOP3 MIB  */
            {1, GT_FALSE,0, NULL, 0x05804000, 0x05804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_3_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_3_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP2 MIB  */
            {2, GT_FALSE,0, NULL, 0x05004000, 0x05004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_2_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_2_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP1 MIB  */
            {3, GT_FALSE,0, NULL, 0x04804000, 0x04804004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_1_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_1_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* GOP0 MIB  */
            {4, GT_FALSE,0, NULL, 0x04004000, 0x04004004,
                prvCpssDxChPortGroupGopIsrRegRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_GOP_ADDRESS_OUT_OF_RANGE_0_E,
                PRV_CPSS_CH_GOP_COUNT_COPY_DONE_0_E,
                GOP_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 0  */
            {5, GT_FALSE,0, NULL, 0x0A800020, 0x0A800024,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT0_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT0_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 1  */
            {6, GT_FALSE,0, NULL, 0x0A800120, 0x0A800124,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT1_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT1_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 2  */
            {7, GT_FALSE,0, NULL, 0x0A800220, 0x0A800224,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT2_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT2_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 3  */
            {8, GT_FALSE,0, NULL, 0x0A800320, 0x0A800324,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT3_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT3_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 4  */
            {9, GT_FALSE,0, NULL, 0x0A800420, 0x0A800424,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT4_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT4_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 5  */
            {10, GT_FALSE,0, NULL, 0x0A800520, 0x0A800524,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT5_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT5_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 6  */
            {11, GT_FALSE,0, NULL, 0x0A800620, 0x0A800624,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT6_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT6_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* Port 7  */
            {12, GT_FALSE,0, NULL, 0x0A800720, 0x0A800724,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT7_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT7_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 8  */
            {13, GT_FALSE,0, NULL, 0x0A800820, 0x0A800824,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT8_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT8_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 9  */
            {14, GT_FALSE,0, NULL, 0x0A800920, 0x0A800924,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT9_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT9_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 10  */
            {15, GT_FALSE,0, NULL, 0x0A800A20, 0x0A800A24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT10_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT10_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 11  */
            {16, GT_FALSE,0, NULL, 0x0A800B20, 0x0A800B24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT11_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT11_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 12  */
            {17, GT_FALSE,0, NULL, 0x0A800C20, 0x0A800C24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT12_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT12_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 13  */
            {18, GT_FALSE,0, NULL, 0x0A800D20, 0x0A800D24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT13_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT13_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 14  */
            {19, GT_FALSE,0, NULL, 0x0A800E20, 0x0A800E24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT14_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT14_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 15  */
            {20, GT_FALSE,0, NULL, 0x0A800F20, 0x0A800F24,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT15_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT15_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 16  */
            {21, GT_FALSE,0, NULL, 0x0A801020, 0x0A801024,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT16_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT16_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 17  */
            {22, GT_FALSE,0, NULL, 0x0A801120, 0x0A801124,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT17_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT17_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 18  */
            {23, GT_FALSE,0, NULL, 0x0A801220, 0x0A801224,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT18_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT18_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 19  */
            {24, GT_FALSE,0, NULL, 0x0A801320, 0x0A801324,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT19_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT19_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 20  */
            {25, GT_FALSE,0, NULL, 0x0A801420, 0x0A801424,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT20_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT20_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 21  */
            {26, GT_FALSE,0, NULL, 0x0A801520, 0x0A801524,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT21_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT21_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 22  */
            {27, GT_FALSE,0, NULL, 0x0A801620, 0x0A801624,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT22_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT22_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 23  */
            {28, GT_FALSE,0, NULL, 0x0A801720, 0x0A801724,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT23_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT23_E,
                PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},


            /* CPU Port SUM  */
        {19, GT_FALSE,0, NULL, 0x0A803F20, 0x0A803F24,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT_CPU_63_E,
                PRV_CPSS_CH_TX_FIFO_UNDERRUN_PORT_CPU_63_E,
            PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Tx SDMA  */
        {21, GT_FALSE,0, NULL, 0x00002810, 0x00002818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_TX_BUFFER_QUEUE0_E,
            PRV_CPSS_CH_TX_END_QUEUE7_E,
            TXSDMA_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Rx SDMA  */
        {22, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_RX_BUFFER_QUEUE0_E,
            PRV_CPSS_CH_SDMA_RX_PACKET_CNT_OFF_E,
            RXSDMA_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* PCL  */
        {23, GT_FALSE,0, NULL, 0x0000280C, 0x00002814,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_PCL_ADDR_OUT_RANGE_E,
            PRV_CPSS_CH_PCL_ADDR_OUT_RANGE_E,
            POLICY_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Port 24  */
        {24, GT_FALSE,0, NULL, 0x0A801814, 0x0A801818,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT24_E,
            PRV_CPSS_CH_XAUI_PHY_PORT24_E,
            HGS_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

             /* Port 25  */
        {25, GT_FALSE,0, NULL, 0x0A801914, 0x0A801918,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT25_E,
            PRV_CPSS_CH_XAUI_PHY_PORT25_E,
            HGS_PORT_NSBM, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

            /* HX port  */
            {8, GT_FALSE,0, NULL, 0x0A803940, 0x0A803944,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_HX_LINK_STATUS_CHANGED_PORT25_E,
                PRV_CPSS_CH_HX_PPM_FIFO_OVERRUN_PORT25_E,
                HX_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Port 26  */
        {26, GT_FALSE,0, NULL, 0x0A801A14, 0x0A801A18,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_LINK_STATUS_CHANGED_PORT26_E,
            PRV_CPSS_CH_XAUI_PHY_PORT26_E,
            HGS_PORT_NSBM, 0x0, 0xFFFFFFFF, 1,NULL, NULL},

        /* HX port  */
            {8, GT_FALSE,0, NULL, 0x0A803A40, 0x0A803A44,
                prvCpssDrvHwPpPortGroupIsrRead,
                prvCpssDrvHwPpPortGroupIsrWrite,
                PRV_CPSS_CH_HX_LINK_STATUS_CHANGED_PORT26_E,
                PRV_CPSS_CH_HX_PPM_FIFO_OVERRUN_PORT26_E,
                HX_PORT_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

            /* Ingress STC  */
        {28, GT_FALSE,0, NULL, 0x0B000020, 0x0B000024,
            prvCpssDrvHwPpPortGroupIsrRead,
            prvCpssDrvHwPpPortGroupIsrWrite,
            PRV_CPSS_CH_MAC_SFLOW_PORT0_E,
            PRV_CPSS_CH_MAC_SFLOW_PORT26_E,
            INGRESS_STC_NSBM, 0x0, 0xFFFFFFFF, 0,NULL, NULL},

};

/* allocate CH HX PCI interrupt scan array */
static PRV_CPSS_DRV_INTERRUPT_SCAN_STC cheetahHxIntrScanArrPci[sizeof(cheetahHxIntrScanArr) /
                                               sizeof(cheetahHxIntrScanArr[0])];


/* info about the global register that in SMI mode used as the "root" of the
   scan tree */
static const PRV_CPSS_DRV_INTERRUPT_SCAN_STC  cheetahIntrScanArr_SMI_global =
    {0, GT_FALSE,0, NULL, 0x00000030, 0x00000034,
        prvCpssDrvHwPpPortGroupIsrRead,
        prvCpssDrvHwPpPortGroupIsrWrite,
        PRV_CPSS_CH_MAS_READ_ERR_E, /* should be 1 to avoid scanning the
                                              * summary bit of the PCI main
                                              * cause interrupt */
        PRV_CPSS_CH_GLOBAL_PRE_EGRESS_SUM_E,    /* enumerators range 1- > 10*/
        GLOBAL_NSBM, 0x0, BITS(11,28), 16, NULL, NULL};

/*
 * Typedef: struct cheetahIntrScanArr_SMI
 *
 * Description: Basic struct for accessing interrupt register in hierarchy
 *              tree. -- Cheetah SMI devices
 *
*/
static PRV_CPSS_DRV_INTERRUPT_SCAN_STC cheetahIntrScanArr_SMI[
            sizeof(cheetahIntrScanArr)/sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC)];


/*******************************************************************************
* prvCpssDrvPpIntDefCheetahInit
*
* DESCRIPTION:
*       Interrupts initialization for the Cheetah devices.
*
* INPUTS:
*       Node.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
void prvCpssDrvPpIntDefCheetahInit(void)
{
    GT_U32  ii;
    PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC *devFamilyPtr;
    GT_U32  *maskRegPtr;
    GT_U32  mainRegParam;/* main parameter from the register  address */
    GT_U32  secRegParam;/* secondary parameter from the register  address */

    /* init all special arrays */
    cpssOsMemCpy(cheetahMaskRegMapArr_SMI,
                 cheetahMaskRegMapArr,
                 sizeof(cheetahMaskRegMapArr));
    maskRegPtr = cheetahMaskRegMapArr_SMI;

    /* replace PCI interrupt register with Global interrupt register */
    for(ii = 0 ; ii < (sizeof(cheetahMaskRegMapArr_SMI) / 4); ii ++)
    {
        if(maskRegPtr[ii] == 0x00000118)
        {
            maskRegPtr[ii] = 0x00000034;
            break;
        }
    }

    cpssOsMemCpy(cheetahMaskRegMapArr_SMI_HX,
                 cheetahMaskRegMapArr_SMI,
                 sizeof(cheetahMaskRegMapArr_SMI));
    maskRegPtr = cheetahMaskRegMapArr_SMI_HX;

    /* add HX ports registers */
    ii = sizeof(cheetahMaskRegMapArr_SMI) / 4;
    maskRegPtr[ii]   = 0x0A803944;  /* HX port 25 */
    maskRegPtr[ii+1] = 0x0A803A44;  /* HX port 26 */

    cpssOsMemCpy(cheetahMaskRegMapArr_SMI_DX106,
                 cheetahMaskRegMapArr_SMI,
                 sizeof(cheetahMaskRegMapArr));

    maskRegPtr = cheetahMaskRegMapArr_SMI_DX106;
    /* replace PCI interrupt register with Global interrupt register */
    for(ii = 0 ; ii < (sizeof(cheetahMaskRegMapArr_SMI_DX106) / 4); ii ++)
    {
        if(maskRegPtr[ii] == 0x00002814 ||
           maskRegPtr[ii] == 0x00002818)
        {
            /*0x00002814,      RxSDMA          interrupt index   96..127 */
            /*0x00002818,      TxSDMA          interrupt index  128..159 */
            maskRegPtr[ii] = STUB_ADDRESS;
            continue;
        }

        mainRegParam  = (maskRegPtr[ii] & 0xFFFFE0FF);
        secRegParam   = (maskRegPtr[ii] & 0x00001F00);

        if(mainRegParam == 0x0A800024 &&
           secRegParam >= 0x00000A00 && /*PORT10 */
           secRegParam <= 0x00001700)   /*PORT23 */
        {
            /* ports 10 .. 23 */
            /*0x0A800A24,      PORT10          interrupt index  544..575 */
            /*0x0A800B24,      PORT11          interrupt index  576..607 */
            /*0x0A800C24,      PORT12          interrupt index  608..639 */
            /*0x0A800D24,      PORT13          interrupt index  640..671 */
            /*0x0A800E24,      PORT14          interrupt index  672..703 */
            /*0x0A800F24,      PORT15          interrupt index  704..735 */
            /*0x0A801024,      PORT16          interrupt index  736..767 */
            /*0x0A801124,      PORT17          interrupt index  768..799 */
            /*0x0A801224,      PORT18          interrupt index  800..831 */
            /*0x0A801324,      PORT19          interrupt index  832..863 */
            /*0x0A801424,      PORT20          interrupt index  864..895 */
            /*0x0A801524,      PORT21          interrupt index  896..927 */
            /*0x0A801624,      PORT22          interrupt index  928..959 */
            /*0x0A801724,      PORT23          interrupt index  960..991 */
            maskRegPtr[ii] = STUB_ADDRESS;
            continue;
        }

        if(mainRegParam == 0x0A800018 &&
           secRegParam >= 0x00001800 &&/*PORT24*/
           secRegParam <= 0x00001F00)  /*PORT31*/
        {
            /*0x0A801818,      PORT24          interrupt index  1024..1055 */
            /*0x0A801918,      PORT25          interrupt index  1056..1087 */
            /*0x0A801A18,      PORT26          interrupt index  1088..1119 */
            maskRegPtr[ii] = STUB_ADDRESS;
            continue;
        }

        if(maskRegPtr[ii] == 0x0180013C )
        {
            /*0x0180013C,      HG.StackMIB     interrupt index  1120..1151 */
            maskRegPtr[ii] = STUB_ADDRESS;
            continue;
        }


        mainRegParam = (maskRegPtr[ii] & 0xFF7FFFFF);

        if(mainRegParam == 0x05004004)
        {
            /*0x05004004,      GOP2 MIB        interrupt index  1600..1631 */
            /*0x05804004       GOP3 MIB        interrupt index  1632..1663 */
            maskRegPtr[ii] = STUB_ADDRESS;
            continue;
        }
    }

    /* duplicate the scan tree of PCI to the SMI */
    /* then override the "root" of the tree */
    cpssOsMemCpy(cheetahIntrScanArr_SMI,
                 (void*)cheetahIntrScanArr,
                 sizeof(cheetahIntrScanArr));

    cpssOsMemCpy(&cheetahIntrScanArr_SMI[0],
                 (void*)&cheetahIntrScanArr_SMI_global,
                 sizeof(cheetahIntrScanArr_SMI_global));

    /* cheetah PCI info */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_E];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_CH_LAST_INT_E;
    devFamilyPtr->maskRegistersAddressesArray = cheetahMaskRegMapArr;
    devFamilyPtr->maskRegistersDefaultValuesArray = cheetahMaskRegDefaultSummaryArr;
    devFamilyPtr->interruptsScanArray = cheetahIntrScanArr;
    devFamilyPtr->numOfScanElements = sizeof(cheetahIntrScanArr)/sizeof(cheetahIntrScanArr[0]);
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_CH_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;


    /* cheetah SMI info */
    /* start with same info as PCI , then override some of it */
    cpssOsMemCpy(&prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_E],
                 &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_E],
                 sizeof(PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC));

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_E];

    devFamilyPtr->maskRegistersAddressesArray = cheetahMaskRegMapArr_SMI;
    devFamilyPtr->interruptsScanArray = cheetahIntrScanArr_SMI;
    devFamilyPtr->numOfScanElements = sizeof(cheetahIntrScanArr_SMI)/sizeof(cheetahIntrScanArr_SMI[0]);

    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_106_E];
    /* cheetah SMI Dx106 info */
    /* start with same info as SMI , then override some of it */
    cpssOsMemCpy(devFamilyPtr,
                 &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_SMI_E],
                 sizeof(PRV_CPSS_DRV_DEVICE_INTERRUPTS_INFO_STC));
    devFamilyPtr->maskRegistersAddressesArray = cheetahMaskRegMapArr_SMI_DX106;

    /* DX 249/269 - SMI */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_HX_SMI_E];
    devFamilyPtr->numOfInterrupts = PRV_CPSS_CH_HX_LAST_INT_E;
    devFamilyPtr->maskRegistersAddressesArray = cheetahMaskRegMapArr_SMI_HX;
    devFamilyPtr->maskRegistersDefaultValuesArray = cheetahMaskRegDefaultSummaryArr;
    devFamilyPtr->interruptsScanArray = cheetahHxIntrScanArr;
    devFamilyPtr->numOfScanElements = sizeof(cheetahHxIntrScanArr)/sizeof(cheetahHxIntrScanArr[0]);
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_CH_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;

    /* initialize CH HX PCI DBs */
    devFamilyPtr = &prvCpssDrvDevFamilyInterruptInfoArray[PRV_CPSS_DRV_FAMILY_CHEETAH_HX_PCI_E];

    /* init all special arrays */
    cpssOsMemCpy(cheetahMaskRegMapArr_PCI_HX,
                 cheetahMaskRegMapArr,
                 sizeof(cheetahMaskRegMapArr));
    maskRegPtr = cheetahMaskRegMapArr_PCI_HX;

    /* add HX ports registers */
    ii = sizeof(cheetahMaskRegMapArr_SMI) / 4;
    maskRegPtr[ii]   = 0x0A803944;  /* HX port 25 */
    maskRegPtr[ii+1] = 0x0A803A44;  /* HX port 26 */

    cpssOsMemCpy(cheetahHxIntrScanArrPci,
                 cheetahHxIntrScanArr,
                 sizeof(cheetahHxIntrScanArr));

    /* copy PCI node from CH PCI one */
    cheetahHxIntrScanArrPci[0] = cheetahIntrScanArr[0];

    devFamilyPtr->numOfInterrupts = PRV_CPSS_CH_HX_LAST_INT_E;
    devFamilyPtr->maskRegistersAddressesArray = cheetahMaskRegMapArr_PCI_HX;
    devFamilyPtr->maskRegistersDefaultValuesArray = cheetahMaskRegDefaultSummaryArr;
    devFamilyPtr->interruptsScanArray = cheetahHxIntrScanArrPci;
    devFamilyPtr->numOfScanElements = sizeof(cheetahHxIntrScanArrPci)/sizeof(cheetahHxIntrScanArrPci[0]);
    devFamilyPtr->fdbTrigEndedId = PRV_CPSS_CH_MAC_AGE_VIA_TRIGGER_ENDED_E;
    devFamilyPtr->fdbTrigEndedCbPtr = NULL;
    devFamilyPtr->hasFakeInterrupts = GT_FALSE;
    devFamilyPtr->firstFakeInterruptId = 0;/* don't care */
    devFamilyPtr->drvIntCauseToUniEvConvertFunc = &prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert;
    devFamilyPtr->numOfInterruptRegistersNotAccessibleBeforeStartInit = 0;
    devFamilyPtr->notAccessibleBeforeStartInitPtr=NULL;
}

