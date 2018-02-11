/*
 * Project Tracker
 * Description:
    This project is to update the Geo-Coordinates every 10 minutes to the Particle cloud fetch from the GPS Module.
 *  If the GPS module is fail to give the fix. It will fall back to the Google Maps Device Locator and detect
 *  the Geo-Coordinates based on the cellar tower location.
 * Author: Rajiv M.
 * Date:10/02/2018
 */

 //Particle core Library
 #include "Particle.h"
 //Google Maps Device Locator Library.
 #include <google-maps-device-locator.h>
 //AssetTracker Library
 #include <AssetTracker.h>
 //---------------------------------------------//

 // Global objects
 // Creating an AssetTracker object.
 AssetTracker t = AssetTracker();
 // Creating GoogleMapsDeviceLocator object.
 GoogleMapsDeviceLocator locator;
 // Creating FuelGauge object.
 FuelGauge batteryMonitor;
//-----------------------------------------------//

//Global variables.
 // Used to keep track of the last time we published data
 long lastPublish = 0;
 // Publish Every 10 minutes.
 int delayMinutes = 10;

 //Declare a callback method to listen the callback
 void locationCallback(float lat, float lon, float accuracy);

 // setup() and loop() are both required. setup() runs once when the device starts
 // and is used for registering functions and variables and initializing things
 void setup() {
   // Sets up all the necessary AssetTracker bits
   t.begin();
   // Enable the GPS module. Defaults to off to save power.
   // Takes 1.5s or so because of delays.
   t.gpsOn();
   //subscribe for the location callback.
   locator.withSubscribe(locationCallback);
   // Opens up a Serial port so you can listen over USB
   Serial.begin(9600);
 }

 void loop() {
     // You'll need to run this every loop to capture the GPS output
     t.updateGPS();
      // if the current time - the last time we published is greater than your set delay...
     if ((millis()-lastPublish > delayMinutes*60*1000)) {
         // Remember when we published
         lastPublish = millis();
         // GPS requires a "fix" on the satellites to give good data,
         // so we should only publish data if there's a fix
         if (t.gpsFix()) {
           // Short publish Geo-Coordinates
           publishToCloud (t.readLatLon());
         } else {
           //Loop GoogleMapsDeviceLocator
           locator.loop();
           locator.publishLocation();
         }
     }
 }

// Location callback to get the Geo-Coordinates
 void locationCallback(float lat, float lon, float accuracy) {
    String latLon = String::format("%f,%f",lat,lon);
    publishToCloud (latLon);
  }

//To get the Battery Percentage.
  String getBatteyPercentage(){
    return String::format("%.2f",batteryMonitor.getSoC());
  }

//To publish the values to Particle cloud.
  void publishToCloud(String latLon){
    // Short publish Geo-Coordinates
    Particle.publish("geocode", latLon, 60, PRIVATE);
    // Short publish Battery Percentage.
    Particle.publish("batteryPercent", getBatteyPercentage(), 60, PRIVATE);
  }
