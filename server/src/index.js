import cors from 'cors';
import express from 'express';
import http from 'http';
import { getDatabaseClient } from './db/index';
import { COMMANDS, getMQTTClient } from './mqtt';

const EFFECTS = {
  rainbow: 1,
  cylon: 2,
  meteor: 3,
  breath: 4,
};

const app = express();

app.use(cors({ optionsSuccessStatus: 200 }));

app.use(async (req, res, next) => {
  req.db = await getDatabaseClient();
  next();
});

app.use('/:path/:subPath/(|fill|pixels|effects)/*', async (req, res, next) => {
  const { params: { path, subPath } = {} } = req || {};
  const topic = `${path}/${subPath}`;
  req.topic = topic;
  req.mqtt = await getMQTTClient(req.db);
  return next();
});

app.use(
  '/:path/:subPath/:id/(|fill|pixels|effects)/*',
  async (req, res, next) => {
    const { params: { path, subPath, id } = {} } = req || {};
    const topic = `${path}/${subPath}/${id}`;
    req.topic = topic;
    req.db = await getDatabaseClient();
    req.mqtt = await getMQTTClient(req.db);
    return next();
  }
);

app.get('/:path/:subPath/fill/:r/:g/:b', async (req, res) => {
  const { params: { r, g, b } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.toggleEffect, s: false })
  );
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.fill, r, g, b })
  );
  return res.json({ r, g, b });
});

app.get('/:path/:subPath/:id/fill/:r/:g/:b', async (req, res) => {
  const { params: { r, g, b } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.toggleEffect, s: false })
  );
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.fill, r, g, b })
  );
  return res.json({ r, g, b });
});

app.get('/:path/:subPath/pixels/:s/:e/:r/:g/:b', async (req, res) => {
  const { params: { s, e, r, g, b } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.toggleEffect, s: false, v: EFFECTS.rainbow })
  );
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.setPixels, r, g, b, s, e })
  );
  return res.json({ r, g, b, s, e });
});

app.get('/:path/:subPath/:id/pixels/:s/:e/:r/:g/:b', async (req, res) => {
  const { params: { s, e, r, g, b } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.toggleEffect, s: false, v: EFFECTS.rainbow })
  );
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.setPixels, r, g, b, s, e })
  );
  return res.json({ r, g, b, s, e });
});

app.get('/:path/:subPath/effects/:effect', async (req, res) => {
  const { params: { effect } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({
      c: COMMANDS.toggleEffect,
      s: true,
      v: EFFECTS[effect] || EFFECTS.rainbow,
    })
  );
  return res.json({ effect: effect });
});

app.get(
  '/:path/:subPath/:id/effects/breath/:r/:g/:b/:s/:t',
  async (req, res) => {
    const { params: { r, g, b, s, t } = {} } = req || {};
    await req.mqtt.publish(
      req.topic,
      JSON.stringify({
        c: COMMANDS.configureBreath,
        r,
        g,
        b,
        s,
        t,
      })
    );
    return res.json({});
  }
);

app.get('/:path/:subPath/:id/effects/:effect', async (req, res) => {
  const { params: { effect } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({
      c: COMMANDS.toggleEffect,
      s: true,
      v: EFFECTS[effect] || EFFECTS.rainbow,
    })
  );
  return res.json({ effect: effect });
});

app.get('/strips', async (req, res) => {
  const data = await req.db.lights.find().exec();
  return res.json(data);
});

app.get('/strips/refresh', async (req, res) => {
  await req.db.lights.find().remove();
  const mqtt = await getMQTTClient(req.db);
  await mqtt.publish('office/lights', JSON.stringify({ c: COMMANDS.getState }));
  return res.json({ state: 'refreshing' });
});

app.get('/strips/:id/refresh', async (req, res) => {
  const {
    params: { id },
  } = req;
  await req.db.lights.findOne().where('id').eq(id).remove();
  const mqtt = await getMQTTClient(req.db);
  await mqtt.publish(
    `office/lights/${id}`,
    JSON.stringify({ c: COMMANDS.getState })
  );
  return res.json({ state: 'refreshing' });
});

app.route('/ping').get((req, res) => {
  return res.send('pong');
});

const server = http.createServer(app);

server.listen(process.env.PORT, async () => {
  const db = await getDatabaseClient();
  await getMQTTClient(db);
  console.log(`Server started on port ${server.address().port}`);
});
