import { COMMANDS } from '../mqtt';

export const setPixelsRoute = async (req, res) => {
  const { params: { s, e, r, g, b } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.setPixels, r, g, b, s, e })
  );
  return res.json({ r, g, b, s, e });
};
