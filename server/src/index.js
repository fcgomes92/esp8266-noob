import cors from "cors";
import express from "express";
import http from "http";
import { OPEN, Server as WebSocketServer } from "ws";

const COMMANDS = {
  fill: 0,
  brightness: 1,
  clear: 2,
  toggleEffect: 3,
  setPixels: 4,
  getState: 5,
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

function toRGB(rgb) {
  return {
    r: (rgb >> 16) & 0xff,
    g: (rgb >> 8) & 0xff,
    b: rgb & 0xff,
  };
}

wss.on("connection", function connection(ws) {
  console.log("new client connected");
  ws.on("message", function incoming(message) {
    const { pixels, effect } = JSON.parse(message);
    if (pixels) {
      ws.pixels = pixels.map((v) => toRGB(v));
      ws.effect = { on: false, selected: null };
    }
    if (effect) {
      ws.pixels = [];
      ws.effect = { on: true, selected: Object.keys(EFFECTS)[effect - 1] };
    }
  });
  ws.on("close", () => {
    console.log("Connection lost");
  });
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
    case "state":
      sendCommandToStrip({
        command: COMMANDS.getState,
      });
      break;
  }
  sendCommandToStrip({
    command: COMMANDS.getState,
  });
  res.json({});
});

app.get("/state", (req, res) => {
  let state = {};
  sendCommandToStrip({
    command: COMMANDS.getState,
  });
  wss.clients.forEach((ws) => {
    if (ws.readyState === OPEN) {
      state = { pixels: ws.pixels, effect: ws.effect };
    }
  });
  res.json(state);
});

server.listen(3000, () => {
  console.log(`Server started on port ${server.address().port}`);
});
