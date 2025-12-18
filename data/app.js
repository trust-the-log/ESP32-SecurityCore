let ws = new WebSocket("ws://" + location.host + "/ws");

ws.onmessage = e => {
  let d = JSON.parse(e.data);

  document.getElementById("state").innerText = d.state;
  document.getElementById("entry").innerText =
    d.entry > 0 ? "ENTRY: " + d.entry : "";

  let html = "";
  d.zones.forEach((z,i)=>{
    html += `<div class="zone">Zone ${i+1}: ${z?"OPEN":"OK"}</div>`;
  });
  document.getElementById("zones").innerHTML = html;
};

function arm() {
  send("ARM");
}

function disarm() {
  send("DISARM");
}

function send(cmd) {
  ws.send(JSON.stringify({
    cmd: cmd,
    pin: document.getElementById("pin").value
  }));
}
