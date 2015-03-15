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

#include <glib.h>

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
    phpalI14443p3a_Sw_DataParams_t palI14443p3a;
    phpalI14443p3b_Sw_DataParams_t palI14443p3b;
    phpalI14443p4a_Sw_DataParams_t palI14443p4a;
    phpalI14443p4_Sw_DataParams_t palI14443p4;
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


uint8_t init_nxprdlib(nxprdlibParams *params, uint8_t *bHalBufferTx, uint8_t *bHalBufferRx);
uint8_t detect_card(nxprdlibParams *params, uint8_t *card_uid, uint8_t *card_uid_len);
int32_t detect_ndef(nxprdlibParams *params);
int32_t do_discovery_loop(nxprdlibParams *params);


#endif /* RPNTTS_NFC_H */
