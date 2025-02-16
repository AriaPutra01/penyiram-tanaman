const express = require("express");
const { Server } = require("ws");

const app = express();
const port = 3000;
const server = app.listen(port, () =>
  console.log(`Server running on port ${port}`)
);

const wss = new Server({ server });

let espSocket = null;
let clientSocket = null;

wss.on("connection", (ws) => {
  console.log("New WebSocket connection");

  ws.on("message", (message) => {
    const data = JSON.parse(message);

    if (data.moisture !== undefined) {
      console.log(`Moisture Level: ${data.moisture}`);
      if (clientSocket) clientSocket.send(JSON.stringify(data));
    } else if (data.pump) {
      if (espSocket) espSocket.send(data.pump);
    }
  });

  ws.on("close", () => console.log("Client disconnected"));

  if (!espSocket) espSocket = ws;
  else clientSocket = ws;
});
