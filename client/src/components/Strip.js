import axios from "axios";
import React from "react";
import { ChromePicker } from "react-color";
import "../styles/components/Strip.scss";
import Pixel from "./Pixel";

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

export default function Strip({ total = 10, hidePixelLabel = false }) {
  const baseColor = { hex: "#fff", rgb: { r: 255, g: 255, b: 255, a: 1 } };
  const createPixelArray = () => {
    return [...new Array(total)].map((_, index) => ({
      color: baseColor,
      index,
    }));
  };
  const [pixels, setPixels] = React.useState(createPixelArray());
  const [isPaintOn, setIsPaintOn] = React.useState(false);
  const [paintColor, setPaintColor] = React.useState(baseColor);
  React.useEffect(async () => {
    const { pixels } = await axios.get("http://localhost:3000/state");
  }, []);
  React.useEffect(() => {
    setPixels(createPixelArray());
  }, [total]);
  const handleSetStripPixels = async () => {
    const commands = pixels
      .reduce(
        (commands, pixel, index) => {
          const [head, ...rest] = commands;
          if (
            head.r === pixel.color.rgb.r &&
            head.g === pixel.color.rgb.g &&
            head.b === pixel.color.rgb.b
          ) {
            return [{ ...head, e: index }, ...rest];
          }
          return [
            { ...pixel.color.rgb, s: index, e: index + 1 },
            head,
            ...rest,
          ];
        },
        [{ s: 0, e: 0, ...baseColor.rgb }]
      )
      .map(async (command) => {
        return axios.get("http://localhost:3000/", {
          params: {
            command: "setPixels",
            ...command,
          },
        });
      });
    await Promise.all(commands);
  };
  const chunkedPixels = chunk(pixels, 20);
  return (
    <React.Fragment>
      <div>
        <button onClick={handleSetStripPixels}>Set Strip Color</button>
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
      <div className="strip">
        {chunkedPixels.map((pxs, lineIndex) => (
          <div className="strip__lane" key={`lane-${lineIndex}`}>
            <div className="strip__connection strip__connection--top"></div>
            {pxs.map(({ color, index }) => {
              const updateColor = (pixel) => {
                setPixels([
                  ...pixels.slice(0, pixel.index),
                  pixel,
                  ...pixels.slice(pixel.index + 1, pixels.length),
                ]);
              };
              return (
                <div className="strip__pixel" key={`pixel-${index}`}>
                  <Pixel
                    hideLabel={hidePixelLabel}
                    color={color}
                    index={index}
                    onHover={
                      isPaintOn
                        ? ({ index }) =>
                            updateColor({ index, color: paintColor })
                        : null
                    }
                    onChangeColor={updateColor}
                  />
                </div>
              );
            })}
            <div className="strip__connection strip__connection--bottom"></div>
          </div>
        ))}
      </div>
    </React.Fragment>
  );
}
