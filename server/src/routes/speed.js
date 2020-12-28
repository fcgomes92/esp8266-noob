import { COMMANDS } from '../mqtt';

export const setSpeed = async (req, res) => {
  const { params: { speed } = {} } = req || {};
  await req.mqtt.publish(
    req.topic,
    JSON.stringify({ c: COMMANDS.setSpeed, v: speed })
  );
  return res.json({ speed });
};
