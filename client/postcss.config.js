const tailwindcss = require("tailwindcss");

const IS_DEVELOPMENT = process.env.NODE_ENV === "development";

const plugins = [tailwindcss];

module.exports = {
  plugins,
};
