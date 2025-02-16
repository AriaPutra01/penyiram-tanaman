const express = require("express");
const { WebSocketServer } = require("ws");

const app = express();
const port = 3000;

const wss = new WebSocketServer({ noServer: true });

let moistureLevel = 50; // Simulasi awal (50%)

setInterval(() => {
  moistureLevel = Math.max(
    0,
    Math.min(100, moistureLevel + (Math.random() * 10 - 5))
  );
  const data = JSON.stringify({
    moisture: Math.round(moistureLevel),
    pump: "OFF",
  });
  wss.clients.forEach((client) => client.send(data));
  console.log("Kirim:", data);
}, 2000);

app.use(express.static("public"));

const server = app.listen(port, () =>
  console.log(`Server berjalan di http://localhost:${port}`)
);

server.on("upgrade", (request, socket, head) => {
  wss.handleUpgrade(request, socket, head, (ws) => wss.emit("connection", ws));
});
