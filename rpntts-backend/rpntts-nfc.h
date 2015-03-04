#ifndef RPNTTS_NFC_H
#define RPNTTS_NFC_H

#include <ph_NxpBuild.h>
#include <ph_Status.h>
#include <phbalReg_RpiSpi.h>
#include <phpalI14443p3a.h>
#include <phpalI14443p4.h>
#include <phpalI14443p4a.h>
#include <phalMful.h>
#include <phalMfc.h>
#include <phKeyStore.h>

/* Keystore constants */
#define MFC_DEFAULT_KEYS 9
#define NUMBER_OF_KEYENTRIES MFC_DEFAULT_KEYS + 1 
#define NUMBER_OF_KEYVERSIONPAIRS 1
#define NUMBER_OF_KUCENTRIES 1

#define HALBUFSIZE 256

typedef struct {
    phbalReg_RpiSpi_DataParams_t balReader;
    phhalHw_Rc523_DataParams_t halReader;
    phpalI14443p3a_Sw_DataParams_t palI14443p3a;
    phpalI14443p4_Sw_DataParams_t palI14443p4;
    phpalMifare_Sw_DataParams_t palMifare;
    phalMful_Sw_DataParams_t alMful;
    phalMfc_Sw_DataParams_t alMfc;
    phKeyStore_Sw_DataParams_t SwkeyStore;
} nxprdlibparams;


uint8_t initNxprdlib(nxprdlibparams *params, uint8_t *bHalBufferTx, uint8_t *bHalBufferRx);


#endif /* RPNTTS_NFC_H */
