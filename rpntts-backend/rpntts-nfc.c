#include <stdlib.h>
#include <stdio.h>

#include "rpntts-common.h"
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

void rdlib_set_interrupt_cb(uint8_t en) {
    en=en;
}

uint8_t init_nxprdlib(nxprdlibParams *params) {

    phKeyStore_Sw_KeyEntry_t KeyEntries[NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KeyVersionPair_t KeyVersionPairs[NUMBER_OF_KEYVERSIONPAIRS * NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KUCEntry_t KUCEntries[NUMBER_OF_KUCENTRIES];

    phbalReg_RpiSpi_DataParams_t *pbalReader = &(params->balReader);
    phhalHw_Rc523_DataParams_t *phalReader = &(params->halReader);
    uint8_t *halBufTx = params->halBufTx;
    uint8_t *halBufRx = params->halBufRx;
    phpalI14443p3a_Sw_DataParams_t *ppalI14443p3a = &(params->palI14443p3a);
    phpalI14443p3b_Sw_DataParams_t *ppalI14443p3b = &(params->palI14443p3b);
    phpalI14443p4_Sw_DataParams_t *ppalI14443p4 = &(params->palI14443p4);
    phpalI14443p4a_Sw_DataParams_t *ppalI14443p4a = &(params->palI14443p4a);
    phpalFelica_Sw_DataParams_t *ppalFelica = &(params->palFelica);
    phpalI18092mPI_Sw_DataParams_t *ppalI18092mPI = &(params->palI18092mPI);
    phpalI18092mT_Sw_DataParams_t *ppalI18092mT = &(params->palI18092mT);
    phpalMifare_Sw_DataParams_t *ppalMifare = &(params->palMifare);
    phalMful_Sw_DataParams_t *palMful = &(params->alMful);
    phalMfc_Sw_DataParams_t *palMfc = &(params->alMfc);
    phalMfdf_Sw_DataParams_t *palMfdf = &(params->alMfdf);
    phalFelica_Sw_DataParams_t *palFelica = &(params->alFelica);
    phalT1T_Sw_DataParams_t *palT1T = &(params->alT1T);
    phalTop_Sw_DataParams_t *ptagop = &(params->tagop);
    phalTop_T1T_t *pt1tparam = &(params->t1tparam);
    phalTop_T2T_t *pt2tparam = &(params->t2tparam);
    phalTop_T3T_t *pt3tparam = &(params->t3tparam);
    phalTop_T4T_t *pt4tparam = &(params->t4tparam);
    phacDiscLoop_Sw_DataParams_t *pdiscLoop = &(params->discLoop);
    phKeyStore_Sw_DataParams_t *pSwkeyStore = &(params->SwkeyStore);
    phOsal_GLib_DataParams_t *posal = &(params->osal);
    GMainContext *pHalMainContext = params->pHalMainContext;

    phStatus_t status;
    int i = 0;

    memset(params, '\0', sizeof(nxprdlibParams));

    phOsal_GLib_Init_GLib(posal, pHalMainContext);

    /* Init Raspberry Pi SPI BAL */
    status = phbalReg_RpiSpi_Init(pbalReader, sizeof(phbalReg_RpiSpi_DataParams_t));
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_BAL;
    }

    /* Initialize OSAL component */
    status = phOsal_Init(posal);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_CM_OSAL;
    }

    /* Init HAL */
    status = phhalHw_Rc523_Init(phalReader, sizeof(phhalHw_Rc523_DataParams_t), pbalReader, 0, halBufTx, HALBUFSIZE, halBufRx, HALBUFSIZE);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_HAL;
    }

    /* Configure HAL for SPI */
    status = phhalHw_SetConfig(phalReader, PHHAL_HW_CONFIG_BAL_CONNECTION, PHHAL_HW_BAL_CONNECTION_SPI);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_HAL_SPICONFIG;
    }

    /* Open BAL */
    status = phbalReg_OpenPort(pbalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_BAL_OPEN;
    }

    /* Initialize the 14443-3A PAL component */
    status = phpalI14443p3a_Sw_Init(ppalI14443p3a, sizeof(phpalI14443p3a_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_I14443P3A;
    }

    /* Initialize the 14443-4A PAL component */
    status =  phpalI14443p4a_Sw_Init(ppalI14443p4a, sizeof(phpalI14443p4a_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_I14443P4A;
    }

    /* Initialize the 14443-4 PAL component */
    status =  phpalI14443p4_Sw_Init(ppalI14443p4, sizeof(phpalI14443p4_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_I14443P4;
    }

    /* Initialize the 14443-3B PAL component */
    status = phpalI14443p3b_Sw_Init(ppalI14443p3b, sizeof(phpalI14443p3b_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_I14443P3B;
    }

    /* Initialize the Felica PAL component */
    status =  phpalFelica_Sw_Init(ppalFelica, sizeof(phpalFelica_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_FELICA;
    }

    /* Init I18092I PAL component */
    status = phpalI18092mPI_Sw_Init(ppalI18092mPI, sizeof(phpalI18092mPI_Sw_DataParams_t), phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_I18092MPI;
    }

    /* Init I18092T PAL component */
    status = phpalI18092mT_Sw_Init(ppalI18092mT, sizeof(phpalI18092mT_Sw_DataParams_t), phalReader, rdlib_set_interrupt_cb);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_I18092MT;
    }

    /* Initialize the OvrHal component (???)
    phlnLlcp_Fri_OvrHal_Init();
    */

    /* Initialize the Mifare PAL component */
    status = phpalMifare_Sw_Init(ppalMifare, sizeof(phpalMifare_Sw_DataParams_t), phalReader, ppalI14443p4);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_PAL_MIFARE;
    }

    /* Initialize Felica AL component */
    status = phalFelica_Sw_Init(palFelica, sizeof(phalFelica_Sw_DataParams_t), ppalFelica);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_AL_FELICA;
    }

    /* Initialize T1T AL component */
    status = phalT1T_Sw_Init(palT1T, sizeof(phalT1T_Sw_DataParams_t), ppalI14443p3a);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_AL_T1T;
    }

    /* Initialize Mifare Ultralight AL component */
    status = phalMful_Sw_Init(palMful, sizeof(phalMful_Sw_DataParams_t), ppalMifare, NULL, NULL, NULL);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_AL_MFUL;
    }

    /* Initialize Mifare Desfire AL component */
    status = phalMfdf_Sw_Init(palMfdf, sizeof(phalMfdf_Sw_DataParams_t), ppalMifare, NULL, NULL, NULL, phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_AL_MFDF;
    }

    /* Initialize Tag Operation AL component */
    status = phalTop_Sw_Init(ptagop, sizeof(phalTop_Sw_DataParams_t), pt1tparam, pt2tparam, pt3tparam, pt4tparam, NULL);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_AL_TAGOP;
    }

    /* Initialize the keystore component */
    status = phKeyStore_Sw_Init(pSwkeyStore, sizeof(phKeyStore_Sw_DataParams_t), KeyEntries, NUMBER_OF_KEYENTRIES, KeyVersionPairs, NUMBER_OF_KEYVERSIONPAIRS, KUCEntries, NUMBER_OF_KUCENTRIES);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_CM_KEYSTORE;
    }

    /* Format keystore and put key into it */
    for (i=0; i<MFC_DEFAULT_KEYS; i++) {
        status = phKeyStore_FormatKeyEntry(pSwkeyStore, i+1, PH_KEYSTORE_KEY_TYPE_MIFARE);
        if (status != PH_ERR_SUCCESS) {
            return RPNTTS_NFC_INIT_ERR_CM_KEYSTORE_FORMAT;
        }
        status = phKeyStore_SetKey(pSwkeyStore, i+1, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, mfc_default_keys[i], 0);
        if (status != PH_ERR_SUCCESS) {
            return RPNTTS_NFC_INIT_ERR_CM_KEYSTORE_SETKEY;
        }
    }

    /* Initialize Mifare Classic AL component */
    status = phalMfc_Sw_Init(palMfc, sizeof(phalMfc_Sw_DataParams_t), ppalMifare, pSwkeyStore);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_AL_MFC;
    }

    ((phalTop_T1T_t *)(ptagop->pT1T))->phalT1TDataParams = palT1T;
    ((phalTop_T2T_t *)(ptagop->pT2T))->phalT2TDataParams = palMful;
    ((phalTop_T3T_t *)(ptagop->pT3T))->phalT3TDataParams = palFelica;
    ((phalTop_T4T_t *)(ptagop->pT4T))->phalT4TDataParams = palMfdf;

    /* Initialize the OSAL timer component */
    status = phOsal_Timer_Init(posal);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_CM_OSAL_TIMER;
    }

    /* Initialize Discoveryloop component */
    status = phacDiscLoop_Sw_Init(pdiscLoop, sizeof(phacDiscLoop_Sw_DataParams_t), phalReader, posal);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_INIT_ERR_CM_DISCLOOP;
    }

    pdiscLoop->pPal1443p3aDataParams = ppalI14443p3a;
    pdiscLoop->pPal1443p3bDataParams = ppalI14443p3b;
    pdiscLoop->pPal1443p4aDataParams = ppalI14443p4a;
    pdiscLoop->pPal18092mPIDataParams = ppalI18092mPI;
    pdiscLoop->pPal18092mTDataParams = ppalI18092mT;
    pdiscLoop->pPalFelicaDataParams = ppalFelica;
    pdiscLoop->pHalDataParams = phalReader; 
    pdiscLoop->pOsal = posal;

    phhalHw_FieldReset(phalReader);

    return 0;

}

uint8_t detect_card(nxprdlibParams *params, uint8_t *card_uid, uint8_t *card_uid_len) {

    phhalHw_Rc523_DataParams_t *phalReader = &(params->halReader);
    phpalI14443p3a_Sw_DataParams_t *ppalI14443p3a = &(params->palI14443p3a);
    uint8_t atqa[2] = { 0, 0 };
    uint8_t sak[1] = { 0 };
    uint8_t morecards = 0;
    phStatus_t status;

    /* Init params */
    *card_uid_len = 0;
    memset(card_uid, '\0', MAXUIDLEN);

    memset(&status, '\0', sizeof(phStatus_t));

    /* Soft reset the IC */
    phhalHw_Rc523_Cmd_SoftReset(phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_DETECTCARD_ERR_SOFTRESET;
    }

    /* Apply the type A protocol settings and activate the RF field. */
    status = phhalHw_ApplyProtocolSettings(phalReader, PHHAL_HW_CARDTYPE_ISO14443A);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_DETECTCARD_ERR_APPLYPROTOCOL;
    }

    /* Perform Request A */
    status = phpalI14443p3a_RequestA(ppalI14443p3a, atqa);
    if (status != PH_ERR_SUCCESS) {
        if (status & PH_ERR_PROTOCOL_ERROR) {
            return RPNTTS_NFC_DETECTCARD_ERR_REQUESTA;
        }
        else {
            /* No card in field */
            return 0;
        }

    }

    /* Reset the RF field  */
    status = phhalHw_FieldReset(phalReader);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_DETECTCARD_ERR_FIELDRESET;
    }

    /* Activate the communication layer part 3 of the ISO 14443A standard. */
    status = phpalI14443p3a_ActivateCard(ppalI14443p3a, NULL, 0x00, card_uid, card_uid_len, sak, &morecards);
    if (status != PH_ERR_SUCCESS) {
        (void) phpalI14443p3a_HaltA(ppalI14443p3a);
        return RPNTTS_NFC_DETECTCARD_ERR_ACTIVATECARD;
    }

    (void) phpalI14443p3a_HaltA(ppalI14443p3a);

    return 0;
}

uint8_t detect_ndef(nxprdlibParams *params, uint8_t tag_type) {
    phalTop_Sw_DataParams_t *ptagop = &(params->tagop);
    phStatus_t status;
    uint8_t ndef_presence = 0;
    uint8_t ndef_record[1024] = { 0 };
    uint16_t ndef_record_length = 0;
    char ndef_text[(1024*2)+1] = { 0 };

    status = phalTop_Reset(ptagop);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_DETECTNDEF_ERR_RESETCONFIG;
    }

    status = phalTop_SetConfig(ptagop, PHAL_TOP_CONFIG_TAG_TYPE, tag_type);
    if (status != PH_ERR_SUCCESS) {
        return RPNTTS_NFC_DETECTNDEF_ERR_SETCONFIG;
    }

    status = phalTop_CheckNdef(ptagop, &ndef_presence);
    if (status != PH_ERR_SUCCESS) {
       return RPNTTS_NFC_DETECTNDEF_ERR_CHECKNDEF; 
    }

    if (ndef_presence) {
        status = phalTop_ReadNdef(ptagop, ndef_record, &ndef_record_length);
        if (status != PH_ERR_SUCCESS) {
            return RPNTTS_NFC_DETECTNDEF_ERR_READNDEF;
        }
        else {
            if (options.verbose) {
                fprintf(stderr, "Got NDEF record with length: %d\n", ndef_record_length);
                bin_to_hex(ndef_record, ndef_record_length, ndef_text);
                fprintf(stdout, "NDEF Record: %s\n", ndef_text);
            }
            return RPNTTS_NFC_DETECTNDEF_NDEFPRESENT;
        }
    }

    return 0;

}

uint8_t do_discovery_loop(nxprdlibParams *params) {
    phhalHw_Rc523_DataParams_t *phalReader = &(params->halReader);
    phpalI14443p4_Sw_DataParams_t *ppalI14443p4 = &(params->palI14443p4);
    phpalI18092mPI_Sw_DataParams_t *ppalI18092mPI = &(params->palI18092mPI);
    phpalI18092mT_Sw_DataParams_t *ppalI18092mT = &(params->palI18092mT);
    phacDiscLoop_Sw_DataParams_t *pdiscLoop = &(params->discLoop);
    uint16_t config_value = 0;
    phStatus_t status;

    status = phacDiscLoop_SetConfig(pdiscLoop, PHAC_DISCLOOP_CONFIG_MODE, PHAC_DISCLOOP_SET_POLL_MODE | PHAC_DISCLOOP_SET_PAUSE_MODE);
    if (status != PH_ERR_SUCCESS) { return RPNTTS_NFC_DISCLOOP_ERR_SETCONFIG; };
    status = phacDiscLoop_SetConfig(pdiscLoop, PHAC_DISCLOOP_CONFIG_DETECT_TAGS, PHAC_DISCLOOP_CON_POLL_A);
    if (status != PH_ERR_SUCCESS) { return RPNTTS_NFC_DISCLOOP_ERR_SETCONFIG; };
    status = phacDiscLoop_SetConfig(pdiscLoop, PHAC_DISCLOOP_CONFIG_PAUSE_PERIOD_MS, 500);
    if (status != PH_ERR_SUCCESS) { return RPNTTS_NFC_DISCLOOP_ERR_SETCONFIG; };
    status = phacDiscLoop_SetConfig(pdiscLoop, PHAC_DISCLOOP_CONFIG_NUM_POLL_LOOPS, 1);
    if (status != PH_ERR_SUCCESS) { return RPNTTS_NFC_DISCLOOP_ERR_SETCONFIG; };
    status = phacDiscLoop_SetConfig(pdiscLoop, PHAC_DISCLOOP_CONFIG_TYPEA_DEVICE_LIMIT, 3);
    if (status != PH_ERR_SUCCESS) { return RPNTTS_NFC_DISCLOOP_ERR_SETCONFIG; };

    phpalI14443p4_ResetProtocol(ppalI14443p4);
    phpalI18092mPI_ResetProtocol(ppalI18092mPI);
    phpalI18092mT_ResetProtocol(ppalI18092mT);

    phhalHw_FieldOff(phalReader);
    status = phacDiscLoop_Start(pdiscLoop);

    if (status == PH_ERR_SUCCESS) {
        status = phacDiscLoop_GetConfig(pdiscLoop, PHAC_DISCLOOP_CONFIG_TAGS_DETECTED, &config_value);
        if (status == PH_ERR_SUCCESS) {
            if (PHAC_DISCLOOP_CHECK_ANDMASK(config_value, PHAC_DISCLOOP_TYPEA_DETECTED)) {
                phhalHw_FieldReset(phalReader);
                status = phacDiscLoop_Sw_ActivateCard(pdiscLoop, PHAC_DISCLOOP_TYPEA_ACTIVATE, 0);
                if (status != PH_ERR_SUCCESS) {
                    phhalHw_FieldReset(phalReader);
                    return RPNTTS_NFC_DISCLOOP_ERR_ACTIVATECARD;
                }
                if (PHAC_DISCLOOP_CHECK_ANDMASK(config_value, PHAC_DISCLOOP_TYPEA_DETECTED_TAG_TYPE1)) {
                    if (options.verbose) {
                        fprintf(stderr, "detected type 1 tag\n");
                    }
                }
                else if (PHAC_DISCLOOP_CHECK_ANDMASK(config_value, PHAC_DISCLOOP_TYPEA_DETECTED_TAG_TYPE2)) {
                    if (options.verbose) {
                        fprintf(stderr, "detected type 2 tag\n");
                        fprintf(stderr, "Detect ndef: %d\n", detect_ndef(params, PHAL_TOP_TAG_TYPE_T2T_TAG));
                    }
                }
                else if (PHAC_DISCLOOP_CHECK_ANDMASK(config_value, PHAC_DISCLOOP_TYPEA_DETECTED_TAG_TYPE4A)) {
                    if (options.verbose) {
                        fprintf(stderr, "detected type 4a tag\n");
                        fprintf(stderr, "Detect ndef: %d\n", detect_ndef(params, PHAL_TOP_TAG_TYPE_T4T_TAG));
                    }
                }
                else {
                    return RPNTTS_NFC_DISCLOOP_UNKNOWNTYPE;
                }
            }
            else {
                return RPNTTS_NFC_DISCLOOP_NOTHING_FOUND;
            }
        }
        else {
            return RPNTTS_NFC_DISCLOOP_ERR_GETCONFIG;
        }
    }
    else {
        return RPNTTS_NFC_DISCLOOP_ERR_DISCLOOPSTART;
    }

    return 0;
}
