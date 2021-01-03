let config;

const _makeConfig = () => () => {
  if (!config)
    config = {
      apiURL: process.env.REACT_APP_API_URL,
    };
  return config;
};

export default _makeConfig();
