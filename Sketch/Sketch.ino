#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include "AnotherIFTTTWebhook.h"
#include <afstandssensor.h>
#include <avr/wdt.h>

AfstandsSensor afstandssensor1(14, 16);
AfstandsSensor afstandssensor2(15, 17);


EthernetClient client;
EthernetServer server(80);

String HTTPRequest;


struct Settings
{
  int MinValue = 110;
  int MaxValue = 70;
  int SensorThreshhold = 90;
 
};

Settings MySettings;

int Sensor1 = 0;
int Sensor2 = 0;



bool Sensor1Reminder = false;
bool Sensor2Reminder = false;






void GetHTTPLocation (char *buf)
{
  IPAddress myIp= Ethernet.localIP();
  sprintf(buf, "Location: http://%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);
  Serial.print (F("HTTPLocation: "));
  Serial.println (buf);
}


void LoadSettings ()
{
  Serial.println (F("Loading Settings"));
  //EEPROM.put (0,MySettings);
  
  EEPROM.get(0, MySettings);

  if (isnan(MySettings.MaxValue))
  {
    Serial.println (F("No Default Values, write default"));
    Settings Default;
    EEPROM.put (0,Default);
    LoadSettings();
  }
  
  Serial.print (F("MaxValue: "));
  Serial.print (MySettings.MaxValue);
  Serial.print (F(" MinValue: "));
  Serial.print (MySettings.MinValue);
  Serial.print (F(" SensorThreshhold: "));
  Serial.print (MySettings.SensorThreshhold);
  Serial.print(F("\n"));
 
}


void CheckSensor ()
{

    int Threshhold = GetThreshholdAsPercent ();


    if (Sensor1 < Threshhold)
    {
      
      if (!Sensor1Reminder)
      {
        Serial.println (F("Sending Reminder!"));
        SendMail ();
        Sensor1Reminder = true;
      }
      else
      {
        
      }
    }
    else if (Sensor1 > Threshhold)
    {
      if (Sensor1Reminder)
      {
        Serial.println (F("Sensor 1 filled reset Reminder!"));
        Sensor1Reminder = false;
      }
    }


   if (Sensor2 < Threshhold)
      {
       
        if (!Sensor2Reminder)
        {
          Serial.println (F("Sending Reminder!"));
          SendMail ();
          Sensor2Reminder = true;
        }
        else
        {
         
        }
      }
      else if (Sensor2 > Threshhold)
      {
        if (Sensor2Reminder)
        {
          Serial.println (F("Sensor 2 filled reset Reminder!"));
          Sensor2Reminder = false;
        }
      }
}


void ReadSensors ()
{
    //Sensor 1
    int MaxValue = MySettings.MaxValue;
    int MinValue = MySettings.MinValue;
    int Range =  MinValue - MaxValue;
    
    int Sensor1Value = afstandssensor1.afstandCM();
    if (Sensor1Value == -1)
    {
      Serial.println ("Sensor 1 not found!");
    }
    else
    {

      Serial.print ("Sensor 1 in CM: ");
      Serial.print (Sensor1Value);
      Serial.print("\n");
  
      Sensor1Value =  MinValue - Sensor1Value; 
      float Sensor1ValuePercent = (float)Sensor1Value/Range*100;
      Sensor1 = Sensor1ValuePercent;
    }

    //Sensor 2
    int Sensor2Value = afstandssensor2.afstandCM();

    if (Sensor2Value == -1)
    {
      Serial.println ("Sensor 2 not found!");
    }
    else
    {
      
    

  
      Sensor2Value =  MinValue - Sensor2Value; 
      float Sensor2ValuePercent = (float)Sensor2Value/Range*100;
      Sensor2 = Sensor2ValuePercent;

    }
    Serial.print ("Sensor 1 in %: ");
    Serial.print (Sensor1);
    Serial.print("\n");


    

}

void SendMail ()
{

  String S1 = String(Sensor1);
  String S2 = String(Sensor2);
  
  char Sensor1Char[5];
  S1.toCharArray(Sensor1Char,5);
  char Sensor2Char[5];  
  S2.toCharArray(Sensor2Char,5);

  sprintf(Sensor1Char,"%i",Sensor1);
  sprintf(Sensor2Char,"%i",Sensor2);
  send_webhook("SendMailPellets","iS_O3iLLBtJTqHtGIv0UXdamgY3gp-WjrcefedBARAX",Sensor1Char,Sensor2Char,"value 3");


}

void SendHTTPResponseHeader(int Refresh)
{
  
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println(F("Connection: close"));  // the connection will be closed after completion of the response
  client.println("Refresh: "+ Refresh);  // refresh the page automatically every 5 sec
  client.println();
}


void WriteHTMLBodyBegin ()
{
  
  client.println(F("<!doctype html>"));
  client.println(F("<html lang=\"en\">"));
  client.println(F("<head>"));
  client.println(F( "<meta charset=\"utf-8\">"));
  client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
  client.println(F("<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">"));
  client.println(F("</head>"));
  client.println(F("<body>"));
}

int GetThreshholdAsPercent ()
{
    int MaxValue =  MySettings.MaxValue;
    int MinValue =  MySettings.MinValue;
    int SensorThreshhold =  MySettings.SensorThreshhold;
        
    int SensorThreshholdValuePercent = (float)SensorThreshhold/MinValue*100;
    return SensorThreshholdValuePercent;
}


void WriteIndexHTML ()
{
     client.println(F("<div class=\"container\">"));
     client.println(F("<div class=\"row\">"));
     client.println(F("<div class=\"col-12\">"));
     client.println(F("<div class=\"card\">"));
     client.println(F("<div class=\"card-body\">"));
     client.println(F("<canvas id=\"myChart\"></canvas>"));
     client.println(F("</div>"));
     client.println(F("</div>"));
     client.println(F("</div>"));
     client.println(F("</div>"));
     client.println(F("</div>"));
     client.println(F("</div>"));

     client.println (F("<p><a href=\"/Settings\" class=\"link-secondary\">Einstellungen</a></p>"));
     client.println(F("<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p\" crossorigin=\"anonymous\"></script>"));
     client.println(F("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js\"></script>"));

     client.println(F("<script>"));
     client.println(F("var xValues = [\"Behälter 1\", \"Behälter 2\"];"));
     client.println("var yValues = [" + (String)Sensor1 + ", " + (String)Sensor2 + "];");


    int SensorThreshholdValuePercent = GetThreshholdAsPercent();
     if (Sensor1 < SensorThreshholdValuePercent && Sensor2 > SensorThreshholdValuePercent)
     {
           client.println(F("var barColors = [ \"red\",\"green\"];"));
     }
     else if (Sensor1 > SensorThreshholdValuePercent && Sensor2 < SensorThreshholdValuePercent)
     {
        client.println(F("var barColors = [ \"green\",\"red\"];"));
     }
     else if (Sensor1 < SensorThreshholdValuePercent && Sensor2 < SensorThreshholdValuePercent)
     {
        client.println(F("var barColors = [ \"red\",\"red\"];"));
     }
     else
     {
        client.println(F("var barColors = [ \"green\",\"green\"];"));
     }
     client.println(F("new Chart(\"myChart\", {"));
     client.println(F("type: \"bar\","));
     client.println(F("data: {"));
     client.println(F("labels: xValues,"));
     client.println(F("datasets: [{"));
     client.println(F("label: \"Füllmenge in %\","));
     client.println(F("backgroundColor: barColors,"));
     client.println(F("data: yValues"));
     client.println(F("}]"));
     client.println(F("},"));
     client.println(F("options: {"));
     client.println(F("legend: {display: true,position: 'bottom'}"));
     client.println(F(","));
     client.println(F("title: {"));
     client.println(F("display: true,"));
     client.println(F("text:\"Behälter Füllmenge\""));
     client.println(F("},"));
     client.println(F("scales: {"));
     client.println(F("yAxes: [{"));
     client.println(F("display: true,"));
     client.println(F("stacked: true,"));
     client.println(F("ticks: {"));
     client.println(F("min: 0,"));
     client.println(F("max: 100"));
     client.println(F("}"));
     client.println(F("}]"));
     client.println(F("}"));
     client.println(F("}"));
     client.println(F("});"));
     client.println(F("</script>"));
           

     client.println(F("</body>"));
     client.println(F("</html>"));
}



void  GetSettingsfromHTTPRequest()
{
  int GetBegin = HTTPRequest.indexOf("GET");
  int GetEnd = HTTPRequest.indexOf('\n',GetBegin);
  String GetString = HTTPRequest.substring (GetBegin,GetEnd);
  HTTPRequest = "";
  Serial.print (F("Get: "));
  Serial.println (GetString);

  int MaxBegin = GetString.indexOf("MaxValue=");
  int MaxEnd = GetString.indexOf('&',MaxBegin);
  String MaxString = GetString.substring (MaxBegin+9,MaxEnd);

  int MinBegin = GetString.indexOf("minValue=");
  int MinEnd = GetString.indexOf('&',MinBegin);
  String MinString = GetString.substring (MinBegin+9,MinEnd);


  int ThreshBegin = GetString.indexOf("threshhold=");
  int ThreshEnd = GetString.indexOf(" HTTP/1.1",ThreshBegin);
  String ThreshString = GetString.substring (ThreshBegin+11,ThreshEnd);



  int tempMaxValue = MaxString.toInt();
  int tempMinValue = MinString.toInt();
  int tempSensorThreshhold =  ThreshString.toInt();
  
  Serial.print (F("MaxString: "));
  Serial.println (tempMaxValue);
  Serial.print (F("MinString: "));
  Serial.println (tempMinValue);
  Serial.print (F("ThreshString: "));
  Serial.println (tempSensorThreshhold);
  
  if (tempMaxValue <= 2 || tempMaxValue > 450 || tempMinValue <= 2 || tempMaxValue > 450 || tempSensorThreshhold <= 2 || tempSensorThreshhold > 450 )
  {
    Serial.print (F("Input not valid!\n")); 
    Serial.print (F("Chose Range from 2 - 450\n")); 
  }
  else
  {

    MySettings.MaxValue = tempMaxValue;
    MySettings.MinValue = tempMinValue;
    MySettings.SensorThreshhold =tempSensorThreshhold;

    EEPROM.put (0,MySettings);
    Serial.println (F("Input Set")); 
  }

  client.println (F("HTTP/1.1 303 See Other"));
  char HTTPLocation[30];
  GetHTTPLocation(HTTPLocation);
  client.println (HTTPLocation);
  
  



}
  




void WriteSettingsHTML ()
{

  
    client.println(F("<form method=\"GET\">"));
    client.println(F("<table class=\"table\">"));
    client.println(F("<tbody>"));
    client.println(F("<tr>"));
    client.println(F("<td>Wert</td>"));
    client.println(F("<td>Aktuell</td>"));
    client.println(F("<td>Neuer Wert</td>"));
    client.println(F("</tr>"));
    client.println(F("<tr>"));
    client.println(F("<td>MaxSensorValue</td>"));
    client.println("<td>" + (String)MySettings.MaxValue +"</td>");
    client.println("<td><input type=\"text\" name=\"MaxValue\" value=\""+ (String)MySettings.MaxValue +"\"></td>");
    client.println(F("</tr>"));
    client.println(F("<tr>"));
    client.println(F("<td>MinSensorValue</td>"));
    client.println("<td>" + (String)MySettings.MinValue + "</td>");
    client.println("<td><input type=\"text\" name=\"minValue\" value=\""+ (String)MySettings.MinValue +"\"></td>");
    client.println(F("</tr>"));
     client.println(F("<tr>"));
    client.println(F("<td>Sensor Threshhold</td>"));
    client.println("<td>" + (String)MySettings.SensorThreshhold + "</td>");
    client.println("<td><input type=\"text\" name=\"threshhold\" value=\""+ (String)MySettings.SensorThreshhold +"\"></td>");
    client.println(F("</tr>"));
    client.println(F("</tbody>"));
    client.println(F("</table>"));
    client.println(F("<button type=\"submit\" class=\"btn btn-primary\">Bestätigen</button>"));
    client.println(F("</form>"));
    client.println (F("<p><a href=\"/Reset\" class=\"link-secondary\">Reset</a></p>"));
  
     client.println(F("</body>"));
     client.println(F("</html>"));
}


void HandleServer ()
{
   client = server.available();
  if (client) {
    Serial.println(F("new client"));


    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        HTTPRequest += c;

        
        if (c == '\n')
        {
      



          if (HTTPRequest.indexOf("GET /Settings?") >= 0)
          {
            Serial.print (HTTPRequest);
            Serial.println (F("Set Settings"));
            GetSettingsfromHTTPRequest();
          }
          else if (HTTPRequest.indexOf("GET /Reset") >= 0) 
          {
              Serial.print (HTTPRequest);
              Serial.println(F("Reset Settings"));
              Settings NewSettings;
              MySettings.MaxValue = NewSettings.MaxValue;
              MySettings.MinValue = NewSettings.MinValue;
              MySettings.SensorThreshhold =NewSettings.SensorThreshhold;
              EEPROM.put(0,MySettings);

              client.println (F("HTTP/1.1 303 See Other"));
              char HTTPLocation[30];
              GetHTTPLocation(HTTPLocation);
              client.println (HTTPLocation);
          }
          else if (HTTPRequest.indexOf("GET /Settings") >= 0) 
          {
               Serial.print (HTTPRequest);
               Serial.println(F("Show Settings"));
               SendHTTPResponseHeader(60);
               WriteHTMLBodyBegin();
               WriteSettingsHTML();
            
          }
          else if (HTTPRequest.indexOf("GET /favicon.ico") >= 0) 
          {
              

          }

          else if (HTTPRequest.indexOf("GET") >= 0) 
          {
              Serial.print (HTTPRequest);
              Serial.println(F("Show Index"));
             SendHTTPResponseHeader(5);
             WriteHTMLBodyBegin();
             WriteIndexHTML ();
          }
          HTTPRequest = "";
          break;
  
        }
        
      }
    }
    // give the web browser time to receive the data
    delay(200);
    HTTPRequest = "";
    // close the connection:
    client.stop();

  }

  delay (500);
}





byte mac[] = {
  0xA8, 0x61, 0x0A, 0xAE, 0x72,0xEF
};




void setup() {

 //Init 
  Serial.begin(9600);
  // Init Watchdog 4 s
  wdt_enable(WDTO_4S);
  LoadSettings();
  
  Ethernet.begin(mac);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());



  
}


void loop() {

  //Reset Watchdog 
  wdt_reset();

  ReadSensors ();

  CheckSensor ();

  HandleServer();
  delay (1000);
  
 
}
