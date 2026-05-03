const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ja">
<meta charset="utf-8">
<title>WebAPI Sample Page</title>

<style>
body {
  text-align: center;
  font-family: sans-serif;
}
.container {
  width: 480px;
  margin: auto;
}
.state {
  background: %COLOR%;
  padding: 10px;
  margin-bottom: 10px;
}
button {
  font-size: 16px;
  margin: 4px;
  padding: 6px 10px;
}
</style>

<body>
<div class="container">
<h2>WebAPI Sample Page</h2>
<div class="state">ボタンの状態： %STATE%</div>
<p><button onclick="cmd('/toggle')">ボタン ON / OFF</button>
<button onclick="cmd('/toggle')">%STATE%状態ボタン反転スイッチ</button></p>
<p><button onclick="cmd('/on')">ONボタン</button>
<button onclick="cmd('/off')">OFFボタン</button></p>
<br><br>
</div>
<script>
function cmd(url) {
  fetch(url).then(() => location.reload());
}
</script>
</body>
</html>
)rawliteral";
