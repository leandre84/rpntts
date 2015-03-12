#include <stdlib.h>
#include <stdio.h>

#include "rpntts-nfc.h"

uint8_t MfcDefaultKeys[][6] = {
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

uint8_t initNxprdlib(nxprdlibparams *params, uint8_t *bHalBufferTx, uint8_t *bHalBufferRx) {

    phKeyStore_Sw_KeyEntry_t pKeyEntries[NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KeyVersionPair_t pKeyVersionPairs[NUMBER_OF_KEYVERSIONPAIRS * NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KUCEntry_t pKUCEntries[NUMBER_OF_KUCENTRIES];

    phbalReg_RpiSpi_DataParams_t *balReader = &(params->balReader);
    phhalHw_Rc523_DataParams_t *halReader = &(params->halReader);
    phpalI14443p3a_Sw_DataParams_t *palI14443p3a = &(params->palI14443p3a);
    phpalI14443p4_Sw_DataParams_t *palI14443p4 = &(params->palI14443p4);
    phpalMifare_Sw_DataParams_t *palMifare = &(params->palMifare);
    phalMful_Sw_DataParams_t *alMful = &(params->alMful);
    phalMfc_Sw_DataParams_t *alMfc = &(params->alMfc);
    phKeyStore_Sw_DataParams_t *SwkeyStore = &(params->SwkeyStore);

    phStatus_t status;
    int i = 0;

    /* Init SPI BAL */
    status = phbalReg_RpiSpi_Init(balReader, sizeof(phbalReg_RpiSpi_DataParams_t));
    if (status != PH_ERR_SUCCESS) {
        return 1;
    }

    /* Open BAL */
    status = phbalReg_OpenPort(balReader);
    if (status != PH_ERR_SUCCESS) {
        return 2;
    }

    /* Init HAL */
    status = phhalHw_Rc523_Init(halReader, sizeof(phhalHw_Rc523_DataParams_t), balReader, 0, bHalBufferTx, HALBUFSIZE, bHalBufferRx, HALBUFSIZE);
    if (status != PH_ERR_SUCCESS) {
        return 3;
    }

    /* Configure HAL for SPI */
    status = phhalHw_SetConfig(halReader, PHHAL_HW_CONFIG_BAL_CONNECTION, PHHAL_HW_BAL_CONNECTION_SPI);
    if (status != PH_ERR_SUCCESS) {
        return 4;
    }

    /* Initialize the 14443-3A PAL (Protocol Abstraction Layer) component */
    status = phpalI14443p3a_Sw_Init(palI14443p3a, sizeof(phpalI14443p3a_Sw_DataParams_t), halReader);
    if (status != PH_ERR_SUCCESS) {
        return 5;
    }

    /* Initialize the 14443-4 PAL component */
    status =  phpalI14443p4_Sw_Init(palI14443p4, sizeof(phpalI14443p4_Sw_DataParams_t), halReader);
    if (status != PH_ERR_SUCCESS) {
        return 6;
    }

    /* Initialize the Mifare PAL component */
    status = phpalMifare_Sw_Init(palMifare, sizeof(phpalMifare_Sw_DataParams_t), halReader, palI14443p3a);
    if (status != PH_ERR_SUCCESS) {
        return 7;
    }

    /* Initialize Ultralight(-C) AL component */
    status = phalMful_Sw_Init(alMful, sizeof(phalMful_Sw_DataParams_t), palMifare, NULL, NULL, NULL);
    if (status != PH_ERR_SUCCESS) {
        return 8;
    }

    /* Initialize the keystore component */
    status = phKeyStore_Sw_Init(SwkeyStore, sizeof(phKeyStore_Sw_DataParams_t), &pKeyEntries[0], NUMBER_OF_KEYENTRIES, &pKeyVersionPairs[0], NUMBER_OF_KEYVERSIONPAIRS, &pKUCEntries[0], NUMBER_OF_KUCENTRIES);
    if (status != PH_ERR_SUCCESS) {
        return 9;
    }

    /* Format keystore and put key into it */
    for (i=0; i<MFC_DEFAULT_KEYS; i++) {
        status = phKeyStore_FormatKeyEntry(SwkeyStore, i+1, PH_KEYSTORE_KEY_TYPE_MIFARE);
        if (status != PH_ERR_SUCCESS) {
            return 10;
        }
        status = phKeyStore_SetKey(SwkeyStore, i+1, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, MfcDefaultKeys[i], 0);
        if (status != PH_ERR_SUCCESS) {
            return 11;
        }
    }

    /* Initialize Classic AL component */
    status = phalMfc_Sw_Init(alMfc, sizeof(phalMfc_Sw_DataParams_t), palMifare, SwkeyStore);
    if (status != PH_ERR_SUCCESS) {
        return 12;
    } 

    return 0;

}

uint8_t detectCard(nxprdlibparams *params, uint8_t *cardUID, uint8_t *cardUIDlen) {

    phhalHw_Rc523_DataParams_t *phalReader = &(params->halReader);
    phpalI14443p3a_Sw_DataParams_t *ppalI14443p3a = &(params->palI14443p3a);
    uint8_t atqa[2] = { 0, 0 };
    uint8_t sak[1] = { 0 };
    uint8_t morecards;
    phStatus_t status;

    /* Init params */
    *cardUIDlen = 0;
    memset(cardUID, '\0', MAXUIDLEN);

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
    status = phpalI14443p3a_ActivateCard(ppalI14443p3a, NULL, 0x00, cardUID, cardUIDlen, sak, &morecards);
    if (status != PH_ERR_SUCCESS) {
        (void) phpalI14443p3a_HaltA(ppalI14443p3a);
        return 5;
    }

    (void) phpalI14443p3a_HaltA(ppalI14443p3a);

    return 0;
}
