import { useEffect, useState } from "react";

const WS_URL = "ws://192.168.1.100:3000";

export default function WebSocketComponent() {
  const [data, setData] = useState({ moisture: 0, pump: "OFF" });
  const [ws, setWs] = useState(null);

  useEffect(() => {
    const socket = new WebSocket(WS_URL);
    setWs(socket);

    socket.onmessage = (event) => {
      try {
        const receivedData = JSON.parse(event.data);
        setData(receivedData);
      } catch (error) {
        console.error("Error parsing WebSocket data:", error);
      }
    };

    socket.onclose = () => console.log("WebSocket closed");

    return () => socket.close();
  }, []);

  const togglePump = () => {
    if (ws && ws.readyState === WebSocket.OPEN) {
      const newPumpState = data.pump === "ON" ? "OFF" : "ON";
      ws.send(JSON.stringify({ pump: newPumpState }));
    }
  };

  return (
    <div className="p-4">
      <h1 className="text-xl font-bold">Monitoring Kelembaban</h1>
      <p>Kelembaban: {data.moisture}%</p>
      <p>Pompa: {data.pump}</p>
      <button
        onClick={togglePump}
        className="mt-2 px-4 py-2 bg-blue-500 text-white rounded">
        Toggle Pompa
      </button>
    </div>
  );
}
