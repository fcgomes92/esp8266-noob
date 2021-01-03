import { COMMANDS } from '../mqtt';

export const fillRoute = async (req, res) => {
  const { params: { r, g, b } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({
      c: COMMANDS.setEffect,
      e: 0,
    })
  );
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.setColor, r, g, b })
  );
  return res.json({ r, g, b });
};
