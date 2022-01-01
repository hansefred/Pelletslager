#include <SPI.h>
#include <Ethernet.h>


String HTTP_METHOD = "GET";
char HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME  = "/trigger/SendMailPellets/with/key/iS_O3iLLBtJTqHtGIv0UXdamgY3gp-WjrcefedBARAX";


#define Request "GET /trigger/SendMailPellets/with/key/iS_O3iLLBtJTqHtGIv0UXdamgY3gp-WjrcefedBARAX?value1=27 HTTP/1.1"

//#define Webhock https://maker.ifttt.com/trigger/SendMailPellets/with/key/iS_O3iLLBtJTqHtGIv0UXdamgY3gp-WjrcefedBARAX

EthernetClient client;
EthernetClient Webclient;
EthernetServer server(80);


int Sensor1 = 0;
int Sensor2 = 0;

float MaxValue = 2.75f;
float MinValue = 0.5f;

bool Sensor1Reminder = false;
bool Sensor2Reminder = false;



void ReadSensors ()
{
    //Sensor 1
    int MaxValueBit = float(1023 / 5) * MaxValue;
    int MinValueBit =  float(1023 / 5) * MinValue;
    int Range = MaxValueBit - MinValueBit;
    
    int Sensor1Value = analogRead (A0);
    Serial.print ("Sensor Value: ");
    Serial.println (Sensor1Value);
    Serial.print ("Range: ");
    Serial.println (Range);

    Sensor1Value = Sensor1Value - MinValueBit; 
    float Sensor1ValuePercent = (float)Sensor1Value/Range*100;
    Sensor1 = Sensor1ValuePercent;
    Serial.print ("Sensor1: ");
    Serial.println (Sensor1);



    //Sensor 2
    MaxValueBit = float(1023 / 5) * MaxValue;
    MinValueBit =  float(1023 / 5) * MinValue;
    Range = MaxValueBit - MinValueBit;
    
    int Sensor2Value = analogRead (A0);
    Serial.print ("Sensor 2 Value: ");
    Serial.println (Sensor2Value);
    Serial.print ("Range: ");
    Serial.println (Range);

    Sensor2Value = Sensor2Value - MinValueBit; 
    float Sensor2ValuePercent = (float)Sensor2Value/Range*100;
    Sensor2 = Sensor2ValuePercent;
    Serial.print ("Sensor2: ");
    Serial.println (Sensor1);
  
  
    

}

void SendMail (int Value1, int Value2)
{


    if ( Webclient.connect(HOST_NAME,80)) {
    // if connected:
    Serial.println("Connected to server");
    // make a HTTP request:
    // send HTTP header
    Webclient.println(HTTP_METHOD + " " + PATH_NAME + "?value1=" + Value1 + "&value2=" + Value2 + " HTTP/1.1");
    Webclient.println("Host: " + String(HOST_NAME));
    Webclient.println("Connection: close");
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
    Serial.println();
    Serial.println("disconnected");
  } else {// if not connected:
    Serial.println("connection failed");
  }
}

byte mac[] = {
  0xA8, 0x61, 0x0A, 0xAE, 0x72,0xEF
};




void setup() {
 
  Ethernet.init(10);  // Most Arduino shields
 

  // Open serial communications and wait for port to open:
  Serial.begin(9600);


  // start the Ethernet connection and the server:
  Ethernet.begin(mac);

 
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

 SendMail (10, 10);

  
}


void loop() {

  ReadSensors ();
  
  client = server.available();
  if (client) {
    Serial.println("new client");
    
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();


         
          client.println(F("<!doctype html>"));
          client.println(F("<html lang=\"en\">"));
          client.println(F("<head>"));
          client.println(F( "<meta charset=\"utf-8\">"));
          client.println(F("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"));
          client.println(F("<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">"));
          client.println(F("</head>"));
          client.println(F("<body>"));
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

          client.println(F("<canvas id=\"myChart\" style=\"width:100%;max-width:700px\"></canvas>)"));
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
    delay(1);
    // close the connection:
    client.stop();

  }

  delay (500);
}
