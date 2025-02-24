import React from "react";
import useWebSocketHook from "./hooks/useWebSocket";

const WS_URL = "ws://192.168.43.216:81/ws";

const App = () => {
  const { moisture, pump, togglePump } = useWebSocketHook(WS_URL);

  return (
    <div style={{ textAlign: "center", padding: "20px" }}>
      <h1>Monitor Kelembaban Tanah</h1>
      <p><strong>Kelembaban:</strong> {moisture !== null ? `${moisture}%` : "Loading..."}</p>
      <p><strong>Status Pompa:</strong> {pump}</p>
      <button onClick={() => togglePump("ON")} style={{ marginRight: "10px" }}>
        Hidupkan Pompa
      </button>
      <button onClick={() => togglePump("OFF")}>
        Matikan Pompa
      </button>
    </div>
  );
};

export default App;
