import { useState, useEffect } from "react";
import useWebSocket from "react-use-websocket";

const useWebSocketHook = (url) => {
  const [moisture, setMoisture] = useState(null);
  const [pump, setPump] = useState(null);

  const { sendJsonMessage, lastJsonMessage } = useWebSocket(url, {
    shouldReconnect: () => true, // Reconnect otomatis jika terputus
    reconnectInterval: 3000, // Coba reconnect setiap 3 detik
  });

  useEffect(() => {
    if (lastJsonMessage) {
      setMoisture(lastJsonMessage.moisture);
      setPump(lastJsonMessage.pump);
    }
  }, [lastJsonMessage]);

  const togglePump = (state) => {
    sendJsonMessage({ pump: state });
  };

  return { moisture, pump, togglePump };
};

export default useWebSocketHook;
