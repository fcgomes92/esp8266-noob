import React from "react";
import { ChromePicker } from "react-color";
import "~styles/components/Pixel.scss";
export default function Pixel({
  color,
  index,
  onChangeColor,
  hideLabel = false,
  onHover = null,
}) {
  const [displayColorPicker, setDisplayColorPicker] = React.useState(false);
  const togglePicker = () => setDisplayColorPicker(!displayColorPicker);
  const { r, g, b, a = 1 } = color.rgb;
  const colorRGB = `rgba(${r},${g},${b},${a})`;
  const textColor = `rgba(${255 - r},${255 - g},${255 - b},1)`;
  return (
    <div className="pixel">
      <div
        className="pixel__led"
        onClick={togglePicker}
        onMouseOver={() => onHover && onHover({ index, color })}
        style={{
          backgroundColor: colorRGB,
          borderColor: colorRGB,
          boxShadow: `inset 0 1px 0 ${colorRGB}`,
        }}
      >
        {hideLabel || (
          <span
            className="pixel__led__label"
            style={{
              color: textColor,
            }}
          >
            {index}
          </span>
        )}
      </div>
      {displayColorPicker && (
        <div className="pixel__color-picker">
          <div className="pixel__color-picker__cover" onClick={togglePicker} />
          <ChromePicker
            color={color.rgb}
            onChange={(color) => onChangeColor({ color, index })}
          />
        </div>
      )}
    </div>
  );
}
