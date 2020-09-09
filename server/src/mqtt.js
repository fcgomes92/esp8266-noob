import mqtt from 'async-mqtt';

let client;

export const COMMANDS = {
  fill: 0,
  brightness: 1,
  clear: 2,
  toggleEffect: 3,
  setPixels: 4,
  getState: 5,
  configureBreath: 6,
};

export const getMQTTClient = async db => {
  if (client) return client;
  client = mqtt.connect(process.env.MQTT_URL);
  client.on('message', async (topic, payload) => {
    console.log(`Payload: ${topic}: ${payload}`);
    try {
      const data = JSON.parse(payload);
      switch (topic) {
        case 'office/lights/status': {
          if (data.id)
            await db.lights.upsert({
              id: data.id,
              path: topic.replace('/status', ''),
              ...(data.effect ? { selectedEffect: data.effect } : {}),
            });
          break;
        }
      }
    } catch (err) {
      console.error(err);
    }
  });

  client.on('connect', async () => {
    try {
      await client.subscribe('office/lights/status');
      await client.publish(
        'office/lights',
        JSON.stringify({ c: COMMANDS.getState })
      );
    } catch (err) {
      console.error(err);
    }
  });

  return client;
};

export default client;
