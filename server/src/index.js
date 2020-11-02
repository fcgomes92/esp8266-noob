import cors from 'cors';
import express from 'express';
import http from 'http';
import { getDatabaseClient } from './db/index';
import { COMMANDS, getMQTTClient } from './mqtt';
import {
  setBreathEffectRoute,
  setRainbowEffectRoute,
  toggleEffectRoute,
} from './routes/effects';
import { fillRoute } from './routes/fill';
import { setPixelsRoute } from './routes/pixels';

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
    req.mqtt = await getMQTTClient(req.db);
    return next();
  }
);

app.get('/:path/:subPath/fill/:r/:g/:b', fillRoute);
app.get('/:path/:subPath/:id/fill/:r/:g/:b', fillRoute);

app.get('/:path/:subPath/pixels/:s/:e/:r/:g/:b', setPixelsRoute);
app.get('/:path/:subPath/:id/pixels/:s/:e/:r/:g/:b', setPixelsRoute);

app.get('/:path/:subPath/effects/:effect', toggleEffectRoute);

app.get(
  '/:path/:subPath/:id/effects/breath/:r/:g/:b/:s/:t',
  setBreathEffectRoute
);
app.get('/:path/:subPath/effects/breath/:r/:g/:b/:s/:t', setBreathEffectRoute);

app.get('/:path/:subPath/:id/effects/rainbow/:t/:i', setRainbowEffectRoute);
app.get('/:path/:subPath/effects/rainbow/:t/:i', setRainbowEffectRoute);

app.get('/strips', async (req, res) => {
  const data = await req.db.lights.find().exec();
  return res.json(
    data.map(strip => ({
      id: strip.id,
      path: strip.path,
      effect: strip.selectedEffect,
      effectActive: strip.isEffectActive,
      pixels: strip.pixels,
    }))
  );
});

app.get('/strips/:id/refresh', async (req, res) => {
  const {
    params: { id },
  } = req;
  const strip = await req.db.lights.findOne().where('id').eq(id).exec();
  if (!strip) return res.status(404).json({ state: 'not found' });
  const mqtt = await getMQTTClient(req.db);
  await mqtt.publish(
    `${strip.path}/${id}`,
    JSON.stringify({ c: COMMANDS.getState })
  );
  return res.json({ state: 'refreshing' });
});

app.post('/');

app.route('/ping').get((req, res) => {
  return res.send('pong');
});

const server = http.createServer(app);

server.listen(process.env.PORT, async () => {
  const db = await getDatabaseClient();
  await getMQTTClient(db);
  console.log(`Server started on port ${server.address().port}`);
});
