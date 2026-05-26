#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

const char* ssid = "ESP32_PORTAL";
const char* password = "12345678";

const char* wifi_cliente = "Manuel";
const char* wifi_password = "09876543";

IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);
DNSServer dnsServer;

String nombre = "";
String matricula = "";
bool accesoInternet = false;
bool portalActivo = true;

void mostrarPortal() {

  String pagina = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>

      <title>Portal Cautivo ESP32</title>

      <meta name="viewport" content="width=device-width, initial-scale=1">

      <style>

        body{
          font-family: Arial;
          text-align:center;
          margin-top:50px;
          background:#f2f2f2;
        }

        .contenedor{
          background:white;
          width:300px;
          margin:auto;
          padding:20px;
          border-radius:10px;
          box-shadow:0px 0px 10px gray;
        }

        input{
          width:90%;
          padding:10px;
          margin:10px;
        }

        button{
          padding:10px 20px;
          background:#007BFF;
          color:white;
          border:none;
          border-radius:5px;
        }

      </style>

    </head>

    <body>

      <div class="contenedor">

        <h1>Bienvenido</h1>

        <form action="/registro" method="POST">

          <input type="text" name="nombre" placeholder="Nombre" required>

          <input type="text" name="matricula" placeholder="Matricula" required>

          <button type="submit">Ingresar</button>

        </form>

      </div>

    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", pagina);
}

void registrarUsuario() {

  nombre = server.arg("nombre");
  matricula = server.arg("matricula");

  Serial.println("==========");
  Serial.println("Nuevo usuario conectado");
  Serial.print("Nombre: ");
  Serial.println(nombre);
  Serial.print("Matricula: ");
  Serial.println(matricula);
  Serial.println("==========");

  accesoInternet = true;
  portalActivo = false;
  String respuesta = R"rawliteral(

    <!DOCTYPE html>

    <html>

    <head>

      <meta http-equiv="refresh" content="3;url=http://google.com">

    </head>

    <body style='font-family:Arial;text-align:center;margin-top:50px;'>

      <h1>Bienvenido</h1>

      <h2>Acceso concedido</h2>

      <p>Intentando conectar a internet...</p>

    </body>

    </html>

  )rawliteral";

  server.send(200, "text/html", respuesta);
}

void redireccionarPortal() {

  if(!accesoInternet){

    server.sendHeader("Location", String("http://") + local_ip.toString(), true);

    server.send(302, "text/plain", "");

  }else{

    server.send(200, "text/plain", "Internet habilitado");

  }
}

void setup() {

  Serial.begin(115200);

 WiFi.mode(WIFI_AP_STA);

  WiFi.softAPConfig(local_ip, gateway, subnet);

  WiFi.softAP(ssid, password);

  WiFi.begin(wifi_cliente, wifi_password);

Serial.println("Conectando al internet...");

while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
}

Serial.println("");
Serial.println("Internet conectado");
Serial.print("IP internet: ");
Serial.println(WiFi.localIP());

  Serial.println("");
  Serial.println("WiFi iniciado");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  dnsServer.start(53, "*", local_ip);

  server.on("/", mostrarPortal);

  server.on("/registro", HTTP_POST, registrarUsuario);

  server.onNotFound(redireccionarPortal);

  server.begin();

  Serial.println("Portal cautivo iniciado");
}

void loop() {

  if(portalActivo){

    dnsServer.processNextRequest();

  }

  server.handleClient();
}