import { COMMANDS } from '../mqtt';

export const setEffectRoute = async (req, res) => {
  const { params: { effect } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({
      c: COMMANDS.setEffect,
      e: effect || 0,
    })
  );
  return res.json({ effect: effect });
};
