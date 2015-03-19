package timetracker.activities;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Toast;

import timetracker.database.DatabaseHandler;
import timetracker.model.Setting;
import timetracker.util.Security;


public class SettingActivity extends BaseActivity implements View.OnClickListener, View.OnFocusChangeListener {
    DatabaseHandler db;
    private EditText idEditText;
    private EditText passwordEditText;
    private Button saveButton;
    private CheckBox deleteEntriesCheckBox;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        LayoutInflater inflater = getLayoutInflater();
        inflater.inflate(R.layout.activity_setting, (ViewGroup) findViewById(R.id.content_frame));

        idEditText = (EditText) findViewById(R.id.editTextId);
        passwordEditText = (EditText) findViewById(R.id.editTextPassword);
        deleteEntriesCheckBox = (CheckBox) findViewById(R.id.checkBockDeleteEntries);

        db = new DatabaseHandler(this);

        idEditText.setText(db.getSetting("id").getValue());
        passwordEditText.setText(db.getSetting("password").getValue());

        deleteEntriesCheckBox.setChecked(db.getSetting("notDeleteAfterTransmission").getValue().equals("t"));

        passwordEditText.setOnFocusChangeListener(this);
        saveButton = (Button) findViewById(R.id.buttonSave);
        saveButton.setOnClickListener(this);
    }


    @Override
    public void onClick(View view) {
        if (idEditText.getText().toString().equals("") || passwordEditText.getText().toString().equals("")) {
            Toast.makeText(getApplicationContext(), getString(R.string.info_set_id_and_pw), Toast.LENGTH_SHORT).show();
        } else {
            if (textFieldsChange()) {
                db.deleteAllSettings();
                db.addSetting(new Setting("id", idEditText.getText().toString()));
                // Password hashing from util, security
                db.addSetting(new Setting("password", Security.shahash(passwordEditText.getText().toString())));
                db.addSetting(new Setting("notDeleteAfterTransmission", deleteEntriesCheckBox.isChecked() ? "t" : "f"));

                Toast.makeText(getApplicationContext(), getString(R.string.info_save_id_and_pw), Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(getApplicationContext(), getString(R.string.info_no_change_id_and_pw), Toast.LENGTH_SHORT).show();

            }

        }
    }

    // Check if there is any modifications
    private boolean textFieldsChange() {

        if (!passwordEditText.getText().toString().equals(db.getSetting("password").getValue()))
            return true;
        if (!idEditText.getText().toString().equals(db.getSetting("id").getValue())) {
            return true;
        }
        if (deleteEntriesCheckBox.isChecked() != db.getSetting("notDeleteAfterTransmission").getValue().equals("t")) {
            return true;
        }
        return false;
    }

    // To delete the text in the password field
    @Override
    public void onFocusChange(View v, boolean hasFocus) {
        if (hasFocus)
            passwordEditText.setText("");
    }
}
