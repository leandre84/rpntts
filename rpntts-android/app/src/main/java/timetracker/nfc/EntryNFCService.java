
package timetracker.nfc;

import android.annotation.TargetApi;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.cardemulation.HostApduService;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Locale;

import timetracker.database.DatabaseHandler;
import timetracker.model.Entry;

@TargetApi(Build.VERSION_CODES.KITKAT)
public class EntryNFCService extends HostApduService {

    private final static byte[] SELECT_APP = new byte[]{(byte) 0x00, (byte) 0xa4, (byte) 0x04, (byte) 0x00,
            (byte) 0x07, (byte) 0xd2, (byte) 0x76, (byte) 0x00, (byte) 0x00, (byte) 0x85, (byte) 0x01, (byte) 0x01,
            (byte) 0x00,
    };

    private final static byte[] SELECT_CC_FILE = new byte[]{(byte) 0x00, (byte) 0xa4, (byte) 0x00, (byte) 0x0c,
            (byte) 0x02, (byte) 0xe1, (byte) 0x03,
    };

    private final static byte[] SELECT_NDEF_FILE = new byte[]{(byte) 0x00, (byte) 0xa4, (byte) 0x00, (byte) 0x0c,
            (byte) 0x02, (byte) 0xe1, (byte) 0x04, // NDEF File Identifier last 2 byte equale File ID
    };

    private final static byte[] CC_FILE = new byte[]{
            0x00, 0x0f, // CCLEN
            0x20, // Mapping Version
            0x00, 0x3b, // Maximum R-APDU data size
            0x00, 0x34, // Maximum C-APDU data size
            0x04, 0x06, // Tag & Length
            (byte) 0xe1, 0x04, // NDEF File Identifier
            0x00, (byte) 0xff, // Maximum NDEF size
            0x00, // NDEF file read access granted
            (byte) 0xff, // NDEF File write access denied
    };

    private final static byte[] SUCCESS_SW = new byte[]{
            (byte) 0x90, (byte) 0x00,
    };

    private final static byte[] FAILURE_SW = new byte[]{
            (byte) 0x6a, (byte) 0x82,
    };
    private static final String TAG = "NFC_XXX";

    private byte[] mNdefRecordFile;

    public EntryNFCService() {
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    public void onCreate() {
        super.onCreate();
        NdefRecord record = createEntryRecord(getSentString(), Locale.GERMAN, true);

        NdefMessage ndefMessage = new NdefMessage(record);

        int nlen = ndefMessage.getByteArrayLength();
        //int nlen = 1;
        mNdefRecordFile = new byte[nlen + 2];
        mNdefRecordFile[0] = (byte) ((nlen & 0xff00) / 256);
        mNdefRecordFile[1] = (byte) (nlen & 0xff);

        System.arraycopy(ndefMessage.toByteArray(), 0, mNdefRecordFile, 2, ndefMessage.getByteArrayLength());
        //System.arraycopy(new byte[]{(byte) 0x00} , 0, mNdefRecordFile, 2, 1);
    }

    enum State {
        NothingSelected, AppSelected, ContainerCapabilitySelected, NdefSelected
    }

    // this method transforms a text into a nfc record. found in web
    private NdefRecord createEntryRecord(String text, Locale locale, boolean encodeInUtf8) {
        byte[] langBytes = locale.getLanguage().getBytes(Charset.forName("US-ASCII"));
        Charset utfEncoding = encodeInUtf8 ? Charset.forName("UTF-8") : Charset.forName("UTF-16");
        byte[] textBytes = text.getBytes(utfEncoding);
        int utfBit = encodeInUtf8 ? 0 : (1 << 7);
        char status = (char) (utfBit + langBytes.length);
        byte[] data = new byte[1 + langBytes.length + textBytes.length];
        data[0] = (byte) status;
        System.arraycopy(langBytes, 0, data, 1, langBytes.length);
        System.arraycopy(textBytes, 0, data, 1 + langBytes.length, textBytes.length);
        return new NdefRecord(NdefRecord.TNF_WELL_KNOWN, NdefRecord.RTD_TEXT, new byte[0], data);
    }

    State mState = State.NothingSelected;
    boolean mInitialized = false;

    @Override
    public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {
        Log.i(TAG, "processCommandApdu");
        Log.i(TAG, "commands: " + buildString(commandApdu));

        if (Arrays.equals(SELECT_APP, commandApdu)) {
            Log.d(TAG, "SELECT APP");
            mState = State.AppSelected;
            mInitialized = true;
            return SUCCESS_SW;
        } else if (mInitialized && Arrays.equals(SELECT_CC_FILE, commandApdu)) {
            Log.d(TAG, "SELECT CC");
            mState = State.ContainerCapabilitySelected;
            return SUCCESS_SW;
        } else if (mInitialized && Arrays.equals(SELECT_NDEF_FILE, commandApdu)) {
            Log.d(TAG, "SELECT NDEF");
            mState = State.NdefSelected;
            return SUCCESS_SW;
        } else if ((commandApdu[0] == (byte) 0x00 && commandApdu[1] == (byte) 0xb0) ||
                (commandApdu[0] == (byte) 0x00 && commandApdu[1] == (byte) 0xa4)) {

            int offset = (0x00ff & commandApdu[2]) * 256 + (0x00ff & commandApdu[3]);
            int le = 0x00ff & commandApdu[4];

            byte[] responseApdu = new byte[le + SUCCESS_SW.length];

            if (mState == State.ContainerCapabilitySelected && offset == 0 && le == CC_FILE.length) {
                Log.d(TAG, "READ CC");
                System.arraycopy(CC_FILE, offset, responseApdu, 0, le);
                System.arraycopy(SUCCESS_SW, 0, responseApdu, le, SUCCESS_SW.length);
                return responseApdu;
            } else if (mState == State.NdefSelected) {
                Log.d(TAG, "READ NDEF: " + offset + " , " + le + "(" + mNdefRecordFile.length + ")");
                if (offset + le <= mNdefRecordFile.length) {
                    System.arraycopy(mNdefRecordFile, offset, responseApdu, 0, le);
                    System.arraycopy(SUCCESS_SW, 0, responseApdu, le, SUCCESS_SW.length);
                    Log.d(TAG, "READ NDEF OK");

                    return responseApdu;
                }
                Log.d(TAG, "READ NDEF NOK");
            }
        }

        Log.d(TAG, "WUUUUUT: " + buildString(commandApdu));
        return FAILURE_SW;
    }

    private String buildString(byte[] bs) {
        StringBuilder sb = new StringBuilder();
        for (byte b : bs) {
            sb.append(Integer.toHexString(0x00ff & b)).append(':');
        }
        return sb.toString();
    }


    @Override
    public void onDeactivated(int reason) {
        Log.d(TAG, "DEACTIVATED:" + ((reason == DEACTIVATION_DESELECTED) ? "DESELECTED" : "LINK LOST"));
        mState = State.NothingSelected;
        mInitialized = false;
    }

    public String getSentString() {
        DatabaseHandler db = new DatabaseHandler(this);
        String sentString = "rpntts|" + db.getSetting("id").getValue() + "|" + db.getSetting("password").getValue();
        // add entry infos
        List<Entry> entries = db.getAllEntries();
        Collections.reverse(entries);
        for (Entry entry : entries) {
            if (!entry.isSent())
                sentString += "|" + entry.toString();
        }
        //CHANGE:pipe
        sentString += "#";
        //ENDCHANGE:pipe
        return sentString;
    }

}
