#include <stdlib.h>
#include <stdio.h>

#include "rpntts-nfc.h"

uint8_t mfc_default_keys[][6] = {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5},
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5},
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd},
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a},
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff},
    {0x71, 0x4c, 0x5c, 0x88, 0x6e, 0x97},
    {0x58, 0x7e, 0xe5, 0xf9, 0x35, 0x0f},
    {0xa0, 0x47, 0x8c, 0xc3, 0x90, 0x91},
    {0x53, 0x3c, 0xb6, 0xc7, 0x23, 0xf6},
    {0x8f, 0xd0, 0xa4, 0xf2, 0x56, 0xe9}
};

uint8_t init_nxprdlib(nxprdlibParams *params, uint8_t *bHalBufferTx, uint8_t *bHalBufferRx) {

    phKeyStore_Sw_KeyEntry_t KeyEntries[NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KeyVersionPair_t KeyVersionPairs[NUMBER_OF_KEYVERSIONPAIRS * NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KUCEntry_t KUCEntries[NUMBER_OF_KUCENTRIES];

    phbalReg_RpiSpi_DataParams_t *pbalReader = &(params->balReader);
    phhalHw_Rc523_DataParams_t *phalReader = &(params->halReader);
    phpalI14443p3a_Sw_DataParams_t *ppalI14443p3a = &(params->palI14443p3a);
    phpalI14443p4_Sw_DataParams_t *ppalI14443p4 = &(params->palI14443p4);
    phpalMifare_Sw_DataParams_t *ppalMifare = &(params->palMifare);
    phalMful_Sw_DataParams_t *palMful = &(params->alMful);
    phalMfc_Sw_DataParams_t *palMfc = &(params->alMfc);
    phalT1T_Sw_DataParams_t *palT1T = &(params->alT1T);
    phalTop_Sw_DataParams_t *ptagop = &(params->tagop);
    phalTop_T1T_t *pt1tparam = &(params->t1tparam);
    phalTop_T2T_t *pt2tparam = &(params->t2tparam);
    phalTop_T3T_t *pt3tparam = &(params->t3tparam);
    phalTop_T4T_t *pt4tparam = &(params->t4tparam);
    phKeyStore_Sw_DataParams_t *pSwkeyStore = &(params->SwkeyStore);

    phStatus_t status;
    int i = 0;

    /* Init SPI BAL */
    status = phbalReg_RpiSpi_Init(pbalReader, sizeof(phbalReg_RpiSpi_DataParams_t));
    if (status != PH_ERR_SUCCESS) {
        return 1;
    }

    /* Open BAL */
    status = phbalReg_OpenPort(pbalReader);
    if (status != PH_ERR_SUCCESS) {
        return 2;
    }

    /* Init HAL */
    status = phhalHw_Rc523_Init(phalReader, sizeof(phhalHw_Rc523_DataParams_t), pbalReader, 0, bHalBufferTx, HALBUFSIZE, bHalBufferRx, HALBUFSIZE);
    if (status != PH_ERR_SUCCESS) {
        return 3;
    }

    /* Configure HAL for SPI */
    status = phhalHw_SetConfig(phalReader, PHHAL_HW_CONFIG_BAL_CONNECTION, PHHAL_HW_BAL_CONNECTION_SPI);
    if (status != PH_ERR_SUCCESS) {
        return 4;
    }

    /* Initialize the 14443-3A PAL (Protocol Abstraction Layer) component */
    status = phpalI14443p3a_Sw_Init(ppalI14443p3a, sizeof(phpalI14443p3a_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return 5;
    }

    /* Initialize the 14443-4 PAL component */
    status =  phpalI14443p4_Sw_Init(ppalI14443p4, sizeof(phpalI14443p4_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return 6;
    }

    /* Initialize the Mifare PAL component */
    status = phpalMifare_Sw_Init(ppalMifare, sizeof(phpalMifare_Sw_DataParams_t), phalReader, ppalI14443p3a);
    if (status != PH_ERR_SUCCESS) {
        return 7;
    }

    /* Initialize Ultralight(-C) AL component */
    status = phalMful_Sw_Init(palMful, sizeof(phalMful_Sw_DataParams_t), ppalMifare, NULL, NULL, NULL);
    if (status != PH_ERR_SUCCESS) {
        return 8;
    }

    /* Initialize the keystore component */
    status = phKeyStore_Sw_Init(pSwkeyStore, sizeof(phKeyStore_Sw_DataParams_t), KeyEntries, NUMBER_OF_KEYENTRIES, KeyVersionPairs, NUMBER_OF_KEYVERSIONPAIRS, KUCEntries, NUMBER_OF_KUCENTRIES);
    if (status != PH_ERR_SUCCESS) {
        return 9;
    }

    /* Format keystore and put key into it */
    for (i=0; i<MFC_DEFAULT_KEYS; i++) {
        status = phKeyStore_FormatKeyEntry(pSwkeyStore, i+1, PH_KEYSTORE_KEY_TYPE_MIFARE);
        if (status != PH_ERR_SUCCESS) {
            return 10;
        }
        status = phKeyStore_SetKey(pSwkeyStore, i+1, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, mfc_default_keys[i], 0);
        if (status != PH_ERR_SUCCESS) {
            return 11;
        }
    }

    /* Initialize Classic AL component */
    status = phalMfc_Sw_Init(palMfc, sizeof(phalMfc_Sw_DataParams_t), ppalMifare, pSwkeyStore);
    if (status != PH_ERR_SUCCESS) {
        return 12;
    }

    /* Initialize T1T AL component */
    status = phalT1T_Sw_Init(palT1T, sizeof(phalT1T_Sw_DataParams_t), ppalI14443p3a);
    if (status != PH_ERR_SUCCESS) {
        return 13;
    }

    /* Initialize Tag Operation AL component */
    status = phalTop_Sw_Init(ptagop, sizeof(phalTop_Sw_DataParams_t), pt1tparam, pt2tparam, pt3tparam, pt4tparam, NULL);
    if (status != PH_ERR_SUCCESS) {
        return 14;
    }

    ((phalTop_T1T_t *)(ptagop->pT1T))->phalT1TDataParams = palT1T;
    ((phalTop_T2T_t *)(ptagop->pT2T))->phalT2TDataParams = palMful;
    ((phalTop_T3T_t *)(ptagop->pT3T))->phalT3TDataParams = NULL;
    ((phalTop_T4T_t *)(ptagop->pT4T))->phalT4TDataParams = NULL;

    return 0;

}

uint8_t detect_card(nxprdlibParams *params, uint8_t *card_uid, uint8_t *card_uid_len) {

    phhalHw_Rc523_DataParams_t *phalReader = &(params->halReader);
    phpalI14443p3a_Sw_DataParams_t *ppalI14443p3a = &(params->palI14443p3a);
    uint8_t atqa[2] = { 0, 0 };
    uint8_t sak[1] = { 0 };
    uint8_t morecards;
    phStatus_t status;

    /* Init params */
    *card_uid_len = 0;
    memset(card_uid, '\0', MAXUIDLEN);

    memset(&status, '\0', sizeof(phStatus_t));

    /* Soft reset the IC */
    phhalHw_Rc523_Cmd_SoftReset(phalReader);
    if (status != PH_ERR_SUCCESS) {
        return 1;
    }

    /* Apply the type A protocol settings and activate the RF field. */
    status = phhalHw_ApplyProtocolSettings(phalReader, PHHAL_HW_CARDTYPE_ISO14443A);
    if (status != PH_ERR_SUCCESS) {
        return 2;
    }

    /* Perform Request A */
    status = phpalI14443p3a_RequestA(ppalI14443p3a, atqa);
    if (status != PH_ERR_SUCCESS) {
        if (status & PH_ERR_PROTOCOL_ERROR) {
            return 3;
        }
        else {
            /* No card in field */
            return 0;
        }

    }

    /* Reset the RF field  */
    status = phhalHw_FieldReset(phalReader);
    if (status != PH_ERR_SUCCESS) {
        return 4;
    }

    /* Activate the communication layer part 3 of the ISO 14443A standard. */
    status = phpalI14443p3a_ActivateCard(ppalI14443p3a, NULL, 0x00, card_uid, card_uid_len, sak, &morecards);
    if (status != PH_ERR_SUCCESS) {
        (void) phpalI14443p3a_HaltA(ppalI14443p3a);
        return 5;
    }

    (void) phpalI14443p3a_HaltA(ppalI14443p3a);

    return 0;
}

int32_t detect_ndef(nxprdlibParams *params) {
    phalTop_Sw_DataParams_t *ptagop = &(params->tagop);
    phStatus_t status;
    uint8_t ndef_presence = 0;
    uint8_t retval = 0;

    status = phalTop_SetConfig(ptagop, PHAL_TOP_CONFIG_TAG_TYPE, PHAL_TOP_TAG_TYPE_T1T_TAG);
    if (status != PH_ERR_SUCCESS) {
        return -1;
    }
    status = phalTop_CheckNdef(ptagop, &ndef_presence);
    if (status != PH_ERR_SUCCESS) {
        return status;
    }

    retval |= ndef_presence;

    status = phalTop_SetConfig(ptagop, PHAL_TOP_CONFIG_TAG_TYPE, PHAL_TOP_TAG_TYPE_T2T_TAG);
    if (status != PH_ERR_SUCCESS) {
        return -3;
    }
    status = phalTop_CheckNdef(ptagop, &ndef_presence);
    if (status != PH_ERR_SUCCESS) {
        return -4;
    }

    retval |= (ndef_presence << 1);

    return retval;

}
