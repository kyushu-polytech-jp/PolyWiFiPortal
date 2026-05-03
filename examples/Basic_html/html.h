const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <title>Hello Web API</title>
  <body>
    <h2>Hello Web page by ESP32</h2>
    <p>Sample index page http://%LOCALIP%/index.html (this page)</p>
    <p><a href="/index.html" target="_blank">this page</a></p><br>
    <h2>Hello Web API World by ESP32</h2>
    <p>Sample Web API : http://%LOCALIP%/get?temp=123.456</p>
    <p><a href="/set?temp=123.456" target="_blank">STA IP addr(%LOCALIP%)/set?temp=123.456</a></p>
  </body>
</html>
)rawliteral";