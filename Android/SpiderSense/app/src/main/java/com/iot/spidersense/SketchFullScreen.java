package com.iot.spidersense;

import android.bluetooth.BluetoothAdapter;

import processing.core.PApplet;

public class SketchFullScreen extends PApplet {

    private String angle = "";
    private String distance = "";
    private String data = "";
    private String noObject;
    private float pixsDistance;
    private int iAngle, iDistance;
    private int index1 = 0;
    private int index2 = 0;
    private BluetoothAdapter mBluetoothAdapter;

    public void settings() {
        fullScreen(); //Set screen resolution
        smooth();  //draw all with antialiasing

    }

    public void setup() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    public void draw() {
        fill(0, 191, 255);
        // simulating motion blur and slow fade of the moving line
        noStroke();
        fill(0, 4);
        rect(0, 0, width, height - height * 0.065f);

        fill(0, 191, 255); // blue color
        // calls the functions for drawing the radar
        drawRadar();
        drawLine();
        drawObject();
        drawText();
    }

    void drawRadar() {
        pushMatrix();
        translate(width/2, height - height * 0.074f); // moves the starting coordinats to new location
        noFill();
        strokeWeight(2);
        stroke(0, 191, 255);
        // draws the arc lines
        arc(0, 0, (width - width * 0.0625f), (width - width * 0.0625f), PI, TWO_PI);
        arc(0, 0, (width - width * 0.27f), (width - width * 0.27f), PI, TWO_PI);
        arc(0, 0, (width - width * 0.479f), (width - width * 0.479f), PI, TWO_PI);
        arc(0, 0, (width - width * 0.687f), (width - width * 0.687f), PI, TWO_PI);
        // draws the angle lines
        line(-width/2, 0, width/2, 0);
        line(0, 0, (-width/2) * cos(radians(30)), (-width/2) * sin(radians(30)));
        line(0, 0, (-width/2) * cos(radians(60)), (-width/2) * sin(radians(60)));
        line(0, 0, (-width/2) * cos(radians(90)), (-width/2) * sin(radians(90)));
        line(0, 0, (-width/2) * cos(radians(120)), (-width/2) * sin(radians(120)));
        line(0, 0, (-width/2) * cos(radians(150)), (-width/2) * sin(radians(150)));
        line((-width/2) * cos(radians(30)), 0, width/2, 0);
        popMatrix();
    }

    void drawObject() {
        pushMatrix();
        translate(width/2, height - height * 0.074f); // moves the starting coordinats to new location
        strokeWeight(9);
        stroke(255, 10, 10); // red color
        pixsDistance = iDistance * ((height - height * 0.1666f) * 0.025f); // covers the distance from the sensor from cm to pixels
        // limiting the range to 40 cms
        if(iDistance < 40) line(pixsDistance * cos(radians(iAngle)), -pixsDistance * sin(radians(iAngle)), (width - width * 0.505f) * cos(radians(iAngle)), -(width - width * 0.505f) * sin(radians(iAngle))); // draws the object according to the angle and the distance
        popMatrix();
    }

    void drawLine() {
        pushMatrix();
        strokeWeight(9);
        stroke(30, 144, 255);
        translate(width/2, height - height * 0.074f); // moves the starting coordinats to new location
        line(0, 0, (height - height * 0.12f) * cos(radians(iAngle)), -(height - height * 0.12f) * sin(radians(iAngle))); // draws the line according to the angle
        popMatrix();
    }

    void drawText() { // draws the texts on the screen
        pushMatrix();

        if(iDistance > 40) noObject = "Out of Range";
        else noObject = "In Range";

        fill(0, 0, 0);
        noStroke();
        rect(0, height - height * 0.0648f, width, height);
        fill(0, 191, 255);
        textSize(25);

        text("10cm", width - width * 0.3854f, height - height * 0.0833f);
        text("20cm", width - width * 0.281f, height - height * 0.0833f);
        text("30cm", width - width * 0.177f, height - height * 0.0833f);
        text("40cm", width - width * 0.0729f, height - height * 0.0833f);
        textSize(40);
        text("Object: " + noObject, width - width * 0.875f, height - height * 0.0277f);
        text("Angle: " + iAngle +" °", width - width * 0.48f, height - height * 0.0277f);
        text("Distance: ", width - width * 0.26f, height - height * 0.0277f);
        if(iDistance < 40) text("        " + iDistance +" cm", width - width * 0.225f, height - height * 0.0277f);
        textSize(25);
        fill(65, 105, 225);
        translate((width - width * 0.4994f) + width/2 * cos(radians(30)), (height - height * 0.0907f) - width/2 * sin(radians(30)));
        rotate(-radians(-60));
        text("30°", 0, 0);
        resetMatrix();
        translate((width - width * 0.503f) + width/2 * cos(radians(60)), (height - height * 0.0888f) - width/2 * sin(radians(60)));
        rotate(-radians(-30));
        text("60°", 0, 0);
        resetMatrix();
        translate((width - width * 0.507f) + width/2 * cos(radians(90)), (height - height * 0.0833f) - width/2 * sin(radians(90)));
        rotate(radians(0));
        text("90°", 0, 0);
        resetMatrix();
        translate(width - width * 0.513f + width/2 * cos(radians(120)), (height - height * 0.07129f) - width/2 * sin(radians(120)));
        rotate(radians(-30));
        text("120°", 0, 0);
        resetMatrix();
        translate((width - width * 0.5104f) + width/2 * cos(radians(150)), (height - height * 0.0574f) - width/2 * sin(radians(150)));
        rotate(radians(-60));
        text("150°", 0, 0);
        popMatrix();
    }
}