import React from "react";
import useWebSocketHook from "./hooks/useWebSocket";
import Background from "./assets/4909450.jpg";
import ReactIcon from "./assets/react.svg";

const WS_URL = "ws://192.168.43.216:81/ws";

const App = () => {
  const { moisture, pump, togglePump } = useWebSocketHook(WS_URL);

  const containerStyle = {
    backgroundImage: `url(${Background})`,
    backgroundSize: "cover",
    display: "flex",
    flexDirection: "column",
    alignItems: "center",
    justifyContent: "center",
    textAlign: "center",
    width: "100vw",
    height: "100vh",
    fontFamily: "Arial, sans-serif",
  };

  const buttonStyle = {
    padding: "10px 20px",
    fontSize: "16px",
    margin: "10px",
    border: "none",
    borderRadius: "5px",
    cursor: "pointer",
    transition: "background 0.3s ease",
  };

  const onButtonStyle = {
    ...buttonStyle,
    backgroundColor: "green",
    color: "white",
  };

  const offButtonStyle = {
    ...buttonStyle,
    backgroundColor: "red",
    color: "white",
  };

  const iconStyle = {
    width: "100px",
    height: "100px",
    animation: "spin 2s linear infinite",
  };

  return (
    <div style={containerStyle}>
      <img src={ReactIcon} alt="react-icon" style={iconStyle} />
      <h1 style={{ color: "brown" }}>Monitor Kelembaban Tanah</h1>
      <div style={{ display: "flex", justifyContent: "center", gap: "20px" }}>
        <p
          style={{
            backgroundColor: "brown",
            color: "white",
            padding: "10px",
            borderRadius: "5px",
          }}>
          <strong>Kelembaban:</strong>{" "}
          {moisture !== null ? `${moisture}%` : "Loading..."}
        </p>
        <p
          style={{
            backgroundColor: "brown",
            color: "white",
            padding: "10px",
            borderRadius: "5px",
          }}>
          <strong>Status Pompa:</strong> {pump}
        </p>
      </div>
      <div
        style={{
          backgroundColor: "gray",
          padding: "1px",
          borderRadius: "5px",
        }}>
        <button onClick={() => togglePump("ON")} style={onButtonStyle}>
          Hidupkan Pompa
        </button>
        <button onClick={() => togglePump("OFF")} style={offButtonStyle}>
          Matikan Pompa
        </button>
      </div>
      <style>
        {`
          @keyframes spin {
            from { transform: rotate(0deg); }
            to { transform: rotate(360deg); }
          }
        `}
      </style>
    </div>
  );
};

export default App;
