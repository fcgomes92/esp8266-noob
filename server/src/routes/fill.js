import { COMMANDS } from '../mqtt';

export const fillRoute = async (req, res) => {
  const { params: { r, g, b } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({
      c: COMMANDS.toggleEffect,
      s: false,
    })
  );
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.fill, r, g, b })
  );
  return res.json({ r, g, b });
};
