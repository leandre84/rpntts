#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <ph_NxpBuild.h>
#include <ph_Status.h>
#include <phbalReg_RpiSpi.h>
#include <phpalI14443p3a.h>
#include <phpalI14443p4.h>
#include <phpalI14443p4a.h>
#include <phalMful.h>
#include <phalMfc.h>
#include <phKeyStore.h>

#define sak_ul                0x00
#define sak_ulc               0x00
#define sak_mini              0x09
#define sak_mfc_1k            0x08
#define sak_mfc_4k            0x18
#define sak_mfp_2k_sl1        0x08
#define sak_mfp_4k_sl1        0x18
#define sak_mfp_2k_sl2        0x10
#define sak_mfp_4k_sl2        0x11
#define sak_mfp_2k_sl3        0x20
#define sak_mfp_4k_sl3        0x20
#define sak_desfire           0x20
#define sak_jcop              0x28
#define sak_layer4            0x20

#define atqa_ul               0x4400
#define atqa_ulc              0x4400
#define atqa_mfc              0x0200
#define atqa_mfp_s            0x0400
#define atqa_mfp_s_2K         0x4400
#define atqa_mfp_x            0x4200
#define atqa_desfire          0x4403
#define atqa_jcop             0x0400
#define atqa_mini             0x0400
#define atqa_nPA              0x0800

#define mifare_ultralight     0x01
#define mifare_ultralight_c   0x02
#define mifare_classic        0x03
#define mifare_classic_1k     0x04
#define mifare_classic_4k     0x05
#define mifare_plus           0x06
#define mifare_plus_2k_sl1    0x07
#define mifare_plus_4k_sl1    0x08
#define mifare_plus_2k_sl2    0x09
#define mifare_plus_4k_sl2    0x0A
#define mifare_plus_2k_sl3    0x0B
#define mifare_plus_4k_sl3    0x0C
#define mifare_desfire        0x0D
#define jcop                  0x0F
#define mifare_mini           0x10
#define nPA                   0x11

/* Keystore constants */
#define NUMBER_OF_KEYENTRIES 2
#define NUMBER_OF_KEYVERSIONPAIRS 1
#define NUMBER_OF_KUCENTRIES 1


uint32_t DetectMifare(void *halReader);
phStatus_t readerIC_Cmd_SoftReset(void *halReader);

int main(int argc, char **argv) {

    phbalReg_RpiSpi_DataParams_t balReader;
    phhalHw_Rc523_DataParams_t halReader;
    phStatus_t status;
    uint8_t bHalBufferTx[0x40];
    uint8_t bHalBufferRx[0x40];

    argc = argc;
    argv = argv;

    /* Init SPI BAL */
    status = phbalReg_RpiSpi_Init(&balReader, sizeof(phbalReg_RpiSpi_DataParams_t));
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing BAL!\n");
        return 1;
    }

    /* Open BAL */
    status = phbalReg_OpenPort(&balReader);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error opening BAL!\n");
        return 2;
    }

    /* Init HAL */
    status = phhalHw_Rc523_Init(&halReader, sizeof(phhalHw_Rc523_DataParams_t), &balReader, 0, bHalBufferTx, sizeof(bHalBufferTx), bHalBufferRx, sizeof(bHalBufferRx));
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing HAL!\n");
        return 3;
    }
    
    /* Configure HAL for SPI */
    status = phhalHw_SetConfig(&halReader, PHHAL_HW_CONFIG_BAL_CONNECTION, PHHAL_HW_BAL_CONNECTION_SPI);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error configuring HAL for SPI!\n");
        return 4;
    }

    /* Let's do something :) */
    while(1) {
        if (! DetectMifare(&halReader)) {
            fprintf(stdout, "Nothing found!\n");
        }
        sleep(1);
    }

    return 0;

}

uint32_t DetectMifare(void *halReader) {

    phpalI14443p3a_Sw_DataParams_t I14443p3a;
    phpalI14443p4_Sw_DataParams_t I14443p4;
    phpalMifare_Sw_DataParams_t palMifare;

    phalMful_Sw_DataParams_t alMful;
    phalMfc_Sw_DataParams_t alMfc;

    phKeyStore_Sw_DataParams_t SwkeyStore;

    phKeyStore_Sw_KeyEntry_t pKeyEntries[NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KeyVersionPair_t pKeyVersionPairs[NUMBER_OF_KEYVERSIONPAIRS * NUMBER_OF_KEYENTRIES];
    phKeyStore_Sw_KUCEntry_t pKUCEntries[NUMBER_OF_KUCENTRIES];
    uint8_t Key[PH_KEYSTORE_KEY_TYPE_MIFARE_SIZE];

    uint8_t bUid[10];
    uint8_t bLength;
    uint8_t bMoreCardsAvailable;
    uint32_t sak_atqa = 0;
    uint8_t pAtqa[2];
    uint8_t bSak[1];
    phStatus_t status;
    uint16_t detected_card = 0xFFFF;
    uint8_t i;
    uint8_t j;
    uint8_t pBlockData[16];
    phStatus_t readstatus;

    /* Initialize the 14443-3A PAL (Protocol Abstraction Layer) component */
    PH_CHECK_SUCCESS_FCT(status, phpalI14443p3a_Sw_Init(&I14443p3a,
            sizeof(phpalI14443p3a_Sw_DataParams_t), halReader));

    /* Initialize the 14443-4 PAL component */
    PH_CHECK_SUCCESS_FCT(status, phpalI14443p4_Sw_Init(&I14443p4,
            sizeof(phpalI14443p4_Sw_DataParams_t), halReader));

    /* Initialize the Mifare PAL component */
    PH_CHECK_SUCCESS_FCT(status, phpalMifare_Sw_Init(&palMifare,
            sizeof(phpalMifare_Sw_DataParams_t), halReader, &I14443p3a));

    /* Initialize Ultralight(-C) AL component */
    PH_CHECK_SUCCESS_FCT(status, phalMful_Sw_Init(&alMful,
            sizeof(phalMful_Sw_DataParams_t), &palMifare, NULL, NULL, NULL));

    /* Initialize Classic AL component */
    PH_CHECK_SUCCESS_FCT(status, phalMfc_Sw_Init(&alMfc,
            sizeof(phalMfc_Sw_DataParams_t), &palMifare, NULL));

    /* Initialize the keystore component */
    PH_CHECK_SUCCESS_FCT(status, phKeyStore_Sw_Init(&SwkeyStore,
        sizeof(phKeyStore_Sw_DataParams_t),
        &pKeyEntries[0],NUMBER_OF_KEYENTRIES,&pKeyVersionPairs[0],
        NUMBER_OF_KEYVERSIONPAIRS, &pKUCEntries[0], NUMBER_OF_KUCENTRIES));

    /* Format the keystore */
    PH_CHECK_SUCCESS_FCT(status, phKeyStore_FormatKeyEntry(&SwkeyStore, 1, PH_KEYSTORE_KEY_TYPE_MIFARE));

    /* Put key into keystore */
        memcpy(Key, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", PH_KEYSTORE_KEY_TYPE_MIFARE_SIZE);
    PH_CHECK_SUCCESS_FCT(status, phKeyStore_SetKey(&SwkeyStore, 1, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, Key, 0));

    /* Soft reset the IC */
    phhalHw_Rc523_Cmd_SoftReset(halReader);

    /* Reset the Rf field */
    phhalHw_FieldReset(halReader);

    /* Reset the RF field */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldReset(halReader));

    /* Apply the type A protocol settings
     * and activate the RF field. */
    PH_CHECK_SUCCESS_FCT(status,
            phhalHw_ApplyProtocolSettings(halReader, PHHAL_HW_CARDTYPE_ISO14443A));

    /* Empty the pAtqa */
    memset(pAtqa, '\0', 2);
    status = phpalI14443p3a_RequestA(&I14443p3a, pAtqa);

    /* Reset the RF field */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldReset(halReader));

    /* Empty the bSak */
    memset(bSak, '\0', 1);

    /* Activate the communication layer part 3
     * of the ISO 14443A standard. */
    status = phpalI14443p3a_ActivateCard(&I14443p3a,
        NULL, 0x00, bUid, &bLength, bSak, &bMoreCardsAvailable);

    sak_atqa = bSak[0] << 24 | pAtqa[0] << 8 | pAtqa[1];
    sak_atqa &= 0xFFFF0FFF;

    if (!status) {
        /* Detect mini or classic */
        switch (sak_atqa) {
            case sak_mfc_1k << 24 | atqa_mfc:
                printf("MIFARE Classic detected\n");
                detected_card &= mifare_classic;
                break;
            case sak_mfc_4k << 24 | atqa_mfc:
                printf("MIFARE Classic detected\n");
                detected_card &= mifare_classic;
                break;
            case sak_mfp_2k_sl1 << 24 | atqa_mfp_s:
                printf("MIFARE Classic detected\n");
                detected_card &= mifare_classic;
                break;
            case sak_mini << 24 | atqa_mini:
                printf("MIFARE Mini detected\n");
                detected_card &= mifare_mini;
                break;
            case sak_mfp_4k_sl1 << 24 | atqa_mfp_s:
                printf("MIFARE Classic detected\n");
                detected_card &= mifare_classic;
                break;
            case sak_mfp_2k_sl1 << 24 | atqa_mfp_x:
                printf("MIFARE Classic detected\n");
                detected_card &= mifare_classic;
                break;
            case sak_mfp_4k_sl1 << 24 | atqa_mfp_x:
                printf("MIFARE Classic detected\n");
                detected_card &= mifare_classic;
                break;
            default:
                break;
            }

            if (detected_card == 0xFFFF) {
                sak_atqa = bSak[0] << 24 | pAtqa[0] << 8 | pAtqa[1];
                switch (sak_atqa)    {
                    case sak_ul << 24 | atqa_ul:
                    printf("MIFARE Ultralight detected\n");
                    detected_card &= mifare_ultralight;
                    break;
                case sak_mfp_2k_sl2 << 24 | atqa_mfp_s:
                    printf("MIFARE Plus detected\n");
                    detected_card &= mifare_plus;
                    break;
                case sak_mfp_2k_sl3 << 24 | atqa_mfp_s_2K:
                    printf("MIFARE Plus detected\n");
                    detected_card &= mifare_plus;
                    break;
                case sak_mfp_2k_sl3 << 24 | atqa_mfp_s:
                    printf("MIFARE Plus detected\n");
                    detected_card &= mifare_plus;
                    break;
                case sak_mfp_4k_sl2 << 24 | atqa_mfp_s:
                    printf("MIFARE Plus detected\n");
                    detected_card &= mifare_plus;
                    break;
                case sak_mfp_2k_sl2 << 24 | atqa_mfp_x:
                    printf("MIFARE Plus detected\n");
                    detected_card &= mifare_plus;
                    break;
                case sak_mfp_2k_sl3 << 24 | atqa_mfp_x:
                    printf("MIFARE Plus detected\n");
                    detected_card &= mifare_plus;
                    break;
                case sak_mfp_4k_sl2 << 24 | atqa_mfp_x:
                    printf("MIFARE Plus detected\n");
                    detected_card &= mifare_plus;
                    break;
                case sak_desfire << 24 | atqa_desfire:
                    printf("MIFARE DESFire detected\n");
                    detected_card &= mifare_desfire;
                    break;
                case sak_layer4 << 24 | atqa_nPA:
                    printf("German eID (neuer Personalausweis) detected\n");
                    detected_card &= nPA;
                    break;
                default:
                    break;
                }
            }
        }
        else {
            /* No MIFARE card is in the field */
            return false;
        }

        /* There is a MIFARE card in the field, but we cannot determine it */
        if (!status && detected_card == 0xFFFF) {
            printf("Unknown MIFARE card detected\n");
        }

        /* Print card's UID */
        if (bLength > 0) {
            printf("UID: ");
            for(i = 0; i < bLength; i++) {
                printf("%02X ", bUid[i]);
            }
            printf("\n\n");
        }

        /* Read MF classic */
        if (detected_card == mifare_classic || detected_card == mifare_ultralight) {
            if (detected_card == mifare_classic) {
                fprintf(stdout, "Trying to authenticate...\n");
                status = phalMfc_Authenticate(&alMfc, 0, PHHAL_HW_MFC_KEYA, 1, 0, bUid, bLength);
                if (status != PH_ERR_SUCCESS) {
                    if (status == PH_ERR_AUTH_ERROR) {
                        fprintf(stderr, "Auth error while trying to authenticate to Mifare Classic\n");
                    }
                    else if (status == PH_ERR_INVALID_PARAMETER) {
                        fprintf(stderr, "Parameter error while trying to authenticate to Mifare Classic\n");
                    }
                    else if (status == PH_ERR_IO_TIMEOUT) {
                        fprintf(stderr, "Timeout while trying to authenticate to Mifare Classic\n");
                    }
                    else {
                        fprintf(stderr, "Unknown error while trying to authenticate to Mifare Classic: 0x%02X\n", status);
                }
                /* return detected_card */
            }
            for(j=0; j<16; j++) {
                    readstatus = phalMfc_Read(&alMfc, j, pBlockData);
            }
                    if (readstatus == PH_ERR_SUCCESS) {
                        printf("Block %d: ", j);
                        for(i = 0; i < 16; i++) {
                            printf("%02X ", pBlockData[i]);
                        }
                        printf("\n");
                    }
                    else {
                        printf("Cannot read block %d\n", j);
                    }
            }
        }

        status = phpalI14443p3a_HaltA(&I14443p3a);

    return detected_card;
}
