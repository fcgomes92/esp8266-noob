import { Server as WebSocketServer, OPEN } from "ws";
import http from "http";
import express from "express";

const COMMANDS = {
  fill: 0,
  brightness: 1,
  clear: 2,
  toggleEffect: 3,
  selectEffect: 4,
};

const sendCommandToStrip = (
  { command = 0, params = { r: 0, g: 0, b: 0 } } = {
    command: 0,
    params: { r: 0, g: 0, b: 0 },
  }
) => {
  wss.clients.forEach((ws) => {
    if (ws.readyState === OPEN) {
      console.log(`sending: ${JSON.stringify({ command, params })}`);
      ws.send(JSON.stringify({ command, params }));
    }
  });
};

const clear = () => {
  sendCommandToStrip();
};

const app = express();

app.route("/ping").get((req, res) => {
  res.send("pong");
});

const server = http.createServer(app);

const wss = new WebSocketServer({ server });

wss.on("connection", function connection(ws) {
  console.log("new client connected");
  // ws.on("message", function incoming(message) {
  //   console.log("received: %s", message);
  // });
  ws.on("close", () => {
    console.log("Connection lost");
  });
  clear();
});

app.get("/", (req, res) => {
  const {
    query: { command = "fill", r = 0, g = 0, b = 0, v = null },
  } = req;
  switch (command) {
    case "fill":
      sendCommandToStrip({ command: COMMANDS[command], params: { r, g, b } });
      if (v)
        sendCommandToStrip({ command: COMMANDS.brightness, params: { v } });
      break;
    case "brightness":
      sendCommandToStrip({ command: COMMANDS[command], params: { v } });
      break;
    case "clear":
      clear();
      break;
    case "rainbow":
      sendCommandToStrip({ command: COMMANDS.toggleEffect });
      sendCommandToStrip({ command: COMMANDS.selectEffect, params: { v } });
  }
  res.json({});
});

server.listen(3000, () => {
  console.log(`Server started on port ${server.address().port}`);
});
