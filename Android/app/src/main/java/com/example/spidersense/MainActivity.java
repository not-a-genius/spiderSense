package com.example.spidersense;

import androidx.appcompat.app.AppCompatActivity;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.NavigationUI;

import android.os.Bundle;
import android.preference.PreferenceManager;

import com.google.android.material.bottomnavigation.BottomNavigationView;

public class MainActivity extends AppCompatActivity {

    private NavController navController;
    private BottomNavigationView bottomNav;

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

    }

    @Override
    public boolean onSupportNavigateUp() {
        return NavigationUI.navigateUp(navController, (DrawerLayout) null);
    }
}
