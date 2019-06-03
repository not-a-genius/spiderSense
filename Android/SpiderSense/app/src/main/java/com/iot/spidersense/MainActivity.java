package com.iot.spidersense;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.karumi.dexter.Dexter;
import com.karumi.dexter.listener.single.DialogOnDeniedPermissionListener;
import com.karumi.dexter.listener.single.PermissionListener;

import java.io.IOException;
import java.util.UUID;

import processing.android.PFragment;
import processing.core.PApplet;

public class MainActivity extends AppCompatActivity {

    private Activity activity;
    private PApplet sketch;
    private FrameLayout radarContainer;
    private LinearLayout settings;
    private SharedPreferences preferenceManager;
    private Boolean doubleBackToExitPressedOnce = false;
    private int theme;
    private Button changeThemeButton, telegramButton, startButton, stopButton;
    private TextView textview;

    private final static int REQUEST_ENABLE_BT = 1;
    private static final int PERMISSION_REQUEST_COARSE_LOCATION = 1;

    private String miMAC = "EB:54:5D:20:93:97";
    private String nucleoMAC = "C6:50:E7:03:82:BE";
    private final UUID PORT_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");//Serial Port Service ID
    private BluetoothManager btManager;
    private BluetoothAdapter btAdapter;
    private BluetoothLeScanner btScanner;
    private BluetoothGatt btGatt;
    private BluetoothDevice nucleo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        activity = this;

        // Gets the saved theme ID from SharedPrefs,
        // or uses default_theme if no theme ID has been saved
        preferenceManager = PreferenceManager.getDefaultSharedPreferences(this);
        theme = preferenceManager.getInt("ActivityTheme", R.style.AppTheme);
        // Set this Activity's theme to the saved theme
        setTheme(theme);

        android.support.v7.app.ActionBar actionbar = getSupportActionBar();
        actionbar.setDisplayShowHomeEnabled(true);
        actionbar.setIcon(R.drawable.ic_spider);

        int display_mode = getResources().getConfiguration().orientation;
        if (display_mode == Configuration.ORIENTATION_LANDSCAPE) {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
            actionbar.hide();
        }

        setContentView(R.layout.activity_main);

        radarContainer = findViewById(R.id.radar_container);
        settings = findViewById(R.id.settings);
        changeThemeButton = findViewById(R.id.changeThemeButton);
        telegramButton = findViewById(R.id.telegramButton);
        startButton = findViewById(R.id.startButton);
        stopButton = findViewById(R.id.stopButton);
        textview = findViewById(R.id.deviceView);
        textview.setMovementMethod(new ScrollingMovementMethod());

        //Set function change theme to button
        changeThemeButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if(theme == R.style.AppTheme) preferenceManager.edit().putInt("ActivityTheme", R.style.AppThemeDark).commit();
                else preferenceManager.edit().putInt("ActivityTheme", R.style.AppTheme).commit();

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

        telegramButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                String url = "https://guarded-mountain-88932.herokuapp.com/notification?device_id=deviceId";
                RequestQueue ExampleRequestQueue = Volley.newRequestQueue(activity);
                StringRequest ExampleStringRequest = new StringRequest(Request.Method.GET, url, new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        //This code is executed if the server responds, whether or not the response contains data.
                        //The String 'response' contains the server's response.
                        //You can test it by printing response.substring(0,500) to the screen.
                        //Log.i(, "Get request performed");
                    }
                }, new Response.ErrorListener() { //Create an error listener to handle errors appropriately.
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        //This code is executed if there is an error.
                    }
                });

                ExampleRequestQueue.add(ExampleStringRequest);

                Toast.makeText(activity, "Request done!", Toast.LENGTH_SHORT).show();
            }
        });
        startButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                startScanning();
                Toast.makeText(activity, "Scan started!", Toast.LENGTH_SHORT).show();
            }
        });
        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                btGatt.disconnect();
            }
        });

        //Navbar
        BottomNavigationView bottomNavigationView = findViewById(R.id.navigation);
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

        btManager = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);
        btAdapter = btManager.getAdapter();
        btScanner = btAdapter.getBluetoothLeScanner();

        //Enable bluetooth
        if (btAdapter != null && !btAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
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

    // Device scan callback.
    private ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            if(device.getAddress().equals(nucleoMAC)) {
                textview.append(device.getName()+"\n");
                stopScanning();
                nucleo = device;
                btGatt = device.connectGatt(getApplicationContext(), false, bleGattCallback);
            }
        }
    };

    public void startScanning() {
        System.out.println("start scanning");
        textview.setText("");
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                btScanner.startScan(leScanCallback);
            }
        });
    }

    public void stopScanning() {
        System.out.println("stopping scanning");
        textview.append("Stopped Scanning");
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                btScanner.stopScan(leScanCallback);
            }
        });
    }

    private BluetoothGattCallback bleGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            switch (newState) {
                case BluetoothProfile.STATE_CONNECTED:
                    Log.i("GattCallback", "Connected");
                    gatt.getServices();
                    break;
                case BluetoothProfile.STATE_DISCONNECTED:
                    Log.i("GattCallback", "Disconnected");
                    break;
            }
        }
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {

            //Now we can start reading/writing characteristics

        }
    };

    @Override
    public void onBackPressed() {
        //Checking for fragment count on backstack
        if (getSupportFragmentManager().getBackStackEntryCount() > 0) {
            getSupportFragmentManager().popBackStack();
        } else if (!doubleBackToExitPressedOnce) {
            radarContainer.setVisibility(View.VISIBLE);
            settings.setVisibility(View.GONE);
            this.doubleBackToExitPressedOnce = true;
            Toast.makeText(this,"Please click BACK again to exit.", Toast.LENGTH_SHORT).show();

            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    doubleBackToExitPressedOnce = false;
                }
            }, 2000);
        } else {
            super.onBackPressed();
            return;
        }
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
