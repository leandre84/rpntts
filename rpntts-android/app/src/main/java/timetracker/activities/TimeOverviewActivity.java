package timetracker.activities;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.nio.charset.Charset;
import java.util.Calendar;
import java.util.List;
import java.util.Locale;

import timetracker.adapter.EntryListAdapter;
import timetracker.database.DatabaseHandler;
import timetracker.model.Entry;
import timetracker.util.SwipeDetector;

public class TimeOverviewActivity extends BaseActivity implements View.OnClickListener, AdapterView.OnItemClickListener {

    private ListView entryListView;
    private Button addEntryButton;
    private Button syncButton;
    private NfcAdapter mNfcAdapter;
    private DatabaseHandler db;

    private SwipeDetector swipeDetector;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //add the layout from file activitytimeoverview to the contentframe of the base activity
        LayoutInflater inflater = getLayoutInflater();
        inflater.inflate(R.layout.activity_time_overview, (ViewGroup) findViewById(R.id.content_frame));

        // getExtras --> Über getIntent kann man nur so auf die Werte der Activities kommen.
        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            Toast.makeText(getApplicationContext(), extras.getString("info"), Toast.LENGTH_SHORT).show();
        }
        db = new DatabaseHandler(this);

        // To check that action is a "swipe" and not click
        swipeDetector = new SwipeDetector();

        //link objects to object form activity_time_overview.xml layout file
        entryListView = (ListView) findViewById(R.id.listView);
        addEntryButton = (Button) findViewById(R.id.button_addEntry);
        syncButton = (Button) findViewById(R.id.button_sync);

        addEntryButton.setOnClickListener(this);
        syncButton.setOnClickListener(this);
        entryListView.setOnItemClickListener(this);
        entryListView.setOnTouchListener(swipeDetector);

        populateEntryList();

        checkNFCHostCardFeature();

    }
    // Check if HCE is supported by this device where the app is currently running
    private void checkNFCHostCardFeature() {
        boolean isHceSupported = getPackageManager().hasSystemFeature("android.hardware.nfc.hce");
        Toast.makeText(this, "HCE Supported: " + getPackageManager().hasSystemFeature("android.hardware.nfc.hce"), Toast.LENGTH_LONG).show();
    }


    /**
     * Populate the entry list based on username and password set in textfields.
     */
    private void populateEntryList() {
        // Build adapter with entries
        List<Entry> entryList = db.getAllEntries();

        // If Entry is empty
        if (entryList.isEmpty())
            entryList.add(new Entry(getString(R.string.info_empty_entry)));


        EntryListAdapter adapter = new EntryListAdapter(getApplicationContext(), R.layout.enty_list_item, entryList);
        entryListView.setAdapter(adapter);
        entryListView.setSelectionAfterHeaderView();
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

    // eventhandler for a click event
    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button_addEntry:
                // checking if name and pw is set
                if (db.getSetting("id").getValue().equals("") || db.getSetting("password").getValue().equals("")) {
                    // showing a toast message to the user
                    Toast.makeText(getApplicationContext(), getString(R.string.info_add_id_and_pw), Toast.LENGTH_SHORT).show();
                } else {
                    Entry e = new Entry(Calendar.getInstance(), false);
                    //save the entry
                    db.addEntry(e);
                    //update view
                    populateEntryList();
                }
                break;
            case R.id.button_sync:
                clickButtonSync();
                break;

        }
    }

    public void clickButtonSync() {
        AlertDialog dialog = new AlertDialog.Builder(this).create();
        dialog.setTitle(getString(R.string.sync_title));
        dialog.setMessage(getString(R.string.sync_ok));
        dialog.setCancelable(false);
        dialog.setButton(DialogInterface.BUTTON_POSITIVE, getString(R.string.yes), new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int buttonId) {
                if (db.getSetting("notDeleteAfterTransmission").getValue().equals("t")) {
                    db.setSentAllEntries(true);
                } else {
                    db.deleteAllEntries();
                }
                populateEntryList();
            }
        });
        dialog.setButton(DialogInterface.BUTTON_NEGATIVE, getString(R.string.no), new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int buttonId) {
                if (db.getSetting("notDeleteAfterTransmission").getValue().equals("t")) {
                    db.setSentAllEntries(true);
                } else {
                    db.deleteAllEntries();
                }
                populateEntryList();
            }
        });
        dialog.show();
    }

    // method to handle the click event when a item in the list is clicked
    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        //access to the entry with the item position
        final Entry entry = (Entry) entryListView.getAdapter().getItem(position);
        if (entry.getId() == -1) return;
        //swipeDetected is true if this click is a swipe
        if (swipeDetector.swipeDetected()) {
            if (swipeDetector.getAction() == SwipeDetector.Action.LR) {
                db.deleteEntry(entry);
                populateEntryList();
                Toast.makeText(getApplicationContext(), getString(R.string.info_delete_entry), Toast.LENGTH_SHORT).show();
            }
        } else {
            if (!entry.isSent()) {
                //create a new intent to change the activity
                Intent intent = new Intent(this, EntryActivity.class);
                //adding a parameter id to get access to the correct object in the database in EntryActivity
                intent.putExtra("entryPosition", entry.getId());
                //start the window
                startActivity(intent);
                // If it is not wipeing, then it is clicking
            } else {
                Toast.makeText(getApplicationContext(), getString(R.string.info_entry_sent), Toast.LENGTH_SHORT).show();
            }

        }
    }

    @Override
    public void onBackPressed() {
        //stay on overview do not switch to EntryActivity or Settings with back button
        // if back button is pressed in main activity go to home
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_HOME);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(intent);
    }
}



