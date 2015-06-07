package timetracker.activities;

import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.TypedArray;
import android.os.Bundle;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.ActionBarDrawerToggle;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.ArrayList;

import timetracker.adapter.NavDrawerListAdapter;
import timetracker.model.NavDrawerItem;


public class BaseActivity extends ActionBarActivity {

    private ListView listView;
    private String[] navMenuTitles;
    private TypedArray navMenuIcons;
    private ArrayList<NavDrawerItem> navDrawerItems;
    private NavDrawerListAdapter adapter;

    private ActionBarDrawerToggle mDrawerToggle;
    private DrawerLayout mDrawerLayout;

    // Wird ausgeführt wenn Acivity gestartet wird. Startpunkt jeder Activity.
    //
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Hier wird das Layout von Base Activity geladen
        setContentView(R.layout.activity_base);
        // Hier wird der Drawer (Menu) geladen
        listView = (ListView) findViewById(R.id.left_drawer);
        // Verweist auf das Hauptlayout
        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        // Verweist auf den Pfeil, der toggelt
        mDrawerToggle = new ActionBarDrawerToggle(
                this,                  /* host Activity */
                mDrawerLayout,         /* DrawerLayout object */
                R.drawable.ic_drawer,  /* nav drawer icon to replace 'Up' caret */
                R.string.app_name  /* "open drawer" description */
        ) {

            /**
             * Called when a drawer has settled in a completely closed state.
             */
            public void onDrawerClosed(View view) {
                super.onDrawerClosed(view);
                getSupportActionBar().setTitle(R.string.app_name);
            }

            /**
             * Called when a drawer has settled in a completely open state.
             */
            public void onDrawerOpened(View drawerView) {
                super.onDrawerOpened(drawerView);
                getSupportActionBar().setTitle(R.string.app_name);
            }
        };

        // Set the drawer toggle as the DrawerListener - Damit dem Pfeil dynamisch angepasst wird.
        mDrawerLayout.setDrawerListener(mDrawerToggle);

        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setHomeButtonEnabled(true);


        // load slide menu items
        // Eintrag im string ist ein array. Hier werden die Items und die Icons rausgeholt.
        navMenuTitles = getResources().getStringArray(R.array.nav_drawer_items);
        navMenuIcons = getResources().obtainTypedArray(R.array.nav_drawer_icons);
        navDrawerItems = new ArrayList<NavDrawerItem>();
        // Hinzufügen der Icons. Zusammenbau mit Beschriftung und Titel
        for (int i = 0; i < navMenuTitles.length; i++) {
            navDrawerItems.add(new NavDrawerItem(navMenuTitles[i], navMenuIcons.getResourceId(i, -1)));
        }

        // Recycle the typed array, sollte automatisch passieren
        navMenuIcons.recycle();
        // Wenn man auf ein Item im Menu drückt, wird es ausgeführt.
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {
                switch (position) {
                    case 0: // first menu item = home
                        startActivity(new Intent(BaseActivity.this, TimeOverviewActivity.class));
                        break;
                    case 1: // second elements
                        startActivity(new Intent(BaseActivity.this, SettingActivity.class));
                }
            }
        });
        // Adapter speichert Informationen. Dann kann die listView auf den Adapter zugreifen,
        // und der Adapter auf die Daten.
        adapter = new NavDrawerListAdapter(getApplicationContext(), navDrawerItems);
        listView.setAdapter(adapter);

    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        // Sync the toggle state after onRestoreInstanceState has occurred.
        mDrawerToggle.syncState();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        mDrawerToggle.onConfigurationChanged(newConfig);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Pass the event to ActionBarDrawerToggle, if it returns
        // true, then it has handled the app icon touch event
        if (mDrawerToggle.onOptionsItemSelected(item)) {

            return true;
        }
        // Handle your other action bar items...

        return super.onOptionsItemSelected(item);
    }

}
