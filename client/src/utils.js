export const COMMANDS = {
  effects: "effects",
  fill: "fill",
  pixels: "pixels",
};

export const EFFECTS = {
  rainbow: "rainbow",
  breath: "breath",
};

export const buildURL = (topic, stripId, command, params) => {
  if (stripId) return [topic, stripId, COMMANDS[command], ...params].join("/");
  return [topic, COMMANDS[command], ...params].join("/");
};

export const buildAllURL = (topic, command, params) => {
  return buildURL(topic, null, command, params);
};

export const buildEffectStripURL = (topic, stripId, effectId, params) => {
  return buildURL(topic, stripId, COMMANDS.effects, [
    EFFECTS[effectId],
    ...params,
  ]);
};

export const buildEffectAllURL = (topic, effectId, params) => {
  return buildAllURL(topic, COMMANDS.effects, [EFFECTS[effectId], ...params]);
};

export const buildFillStripURL = (topic, stripId, r, g, b) => {
  return buildURL(topic, stripId, COMMANDS.fill, [r, g, b]);
};

export const buildFillAllURL = (topic, r, g, b) => {
  return buildAllURL(topic, COMMANDS.fill, [r, g, b]);
};

export const buildSetPixelStripURL = (topic, stripId, start, end, r, g, b) => {
  return buildURL(topic, stripId, COMMANDS.pixels, [start, end, r, g, b]);
};

export const buildSetPixelAllURL = (topic, start, end, r, g, b) => {
  return buildAllURL(topic, COMMANDS.pixels, [start, end, r, g, b]);
};
