package com.example.spidersense;

import androidx.appcompat.app.AppCompatActivity;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.NavigationUI;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.Bundle;
import android.preference.PreferenceManager;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.karumi.dexter.Dexter;
import com.karumi.dexter.listener.single.DialogOnDeniedPermissionListener;
import com.karumi.dexter.listener.single.PermissionListener;

public class MainActivity extends AppCompatActivity {

    private NavController navController;
    private BottomNavigationView bottomNav;
    private BluetoothAdapter mBluetoothAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Gets the saved theme ID from SharedPrefs,
        // or uses default_theme if no theme ID has been saved
        int theme = PreferenceManager.getDefaultSharedPreferences(this).getInt("ActivityTheme", R.style.AppTheme);
        // Set this Activity's theme to the saved theme
        setTheme(theme);
        setContentView(R.layout.activity_main);

        //Getting the Navigation Controller and Bottom Navigation
        navController = Navigation.findNavController(this, R.id.fragment);
        bottomNav = findViewById(R.id.bottomNav);

        //Setting the navigation controller to Bottom Nav
        NavigationUI.setupWithNavController(bottomNav, navController);

        //Setting up the action bar
        NavigationUI.setupActionBarWithNavController(this, navController);

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

    }

    @Override
    public boolean onSupportNavigateUp() {
        return NavigationUI.navigateUp(navController, (DrawerLayout) null);
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
}
