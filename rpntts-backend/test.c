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
#define MFC_DEFAULT_KEYS 9
#define NUMBER_OF_KEYENTRIES MFC_DEFAULT_KEYS + 1 
#define NUMBER_OF_KEYVERSIONPAIRS 1
#define NUMBER_OF_KUCENTRIES 1


uint32_t DetectMifare(void *halReader);

int main(int argc, char **argv) {

    phbalReg_RpiSpi_DataParams_t balReader;
    phhalHw_Rc523_DataParams_t halReader;
    phStatus_t status;
    uint8_t bHalBufferTx[256];
    uint8_t bHalBufferRx[256];

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

    uint8_t datablocks = 0;

    /* Initialize the 14443-3A PAL (Protocol Abstraction Layer) component */
    status = phpalI14443p3a_Sw_Init(&I14443p3a, sizeof(phpalI14443p3a_Sw_DataParams_t), halReader);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing the 14443-3A PAL\n");
        return false;
    }

    /* Initialize the 14443-4 PAL component */
    status =  phpalI14443p4_Sw_Init(&I14443p4, sizeof(phpalI14443p4_Sw_DataParams_t), halReader);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing the 14443-P4 PAL\n");
        return false;
    }

    /* Initialize the Mifare PAL component */
    status = phpalMifare_Sw_Init(&palMifare, sizeof(phpalMifare_Sw_DataParams_t), halReader, &I14443p3a);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing the Mifare PAL\n");
        return false;
    }

    /* Initialize Ultralight(-C) AL component */
    status = phalMful_Sw_Init(&alMful, sizeof(phalMful_Sw_DataParams_t), &palMifare, NULL, NULL, NULL);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing the Mifare Ultralight AL\n");
        return false;
    }

    /* Initialize the keystore component */
    status = phKeyStore_Sw_Init(&SwkeyStore, sizeof(phKeyStore_Sw_DataParams_t), &pKeyEntries[0], NUMBER_OF_KEYENTRIES, &pKeyVersionPairs[0], NUMBER_OF_KEYVERSIONPAIRS, &pKUCEntries[0], NUMBER_OF_KUCENTRIES);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing the keystore\n");
        return false;
    }

    /* Format keystore and put key into it */
    for (i=0; i<MFC_DEFAULT_KEYS; i++) {
        status = phKeyStore_FormatKeyEntry(&SwkeyStore, i+1, PH_KEYSTORE_KEY_TYPE_MIFARE);
        if (status != PH_ERR_SUCCESS) {
            fprintf(stderr, "Error formating keystore element %d\n", i+1);
            return false;
        }
        status = phKeyStore_SetKey(&SwkeyStore, i+1, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, MfcDefaultKeys[i], 0);
        if (status != PH_ERR_SUCCESS) {
            fprintf(stderr, "Error setting keystore element %d\n", i+1);
            return false;
        }
    }

    /* Initialize Classic AL component */
    status = phalMfc_Sw_Init(&alMfc, sizeof(phalMfc_Sw_DataParams_t), &palMifare, &SwkeyStore);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error initializing the Mifare Classic AL\n");
        return false;
    }

    /* Soft reset the IC */
    phhalHw_Rc523_Cmd_SoftReset(halReader);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error soft resetting the HAL\n");
        return false;
    }

    /* Apply the type A protocol settings and activate the RF field. */
    status = phhalHw_ApplyProtocolSettings(halReader, PHHAL_HW_CARDTYPE_ISO14443A);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error applying protocol settings and activating the RF field\n");
        return false;
    }

    /* Empty the pAtqa */
    memset(pAtqa, '\0', 2);
    status = phpalI14443p3a_RequestA(&I14443p3a, pAtqa);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error doing Request A\n");
        return false;
    }

    /* Reset the RF field  */
    status = phhalHw_FieldReset(halReader);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error reseting the field\n");
        return false;
    }

    /* Empty the bSak */
    memset(bSak, '\0', 1);

    /* Activate the communication layer part 3 of the ISO 14443A standard. */
    status = phpalI14443p3a_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength, bSak, &bMoreCardsAvailable);
    if (status != PH_ERR_SUCCESS) {
        fprintf(stderr, "Error activating card\n");
        /* return false; */
    }

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
            }

            /* Set number of data blocks according to detected card */
            datablocks = (detected_card == mifare_classic) ? 64 : 32;

            for (i = 0; i < datablocks; i++) {
                    /* Authenticate to Mifare Classic Sector */
                    if (detected_card == mifare_classic && i % 4 == 0) {
                        for (j = 0; j < NUMBER_OF_KEYENTRIES; j++) {
                            /* fprintf(stdout, "Trying to authenticate with key %d\n", j+1); */
                            status = phalMfc_Authenticate(&alMfc, i, PHHAL_HW_MFC_KEYA, j+1, 0, bUid, bLength);
                            if (status != PH_ERR_SUCCESS) {
                                /* fprintf(stderr, "Error while trying to authenticate to Mifare Classic: 0x%02X\n", status); */
                                status = phpalI14443p3a_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength, bSak, &bMoreCardsAvailable);
                                if (status != PH_ERR_SUCCESS) {
                                    fprintf(stderr, "Error reactivating card after failed auth\n");
                                    status = phpalI14443p3a_HaltA(&I14443p3a);
                                    return detected_card;
                                }
                            }
                            else {
                                break;
                            }
                        }
                    }
                    /* Read block */
                    readstatus = phalMfc_Read(&alMfc, i, pBlockData);
                    if (readstatus == PH_ERR_SUCCESS) {
                        printf("Block %2d: ", i);
                        for(j = 0; j < 16; j++) {
                            printf("%02X ", pBlockData[j]);
                        }
                        printf("\n");
                    }
                    else {
                        printf("Cannot read block %d\n", i);
                    }
            }
        }

        printf("\n\n");

        status = phpalI14443p3a_HaltA(&I14443p3a);

    return detected_card;
}
