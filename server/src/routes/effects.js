import { COMMANDS } from '../mqtt';

const EFFECTS = {
  rainbow: 1,
  cylon: 2,
  meteor: 3,
  breath: 4,
};

export const setRainbowEffectRoute = async (req, res) => {
  const { params: { i, t } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({
      c: COMMANDS.configureRainbow,
      i,
      t,
    })
  );
  return res.json({});
};

export const setBreathEffectRoute = async (req, res) => {
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
};

export const toggleEffectRoute = async (req, res) => {
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
};
