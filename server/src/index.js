import { Server as WebSocketServer } from "ws";
import http from "http";
import express from "express";

const app = express();

app.route("/ping").get((req, res) => {
  res.send("pong");
});

const server = http.createServer(app);

const wss = new WebSocketServer({ server });

wss.on("connection", function connection(ws) {
  ws.on("message", function incoming(message) {
    console.log("received: %s", message);
    ws.send("reply from server : " + message);
  });
  ws.send("something");
});

server.listen(8080, () => {
  console.log(`Server started on port ${server.address().port}`);
});
