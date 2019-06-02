package com.iot.spidersense;

import android.Manifest;
import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.karumi.dexter.Dexter;
import com.karumi.dexter.listener.single.DialogOnDeniedPermissionListener;
import com.karumi.dexter.listener.single.PermissionListener;

import processing.android.PFragment;
import processing.core.PApplet;

public class MainActivity extends AppCompatActivity {

    private Activity activity = this;
    private PApplet sketch;
    private FrameLayout radarContainer;
    private LinearLayout settings;
    private android.support.v7.app.ActionBar actionbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        actionbar = getSupportActionBar();
        actionbar.setDisplayShowHomeEnabled(true);
        actionbar.setIcon(R.drawable.ic_spider);

        int display_mode = getResources().getConfiguration().orientation;
        if (display_mode == Configuration.ORIENTATION_LANDSCAPE) {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
            actionbar.hide();
        }

        // Gets the saved theme ID from SharedPrefs,
        // or uses default_theme if no theme ID has been saved
        int theme = PreferenceManager.getDefaultSharedPreferences(this).getInt("ActivityTheme", R.style.AppTheme);
        // Set this Activity's theme to the saved theme
        setTheme(theme);

        setContentView(R.layout.activity_main);

        radarContainer = findViewById(R.id.radar_container);
        settings = findViewById(R.id.settings);
        Button changeThemeBtn = findViewById(R.id.changeTheme);

        //Set function change theme to button
        changeThemeBtn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                final int theme = PreferenceManager.getDefaultSharedPreferences(activity).getInt("ActivityTheme", R.style.AppTheme);
                if(theme == R.style.AppTheme) PreferenceManager.getDefaultSharedPreferences(activity).edit().putInt("ActivityTheme", R.style.AppThemeDark).commit();
                else PreferenceManager.getDefaultSharedPreferences(activity).edit().putInt("ActivityTheme", R.style.AppTheme).commit();

                // setup the alert builder
                AlertDialog.Builder builder = new AlertDialog.Builder(activity);
                builder.setTitle("AlertDialog");
                builder.setMessage("It's necessary to restart the app for changing the theme!");

                // add the buttons
                builder.setPositiveButton("Restart now", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        activity.recreate();
                        Toast.makeText(activity, "Theme switched to " + ((theme == R.style.AppTheme) ? "dark" : "light"), Toast.LENGTH_SHORT).show();
                    }
                });
                builder.setNegativeButton("Restart later", null);

                // create and show the alert dialog
                AlertDialog dialog = builder.create();
                dialog.show();
            }
        });

        //Navbar
        BottomNavigationView bottomNavigationView = (BottomNavigationView) findViewById(R.id.navigation);
        bottomNavigationView.setOnNavigationItemSelectedListener
                (new BottomNavigationView.OnNavigationItemSelectedListener() {
                    @Override
                    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                        switch (item.getItemId()) {
                            case R.id.action_home:
                                radarContainer.setVisibility(View.VISIBLE);
                                settings.setVisibility(View.GONE);
                                break;
                            case R.id.action_settings:
                                radarContainer.setVisibility(View.GONE);
                                settings.setVisibility(View.VISIBLE);
                                break;
                        }
                        return true;
                    }
                });

        //Request permission
        PermissionListener dialogPermissionListener =
                DialogOnDeniedPermissionListener.Builder
                        .withContext(this.getApplicationContext())
                        .withTitle("Location permission")
                        .withMessage("Location permission is needed to use bluetooth!")
                        .withButtonText(android.R.string.ok)
                        .withIcon(R.drawable.ic_location)
                        .build();
        Dexter.withActivity(this)
                .withPermission(Manifest.permission.ACCESS_COARSE_LOCATION)
                .withListener(dialogPermissionListener).check();

        //Enable bluetooth
        BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, 0);
        }

        //Processing
        if (display_mode == Configuration.ORIENTATION_LANDSCAPE) {
            bottomNavigationView.setVisibility(View.GONE);
            sketch = new SketchFullScreen();
        }
        else {
            bottomNavigationView.setVisibility(View.VISIBLE);
            sketch = new Sketch();
        }
        PFragment fragment = new PFragment(sketch);
        fragment.setView(radarContainer, this);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode == RESULT_OK){
            // bluetooth enabled
        }else{
            // show error
        }
    }

    //Processing functions
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        if (sketch != null) {
            sketch.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    @Override
    public void onNewIntent(Intent intent) {
        if (sketch != null) {
            sketch.onNewIntent(intent);
        }
    }
}
