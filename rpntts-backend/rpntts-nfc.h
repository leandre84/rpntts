#ifndef RPNTTS_NFC_H
#define RPNTTS_NFC_H

#include <ph_NxpBuild.h>
#include <ph_Status.h>
#include <phbalReg_RpiSpi.h>
#include <phpalI14443p3a.h>
#include <phpalI14443p3b.h>
#include <phpalI14443p4.h>
#include <phpalI14443p4a.h>
#include <phpalI18092mPI.h>
#include <phpalI18092mT.h>
#include <phpalFelica.h>
#include <phalMful.h>
#include <phalMfc.h>
#include <phalMfdf.h>
#include <phalFelica.h>
#include <phalTop.h>
#include <phalT1T.h>
#include <phacDiscLoop.h>
#include <phOsal_GLib.h>
#include <phKeyStore.h>
#include <phacDiscLoop_Sw.h>

#include <glib.h>

/* Error code definitions */
#define RPNTTS_NFC_INIT_ERR_BAL 1
#define RPNTTS_NFC_INIT_ERR_HAL 2
#define RPNTTS_NFC_INIT_ERR_HAL_SPICONFIG 3
#define RPNTTS_NFC_INIT_ERR_BAL_OPEN 4
#define RPNTTS_NFC_INIT_ERR_PAL_I14443P3A 5
#define RPNTTS_NFC_INIT_ERR_PAL_I14443P4A 6 
#define RPNTTS_NFC_INIT_ERR_PAL_I14443P4 7
#define RPNTTS_NFC_INIT_ERR_PAL_I14443P3B 8 
#define RPNTTS_NFC_INIT_ERR_PAL_FELICA 9
#define RPNTTS_NFC_INIT_ERR_PAL_I18092MPI 10
#define RPNTTS_NFC_INIT_ERR_PAL_I18092MT 11
#define RPNTTS_NFC_INIT_ERR_PAL_MIFARE 12
#define RPNTTS_NFC_INIT_ERR_AL_FELICA 13
#define RPNTTS_NFC_INIT_ERR_AL_T1T 14
#define RPNTTS_NFC_INIT_ERR_AL_MFUL 15
#define RPNTTS_NFC_INIT_ERR_AL_MFDF 16
#define RPNTTS_NFC_INIT_ERR_AL_TAGOP 17
#define RPNTTS_NFC_INIT_ERR_AL_MFC 18
#define RPNTTS_NFC_INIT_ERR_CM_KEYSTORE 19 
#define RPNTTS_NFC_INIT_ERR_CM_KEYSTORE_FORMAT 20
#define RPNTTS_NFC_INIT_ERR_CM_KEYSTORE_SETKEY 21
#define RPNTTS_NFC_INIT_ERR_CM_OSAL 22
#define RPNTTS_NFC_INIT_ERR_CM_OSAL_TIMER 23
#define RPNTTS_NFC_INIT_ERR_CM_DISCLOOP 24
#define RPNTTS_NFC_INIT_ERR_LLCP_OVRHAL 25

#define RPNTTS_NFC_DETECTCARD_ERR_SOFTRESET 1
#define RPNTTS_NFC_DETECTCARD_ERR_APPLYPROTOCOL 2
#define RPNTTS_NFC_DETECTCARD_ERR_REQUESTA 3
#define RPNTTS_NFC_DETECTCARD_ERR_FIELDRESET 4
#define RPNTTS_NFC_DETECTCARD_ERR_ACTIVATECARD 5

#define RPNTTS_NFC_DETECTNDEF_NDEFPRESENT 1
#define RPNTTS_NFC_DETECTNDEF_ERR_RESETCONFIG 2
#define RPNTTS_NFC_DETECTNDEF_ERR_SETCONFIG 3
#define RPNTTS_NFC_DETECTNDEF_ERR_CHECKNDEF 4
#define RPNTTS_NFC_DETECTNDEF_ERR_READNDEF 5

#define RPNTTS_NFC_DISCLOOP_ERR_SETCONFIG 1
#define RPNTTS_NFC_DISCLOOP_ERR_DISCLOOPSTART 2
#define RPNTTS_NFC_DISCLOOP_ERR_GETCONFIG 3
#define RPNTTS_NFC_DISCLOOP_ERR_ACTIVATECARD 5 
#define RPNTTS_NFC_DISCLOOP_UNKNOWNTYPE 32
#define RPNTTS_NFC_DISCLOOP_NOTHING_FOUND 33

/* Keystore constants */
#define MFC_DEFAULT_KEYS 9
#define NUMBER_OF_KEYENTRIES MFC_DEFAULT_KEYS + 1 
#define NUMBER_OF_KEYVERSIONPAIRS 1
#define NUMBER_OF_KUCENTRIES 1

#define HALBUFSIZE 256
#define MAXUIDLEN 10

typedef struct {
    phbalReg_RpiSpi_DataParams_t balReader;
    phhalHw_Rc523_DataParams_t halReader;
    uint8_t halBufTx[HALBUFSIZE];
    uint8_t halBufRx[HALBUFSIZE];
    phpalI14443p3a_Sw_DataParams_t palI14443p3a;
    phpalI14443p3b_Sw_DataParams_t palI14443p3b;
    phpalI14443p4_Sw_DataParams_t palI14443p4;
    phpalI14443p4a_Sw_DataParams_t palI14443p4a;
    phpalFelica_Sw_DataParams_t palFelica;
    phpalI18092mPI_Sw_DataParams_t palI18092mPI;
    phpalI18092mT_Sw_DataParams_t palI18092mT;
    phpalMifare_Sw_DataParams_t palMifare;
    phalMful_Sw_DataParams_t alMful;
    phalMfc_Sw_DataParams_t alMfc;
    phalMfdf_Sw_DataParams_t alMfdf;
    phalFelica_Sw_DataParams_t alFelica;
    phalT1T_Sw_DataParams_t alT1T;
    phalTop_Sw_DataParams_t tagop;
    phalTop_T1T_t t1tparam;
    phalTop_T2T_t t2tparam;
    phalTop_T3T_t t3tparam;
    phalTop_T4T_t t4tparam;
    phOsal_GLib_DataParams_t osal;
    phacDiscLoop_Sw_DataParams_t discLoop;
    phKeyStore_Sw_DataParams_t SwkeyStore;
    GMainContext *pHalMainContext;
    GMainLoop *pHalMainLoop;
} nxprdlibParams;


uint16_t init_nxprdlib(nxprdlibParams *params);
uint16_t detect_card(nxprdlibParams *params, uint8_t *card_uid, uint8_t *card_uid_len);
uint16_t detect_ndef(nxprdlibParams *params, uint8_t tag_type);
uint16_t do_discovery_loop(nxprdlibParams *params);


#endif /* RPNTTS_NFC_H */
