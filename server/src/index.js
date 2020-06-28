import { Server as WebSocketServer, OPEN } from "ws";
import http from "http";
import express from "express";
import cors from "cors";

const COMMANDS = {
  fill: 0,
  brightness: 1,
  clear: 2,
  toggleEffect: 3,
  setPixels: 4,
};

const EFFECTS = {
  rainbow: 1,
  cylon: 2,
  meteor: 3,
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
      ws.send(JSON.stringify({ c: command, ...params }));
    }
  });
};

const turnOffEffect = () => {
  sendCommandToStrip({
    command: COMMANDS.toggleEffect,
    params: { v: 0, s: false },
  });
};

const clear = () => {
  sendCommandToStrip();
};

const app = express();

app.use(cors({ optionsSuccessStatus: 200 }));

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
    query: { command = "fill", ...params },
  } = req;
  switch (command) {
    case "setPixels":
      turnOffEffect();
      sendCommandToStrip({
        command: COMMANDS.setPixels,
        params,
      });
      break;
    case "fill":
      turnOffEffect();
      sendCommandToStrip({ command: COMMANDS[command], params });
      if (params.v)
        sendCommandToStrip({
          command: COMMANDS.brightness,
          params: { v: params.v },
        });
      break;
    case "brightness":
      sendCommandToStrip({ command: COMMANDS[command], params });
      break;
    case "clear":
      turnOffEffect();
      clear();
      break;
    case "rainbow":
      sendCommandToStrip({
        command: COMMANDS.toggleEffect,
        params: { v: EFFECTS.rainbow, s: true },
      });
      break;
    case "cylon":
      sendCommandToStrip({
        command: COMMANDS.toggleEffect,
        params: { v: EFFECTS.cylon, s: true },
      });
      break;
    case "meteor":
      sendCommandToStrip({
        command: COMMANDS.toggleEffect,
        params: { v: EFFECTS.meteor, s: true },
      });
      break;
  }
  res.json({});
});

server.listen(3000, () => {
  console.log(`Server started on port ${server.address().port}`);
});
