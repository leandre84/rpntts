package timetracker.model;

import java.text.SimpleDateFormat;
import java.util.Calendar;

/**
 * This class is to model the object for an entry (id (PK), date, flag if it is sent)
 */
public class Entry {
    private int id;
    private Calendar date;
    private boolean sent;
    //not saved in database
    private String text;

    public Entry() {

    }


    public Entry(String text) {
        id = -1;
        this.text = text;
    }

    public Entry(int id, Calendar date, boolean sent) {
        this.id = id;
        this.date = date;
        this.sent = sent;
    }

    public Entry(int id, Calendar date) {
        this.id = id;
        this.date = date;
        this.sent = true;
    }

    public Entry(Calendar date) {
        this.date = date;
    }

    public Entry(Calendar date, boolean sent) {
        this.date = date;
        this.sent = sent;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public Calendar getDate() {
        return date;
    }

    public void setDate(Calendar date) {
        this.date = date;
    }

    @Override
    public String toString() {
        if (id == -1) // empty entry
            return text;

        SimpleDateFormat dt = new SimpleDateFormat("dd.MM.yyyy HH:mm");
        return dt.format(getDate().getTime());
    }

    public boolean isSent() {
        return sent;
    }

    public void setSent(boolean sent) {
        this.sent = sent;
    }
}
