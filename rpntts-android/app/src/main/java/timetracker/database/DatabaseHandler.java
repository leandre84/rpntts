package timetracker.database;

/**
 * This class is for all database action. it generates tables and the access to the objects (entry, setting).
 */

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import timetracker.model.Entry;
import timetracker.model.Setting;

public class DatabaseHandler extends SQLiteOpenHelper {

    // All Static variables
    // Database Version
    // Bei Änderungen muss die aufgezählt werden.
    private static final int DATABASE_VERSION = 2;

    // Database Name
    private static final String DATABASE_NAME = "timetracker";

    // Entry table name
    private static final String TABLE_ENTRY = "entry";
    // Setting table name
    private static final String TABLE_SETTING = "setting";
    // Entry Table Columns names
    private static final String KEY_ID = "id";
    private static final String KEY_DATE = "date";
    private static final String KEY_SENT = "sent";
    private static final String KEY_TYPE = "type";

    //Setting Table Columns names
    private static final String KEY_KEY = "key";
    private static final String KEY_VALUE = "value";


    public DatabaseHandler(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    // Creating Tables
    @Override
    public void onCreate(SQLiteDatabase db) {
        String CREATE_ENTRY_TABLE = "CREATE TABLE " + TABLE_ENTRY + "("
                + KEY_ID + " INTEGER PRIMARY KEY,"
                + KEY_DATE + " TEXT,"
                + KEY_SENT + " TEXT,"
                + KEY_TYPE + " TEXT)";
        db.execSQL(CREATE_ENTRY_TABLE);

        String CREATE_SETTING_TABLE = "CREATE TABLE " + TABLE_SETTING + "("
                + KEY_KEY + " TEXT PRIMARY KEY," + KEY_VALUE + " TEXT)";
        db.execSQL(CREATE_SETTING_TABLE);
    }

    // Upgrading database
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // Drop older table if existed
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_ENTRY);
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_SETTING);
        // Create tables again
        onCreate(db);
    }

    /**
     * All CRUD(Create, Read, Update, Delete) Operations
     */

    public void addSetting(Setting setting) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_KEY, setting.getKey());                      // Setting key
        values.put(KEY_VALUE, setting.getValue());              // Setting value

        // Inserting Row
        db.insert(TABLE_SETTING, null, values);
        db.close(); // Closing database connection
    }

    public Setting getSetting(String key) {
        SQLiteDatabase db = this.getReadableDatabase();
        // =? ist ein Platzhalter der den String unten nimmt
        Cursor cursor = db.query(TABLE_SETTING, new String[]{KEY_KEY, KEY_VALUE}, KEY_KEY + "=?",
                new String[]{key}, null, null, null);

        Setting setting = new Setting("","");
        if (cursor != null && cursor.moveToFirst()) {
            setting = new Setting(cursor.getString(0), cursor.getString(1));
        }
        // return setting
        return setting;
    }

    public int updateSetting(Setting setting) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_KEY, setting.getKey());
        values.put(KEY_VALUE, setting.getValue());

        // updating row
        return db.update(TABLE_SETTING, values, KEY_KEY + " = ?",
                new String[]{setting.getKey()});
    }

    public void deleteSetting(Setting setting) {
        SQLiteDatabase db = this.getWritableDatabase();
        db.delete(TABLE_SETTING, KEY_KEY + " = ?",
                new String[]{setting.getKey()});
        db.close();
    }

    // Adding new entry
    public void addEntry(Entry entry) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        // Da in DB nur Integer muss man es in String umwandeln.
        values.put(KEY_DATE, Long.toString(entry.getDate().getTime().getTime())); // Date converted into long and then into a string
        values.put(KEY_SENT, Boolean.toString(entry.isSent()));
        values.put(KEY_TYPE, entry.getType());
        Log.i("values", values.toString());

        // Inserting Row
        db.insert(TABLE_ENTRY, null, values);
        db.close(); // Closing database connection
    }

    // Getting single entry
    public Entry getEntry(int id) {
        SQLiteDatabase db = this.getReadableDatabase();
        Cursor cursor = db.query(TABLE_ENTRY, new String[]{KEY_ID,
                        KEY_DATE, KEY_SENT, KEY_TYPE}, KEY_ID + "=?",
                new String[]{String.valueOf(id)}, null, null, null);
        if (cursor != null)
            cursor.moveToFirst();

        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(Long.parseLong(cursor.getString(1)));

        Entry entry = new Entry(Integer.parseInt(cursor.getString(0)), calendar, Boolean.parseBoolean(cursor.getString(2)), cursor.getString(3));
        // return entry
        return entry;
    }

    // Getting All Entries
    public List<Entry> getAllEntries() {
        List<Entry> entryList = new ArrayList<>();
        // Select All Query
        String selectQuery = "SELECT  * FROM " + TABLE_ENTRY;

        SQLiteDatabase db = this.getWritableDatabase();
        Cursor cursor = db.rawQuery(selectQuery, null);

        // looping through all rows and adding to list
        if (cursor.moveToFirst()) {
            do {
                Calendar calendar = Calendar.getInstance();

                calendar.setTimeInMillis(Long.parseLong(cursor.getString(1)));
                Entry entry = new Entry(Integer.parseInt(cursor.getString(0)), calendar, Boolean.parseBoolean(cursor.getString(2)), cursor.getString(3));
                // Adding entry to list
                entryList.add(entry);
            } while (cursor.moveToNext());
        }
        //sorting entryList based on date
        Collections.sort(entryList, new Comparator<Entry>() {
            @Override
            public int compare(Entry entry1, Entry entry2) {
                return entry2.getDate().compareTo(entry1.getDate());
            }
        });
        // return entry list
        return entryList;
    }

    // Updating single entry
    public int updateEntry(Entry entry) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_DATE, Long.toString(entry.getDate().getTime().getTime())); // Date converted into long and then into a string
        values.put(KEY_SENT, Boolean.toString(entry.isSent()));
        values.put(KEY_TYPE, entry.getType());

        // updating row
        return db.update(TABLE_ENTRY, values, KEY_ID + " = ?",
                new String[]{String.valueOf(entry.getId())});
    }

    // Deleting single entry
    public int deleteEntry(Entry entry) {
        SQLiteDatabase db = this.getWritableDatabase();
        int ret = db.delete(TABLE_ENTRY, KEY_ID + " = ?",
                new String[]{String.valueOf(entry.getId())});
        db.close();
        return ret;
    }


    public void deleteAllEntries() {
        SQLiteDatabase db = this.getWritableDatabase();
        // mit 1 = 1 -->  man löscht alle.
        db.delete(TABLE_ENTRY, "1 = 1", new String[]{});
        db.close();
    }

    public void deleteAllSettings() {
        SQLiteDatabase db = this.getWritableDatabase();
        db.delete(TABLE_SETTING, "1 = 1", new String[]{});
        db.close();
    }

    public void setSentAllEntries(boolean sent) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_SENT, Boolean.toString(true));

        // updating row
        db.update(TABLE_ENTRY, values, "1 = 1", null);

        db.close();
    }
}