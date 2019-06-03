package com.iot.spidersense;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
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
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.util.Set;
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
    private TextView textview;

    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothSocket socket;
    private OutputStream outputStream;
    private InputStream inputStream;
    private String miMAC = "EB:54:5D:20:93:97";
    private String nucleoMAC = "C6:50:E7:03:82:BE";
    private final UUID PORT_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");//Serial Port Service ID
    private BluetoothDevice nucleo;
    private boolean scanning;
    private BluetoothLeScanner scanner;
    private Handler handler;
    private final static int SCAN_PERIOD = 10000;
    boolean deviceConnected=false;
    Thread thread;
    byte buffer[];
    int bufferPosition;
    boolean stopThread;

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
        Button changeThemeButton = findViewById(R.id.changeThemeButton);
        Button telegramButton = findViewById(R.id.telegramButton);
        Button startButton = findViewById(R.id.startButton);
        Button stopButton = findViewById(R.id.stopButton);
        textview = findViewById(R.id.deviceView);

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

        //Enable bluetooth
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, 0);
        }
        startButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {

                scanner = mBluetoothAdapter.getBluetoothLeScanner();
                handler = new Handler();
                startBleScan();
                Toast.makeText(activity, "Scan started!", Toast.LENGTH_SHORT).show();
            }
        });
        stopButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                try {
                    Method method = nucleo.getClass().getMethod("removeBond", (Class[]) null);
                    method.invoke(nucleo, (Object[]) null);

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });


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

    public void startBleScan() {
        scanning = true;
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                scanner.startScan(leScanCallback);
            }
        });
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                scanner.stopScan(leScanCallback);
                scanning = false;
                Set<BluetoothDevice> bondedDevices = mBluetoothAdapter.getBondedDevices();
                if(!bondedDevices.contains(nucleo))  {
                    try {
                        Method method = nucleo.getClass().getMethod("createBond", (Class[]) null);
                        method.invoke(nucleo, (Object[]) null);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                boolean connected=true;
                try {
                    socket = nucleo.createRfcommSocketToServiceRecord(PORT_UUID);
                    socket.connect();
                } catch (IOException e) {
                    e.printStackTrace();
                    connected=false;
                }
                if(connected)
                {
                    try {
                        outputStream=socket.getOutputStream();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    try {
                        inputStream=socket.getInputStream();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    beginListenForData();
                }
            }
        }, SCAN_PERIOD);
    }
    private ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            Log.i("DEVICES", result.getDevice().getAddress());
            if(result.getDevice().getAddress().equals(nucleoMAC)) nucleo = result.getDevice();
        }
    };
    void beginListenForData()
    {
        final Handler handler = new Handler();
        stopThread = false;
        buffer = new byte[1024];
        Thread thread  = new Thread(new Runnable()
        {
            public void run()
            {
                while(!Thread.currentThread().isInterrupted() && !stopThread)
                {
                    try
                    {
                        int byteCount = inputStream.available();
                        if(byteCount > 0)
                        {
                            byte[] rawBytes = new byte[byteCount];
                            inputStream.read(rawBytes);
                            final String string=new String(rawBytes,"UTF-8");
                            handler.post(new Runnable() {
                                public void run()
                                {
                                    textview.setText(string);
                                    Log.i("DEBUG", string);
                                }
                            });

                        }
                    }
                    catch (IOException ex)
                    {
                        stopThread = true;
                    }
                }
            }
        });

        thread.start();
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
