package com.example.spidersense;

import android.content.DialogInterface;
import android.os.Bundle;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;

import android.preference.PreferenceManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

public class SettingsFragment extends Fragment  implements View.OnClickListener {

    public SettingsFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        // Inflate the layout for this fragment
        View rootView = inflater.inflate(R.layout.fragment_settings, container, false);
        Button changeThemeBtn = rootView.findViewById(R.id.changeTheme);
        changeThemeBtn.setOnClickListener(this);
        return rootView;
    }

    @Override
    public void onClick(View v) {
        final int theme = PreferenceManager.getDefaultSharedPreferences(getActivity()).getInt("ActivityTheme", R.style.AppTheme);
        if(theme == R.style.AppTheme) PreferenceManager.getDefaultSharedPreferences(getActivity()).edit().putInt("ActivityTheme", R.style.AppThemeDark).commit();
        else PreferenceManager.getDefaultSharedPreferences(getActivity()).edit().putInt("ActivityTheme", R.style.AppTheme).commit();

        // setup the alert builder
        AlertDialog.Builder builder = new AlertDialog.Builder(this.getActivity());
        builder.setTitle("AlertDialog");
        builder.setMessage("It's necessary to restart the app for changing the theme!");

        // add the buttons
        builder.setPositiveButton("Restart now", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                getActivity().recreate();
                Toast.makeText(getActivity(), "Theme switched to " + ((theme == R.style.AppTheme) ? "dark" : "light"), Toast.LENGTH_SHORT).show();
            }
        });
        builder.setNegativeButton("Restart later", null);

        // create and show the alert dialog
        AlertDialog dialog = builder.create();
        dialog.show();

    }
}
