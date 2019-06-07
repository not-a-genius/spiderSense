package com.iot.spidersense;

import java.util.HashMap;

public class GattAttributes {
    private static HashMap<String, String> attributes = new HashMap();
    public static String DISTANCE_MEASUREMENT = "00009a99-0000-1000-8000-00805f9b34fb";
    public static String ANGLE_MEASUREMENT = "00009a90-0000-1000-8000-00805f9b34fb";
    public static String DISTANCE_SERVICE = "0000990d-0000-1000-8000-00805f9b34fb";
    public static String ANGLE_SERVICE = "0000990f-0000-1000-8000-00805f9b34fb";
    
    static {
        // Services
        attributes.put(DISTANCE_SERVICE, "Distance Service");
        attributes.put(ANGLE_SERVICE, "Angle Service");
        attributes.put("0000180a-0000-1000-8000-00805f9b34fb", "Device Information Service");
        // Characteristics
        attributes.put(DISTANCE_MEASUREMENT, "Distance Measurement");
        attributes.put(ANGLE_MEASUREMENT, "Angle Measurement");
        attributes.put("00002a29-0000-1000-8000-00805f9b34fb", "Manufacturer Name String");
    }
}
