const express = require("express");
const { Server } = require("ws");

const app = express();
const port = 3000;
const server = app.listen(port, () =>
  console.log(`Server running on port ${port}`)
);

const wss = new Server({ server });

let espSocket = null;
const clients = new Set(); // Menyimpan semua client frontend

wss.on("connection", (ws) => {
  console.log("New WebSocket connection");

  ws.on("message", (message) => {
    try {
      const data = JSON.parse(message);

      // Jika data dari ESP8266
      if (data.moisture !== undefined) {
        console.log(`Moisture Level: ${data.moisture}% | Pump: ${data.pump}`);

        // Kirim ke semua client yang terhubung
        clients.forEach((client) => {
          if (client.readyState === ws.OPEN) {
            client.send(JSON.stringify(data));
          }
        });
      }

      // Jika data dari client frontend untuk mengontrol pompa
      else if (data.pump) {
        if (espSocket && espSocket.readyState === ws.OPEN) {
          espSocket.send(JSON.stringify({ pump: data.pump }));
        }
      }
    } catch (error) {
      console.error("Invalid JSON received:", message);
    }
  });

  ws.on("close", () => {
    console.log("Client disconnected");

    // Jika ESP8266 yang terputus, set null agar bisa diganti ESP baru
    if (ws === espSocket) {
      espSocket = null;
      console.log("ESP8266 disconnected");
    }

    // Hapus client dari daftar
    clients.delete(ws);
  });

  // Jika ini koneksi pertama dari ESP, simpan sebagai espSocket
  if (!espSocket) {
    espSocket = ws;
    console.log("ESP8266 connected");
  } else {
    clients.add(ws); // Tambahkan client ke daftar
    console.log("Frontend client connected");
  }
});
