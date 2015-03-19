package timetracker.activities;

import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.EditText;
import android.widget.TimePicker;

import java.text.SimpleDateFormat;
import java.util.Calendar;

import timetracker.database.DatabaseHandler;
import timetracker.model.Entry;

/*
This class extends the class ActionBarActivity, and implements a onClickListener with the method
public void onClick(View v)
This method handle the actions for an click and a focus change event.

*/

public class EntryActivity extends BaseActivity implements View.OnClickListener {
    private Entry entry;
    //With the databasehandler all database actions are performed
    private DatabaseHandler db;
    //to save the date for the datepicker and the timepicker
    private Calendar calendar = Calendar.getInstance();


    private EditText dateEditText;
    private EditText timeEditText;

    private Button deleteButton;
    private Button okButton;

    //this method is the start point in this activity
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Hängt das Layout für EntrityActivit in den Contentframe rein
        super.onCreate(savedInstanceState);
        LayoutInflater inflater = getLayoutInflater();
        inflater.inflate(R.layout.activity_entry, (ViewGroup) findViewById(R.id.content_frame));

        db = new DatabaseHandler(this);
        // with getExtras() we can access to an input parameter sent with the intent to the activity.
        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            // access to the modified entry
            // entryPosition Wert ist in der TimeOverviewActivity gespeichert unter "
            // "entryPosition" --> Primary Key von der DB
            entry = db.getEntry(extras.getInt("entryPosition"));
            calendar.setTimeInMillis(entry.getDate().getTimeInMillis());
        }

        //binding between the elements of the xml files to java objects
        dateEditText = (EditText) findViewById(R.id.editTextDatePicker);
        timeEditText = (EditText) findViewById(R.id.editTextTimePicker);

        deleteButton = (Button) findViewById(R.id.buttonDelete);
        okButton = (Button) findViewById(R.id.buttonOk);

        deleteButton.setOnClickListener(this);
        okButton.setOnClickListener(this);

        dateEditText.setFocusable(false);
        timeEditText.setFocusable(false);


        dateEditText.setOnClickListener(this);
        timeEditText.setOnClickListener(this);

        fillTextFields(entry.getDate());
    }
    // Time Method which is writing the time in the textfields
    private void fillTextFields(Calendar cal) {
        SimpleDateFormat dateFormat = new SimpleDateFormat("dd.MM.yyyy");
        dateEditText.setText(dateFormat.format(cal.getTime().getTime()));
        SimpleDateFormat timeFormat = new SimpleDateFormat("HH:mm");
        timeEditText.setText(timeFormat.format(cal.getTime().getTime()));
    }

    // Looks if there is a change in the textfields
    public boolean updateEntry() {
        boolean change = false;
        //detect if anything changes
        if (!calendar.equals(entry.getDate())) {
            change = true;
        }
        entry.setDate(calendar);
        return change;
    }

    // callback of the datepicker
    // If the timepicker is open and edited, then open datePickerDialog
    DatePickerDialog.OnDateSetListener date = new DatePickerDialog.OnDateSetListener() {
        @Override
        public void onDateSet(DatePicker view, int year, int monthOfYear, int dayOfMonth) {
            calendar.set(Calendar.YEAR, year);
            calendar.set(Calendar.MONTH, monthOfYear);
            calendar.set(Calendar.DAY_OF_MONTH, dayOfMonth);
            fillTextFields(calendar);
        }
    };

    //callback of the timepicker
    TimePickerDialog.OnTimeSetListener time = new TimePickerDialog.OnTimeSetListener() {
        @Override
        public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
            calendar.set(Calendar.MINUTE, minute);
            calendar.set(Calendar.HOUR_OF_DAY, hourOfDay);
            fillTextFields(calendar);
        }
    };

    private void showDateOrTimePicker(int id) {
        switch (id) {
            case R.id.editTextDatePicker:
                // ID decides if datepicker or timepicker (icons to edit the time and date)
                new DatePickerDialog(this, date, calendar.get(Calendar.YEAR), calendar.get(Calendar.MONTH), calendar.get(Calendar.DAY_OF_MONTH)).show();
                // To go out, then you can switch "manually" to editTextTimePicker
                break;
            case R.id.editTextTimePicker:
                new TimePickerDialog(this, time, calendar.get(Calendar.HOUR_OF_DAY), calendar.get(Calendar.MINUTE), true).show();
        }
    }


    @Override
    public void onClick(View v) {
        //with this intent it is possible to switch bewteen the activities of the application
        Intent intent = new Intent(this, TimeOverviewActivity.class);
        // Switch for Editing of Entries
        switch (v.getId()) {
            case R.id.buttonDelete:
                db.deleteEntry(entry);
                // add informations to the activity
                // Text is memoried in the "key" info
                intent.putExtra("info", getString(R.string.info_entry_deleted));
                //here an activity with the intent is started
                startActivity(intent);
                break;

            case R.id.buttonOk:
                if (updateEntry()) {
                    db.updateEntry(entry);
                    intent.putExtra("info", getString(R.string.info_enty_changed));
                } else {
                    intent.putExtra("info", getString(R.string.info_nothing_changed));
                }
                startActivity(intent);
                break;
            case R.id.editTextDatePicker:
                showDateOrTimePicker(v.getId());
                break;

            case R.id.editTextTimePicker:
                showDateOrTimePicker(v.getId());
                break;

        }

    }
}
