#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

const char* ssid="wifi-ssid";
const char* password="password";

ESP8266WebServer server(80);

#define     ledPin     D3

String      htmlPage;                   // webpage text to be sent out by server when main page is accessed
int v=0; 
bool on_off;


void setup() {
  digitalWrite(ledPin, LOW);            // led is off initially
  on_off=false;
  pinMode(ledPin, OUTPUT);
  analogWriteRange(100);
  // Connect to WiFi network
  ///Station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();                    // disconnect if previously connected
  delay(100);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  // functions to call when client requests are received
  server.on("/", handleRoot);     
  server.on("/LEDTOGGLE", handleLEDToggle);  
  server.on("/PWM", handlePWM);
  server.onNotFound(handleNotFound);        

  server.begin();                           // start web server
 
}

void loop(void) {
  server.handleClient();                    // listen for HTTP requests from clients
}

// send main web page when ip+"/" is accessed
void handleRoot() {
  buildHtmlPage();
  server.send(200, "text/html", htmlPage);
}

// toggle led and redirect to main page
void handleLEDToggle() {  
  if(on_off==false) {digitalWrite(ledPin, HIGH); on_off=true; v=100;}   
  else {digitalWrite(ledPin, LOW); on_off=false; v=0;}
  server.sendHeader("Location", "/");       
  server.send(303);                         
}

void handlePWM()
{
  String valoare=server.arg("pwm_val");
  v=valoare.toInt();
  analogWrite(ledPin,v);
  if(v!=0) on_off=true;  
  else on_off=false;
  server.sendHeader("Location", "/");       
  server.send(303); 
}

// send HTTP status 404: Not Found when there's no handler for the client request
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found"); 
}

// create the html page for the root path of the web server
void buildHtmlPage() {
  htmlPage = "<!DOCTYPE html>";
  htmlPage += "<html>";
  htmlPage += "<head>";    
  htmlPage +="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  htmlPage +="<meta charset=\"UTF-8\">";
  htmlPage += "<title>ESP8266 Web Server</title>";             // title for browser window
  htmlPage += "<style>";
  htmlPage += ".button{ background-color: #4CAF50; border: none; color: white; padding: 12px 28px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin:4px; border-radius: 10px;}";
  htmlPage += ".sli_bar{-webkit-appearance: none; width: 100%; height: 25px; background: #d3d3d3; background-image: linear-gradient(to right,black, white); outline: none;}";
  htmlPage += ".sli_bar::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 25px; height: 25px; background: #4CAF50;}";
  htmlPage += ".slider::-moz-range-thumb {-webkit-appearance: none; width: 25px; height: 25px; background: #4CAF50;} ";
  htmlPage += "</style>";
  htmlPage += "</head>";
  
  htmlPage += "<BODY bgcolor='#E0E0D0'>";                      // body section, set background color 
  htmlPage += "<center><a href=\"/LEDTOGGLE\" class=\"button\">";
  if(on_off==false) htmlPage +="OFF";
  else htmlPage +="ON";
  htmlPage += "</a></center>";
 
  htmlPage += "<hr>";
 htmlPage += "<form action=\"/PWM\" method=\"post\" id=\"form_pwm\">";
 htmlPage += "<label>Intensitate: <span id=\"label_text\">";
 htmlPage += v;
 htmlPage += "</span> %</label><br>";
 htmlPage += "<input ontouchend=\"rel_pwm()\" onmouseup=\"rel_pwm()\" oninput=\"asdf()\" class=\"sli_bar\" id=\"slider\" style=\"display:inline-block; width:100%;\" type=\"range\" name=\"pwm_val\" min=\"0\" max=\"100\" value=\"";
 htmlPage += v;
 htmlPage += "\">";
 htmlPage += "</form>";
 htmlPage += "<script>";
 htmlPage += "function asdf(){document.getElementById(\"label_text\").innerHTML=document.getElementById(\"slider\").value;}";
 htmlPage += "function rel_pwm(){const form  = document.getElementById('form_pwm'); form.submit();}";
 htmlPage += "</script>";

  htmlPage += "</body>";
  htmlPage += "</html>";
}
