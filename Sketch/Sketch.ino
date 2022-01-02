#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>


String HTTP_METHOD = "GET";
// "GET /trigger/SendMailPellets/with/key/iS_O3iLLBtJTqHtGIv0UXdamgY3gp-WjrcefedBARAX?value1=27 HTTP/1.1"
String WebSocketHost = "maker.ifttt.com";
String WebsocketURL  = "/trigger/SendMailPellets/with/key/iS_O3iLLBtJTqHtGIv0UXdamgY3gp-WjrcefedBARAX";


EthernetClient client;
EthernetClient Webclient;
EthernetServer server(80);

String HTTPRequest;


struct Settings
{
  float MaxValue = 2.75f;
  float MinValue = 0.5f;
  float SensorThreshhold = 0.75f;
 
};

Settings MySettings;

int Sensor1 = 0;
int Sensor2 = 0;



bool Sensor1Reminder = false;
bool Sensor2Reminder = false;



void LoadSettings ()
{
  Serial.println (F("Loading Settings"));
// EEPROM.put (0,MySettings);
  
  EEPROM.get(0, MySettings);

  if (isnan(MySettings.MaxValue))
  {
    Serial.println (F("No Default Values, write default"));
    Settings Default;
    EEPROM.put (0,Default);
    LoadSettings();
  }
  
  Serial.print (F("MaxValue: "));
  Serial.print (MySettings.MaxValue,2);
  Serial.print (F(" MinValue: "));
  Serial.print (MySettings.MinValue,2);
  Serial.print (F(" SensorThreshhold: "));
  Serial.print (MySettings.SensorThreshhold,2);
  Serial.print(F("\n"));
 
}


void ReadSensors ()
{
    //Sensor 1
    int MaxValueBit = float(1023 / 5) * MySettings.MaxValue;
    int MinValueBit =  float(1023 / 5) * MySettings.MinValue;
    int Range = MaxValueBit - MinValueBit;
    
    int Sensor1Value = analogRead (A0);

    Sensor1Value = Sensor1Value - MinValueBit; 
    float Sensor1ValuePercent = (float)Sensor1Value/Range*100;
    Sensor1 = Sensor1ValuePercent;




    //Sensor 2
    MaxValueBit = float(1023 / 5) * MySettings.MaxValue;
    MinValueBit =  float(1023 / 5) * MySettings.MinValue;
    Range = MaxValueBit - MinValueBit;
    
    int Sensor2Value = analogRead (A0);


    Sensor2Value = Sensor2Value - MinValueBit; 
    float Sensor2ValuePercent = (float)Sensor2Value/Range*100;
    Sensor2 = Sensor2ValuePercent;

  
    

}

void SendMail (int Value1, int Value2)
{
    IPAddress ip;
    ip = ip.fromString(WebSocketHost);

    if ( Webclient.connect(ip,80)) {
    // if connected:
    Serial.println(F("Connected to server"));
    // make a HTTP request:
    // send HTTP header
    Webclient.println(HTTP_METHOD + " " + WebsocketURL + "?value1=" + Value1 + "&value2=" + Value2 + " HTTP/1.1");
    Webclient.println("Host: " + String(WebSocketHost));
    Webclient.println(F("Connection: close"));
    Webclient.println(); // end HTTP header

    while (Webclient.connected()) {
      if (Webclient.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = Webclient.read();
        Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    Webclient.stop();
    Serial.println(F("disconnected"));
  } else {// if not connected:
    Serial.println(F("connection failed"));
  }
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
     client.println(F("var barColors = [ \"green\",\"blue\"];"));
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

String DisplayAddress(IPAddress address)
{
 return String(address[0]) + "." + 
        String(address[1]) + "." + 
        String(address[2]) + "." + 
        String(address[3]);
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



  float tempMaxValue = MaxString.toFloat();
  float tempMinValue = MinString.toFloat();
  float tempSensorThreshhold =  ThreshString.toFloat();
  
  Serial.print (F("MaxString: "));
  Serial.println (tempMaxValue);
  Serial.print (F("MinString: "));
  Serial.println (tempMinValue);
  Serial.print (F("ThreshString: "));
  Serial.println (tempSensorThreshhold);
  
  if (tempMaxValue <= 0 || tempMaxValue > 5 || tempMinValue <= 0 || tempMaxValue > 5 || tempSensorThreshhold <= 0 || tempSensorThreshhold > 5 )
  {
    Serial.print (F("Input not valid!")); 
  }
  else
  {

    MySettings.MaxValue = tempMaxValue;
    MySettings.MinValue = tempMinValue;
    MySettings.SensorThreshhold =tempSensorThreshhold;

    EEPROM.put (0,MySettings);
    Serial.println (F("Input Set")); 
  }

  Serial.println (F("HTTP/1.1 301 Moved Permanently"));
  char buffer [15];
  sprintf(buffer,"%s",Ethernet.localIP());
  String IP = buffer;
  Serial.println (IP);
  Serial.println ("Location: http://" + IP);
  



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
    client.println (F("<p><a href=\"/Settings/Reset\" class=\"link-secondary\">Reset</a></p>"));
  
     client.println(F("</body>"));
     client.println(F("</html>"));
}


void HandleServer ()
{
   client = server.available();
  if (client) {
    Serial.println(F("new client"));

    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        HTTPRequest += c;

        
        if (c == '\n' && currentLineIsBlank) {
      
      
Serial.println (HTTPRequest);
          

          if (HTTPRequest.indexOf("GET /Settings?") >= 0)
          {
            Serial.println (HTTPRequest);
            Serial.println (F("Set Settings"));
            GetSettingsfromHTTPRequest();
          }
          else if (HTTPRequest.indexOf("GET /Settings/Reset") >= 0) 
          {
              
              Serial.println(F("Reset Settings"));
              Settings NewSettings;
              MySettings.MaxValue = NewSettings.MaxValue;
              MySettings.MinValue = NewSettings.MinValue;
              MySettings.SensorThreshhold =NewSettings.SensorThreshhold;
              EEPROM.put(0,MySettings);

              client.println (F("HTTP/1.1 301 Moved Permanently"));
              client.println ("Location: http://" + DisplayAddress(Ethernet.localIP()) + "/Settings");
          }
          else if (HTTPRequest.indexOf("GET /Settings") >= 0) 
          {
               Serial.println (HTTPRequest);
               Serial.println(F("Show Settings"));
               SendHTTPResponseHeader(60);
               WriteHTMLBodyBegin();
               WriteSettingsHTML();
            
          }
          else if (HTTPRequest.indexOf("GET /favicon.ico") >= 0) 
          {
              

          }

          else
          {
              Serial.println (HTTPRequest);
              Serial.println(F("Show Index"));
             SendHTTPResponseHeader(5);
             WriteHTMLBodyBegin();
             WriteIndexHTML ();
          }


         
 
       
           




          
          break;

          
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
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
  Ethernet.init(10);
  Serial.begin(9600);
  LoadSettings();
  
  Ethernet.begin(mac);
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println(F("Ethernet cable is not connected."));
  }

  
  server.begin();
  Serial.print(F("server is at "));
  Serial.println(Ethernet.localIP());



  
}


void loop() {

  ReadSensors ();
  HandleServer();
  
 
}
