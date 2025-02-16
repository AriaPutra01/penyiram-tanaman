import { useState } from "react";
import useWebSocket from "react-use-websocket";

export default function PlantMonitor() {
  const [moisture, setMoisture] = useState(0);
  const [pumpStatus, setPumpStatus] = useState("OFF");

  const { sendJsonMessage, readyState } = useWebSocket(
    "ws://localhost:3000/ws",
    {
      shouldReconnect: () => true,
      onMessage: (event) => {
        const data = JSON.parse(event.data);
        console.log("Data diterima:", data);

        if (data.moisture !== undefined) setMoisture(data.moisture);
        if (data.pump) setPumpStatus(data.pump);
      },
    }
  );

  const togglePump = () => {
    const newStatus = pumpStatus === "OFF" ? "ON" : "OFF";
    sendJsonMessage({ pump: newStatus });
    setPumpStatus(newStatus);
  };

  return (
    <div className="p-4 text-center">
      <h1 className="text-2xl font-bold">Monitoring Tanaman</h1>
      <p className="text-lg">
        Kelembaban Tanah: <span className="font-bold">{moisture}%</span>
      </p>
      <p className="text-lg">
        Status Pompa: <span className="font-bold">{pumpStatus}</span>
      </p>
      <button
        onClick={togglePump}
        className={`mt-4 px-4 py-2 rounded ${
          pumpStatus === "ON" ? "bg-red-500" : "bg-green-500"
        }`}>
        {pumpStatus === "ON" ? "Matikan Pompa" : "Nyalakan Pompa"}
      </button>
      <p className="mt-2 text-sm">
        WebSocket Status:{" "}
        {["Connecting...", "Open", "Closing...", "Closed"][readyState]}
      </p>
    </div>
  );
}
