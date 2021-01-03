import React from "react";
import { ChromePicker } from "react-color";
import api from "~api";
import "~styles/components/Strip.scss";
import { buildSetPixelAllURL, EFFECTS, buildEffectAllURL } from "~utils";
import Pixel from "~/components/Pixel";

const chunk = (arr, size) => {
  return [...arr].reduce(
    (chunked, el) => {
      const tail = chunked.slice(-1)[0];
      const rest = chunked.slice(0, -1);
      if (tail.length === size) {
        return [...rest, tail, [el]];
      }
      return [...rest, [...tail, el]];
    },
    [[]]
  );
};

const createPixelArray = (total, color) => {
  return [...new Array(total)].map((_, index) => ({
    color,
    index,
  }));
};

const generateSetPixelCommands = (commands, pixel, index) => {
  const [head, ...rest] = commands;
  if (
    head.r === pixel.color.rgb.r &&
    head.g === pixel.color.rgb.g &&
    head.b === pixel.color.rgb.b
  ) {
    return [{ ...head, e: index }, ...rest];
  }
  return [{ ...pixel.color.rgb, s: index, e: index + 1 }, head, ...rest];
};

export default function Strip({ total = 10, hidePixelLabel = false }) {
  const baseColor = { hex: "#fff", rgb: { r: 255, g: 255, b: 255, a: 1 } };
  const [pixels, setPixels] = React.useState(
    createPixelArray(total, baseColor)
  );
  const [isPaintOn, setIsPaintOn] = React.useState(false);
  const [paintColor, setPaintColor] = React.useState(baseColor);
  const [selectedEffect, setEffect] = React.useState(EFFECTS.rainbow);

  React.useEffect(() => {
    setPixels(
      createPixelArray(total, {
        hex: "#fff",
        rgb: { r: 255, g: 255, b: 255, a: 1 },
      })
    );
  }, [total]);

  const handleSetStripPixels = async () => {
    const commands = pixels
      .reduce(generateSetPixelCommands, [{ s: 0, e: 0, ...baseColor.rgb }])
      .map(async ({ s, e, r, g, b }) => {
        console.log(buildSetPixelAllURL("office/lights", s, e, r, g, b));
        return api.get(buildSetPixelAllURL("office/lights", s, e, r, g, b));
      });
    await Promise.all(commands);
  };

  const chunkedPixels = chunk(pixels, 20);

  const handleColorChange = (pixel) => {
    setPixels([
      ...pixels.slice(0, pixel.index),
      pixel,
      ...pixels.slice(pixel.index + 1, pixels.length),
    ]);
  };
  const handleHoverPixel = ({ index }) => {
    isPaintOn && handleColorChange({ index, color: paintColor });
  };

  const renderPixels = ({ color, index }) => {
    return (
      <div className="strip__pixel" key={`pixel-${index}`}>
        <Pixel
          hideLabel={hidePixelLabel}
          color={color}
          index={index}
          onHover={handleHoverPixel}
          onChangeColor={handleColorChange}
        />
      </div>
    );
  };

  const handleRadioChange = (e) => {
    setEffect(e.target.value);
  };

  const handleEffectFormSubmit = async (e) => {
    e.preventDefault();
    await api.get(buildEffectAllURL("office/lights", selectedEffect, [0, 128]));
  };

  return (
    <React.Fragment>
      <div>
        Effects:
        <form onSubmit={handleEffectFormSubmit}>
          <fieldset id="group1">
            {Object.values(EFFECTS).map((effect) => (
              <label key={effect}>
                {effect}
                <input
                  checked={selectedEffect === effect}
                  type="radio"
                  value={effect}
                  name="effect"
                  onChange={handleRadioChange}
                />
              </label>
            ))}
          </fieldset>
          <button type="submit">Set effect</button>
        </form>
      </div>
      <div>
        <h5>Paint Tool:</h5>
        <ChromePicker
          color={paintColor.rgb}
          onChange={(color) => setPaintColor(color)}
        />
        <label>
          Active:
          <input
            type="checkbox"
            value={isPaintOn}
            onChange={() => setIsPaintOn(!isPaintOn)}
          />
        </label>
      </div>
      <div>
        <button onClick={handleSetStripPixels}>Set Strip Color</button>
      </div>
      <div className="strip">
        {chunkedPixels.map((pxs, lineIndex) => (
          <div className="strip__lane" key={`lane-${lineIndex}`}>
            <div className="strip__connection strip__connection--top"></div>
            {pxs.map(renderPixels)}
            <div className="strip__connection strip__connection--bottom"></div>
          </div>
        ))}
      </div>
    </React.Fragment>
  );
}
