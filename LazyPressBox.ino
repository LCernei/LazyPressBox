#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Stepper.h>

#define STEPPER_STEPS 64

// Provide the SSID and Password of the WiFi connection
const char *ssid = "";
const char *pass = "";

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>Lazy Press Box</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      .button {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
        outline: none;
        color: #fff;
        background-color: #2f4468;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
      .button:hover {background-color: #1f2e45}
      .button:active {
        background-color: #1f2e45;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
      }
    </style>
  </head>
  <body>
    <h1>Lazy Press Box</h1>
    <button class="button" onmousedown="pressButton();">PRESS</button>
   <script>
   function pressButton() {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/on", true);
     xhr.send();
   }
  </script>
  </body>
</html>)rawliteral";

WebServer server(80);
Stepper stepper(STEPPER_STEPS, 13, 12, 14, 27);

void handleRoot()
{
  server.send(200, "text/html", index_html);
}

void handleOn()
{
  server.send(200, "text/plain", "this works");
  int steps = STEPPER_STEPS * 4;
  stepper.step(steps);
  delay(1000);
  stepper.step(-1 * steps);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void)
{

  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);
  stepper.setSpeed(300);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32"))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
}
