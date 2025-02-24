import { useEffect, useState } from "react";
import useWebSocket from "react-use-websocket";

const WS_URL = "ws://localhost:3000";

export default function WebSocketComponent() {
  const { sendMessage, lastMessage } = useWebSocket(WS_URL);
  const [data, setData] = useState({ moisture: 0, pump: "OFF" });
  const [messageHistory, setMessageHistory] = useState([]);

  useEffect(() => {
    if (lastMessage !== null) {
      const receivedData = JSON.parse(lastMessage.data);
      setData(receivedData);
      setMessageHistory((prev) => [...prev, receivedData]);
    }
  }, [lastMessage]);

  const togglePump = () => {
    const newPumpState = data.pump === "ON" ? "OFF" : "ON";
    sendMessage(JSON.stringify({ pump: newPumpState }));
  };

  return (
    <div className="flex flex-col items-center justify-center h-screen text-center">
      <h1 className="text-2xl font-bold">Monitoring Kelembaban</h1>
      <p className="text-lg">Kelembaban: {data.moisture}%</p>
      <p className="text-lg">Pompa: {data.pump}</p>
      <button
        className="px-4 py-2 mt-4 text-white bg-blue-500 rounded hover:bg-blue-600"
        onClick={togglePump}>
        Toggle Pompa
      </button>
    </div>
  );
}
