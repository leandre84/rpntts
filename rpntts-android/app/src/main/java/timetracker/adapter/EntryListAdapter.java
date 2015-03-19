package timetracker.adapter;

import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;

import timetracker.activities.R;
import timetracker.model.Entry;

public class EntryListAdapter extends ArrayAdapter<Entry> {

    private Context context;
    private List<Entry> entries;

    public EntryListAdapter(Context context, int resource, List<Entry> entries) {
        super(context, resource, entries);
        this.context = context;
        this.entries = entries;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            LayoutInflater mInflater = (LayoutInflater)
                    context.getSystemService(Activity.LAYOUT_INFLATER_SERVICE);
            convertView = mInflater.inflate(R.layout.enty_list_item, null);
        }
        Entry entry = entries.get(position);

        ImageView imgIcon = (ImageView) convertView.findViewById(R.id.entry_item_icon);
        TextView txtTitle = (TextView) convertView.findViewById(R.id.entry_item_title);
        // If Record is "übertragen" or not :) --> Icon setting
        if (entry.isSent()) {
            imgIcon.setImageResource(android.R.drawable.presence_online);
        } else {
            imgIcon.setImageResource(android.R.drawable.presence_invisible);
        }
        txtTitle.setText(entry.toString());

        return convertView;

    }
}
