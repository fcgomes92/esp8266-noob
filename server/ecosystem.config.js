module.exports = {
  apps: [
    {
      name: 'esp8266-server',
      script: './lib/index.js',
      instances: 1,
      kill_timeout: 3000,
      wait_ready: true,
      listen_timeout: 3000,
    },
  ],
};
