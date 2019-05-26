package com.example.spidersense;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.preference.PreferenceManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

public class SettingsFragment extends Fragment  implements View.OnClickListener {

    private Button changeThemeBtn;


    public SettingsFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        // Inflate the layout for this fragment
        View rootView =  inflater.inflate(R.layout.fragment_settings, container, false);
        changeThemeBtn = rootView.findViewById(R.id.changeTheme);
        changeThemeBtn.setOnClickListener(this);
        return rootView;
    }

    @Override
    public void onClick(View v) {
        int theme = PreferenceManager.getDefaultSharedPreferences(getActivity()).getInt("ActivityTheme", R.style.AppTheme);
        if(theme == R.style.AppTheme) PreferenceManager.getDefaultSharedPreferences(getActivity()).edit().putInt("ActivityTheme", R.style.AppThemeDark).commit();
        else PreferenceManager.getDefaultSharedPreferences(getActivity()).edit().putInt("ActivityTheme", R.style.AppTheme).commit();
        getActivity().recreate();
        Toast.makeText(getActivity(), "Theme switched to " + ((theme == R.style.AppTheme) ? "dark" : "light"), Toast.LENGTH_SHORT).show();
    }
}
