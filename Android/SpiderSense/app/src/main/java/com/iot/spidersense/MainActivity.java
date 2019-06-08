package com.iot.spidersense;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.location.Location;
import android.location.LocationManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
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

import java.util.UUID;

import processing.android.PFragment;

public class MainActivity extends AppCompatActivity {

    private final String TAG = "DEBUG"; //For debug

    // Activity variables
    private Activity activity;
    private Sketch sketch;
    private FrameLayout radarContainer;
    private LinearLayout settings;
    private SharedPreferences preferenceManager;
    private Boolean doubleBackToExitPressedOnce = false;
    private int theme;
    private Button changeThemeButton, stopAlertButton, connectButton, disconnectButton, saveButton;
    private TextView textview, timerView;
    private BottomNavigationView bottomNavigationView;
    private android.support.v7.app.ActionBar actionbar;
    private EditText nameText;

    // BLE
    private final static int REQUEST_ENABLE_BT = 1;
    private final String nucleoMAC = "C6:50:E7:03:82:BE";
    private final static UUID UUID_DISTANCE_MEASUREMENT = UUID.fromString(GattAttributes.DISTANCE_MEASUREMENT);
    private final static UUID UUID_ANGLE_MEASUREMENT = UUID.fromString(GattAttributes.ANGLE_MEASUREMENT);
    private BluetoothManager btManager;
    private BluetoothAdapter btAdapter;
    private BluetoothLeScanner btScanner;
    private BluetoothGatt btGatt;

    // Alert
    private int numOfPresence = 0, presenceTreshold = 20, rangeDistance = 40, countdownTimer = 10;
    private boolean sentAlert = false;
    private LocationManager locationManager;
    private Handler alertHandler, timerHandler;

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

        actionbar = getSupportActionBar();
        actionbar.setDisplayShowHomeEnabled(true);
        actionbar.setIcon(R.drawable.ic_spider);

        setContentView(R.layout.activity_main);

        radarContainer = findViewById(R.id.radar_container);
        settings = findViewById(R.id.settings);
        changeThemeButton = findViewById(R.id.changeThemeButton);
        stopAlertButton = findViewById(R.id.stopAlertButton);
        connectButton = findViewById(R.id.connectButton);
        disconnectButton = findViewById(R.id.disconnectButton);
        saveButton = findViewById(R.id.saveNameButton);
        textview = findViewById(R.id.deviceView);
        timerView = findViewById(R.id.timer);
        nameText = findViewById(R.id.name);

        //Set listeners to buttons
        changeThemeButton.setOnClickListener(changeThemeListener);
        stopAlertButton.setOnClickListener(stopAlertListener);
        connectButton.setOnClickListener(connectListener);
        disconnectButton.setOnClickListener(disconnectListener);
        saveButton.setOnClickListener(saveListener);

        //Navbar
        bottomNavigationView = findViewById(R.id.navigation);
        bottomNavigationView.setOnNavigationItemSelectedListener(navbarListener);

        //Request location permission
        PermissionListener dialogPermissionListener = DialogOnDeniedPermissionListener.Builder
                        .withContext(this.getApplicationContext())
                        .withTitle("Location permission")
                        .withMessage("Location permission is needed to use bluetooth!")
                        .withButtonText(android.R.string.ok)
                        .withIcon(R.drawable.ic_location)
                        .build();
        Dexter.withActivity(this).withPermission(Manifest.permission.ACCESS_FINE_LOCATION).withListener(dialogPermissionListener).check();

        btManager = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);
        btAdapter = btManager.getAdapter();
        btScanner = btAdapter.getBluetoothLeScanner();

        //Enable bluetooth
        if (btAdapter != null && !btAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }

        //Enable gps
        locationManager = (LocationManager) getSystemService(LOCATION_SERVICE);
        if(!locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
            MyLocation.enableGPS(activity);
        }

        // User's data
        SharedPreferences.Editor editor = preferenceManager.edit();
        boolean firstTime = preferenceManager.getBoolean("firstTime", false);
        if (!firstTime) {
            editor.putBoolean("firstTime", true);
            editor.putString("name", "");
            editor.commit();
        }
        nameText.setText(preferenceManager.getString("name", null));

        //Processing
        sketch = new Sketch();
        PFragment fragment = new PFragment(sketch);
        fragment.setView(radarContainer, this);

        alertHandler = new Handler(Looper.getMainLooper());
        timerHandler = new Handler(Looper.getMainLooper());
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            bottomNavigationView.setVisibility(View.GONE);
            View decorView = getWindow().getDecorView();
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                                            // Set the content to appear under the system bars so that the
                                            // content doesn't resize when the system bars hide and show.
                                            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                            // Hide the nav bar and status bar
                                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                            | View.SYSTEM_UI_FLAG_FULLSCREEN);
            sketch = new Sketch("fullscreen");
            PFragment fragment = new PFragment(sketch);
            fragment.setView(radarContainer, this);
        }
        else {
            bottomNavigationView.setVisibility(View.VISIBLE);
            View decorView = getWindow().getDecorView();
            decorView.setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
            sketch = new Sketch();
            PFragment fragment = new PFragment(sketch);
            fragment.setView(radarContainer, this);
        }

    }

    // Device scan callback.
    private ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            if(device.getAddress().equals(nucleoMAC)) { // Nucleo device found
                btScanner.stopScan(leScanCallback);
                textview.append("Connected to: " + device.getName()+"\n");
                btGatt = device.connectGatt(getApplicationContext(), false, bleGattCallback);
            }
        }
    };

    // Function that starts the scanning of Nucleo device
    public void startScanning() {
        Log.d(TAG, "Start scanning");
        textview.setText("");
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                if(btScanner == null) btScanner = btAdapter.getBluetoothLeScanner();
                btScanner.startScan(leScanCallback);
            }
        });
    }

    // Main BLE device callback where much of the logic occurs.
    private final BluetoothGattCallback bleGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.d(TAG, "Connected to GATT server.");
                // Attempts to discover services after successful connection.
                Log.d(TAG, "Attempting to start service discovery:" + btGatt.discoverServices());

            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Log.d(TAG, "Disconnected from GATT server.");
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                for(BluetoothGattService service : gatt.getServices()) {
                    Log.d(TAG, "Service: " + service.getUuid());
                    for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
                        Log.d(TAG, "Characteristic: " + characteristic.getUuid());
                        gatt.setCharacteristicNotification(characteristic, true);
                        gatt.readCharacteristic(characteristic);
                    }
                }
            } else {
                Log.d(TAG, "onServicesDiscovered received: " + status);
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                receiveData(characteristic);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            receiveData(characteristic);
        }
    };

    // Function that receives data from Nucleo board
    private void receiveData(final BluetoothGattCharacteristic characteristic) {
        // Receiving distance
        if (UUID_DISTANCE_MEASUREMENT.equals(characteristic.getUuid())) {
            int format = BluetoothGattCharacteristic.FORMAT_UINT8;
            Log.d(TAG, "Distance format UINT8.");
            final int distance = characteristic.getIntValue(format, 0);
            Log.d(TAG, "Received distance: " + distance);
            sketch.setDistance(distance);
            if(distance < rangeDistance)
                numOfPresence++;  //one detection more to count
            Log.d(TAG, "" + numOfPresence);
            checkThreat();
        }
        // Receiving angle
        else if (UUID_ANGLE_MEASUREMENT.equals(characteristic.getUuid())) {
            int format = BluetoothGattCharacteristic.FORMAT_UINT8;
            Log.d(TAG, "Angle format UINT8.");
            final int angle = characteristic.getIntValue(format, 0);
            Log.d(TAG, "Received angle: " + angle);
            sketch.setAngle(angle);

            if(angle == 0 || angle == 165) {
                numOfPresence = 0;    //reset counter of detection
                sentAlert = false;
            }
        }
        else {
            Log.d(TAG,"Profile not recognized");
            // For all other profiles, writes the data formatted in HEX.
            final byte[] data = characteristic.getValue();
            if (data != null && data.length > 0) {
                final StringBuilder stringBuilder = new StringBuilder(data.length);
                for(byte byteChar : data)
                    stringBuilder.append(String.format("%02X ", byteChar));

            }
        }

    }

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

    // Function that starts the countdown if a threat is detected
    private void checkThreat(){
        if(numOfPresence > presenceTreshold) {
            stopAlertButton.setEnabled(true);
            stopAlertButton.refreshDrawableState();
            if(!sentAlert) {
                Log.d(TAG,"Sending to telegram");
                alertHandler.postDelayed(alertRunnable, countdownTimer * 1000);
                timerHandler.postDelayed(timerRunnable, 1000);
                sentAlert = true;
            }
        }
    }

    // Function that sends Name and position to telegram bot
    private void sendToTelegram(){
        String url;
        String name = preferenceManager.getString("name", null);
        if (ContextCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED
                || ContextCompat.checkSelfPermission(this, android.Manifest.permission.ACCESS_COARSE_LOCATION) == PackageManager.PERMISSION_GRANTED) {
            double latitude = 0, longitude = 0;
            LocationManager locManager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);
            locManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000L, 500.0f, MyLocation.locationListener, Looper.getMainLooper());
            Location myLocation = MyLocation.getLastKnownLocation(getApplicationContext());
            if (myLocation != null) {
                latitude = myLocation.getLatitude();
                longitude = myLocation.getLongitude();
                url = "https://guarded-mountain-88932.herokuapp.com/notification?device_id=deviceId&name=" + name +"&lat=" + latitude + "&lon=" + longitude;
            }
            else url = "https://guarded-mountain-88932.herokuapp.com/notification?device_id=deviceId&name=" + name;
        }
        else url = "https://guarded-mountain-88932.herokuapp.com/notification?device_id=deviceId&name=" + name;
        RequestQueue ExampleRequestQueue = Volley.newRequestQueue(activity);
        StringRequest ExampleStringRequest = new StringRequest(Request.Method.GET, url, new Response.Listener<String>() {
            @Override
            public void onResponse(String response) {
                //This code is executed if the server responds, whether or not the response contains data.
                //The String 'response' contains the server's response.
            }
        }, new Response.ErrorListener() { //Create an error listener to handle errors appropriately.
            @Override
            public void onErrorResponse(VolleyError error) {
                //This code is executed if there is an error.
            }
        });
        ExampleRequestQueue.add(ExampleStringRequest);
        timerView.setText("Alert sent!");
    }

    // Button listeners
    private View.OnClickListener changeThemeListener = new View.OnClickListener() {
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
    };

    private View.OnClickListener stopAlertListener = new View.OnClickListener() {
        public void onClick(View v) {
            stopAlertButton.setEnabled(false);
            stopAlertButton.refreshDrawableState();
            alertHandler.removeCallbacks(alertRunnable);
            timerHandler.removeCallbacks(timerRunnable);
            timerView.setText("10s");
            countdownTimer = 10;
            numOfPresence = 0;
        }
    };

    private View.OnClickListener connectListener = new View.OnClickListener() {
        public void onClick(View v) {
            startScanning();
            connectButton.setVisibility(View.GONE);
            disconnectButton.setVisibility(View.VISIBLE);
        }
    };

    private View.OnClickListener disconnectListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            connectButton.setVisibility(View.VISIBLE);
            disconnectButton.setVisibility(View.GONE);
            textview.setText("");
            if(btGatt != null) {
                btGatt.disconnect();
                btGatt.close();
                btGatt = null;
            }
        }
    };

    private View.OnClickListener saveListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            SharedPreferences.Editor editor = preferenceManager.edit();
            editor.putString("name", nameText.getText().toString());
            editor.commit();
            Toast.makeText(activity, "Name saved!", Toast.LENGTH_SHORT).show();
        }
    };

    // Navbar Listener
    private BottomNavigationView.OnNavigationItemSelectedListener navbarListener = new BottomNavigationView.OnNavigationItemSelectedListener() {
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
    };

    // Handler runnables
    private Runnable alertRunnable = new Runnable() {
        @Override
        public void run() {
            sendToTelegram();
        }
    };

    private Runnable timerRunnable = new Runnable() {
        @Override
        public void run() {
            countdownTimer--;
            timerView.setText(countdownTimer + "s");
            if(countdownTimer != 0) timerHandler.postDelayed(timerRunnable, 1000);
        }
    };
}
