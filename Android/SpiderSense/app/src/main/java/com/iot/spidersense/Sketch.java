package com.iot.spidersense;

import processing.core.PApplet;

public class Sketch extends PApplet {
    public void setup() {
        background(0);
    }

    public void draw() {
        background(0);
        if (mousePressed) {
            background(255);
        }
    }

    public void settings() {  fullScreen(); }

    public static void main(String[] passedArgs) {
        String[] appletArgs = new String[] { "Sketch" };
        if (passedArgs != null) {
            PApplet.main(concat(appletArgs, passedArgs));
        } else {
            PApplet.main(appletArgs);
        }
    }
}
