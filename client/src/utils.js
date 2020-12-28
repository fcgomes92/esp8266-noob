export const COMMANDS = {
  effects: "effects",
  fill: "fill",
  color: "color",
  pixels: "pixels",
  speed: "speed",
};

export const EFFECTS = [
  { name: "Static", desciption: "No blinking. Just plain old static light." },
  { name: "Blink", desciption: "Normal blinking. 50% on/off time." },
  {
    name: "Breath",
    desciption:
      'Does the "standby-breathing" of well known i-Devices. Fixed Speed',
  },
  {
    name: "Color Wipe",
    desciption:
      "Lights all LEDs after each other up. Then turns them in that order off. Repeat.",
  },
  {
    name: "Color Wipe Inverse",
    desciption: "Same as Color Wipe, except swaps on/off colors.",
  },
  {
    name: "Color Wipe Reverse",
    desciption:
      "Lights all LEDs after each other up. Then turns them in reverse order off. Repeat.",
  },
  {
    name: "Color Wipe Reverse Inverse",
    desciption: "Same as Color Wipe Reverse, except swaps on/off colors.",
  },
  {
    name: "Color Wipe Random",
    desciption:
      "Turns all LEDs after each other to a random color. Then starts over with another color.",
  },
  {
    name: "Random Color",
    desciption:
      "Lights all LEDs in one random color up. Then switches them to the next random color.",
  },
  {
    name: "Single Dynamic",
    desciption:
      "Lights every LED in a random color. Changes one random LED after the other to a random color.",
  },
  {
    name: "Multi Dynamic",
    desciption:
      "Lights every LED in a random color. Changes all LED at the same time to new random colors.",
  },
  { name: "Rainbow", desciption: "Cycles all LEDs at once through a rainbow." },
  {
    name: "Rainbow Cycle",
    desciption: "Cycles a rainbow over the entire string of LEDs.",
  },
  { name: "Scan", desciption: "Runs a single pixel back and forth." },
  {
    name: "Dual Scan",
    desciption: "Runs two pixel back and forth in opposite directions.",
  },
  { name: "Fade", desciption: "Fades the LEDs on and (almost) off again." },
  {
    name: "Theater Chase",
    desciption: "Theatr",
    desciption: "tyle crawling lights. Inspired by the Adafruit examples.",
  },
  {
    name: "Theater Chase Rainbow",
    desciption: "Theatr",
    desciption:
      "tyle crawling lights with rainbow effect. Inspired by the Adafruit examples.",
  },
  {
    name: "Running Lights",
    desciption: "Running lights effect with smooth sine transition.",
  },
  { name: "Twinkle", desciption: "Blink several LEDs on, reset, repeat." },
  {
    name: "Twinkle Random",
    desciption: "Blink several LEDs in random colors on, reset, repeat.",
  },
  { name: "Twinkle Fade", desciption: "Blink several LEDs on, fading out." },
  {
    name: "Twinkle Fade Random",
    desciption: "Blink several LEDs in random colors on, fading out.",
  },
  { name: "Sparkle", desciption: "Blinks one LED at a time." },
  {
    name: "Flash Sparkle",
    desciption:
      "Lights all LEDs in the selected color. Flashes single white pixels randomly.",
  },
  { name: "Hyper Sparkle", desciption: "Like flash sparkle. With more flash." },
  { name: "Strobe", desciption: "Classic Strobe effect." },
  {
    name: "Strobe Rainbow",
    desciption: "Classic Strobe effect. Cycling through the rainbow.",
  },
  {
    name: "Multi Strobe",
    desciption:
      "Strobe effect with different strobe count and pause, controlled by speed setting.",
  },
  {
    name: "Blink Rainbow",
    desciption: "Classic Blink effect. Cycling through the rainbow.",
  },
  { name: "Chase White", desciption: "Color running on white." },
  { name: "Chase Color", desciption: "White running on color." },
  {
    name: "Chase Random",
    desciption: "White running followed by random color.",
  },
  { name: "Chase Rainbow", desciption: "White running on rainbow." },
  { name: "Chase Flash", desciption: "White flashes running on color." },
  {
    name: "Chase Flash Random",
    desciption: "White flashes running, followed by random color.",
  },
  { name: "Chase Rainbow White", desciption: "Rainbow running on white." },
  { name: "Chase Blackout", desciption: "Black running on color." },
  { name: "Chase Blackout Rainbow", desciption: "Black running on rainbow." },
  {
    name: "Color Sweep Random",
    desciption:
      "Random color introduced alternating from start and end of strip.",
  },
  {
    name: "Running Color",
    desciption: "Alternating color/white pixels running.",
  },
  {
    name: "Running Red Blue",
    desciption: "Alternating red/blue pixels running.",
  },
  { name: "Running Random", desciption: "Random colored pixels running." },
  { name: "Larson Scanner", desciption: "K.I.T.T." },
  { name: "Comet", desciption: "Firing comets from one end." },
  { name: "Fireworks", desciption: "Firework sparks." },
  { name: "Fireworks Random", desciption: "Random colored firework sparks." },
  {
    name: "Merry Christmas",
    desciption: "Alternating green/red pixels running.",
  },
  {
    name: "Fire Flicker",
    desciption: "Fire flickering effect. Like in harsh wind.",
  },
  {
    name: "Fire Flicker (soft)",
    desciption: "Fire flickering effect. Runs slower/softer.",
  },
  {
    name: "Fire Flicker (intense)",
    desciption: "Fire flickering effect. More range of color.",
  },
  {
    name: "Circus Combustus",
    desciption: "Alternating white/red/black pixels running.",
  },
  {
    name: "Halloween",
    desciption: "Alternating orange/purple pixels running.",
  },
  {
    name: "Bicolor Chase",
    desciption: "Two LEDs running on a background color.",
  },
  {
    name: "Tricolor Chase",
    desciption: "Alternating three color pixels running.",
  },
];

export const buildURL = (topic, stripId, command, params) => {
  if (stripId)
    return [topic, stripId, COMMANDS[command], params.join("/")].join("/");
  return [topic, COMMANDS[command], params.join("/")].join("/");
};

export const buildAllURL = (topic, command, params) => {
  return buildURL(topic, null, command, params);
};

export const buildEffectStripURL = (topic, stripId, effectId) => {
  return buildURL(topic, stripId, COMMANDS.effects, [effectId]);
};

export const buildColorStripURL = (topic, stripId, { r, g, b }) => {
  return buildURL(topic, stripId, COMMANDS.color, [r, g, b]);
};

export const buildSpeedStripURL = (topic, stripId, speed) => {
  return buildURL(topic, stripId, COMMANDS.speed, [speed]);
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
