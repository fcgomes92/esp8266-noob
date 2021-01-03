import mqtt from 'async-mqtt';

let client;

export const COMMANDS = {
  setColor: 0,
  setBrightness: 1,
  setEffect: 3,
  setSpeed: 4,
  getState: 5,
};

export const getMQTTClient = async db => {
  if (client) return client;
  client = mqtt.connect(process.env.MQTT_URL);
  client.on('message', async (topic, payload) => {
    console.log(`Payload: ${topic}: ${payload}`);
    try {
      const data = JSON.parse(payload);
      const [place, type] = topic.split('/');
      switch (type) {
        case 'lights': {
          if (data.id) {
            await Promise.all([
              db.lights.upsert({
                id: data.id,
                path: topic.replace('/status', ''),
                color: `#${Number(data.color).toString(16)}`,
                speed: data.speed,
                effect: data.effect,
              }),
              db.places.upsert({
                name: place,
              }),
            ]);
          }
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
