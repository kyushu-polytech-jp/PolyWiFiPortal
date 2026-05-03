const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <title>Hello Web</title>
  <body>
    <h2>Hello Web API World by ESP32</h2>
    <p>Sample Web Page : http://%LOCALIP%/index.html</p>
    <p><a href="/index.html" target="_blank"> this page </a></p><br>
  </body>
</html>
)rawliteral";