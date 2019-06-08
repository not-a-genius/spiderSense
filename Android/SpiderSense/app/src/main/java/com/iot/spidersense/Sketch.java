package com.iot.spidersense;

import processing.core.PApplet;

public class Sketch extends PApplet {

    private String noObject;
    private String screenMode = "normal";

    private float pixsDistance;
    private int iAngle, iDistance, rangeDistance = 40;


    public Sketch() {}

    public Sketch(String mode){
        this.screenMode=mode;
    }

    public void setAngle(int angle) {
        this.iAngle = angle;
    }

    public void setDistance(int distance) {
        this.iDistance = distance;
    }

    public void settings() {
        if(screenMode.equals("normal")) size (1024, 768); //Set screen resolution
        else if (screenMode.equals("fullscreen")) fullScreen();
        smooth();  //draw all with antialiasing
    }

    public void setup() {}

    public void draw() {
        fill(0, 191, 255);
        // simulating motion blur and slow fade of the moving line
        noStroke();
        fill(0, 4);
        rect(0, 0, width, height * 0.935f);

        fill(0, 191, 255); // blue color
        // calls the functions for drawing the radar
        drawRadar();
        drawLine();
        drawObject();
        drawText();
    }

    private void drawRadar() {
        pushMatrix();
        translate(width * 0.5f, height  * 0.926f); // moves the starting coordinates to new location
        noFill();
        strokeWeight(2);
        stroke(0, 191, 255);
        // draws the arc lines
        arc(0, 0, (width * 0.9375f), (width * 0.9375f), PI, TWO_PI);
        arc(0, 0, (width * 0.73f), (width * 0.73f), PI, TWO_PI);
        arc(0, 0, (width * 0.521f), (width * 0.521f), PI, TWO_PI);
        arc(0, 0, (width * 0.313f), (width * 0.313f), PI, TWO_PI);
        // draws the angle lines
        line(-width * 0.5f, 0, width * 0.5f, 0);
        line(0, 0, (-width * 0.5f) * cos(radians(30)), (-width * 0.5f) * sin(radians(30)));
        line(0, 0, (-width * 0.5f) * cos(radians(60)), (-width * 0.5f) * sin(radians(60)));
        line(0, 0, (-width * 0.5f) * cos(radians(90)), (-width * 0.5f) * sin(radians(90)));
        line(0, 0, (-width * 0.5f) * cos(radians(120)), (-width * 0.5f) * sin(radians(120)));
        line(0, 0, (-width * 0.5f) * cos(radians(150)), (-width * 0.5f) * sin(radians(150)));
        line((-width * 0.5f) * cos(radians(30)), 0, width * 0.5f, 0);
        popMatrix();
    }

    private void drawObject() {
        pushMatrix();
        translate(width * 0.5f, height * 0.926f); // moves the starting coordinates to new location
        strokeWeight(9);
        stroke(255, 10, 10); // red color
        pixsDistance = iDistance * (height * 0.020835f); // covers the distance from the sensor from cm to pixels
        // limiting the range to 40 cms
        if(iDistance < rangeDistance) line(pixsDistance * cos(radians(iAngle)), -pixsDistance * sin(radians(iAngle)), (width * 0.495f) * cos(radians(iAngle)), -(width * 0.495f) * sin(radians(iAngle))); // draws the object according to the angle and the distance
        popMatrix();
    }

    private void drawLine() {
        pushMatrix();
        strokeWeight(9);
        stroke(30, 144, 255);
        translate(width * 0.5f, height * 0.926f); // moves the starting coordinats to new location
        line(0, 0, (height * 0.88f) * cos(radians(iAngle)), -(height * 0.88f) * sin(radians(iAngle))); // draws the line according to the angle
        popMatrix();
    }

    private void drawText() { // draws the texts on the screen
        pushMatrix();

        if(iDistance > rangeDistance) noObject = "Out of Range";
        else noObject = "In Range";

        fill(0, 0, 0);
        noStroke();
        rect(0, height * 0.9352f, width, height);
        fill(0, 191, 255);
        textSize(25);

        text("10cm", width * 0.6146f, height * 0.9167f);
        text("20cm", width * 0.719f, height * 0.9167f);
        text("30cm", width * 0.823f, height * 0.9167f);
        text("40cm", width * 0.9271f, height * 0.9167f);
        textSize(40);
        text("Object: " + noObject, width * 0.025f, height * 0.9723f);
        text("Angle: " + iAngle +" °", width  * 0.43f, height * 0.9723f);
        text("Distance: " + iDistance +" cm", width * 0.7f, height * 0.9723f);
        textSize(25);
        fill(65, 105, 225);
        translate((width * 0.5006f) + width * 0.5f * cos(radians(30)), (height * 0.9093f) - width * 0.5f * sin(radians(30)));
        rotate(-radians(-60));
        text("30°", 0, 0);
        resetMatrix();
        translate((width * 0.497f) + width * 0.5f * cos(radians(60)), (height * 0.9112f) - width * 0.5f * sin(radians(60)));
        rotate(-radians(-30));
        text("60°", 0, 0);
        resetMatrix();
        translate((width * 0.493f) + width * 0.5f * cos(radians(90)), (height * 0.9167f) - width * 0.5f * sin(radians(90)));
        rotate(radians(0));
        text("90°", 0, 0);
        resetMatrix();
        translate(width * 0.487f + width * 0.5f * cos(radians(120)), (height * 0.92871f) - width * 0.5f * sin(radians(120)));
        rotate(radians(-30));
        text("120°", 0, 0);
        resetMatrix();
        translate((width * 0.4896f) + width * 0.5f * cos(radians(150)), (height * 0.9426f) - width * 0.5f * sin(radians(150)));
        rotate(radians(-60));
        text("150°", 0, 0);
        popMatrix();
    }
}