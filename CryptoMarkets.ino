#include <Bridge.h>
#include <Temboo.h>
#include <HttpClient.h>
#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>
#include "TembooAccount.h"

void setup() {
  Tft.TFTinit();  //init TFT library
  Tft.setDisplayDirect(DOWN2UP);
  Tft.drawString("CryptoMarkets",0,320,1,WHITE);
  Tft.drawLine(8,320,8,0,WHITE);
  Tft.drawLine(230,320,230,0,WHITE);
  Tft.drawString("Powered by Vircurex API",232,250,1,WHITE);
  Tft.drawString("DOGE -> BTC",20,290,3,YELLOW);
  Tft.drawString("DOGE -> USD",90,290,3,YELLOW);
  Tft.drawString("Not Available.",114,290,1,WHITE);
  Bridge.begin();
}
void loop()
{
  //Serial.println("Running Get - Run #" + String(numRuns++));
  Tft.drawString("Updating...",0,90,1,RED);
  
  TembooChoreo GetChoreo;
  GetChoreo.begin();
  
  GetChoreo.setAccountName(TEMBOO_ACCOUNT);
  GetChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  GetChoreo.setAppKey(TEMBOO_APP_KEY);
  
  GetChoreo.addInput("URL", "https://api.vircurex.com/api/get_info_for_1_currency.xml?base=DOGE&alt=BTC");
  
  GetChoreo.setChoreo("/Library/Utilities/HTTP/Get");
  
  //Output filters
  GetChoreo.addOutputFilter("lowest-ask", "/hash/lowest-ask", "Response");
  GetChoreo.addOutputFilter("highest-bid", "/hash/highest-bid", "Response");
  GetChoreo.addOutputFilter("last-trade", "/hash/last-trade", "Response");
  GetChoreo.addOutputFilter("volume", "/hash/volume", "Response");

  //Run it!
  unsigned int returnCode = GetChoreo.run();
  
  //If we get a returnCode of zero, everything went just fine.
  if (returnCode == 0) {
    char lowestAsk[100]; //Char array for the lowest ask value.
    char highestBid[100]; //Char array for the highest bid value.
    char lastTrade[100]; //Char array for the last trade value.
    char volume[100]; //Char array for the volume value.
    
    while (GetChoreo.available()) {
      //Let's parse the GetChoreo's output data
      String identifier = GetChoreo.readStringUntil('\x1F');
      identifier.trim();
      String data = GetChoreo.readStringUntil('\x1E');
      data.trim();
      Tft.drawString("Updating...",0,90,1,BLACK);
      //Assign the value to the appropriate array
      if (identifier == "lowest-ask") {
        String msg = "Lowest Ask: "+data;
        msg.toCharArray(lowestAsk, sizeof(lowestAsk));
      } else if (identifier == "highest-bid") {
        String msg = "Highest Bid: "+data;
        msg.toCharArray(highestBid, sizeof(highestBid));
      } else if (identifier == "last-trade") {
        String msg = "Last Trade: "+data;
        msg.toCharArray(lastTrade, sizeof(lastTrade));
      } else if (identifier == "volume") {
        String msg = "Volume: "+data;
        msg.toCharArray(volume, sizeof(volume));
      }
    }
    //Let's now put all the info in the screen.
    Tft.fillScreen(44,82,80,290,BLACK);
    Tft.drawString(lastTrade,44,290,1,WHITE);
    Tft.drawString(lowestAsk,54,290,1,WHITE);
    Tft.drawString(highestBid,64,290,1,WHITE);
    Tft.drawString(volume,74,290,1,WHITE);
  } else {
    //There was an error of some kind. Should we care?
    while (GetChoreo.available()) {
      char c = GetChoreo.read();
      //TODO: Detailed info about the error.
      Tft.drawString("Updating...",0,90,1,BLACK);
      Tft.drawString("Error fetching data",0,120,1,RED);
      delay(10000);
      Tft.drawString("Error fetching data",0,120,1,BLACK);
    }
  }
  GetChoreo.close(); //TODO: Maybe I don't need to close the Choreo every time I use it.
  delay(60000); // wait one minute between Get calls
}
